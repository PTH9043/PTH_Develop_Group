#pragma once
#include "ModelDefines.h"
#include "CImGui_Manager_MD.h"
#include "CData_Manager_MD.h"
#include "CBase.h"
#include "CGameInstance.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Model)

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;
public:
	HRESULT NativeConstruct();
	void Tick(const _double& _dDeltaSeconds);
	HRESULT Render();
private:
	// Renderer
	HRESULT Ready_Static_Component();
	// Ready Static GameObject
	HRESULT Ready_Static_GameObj();
	// Open Level 
	HRESULT Open_Level(const LEVEL& _eLevel);
private:
	// Game Instance
	CGameInstance*						m_pGameInstance = nullptr;
	// ImGUiManager
	CImGui_Manager_MD*			m_pImGuiManager = nullptr;
	// Data Manager 
	CData_Manager_MD*			m_pDataManger_MD = nullptr;
	// Renderer
	CRenderer* m_pRendererComp = nullptr;
private:
	ID3D11Device*						m_pDevice = nullptr; 
	ID3D11DeviceContext*			m_pContext = nullptr;
#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH];
	_uint					m_iNumRender = 0;
	_double					m_dTimeAcc = 0.f;
#endif // _DEBUG
public:
	static CMainApp* Create();
	virtual void Free() override;
};

END