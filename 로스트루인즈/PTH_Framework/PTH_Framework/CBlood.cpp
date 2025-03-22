#include "pch.h"
#include "CBlood.h"
#include "CGameMgr.h"

CBlood::CBlood()
{
	Initialize();
}

CBlood::~CBlood()
{
	Release();
}

void CBlood::Initialize()
{
	m_v2Size = VECTOR2(100.f, 100.f);
	m_v2DrawSize = VECTOR2(100.f, 100.f);
	m_tLifeTimer.iPassTime = 100;

	m_vecBmpList.resize(4);
	m_vecBmpList[BLOOD1_L] = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"BLOOD1_L");
	m_vecBmpList[BLOOD1_R] = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"BLOOD1_R");
	m_vecBmpList[BLOOD2_L] = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"BLOOD2_L");
	m_vecBmpList[BLOOD2_R] = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"BLOOD2_R");
}

int CBlood::Update()
{
	if (m_tLifeTimer.Is_PassTime())
	{
		m_bEnable = false;
		m_pObj = nullptr;
	}
	Update_Rect();
	return OBJ_NOEVENT;
}

void CBlood::Late_Update()
{
}

void CBlood::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (nullptr != m_pDrawBmp)
	{
		GdiTransparentBlt(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x),
			static_cast<int>(m_tRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
			// 그릴 애니메이션 그림의 위치
			m_pDrawBmp->Get_MemDC(), 0, 0, m_pDrawBmp->Get_Width(), m_pDrawBmp->Get_Height(), RGB(0, 0, 0));
	}
}

void CBlood::Collision_Actor(CObj *& pObj)
{
}

void CBlood::Release()
{
}

void CBlood::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive; 
	m_tLifeTimer.dwTime = GetTickCount();	
}

void CBlood::Update_Rect()
{
	if (nullptr != m_pObj)
	{
		m_v2CenterPos = m_pObj->Get_CenterPos();
	}
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CenterPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CenterPos.y - v2RealSize.y);
	m_tRect.right = static_cast<long>(m_v2CenterPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CenterPos.y + v2RealSize.y);
	// CenterPos, Left, Right
	m_v2CurPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y + v2RealSize.y);
	// Draw Siz e
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CenterPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CenterPos.y - v2RealSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CenterPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CenterPos.y + v2RealSize.y);
}

void CBlood::Set_BloodBmp(const int & iIndex)
{
	if (nullptr != m_pObj)
	{
		if (true == m_pObj->Is_Dead())
			m_bEnable = false;
	}

	if (iIndex == 0)
	{
		if (m_v2Dir.x > 0)
			m_pDrawBmp = m_vecBmpList[BLOOD1_R];
		else
			m_pDrawBmp = m_vecBmpList[BLOOD1_L];
	}
	else if (iIndex == 1)
	{
		if (m_v2Dir.x > 0)
			m_pDrawBmp = m_vecBmpList[BLOOD2_R];
		else
			m_pDrawBmp = m_vecBmpList[BLOOD2_L];
	}
	m_v2Dir = VECTOR2() - (m_v2Dir * 10.f);
}
