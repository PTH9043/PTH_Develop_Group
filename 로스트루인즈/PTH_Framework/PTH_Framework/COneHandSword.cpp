#include "pch.h"
#include "COneHandSword.h"
#include "CGameMgr.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CCollision.h"

COneHandSword::COneHandSword()
{
	Initialize();
}

COneHandSword::COneHandSword(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const ESWORDID & eSwordID, const int & iPower, const int& iGold)
	: CSword(pOwner, szImgPath, strName, eSwordID, iPower, iGold)
{
	Initialize();
}

COneHandSword::~COneHandSword()
{
	Release();
}

void COneHandSword::Initialize()
{
	m_pRotate = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_ROTATE");
	m_pReset = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_RESET");
	m_v2DrawSize = VECTOR2(180.f, 180.f);
	m_v2Size = VECTOR2(30.f, 80.f);
	m_vecCollisionPoint.reserve(4);
	for (int i = 0; i < 4; ++i)
		m_vecCollisionPoint.emplace_back(VECTOR2());

	m_eObjID = OBJ_EQUIP;
}

int COneHandSword::Update()
{
	if (m_bDead)
		return OBJ_DEADEVENT;
	Update_OnwerInfo();
	Rotate();
	Fall();
	Update_Rect();
	return OBJ_NOEVENT;
}

void COneHandSword::Late_Update()
{

}

void COneHandSword::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	CBmpMgr* pBmpMgr = CGameMgr::Get_Instance()->Get_BmpMgr();
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
		return;

	HDC	hRotaionDC = m_pRotate->Get_MemDC();
	HDC	hResetDC = m_pReset->Get_MemDC();
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

		Rotation_V2(m_vecCollisionPoint[0], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[1], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[2], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
		Rotation_V2(m_vecCollisionPoint[3], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);

		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[0], m_vecCollisionPoint[1]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[1], m_vecCollisionPoint[2]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[2], m_vecCollisionPoint[3]);
		Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[3], m_vecCollisionPoint[0]);
	}
	if (m_bRotate)
	{
		HDC	hRotaionDC = m_pRotate->Get_MemDC();
		HDC	hResetDC = m_pReset->Get_MemDC();

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
	else
	{
		if (false == m_bItemPicked)
		{
			// 현재 애니메이션이 그려질 위치
			GdiTransparentBlt(_hdc, static_cast<int>(m_tDrawRect.left + v2ScrollPos.x),
				static_cast<int>(m_tDrawRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
				// 그릴 애니메이션 그림의 위치
				m_pImgBmp->Get_MemDC(), 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));
		}
	}
}

void COneHandSword::Collision_Actor(CObj *& pObj)
{
	if (false == m_bEnable)
		return;
	if (nullptr == pObj)
		return;
	if (nullptr == m_pOwner)
		return;
	if (true == m_bCollision)
		return;

	if (OBJ_PLAYER == m_pOwner->Get_ObjID())
	{
		if (OBJ_MONSTER == pObj->Get_ObjID())
		{
			if (false == CCollision::Is_ObbColl_Equip(pObj, this))
				return;

			VECTOR2 v2Pos = m_pOwner->Get_CurPos();
			VECTOR2 v2MonPos = pObj->Get_CurPos();
			VECTOR2 v2Dir = v2MonPos - v2Pos;
			v2Dir.Normalize();
			v2Dir.y = 0.f;
			static_cast<CMonster*>(pObj)->Set_DamagedDir(v2Dir * 2.f);
			CGameMgr::Get_Instance()->Get_EffectMgr()->Blood_Effect(pObj, v2Dir, 0);
			float fDamge = static_cast<float>(static_cast<CCharacter*>(m_pOwner)->Get_Status().iAtkPower +
				m_tStatus.iAtkPower);
			static_cast<CMonster*>(pObj)->Damaged(fDamge);
			m_bCollision = true;
		}
	}
}

void COneHandSword::Release()
{
}

CItem * COneHandSword::Copy_Item()
{
	COneHandSword* pOneHand = new  COneHandSword(m_pOwner, m_szImgPath, m_strName, m_eSwordID, m_tStatus.iAtkPower, m_iGold);
	pOneHand->Set_Name(m_strName);
	return pOneHand;
}

void COneHandSword::Obj_Active(const bool & IsActive)
{
	m_bRotate = IsActive;
	m_bUse = IsActive;
	m_fDistance = 50.;
	m_bCollision = false;
}

void COneHandSword::Update_Rect()
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
	// Attack Pos 
	m_v2AttackPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y - v2Size.y / 2.f);
}

void COneHandSword::Rotate()
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

void COneHandSword::Update_OnwerInfo()
{
	if (true == m_bUse && true == m_bRotate)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		if (nullptr != pPlayer)
		{
			VECTOR2 v2Pos = VECTOR2(cosf(RADIAN_TO_DEGREE(m_fSpinAngle)), sinf(RADIAN_TO_DEGREE(m_fSpinAngle))) * m_fDistance;
			VECTOR2 v2WeaponPos = pPlayer->Get_EquipPos();
			VECTOR2 v2Size = pPlayer->Get_Size();
			v2Size = v2Size / 2.f;
			v2Size.x = 0.f;
			m_v2CenterPos = v2Pos + v2WeaponPos - v2Size;
			m_fRotValue -= 6.f * m_iMultipleValue;
			m_fSpinAngle += 12.f* m_iMultipleValue;
			m_fDistance += 2.5f;
		}
	}
}

void COneHandSword::Fall()
{
	if (false == m_bItemPicked)
	{
		CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

		float fY = 0.f;
		bool bIsColl = pLineMgr->Collision_Line(m_v2CenterPos, &fY);
		if (false == bIsColl)
		{
			//자유낙하 공식	
			m_v2CenterPos.y -= 50 * -9.8f * m_fDownTime;
			m_fDownTime += 0.001f;

			// 만약 충돌 상태이고 점프하지 않은 상태라면
			if (true == bIsColl)
			{
				// 그라운드를 true로 변경
				// 현재 Y값을 고정시킨다. 
				if (m_v2CenterPos.y >= fY)
				{
					m_v2CenterPos.y = fY;
				}
			}

			// 일정 시간 지날 때 마다 타이머 초기화
			if (m_fDownTime > m_fRestrictTime)
				m_fDownTime = m_fRestrictTime;
		}
	}
}
