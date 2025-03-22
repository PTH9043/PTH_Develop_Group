#include "pch.h"
#include "CArrow.h"
#include "CGameMgr.h"
#include "CCollision.h"
#include "CCharacter.h"

CArrow::CArrow() : m_tDeadTimer(TIMER(1500)), m_iAttackPower(0), m_iSpeed(0), m_bFloor(false)
{
	Initialize();
}

CArrow::CArrow(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName)
	: CItem(pOwner, szImgPath, strName), m_tDeadTimer(TIMER(1500)), m_iAttackPower(0), m_iSpeed(0), m_bFloor(false)
{
	Initialize();
}

CArrow::~CArrow()
{
	Release();
}

void CArrow::Initialize()
{
	m_vecCollisionPoint.reserve(4);
	for (int i = 0; i < 5; ++i)
		m_vecCollisionPoint.emplace_back(VECTOR2());

	m_v2Size = VECTOR2(30.f, 30.f);
	m_iGold = 1;
	
}

int CArrow::Update()
{
	if (m_tDeadTimer.Is_PassTime())
	{
		return OBJ_DEADEVENT;
	}
	if(m_bDead)
		return OBJ_DEADEVENT;
	
	Rotate();
	Fall();
	Update_Rect();
	return OBJ_NOEVENT;
}

void CArrow::Late_Update()
{
	for (auto& iter : CGameMgr::Get_Instance()->Get_ObjMgr()->Get_ObjList(OBJ_MONSTER))
	{
		Collision_Actor(iter);
	}
	CObj* pPlayer = CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Player();
	Collision_Actor(pPlayer);
}

void CArrow::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	CBmpMgr* pBmpMgr = CGameMgr::Get_Instance()->Get_BmpMgr();
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
		return;

	HDC	hRotaionDC = pBmpMgr->Find_Image(L"ITEM_ROTATE");
	HDC	hResetDC = pBmpMgr->Find_Image(L"ITEM_RESET");
	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));
	if (CGameMgr::Get_Instance()->Is_DevelopMode())
	{
		VECTOR2 v2Size = m_v2DrawSize / 2.f;
		// Left Up
		m_vecCollisionPoint[0].x = m_v2CenterPos.x - v2Size.x / 3.f;
		m_vecCollisionPoint[0].y = m_v2CenterPos.y - v2Size.y;
		// Right Up
		m_vecCollisionPoint[1].x = m_v2CenterPos.x + v2Size.x / 3.f;
		m_vecCollisionPoint[1].y = m_v2CenterPos.y - v2Size.y;
		// Right Down
		m_vecCollisionPoint[2].x = m_v2CenterPos.x + v2Size.x / 3.f;
		m_vecCollisionPoint[2].y = m_v2CenterPos.y + v2Size.y;
		// Left Down
		m_vecCollisionPoint[3].x = m_v2CenterPos.x - v2Size.x / 3.f;
		m_vecCollisionPoint[3].y = m_v2CenterPos.y + v2Size.y;

		m_vecCollisionPoint[4].x = m_v2CenterPos.x + v2Size.x / 2.f;
		m_vecCollisionPoint[4].y = m_v2CenterPos.y;

		Rotation_V2(m_vecCollisionPoint[0], m_v2CenterPos, m_v2Size.y, m_v2Size.x, 90.f - m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[1], m_v2CenterPos, m_v2Size.y, m_v2Size.x, 90.f - m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[2], m_v2CenterPos, m_v2Size.y, m_v2Size.x, 90.f - m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[3], m_v2CenterPos, m_v2Size.y, m_v2Size.x, 90.f - m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[4], m_v2CenterPos, m_v2Size.y, m_v2Size.x, 90.f - m_fRotValue);

		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[0], m_vecCollisionPoint[1]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[1], m_vecCollisionPoint[2]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[2], m_vecCollisionPoint[3]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[3], m_vecCollisionPoint[0]);
	}

	PlgBlt(hRotaionDC,		// 회전 시킬 이미지를 어디에 그릴 것인가6
		m_tPoint,			// 그림을 그릴 사각형 좌표 포인트 배열의 이름(순서는 좌 상단, 우 상단, 좌 하단)
		m_pImgBmp->Get_MemDC(),				// 어디에 있는 이미지를 회전시킨 이미지 DC에 그릴 것인가
		0,					// 4,5 인자 : 그림을 그릴 이미지의 X,Y 시작 좌표
		0,
		static_cast<int>(m_v2DrawSize.x),	// 6,7 인자 : 이미지의 가로, 세로 길이
		static_cast<int>(m_v2DrawSize.y),
		NULL,
		NULL,
		NULL);

	// 현재 애니메이션이 그려질 위치
	GdiTransparentBlt(_hdc, static_cast<int>(m_tDrawRect.left + v2ScrollPos.x),
		static_cast<int>(m_tDrawRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
		// 그릴 애니메이션 그림의 위치
		hRotaionDC, 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));

	BitBlt(hRotaionDC, 0, 0, static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y), hResetDC, 0, 0, SRCCOPY);
}

