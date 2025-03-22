#pragma once
#include "CSingleTon.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CSceneMgr.h"
#include "CLineMgr.h"
#include "CObjMgr.h"
#include "CInputMgr.h"
#include "CSoundMgr.h"
#include "CParticleMgr.h"
#include "CEffectMgr.h"

class CGameMgr : public CSingleTon<CGameMgr>
{
public:
	explicit CGameMgr();
	virtual ~CGameMgr();

protected:
	// Manager들 선언
	CBmpMgr*			m_pBmpMgr;
	CScrollMgr*		m_pScrollMgr;
	CInputMgr*		m_pInputMgr;
	CObjMgr*			m_pObjMgr;
	CSceneMgr*		m_pSceneMgr;
	CLineMgr*			m_pLineMgr;
	CSoundMgr*		m_pSoundMgr;
	CParticleMgr*		m_pParticleMgr;
	CEffectMgr*		m_pEffectMgr;
	// 백 DC
	CMyBmp*			m_pBackMyBmp;
	// 백 DC
	CMyBmp*			m_pDrawBmp;
	// FPS
	DWORD				m_dwTime;
	int						m_iFPS;
	TCHAR					m_szFPS[16];
	// Client Rect
	RECT					m_tClientRect;
	bool						m_bDevelopMode;
	bool						m_bGobMode;
public:
	void Initialize();
	int Update();
	void Late_Update();
	void Render(HDC _hdc);
	void Release();
protected:
	void FPS_Out();
	// Sound Control
	void Sound_Control();
public:
	CBmpMgr* Get_BmpMgr() { return m_pBmpMgr; }
	CScrollMgr* Get_CScrollMgr() { return m_pScrollMgr; }
	CInputMgr* Get_InputMgr() { return m_pInputMgr; }
	CObjMgr* Get_ObjMgr() { return m_pObjMgr; }
	CSceneMgr * Get_SceneMgr() { return m_pSceneMgr; }
	CLineMgr* Get_LineMgr() { return m_pLineMgr; }
	CSoundMgr* Get_SoundMgr() { return m_pSoundMgr; }
	CParticleMgr* Get_ParticleMgr() { return m_pParticleMgr; }
	CEffectMgr* Get_EffectMgr() { return m_pEffectMgr; }
	const RECT& Get_ClientRect() const { return m_tClientRect; }

	const bool& Is_DevelopMode() const { return m_bDevelopMode; }
	const bool& Is_GobMode() const { return m_bGobMode; }
};

