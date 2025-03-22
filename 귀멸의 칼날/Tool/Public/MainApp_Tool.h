#pragma once
#include "GameInstance.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Tool)
class CMainView_Tool;

class CMainApp_Tool final : public CBase
{
private:
	explicit CMainApp_Tool();
	virtual ~CMainApp_Tool() = default;

public:
	HRESULT Native_Construct();
	void Tick(const _double& _dDeltaSeconds);
	HRESULT Render();
private:
	// Open Level 
	HRESULT Open_Level(const LEVEL& _eLevel);
private:
	//GameInstance, ImGuiManager
	CGameInstance* m_pGameInstance = nullptr;
	class CImGuiClass_Manager_Tool* m_pImGuiClassManager_Tool = nullptr;
	// Devie, Context
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// Renderer
	CRenderer* m_pRendererComp = nullptr;
private:
	// Renderer
	HRESULT Ready_Static_Component();
	// Ready Static GameObject
	HRESULT Ready_Static_GameObj();
private:
	list<_tchar*> m_vecEffectProtoTag;
#ifdef _DEBUG
private:
	_tchar									m_szFPS[MAX_PATH] = {};
	_uint										m_iNumRender = 0;
	_double									m_dTimeAcc = 0.f;
#endif // _DEBUG
public:
	static CMainApp_Tool* Create();
	virtual void Free() override;
};

END