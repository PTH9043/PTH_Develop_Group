#include "pch.h"
#include "CScrollMgr.h"

CScrollMgr::CScrollMgr() : m_v2Pos(VECTOR2())
{
}
CScrollMgr::CScrollMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr)
{

}

CScrollMgr::~CScrollMgr()
{
}
void CScrollMgr::Initialize()
{
}

int CScrollMgr::Update()
{
	Scroll_Lock();
	return 0;
}

void CScrollMgr::Render(HDC _hdc)
{
}

void CScrollMgr::Release()
{
}

void CScrollMgr::Move_Scroll()
{
}

void CScrollMgr::Scroll_Lock()
{
	if (m_v2Pos.x > 0)
		m_v2Pos.x = 0.f;
	if (m_v2Pos.y > 0)
		m_v2Pos.y = 0.f;
	if (m_v2Pos.x <= -(MAP_X - WINCX ))
		m_v2Pos.x = -(MAP_X - WINCX );
	if (m_v2Pos.y <= -(MAP_Y - WINCY / 2.f))
		m_v2Pos.y = -(MAP_Y - WINCY / 2.f);
}

