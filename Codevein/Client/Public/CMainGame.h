#pragma once
#include "CBase.h"
#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)
class CImGui_Manager;
class CData_Manager;

class CMainGame final : public CBase
{
private:
	explicit CMainGame();
	virtual ~CMainGame() = default;

public:
	HRESULT Native_Construct();
	void Tick(const _double& _dDeltaSeconds);
	HRESULT Render();
public:
	static void _stdcall Run_Thread(void* _pArg);
	// Running
	void Running();
private:
	// Open Level 
	HRESULT Open_Level(const LEVEL& _eLevel);
private:
	// Renderer
	HRESULT Ready_Static_Component();
	// Ready Static GameObject
	HRESULT Ready_Static_GameObj();
	// Ready Model
	HRESULT Ready_Static_Model_1();
	// Ready Model
	HRESULT Ready_Static_Texture_1();
	// Ready Static Tewxture 2
	HRESULT Ready_Static_Texture_2();
	// Laod Particle
	HRESULT Ready_Load_Partiacle_1();
private:
	//GameInstance, ImGuiManager
	CGameInstance*					m_pGameInstance = nullptr;
	CImGui_Manager*				m_pImGuiManager = nullptr;
	CData_Manager*				m_pDataManager = nullptr;
	// Devie, Context
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	// Renderer
	CRenderer*							m_pRendererComp = nullptr;
private:
	_uint										m_iThreadLoader = 0;
#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH];
	_uint						m_iNumRender = 0;
	_double					m_dTimeAcc = 0.f;
#endif // _DEBUG
public:
	static CMainGame* Create();
	virtual void Free() override;
};

END