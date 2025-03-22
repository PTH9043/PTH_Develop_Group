#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
class CImGuiClass_Manager;
class CBattle_Manager;
class CCamera_Manager;
class CCutScene_Manager;

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;
public:
	HRESULT NativeConstruct();
	void	Tick(const _double& _dTimeDelta);
	HRESULT Render();
private:
	CGameInstance*					m_pGameInstance = nullptr;
	CImGuiClass_Manager*	m_pImGuiManager = nullptr;
	CBattle_Manager*				m_pBattleManager = nullptr;
	CCamera_Manager*			m_pCameraManager = nullptr;
	CCutScene_Manager*		m_pCutSceneManager = nullptr;
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CRenderer*							m_pRenderer = nullptr;
private:
	list<_tchar*> m_vecEffectProtoTag;
	_uint		m_iCurLevel = 0;

#ifdef _DEBUG
private:
	_double					m_TimeAcc = 0.0;
	_uint					m_iNumRender = 0;
	_tchar					m_szFPS[MAX_PATH] = L"";
#endif // DEBUG


public:
	HRESULT Open_Level(LEVEL eLevel);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Components();

private:
	void	SetUp_RendererDesc();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END