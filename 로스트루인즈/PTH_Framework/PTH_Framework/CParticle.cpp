#include "pch.h"
#include "CParticle.h"
#include "CGameMgr.h"
#include "CMyBmp.h"

CParticle::CParticle() : m_pLine(nullptr), m_fDownTime(0.f), m_fRestrictTime(0.f)
{
	Initialize();
}


CParticle::~CParticle()
{
	m_pDrawBmp = nullptr;
}

void CParticle::Initialize()
{
	m_v2Size = VECTOR2(100.f, 100.f);
	m_v2DrawSize = VECTOR2(100.f, 100.f);
	m_tLifeTimer.iPassTime = 1500;
	m_fRestrictTime = 0.0001f;
}

int CParticle::Update()
{
	if (true == m_tLifeTimer.Is_PassTime() && true == m_bEnable)
	{
		m_bDead = true;
		m_fDownTime = 0.f;
	}

	if (m_bDead)
		return OBJ_DEADEVENT;

	Fall();
	return OBJ_NOEVENT;
}

void CParticle::Late_Update()
{
}

void CParticle::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (nullptr != m_pDrawBmp)
	{
		Update_Rect();
		BitBlt(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x),
				static_cast<int>(m_tRect.top + v2ScrollPos.y), 
			static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y),
				// 그릴 애니메이션 그림의 위치
			m_pDrawBmp->Get_MemDC(), 0, 0, SRCCOPY);
	}
}

void CParticle::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;


}

void CParticle::Release()
{
}

void CParticle::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos, Left, Right
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	// Draw Siz e
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}

void CParticle::Fall()
{
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();
	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2CurPos, &fY);
	//자유낙하 공식	

	m_v2CurPos.y -= 30 * -9.8f * m_fDownTime;
	//m_fDownTime += 0.00001f;

	m_v2CurPos = m_v2CurPos + m_v2Dir * m_fMoveSpeed;
	// 일정 시간 지날 때 마다 타이머 초기
	if (bIsColl)
	{
		if (m_v2CurPos.y <= fY)
			m_v2CurPos.y = fY;
	}
	else
		m_tLifeTimer.dwTime = GetTickCount();
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;

}
