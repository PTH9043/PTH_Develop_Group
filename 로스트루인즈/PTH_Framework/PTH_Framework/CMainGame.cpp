#include "pch.h"
#include "CMainGame.h"
#include "CGameMgr.h"

CMainGame::CMainGame() : m_hDC(NULL)
{
	Initialize();
}

CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize()
{
	m_hDC = GetDC(g_hWnd);
	CGameMgr::Get_Instance()->Initialize();
}

void CMainGame::Release()
{
	ReleaseDC(g_hWnd, m_hDC);
	CGameMgr::Destory_Instance();
}

void CMainGame::Update()
{
	CGameMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CGameMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	CGameMgr::Get_Instance()->Render(m_hDC);
}
