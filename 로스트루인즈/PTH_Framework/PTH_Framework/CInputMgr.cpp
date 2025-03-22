#include "pch.h"
#include "CInputMgr.h"


CInputMgr::CInputMgr()
{
	ZeroMemory(&m_bKeyList, sizeof(bool) * 0xff);
}

CInputMgr::CInputMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr)
{
	ZeroMemory(&m_bKeyList, sizeof(bool) * 0xff);
}

CInputMgr::~CInputMgr()
{

}

void CInputMgr::Initialize()
{
}

int CInputMgr::Update()
{
	// Mouse Move 
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);	// 스크린 상태의 마우스 좌표를 우리가 생성한 윈도우 창 좌표로 변경해주는 함수
	m_v2MousePos = VECTOR2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	return 0;
}

void CInputMgr::Release()
{
}

bool CInputMgr::Is_Press(const int & iKey)
{
	// 누르지 않은 상태, 호출 시점에 눌렀을 떄
	if (m_bKeyList[iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyList[iKey] = !m_bKeyList[iKey];
		return true;
	}
	// 누른 상태, 호출 시점에 눌렀을 떄
	else if (!m_bKeyList[iKey] && (GetAsyncKeyState(iKey) & 0x8000))
		m_bKeyList[iKey] = !m_bKeyList[iKey];

	return false;
}

bool CInputMgr::Is_Pressing(const int & iKey)
{
	// 누르지 않았던 상태, 호출 시점에 누르고 있을 때
	if (m_bKeyList[iKey] && !(GetAsyncKeyState(iKey) & 0x8001))
	{
		m_bKeyList[iKey] = !m_bKeyList[iKey];
		return true;
	}
	// 누르던 상태, 호출 시점에 누르고 있을 때
	else if (!m_bKeyList[iKey] && (GetAsyncKeyState(iKey) & 0x8001))
		return true;
	
	return false;
}

bool CInputMgr::Is_MouseClick()
{
	bool press = GetAsyncKeyState(VK_LBUTTON) && 0x8001;
	if (true == press)
	{
		m_bKeyList[VK_LBUTTON] = true;
		return true;
	}
	else
		m_bKeyList[VK_LBUTTON] = false;
	return false;
}

