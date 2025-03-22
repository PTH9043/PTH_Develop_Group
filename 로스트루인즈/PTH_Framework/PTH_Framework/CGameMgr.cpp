#include "pch.h"
#include "CGameMgr.h"

float g_fSound = 0.f;

CGameMgr::CGameMgr() : m_pBmpMgr(nullptr), m_pScrollMgr(nullptr)
, m_dwTime(GetTickCount()), m_iFPS(0), m_bDevelopMode(false), m_bGobMode(false)
{
	ZeroMemory(m_szFPS, sizeof(m_szFPS));
	ZeroMemory(&m_tClientRect, sizeof(RECT));
}

CGameMgr::~CGameMgr()
{
	Release();
}

void CGameMgr::Initialize()
{
	srand(static_cast<unsigned int>(NULL));

	g_fSound = 0.5f;
	GetClientRect(g_hWnd, &m_tClientRect);

	// Back DC 가져옴
	m_pBackMyBmp = new CMyBmp();
	m_pBackMyBmp->Load_Bmp(L"Resources\\Background.bmp");
	m_pDrawBmp = new CMyBmp();
	m_pDrawBmp->Load_Bmp(L"Resources\\Background.bmp");

	
	CGameMgr* pGameMgr = this;
	m_pSoundMgr = new CSoundMgr(pGameMgr);
	m_pBmpMgr = new CBmpMgr(pGameMgr);
	m_pScrollMgr = new CScrollMgr(pGameMgr);
	m_pInputMgr = new CInputMgr(pGameMgr);
	m_pObjMgr = new CObjMgr(pGameMgr);
	m_pLineMgr = new CLineMgr(pGameMgr);
	m_pSceneMgr = new CSceneMgr(pGameMgr);
	m_pParticleMgr = new CParticleMgr(pGameMgr);
	m_pEffectMgr = new CEffectMgr(pGameMgr);
}

int CGameMgr::Update()
{
	if (true == m_pInputMgr->Is_Press(VK_F1))
		m_bDevelopMode = !m_bDevelopMode;

	if (true == m_pInputMgr->Is_Press(VK_F6))
		m_bGobMode = !m_bGobMode;

	Sound_Control();
	m_pSceneMgr->Update();
	m_pObjMgr->Update();
	m_pScrollMgr->Update();
	return 0;
}

void CGameMgr::Late_Update()
{
	m_pSceneMgr->Late_Update();
	m_pObjMgr->Late_Update();
}

void CGameMgr::Render(HDC _hdc)
{
	VECTOR2 v2ScrollPos = m_pScrollMgr->Get_Pos();
	HDC hBackDC = m_pBackMyBmp->Get_MemDC();
	HDC hDrawDC = m_pDrawBmp->Get_MemDC();
	// 여기서 렌더링 합니다... 
	m_pSceneMgr->Render(hBackDC, v2ScrollPos);
	m_pObjMgr->Render(hBackDC, v2ScrollPos);
	BitBlt(_hdc, 0, 0, WINCX, WINCY, hBackDC, 0, 0, SRCCOPY);
	BitBlt(hBackDC, 0, 0, WINCX, WINCY, hDrawDC, 0, 0, SRCCOPY);
	FPS_Out();
}

void CGameMgr::Release()
{
	DELETE_MEMORY(m_pBackMyBmp);
	DELETE_MEMORY(m_pDrawBmp);
	DELETE_MEMORY(m_pBmpMgr);
	DELETE_MEMORY(m_pScrollMgr);
	DELETE_MEMORY(m_pInputMgr);
	DELETE_MEMORY(m_pLineMgr);
	DELETE_MEMORY(m_pSoundMgr);
	DELETE_MEMORY(m_pParticleMgr);
	DELETE_MEMORY(m_pEffectMgr);
	DELETE_MEMORY(m_pSceneMgr);
	DELETE_MEMORY(m_pObjMgr);
}

void CGameMgr::FPS_Out()
{
	// 프레임 출력
	++m_iFPS;

	if (m_dwTime + 1000 < GetTickCount())
	{
		wsprintf(m_szFPS, L"FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, m_szFPS);

		m_iFPS = 0;
		m_dwTime = GetTickCount();
	}
}

void CGameMgr::Sound_Control()
{
	if (m_pInputMgr->Is_Pressing(VK_F3))
	{
		g_fSound -= 0.1f;

		if (g_fSound < 0.f)
			g_fSound = 0.f;

		m_pSoundMgr->SetChannelVolume(SOUND_EFFECT, g_fSound);
	}

	if (m_pInputMgr->Is_Pressing(VK_F4))
	{
		g_fSound += 0.1f;

		if (g_fSound > 1.f)
			g_fSound = 1.f;

		m_pSoundMgr->SetChannelVolume(SOUND_EFFECT, g_fSound);
	}
}