void CArrow::Collision_Actor(CObj *& pObj)
{
	if (true == m_bFloor)
		return;
	if (true == m_bCollision)
		return;
	if (nullptr == pObj)
		return;
	if (nullptr == m_pOwner)
		return;
	if (true == m_bDead)
		return;

	CCharacter * pCharacter = dynamic_cast<CCharacter*>(pObj);
	if (nullptr == pCharacter)
		return;

	if (pObj == m_pOwner)
		return;
	if (pObj->Get_ObjID() == m_pOwner->Get_ObjID())
		return;
	if (false == CCollision::Is_OBBColl(pObj, this))
		return;

	float fDamge = static_cast<float>(static_cast<CCharacter*>(m_pOwner)->Get_Status().iAtkPower + m_iAttackPower);
	pCharacter->Damaged(fDamge);
	pCharacter->Set_DamagedDir(m_pOwner->Get_Dir());
	VECTOR2 v2Pos = m_pOwner->Get_CurPos();
	VECTOR2 v2MonPos = pObj->Get_CurPos();
	VECTOR2 v2Dir = v2MonPos - v2Pos;
	v2Dir.Normalize();
	v2Dir.y = 0.f;
	CGameMgr::Get_Instance()->Get_EffectMgr()->Blood_Effect(pObj, v2Dir, 0);
	m_bCollision = true;
}

void CArrow::Release()
{
}

CItem * CArrow::Copy_Item()
{
	return nullptr;
}


void CArrow::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive;
	if (DIR_FOR == m_eDirection)
	{
		m_fRotValue = 0.f;
		m_v2Dir = VECTOR2(1.f, 0.f);
	}
	else
	{
		m_fRotValue = 180.f;
		m_v2Dir = VECTOR2(-1.f, 0.f);
	}
}

void CArrow::Update_Rect()
{
	VECTOR2 v2Size = m_v2DrawSize / 2.f;
	// Collision Size
	//m_fRotValue += 1.f;
	// CenterPos, Left, Right
	m_v2CurPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y - v2Size.y);
	// Draw Size
	m_tDrawRect.left = static_cast<long>(m_v2CenterPos.x - v2Size.x);
	m_tDrawRect.top = static_cast<long>(m_v2CenterPos.y - v2Size.y);
	m_tDrawRect.right = static_cast<long>(m_v2CenterPos.x + v2Size.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CenterPos.y + v2Size.y);
	m_tRect.left = static_cast<long>(m_v2CenterPos.x - v2Size.x);
	m_tRect.top = static_cast<long>(m_v2CenterPos.y - v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CenterPos.x + v2Size.x);
	m_tRect.bottom = static_cast<long>(m_v2CenterPos.y + v2Size.y);

//	VECTOR2 v2Pos = m_v2CenterPos + VECTOR2()
}
void CArrow::Rotate()
{
	VECTOR2 v2Size = m_v2DrawSize / 2.f;
	m_fDiagonal = sqrtf(v2Size.x * v2Size.x + v2Size.y * v2Size.y);
	//좌 상단
	Rotation_Point(m_tPoint[0], VECTOR2(v2Size.x, v2Size.x), 135.f, m_fRotValue, m_fDiagonal, m_fDiagonal);
	//우 상단
	Rotation_Point(m_tPoint[1], VECTOR2(v2Size.x, v2Size.y), 45.f, m_fRotValue, m_fDiagonal, m_fDiagonal);
	//좌 하단
	Rotation_Point(m_tPoint[2], VECTOR2(v2Size.x, v2Size.x), 225.f, m_fRotValue, m_fDiagonal, m_fDiagonal);
}

void CArrow::Fall()
{
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2CenterPos, &fY);
	if (false == bIsColl)
	{
		//자유낙하 공식	
		m_v2CenterPos.y -= 30 * -9.8f * m_fDownTime;
		m_fDownTime += 0.001f;

		// 만약 충돌 상태이고 점프하지 않은 상태라면
		if (DIR_FOR == m_eDirection)
			m_fRotValue -= 1.f;
		else
			m_fRotValue += 1.f;

		m_v2CenterPos = m_v2CenterPos + m_v2Dir * static_cast<float>(m_iSpeed);
		m_tDeadTimer.dwTime = GetTickCount();
	}
	else
	{
		m_bFloor = true;
		m_bCollision = true;
	}
	// 일정 시간 지날 때 마다 타이머 초기화
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;
}