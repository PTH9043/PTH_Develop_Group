#include "CMainApp.h"
#include "CLevel_Loading_MD.h"
#include "CShowObject.h"
#include "CCamera_MD.h"
#include "CAnimObject.h"

CMainApp::CMainApp() : 
	m_pGameInstance(CGameInstance::Get_Instance()),
	m_pImGuiManager(CImGui_Manager_MD::Get_Instance()),
	m_pDataManger_MD(CData_Manager_MD::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pDataManger_MD);
}

HRESULT CMainApp::NativeConstruct()
{
	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWindowed = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinSizeX;
	GraphicDesc.iWinCY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	// Find File
	if (FAILED(m_pGameInstance->Load_FirstFolder(L"..\\..\\Resource")))
		return E_FAIL;
	//// Furst Culling
	//CFrustCulling::FRUSTDESC tFrustDesc = CFrustCulling::FRUSTDESC(60.f, _float(g_iWinSizeX) / g_iWinSizeY, 0.2f, 500.f);
	//if (FAILED(m_pGameInstance->NativeConstruct_FrustCull(m_pDevice, m_pContext, tFrustDesc)))
	//	return E_FAIL;

	if (FAILED(Ready_Static_Component()))
		return E_FAIL;

	if (FAILED(Ready_Static_GameObj()))
		return E_FAIL;

	if (FAILED(m_pDataManger_MD->NativeConstruct(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->NativeConstruct(m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _double& _dDeltaSeconds)
{	
	/*  ImGui Tick */
	m_pImGuiManager->Tick(_dDeltaSeconds);
	m_pGameInstance->Tick_Engine(_dDeltaSeconds);
	m_pGameInstance->LateTick_Engine(_dDeltaSeconds);

#ifdef _DEBUG
	m_dTimeAcc += _dDeltaSeconds;
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	m_pImGuiManager->Render();

	m_pGameInstance->Render_Engine();
	m_pRendererComp->Draw();
	m_pGameInstance->Present();

#ifdef _DEBUG
	++m_iNumRender;
	if (m_dTimeAcc > 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d \n"), m_iNumRender);
		SetWindowText(g_hWnd, m_szFPS);

		m_dTimeAcc = 0.f;
		m_iNumRender = 0;
	}
#endif
	return S_OK;
}

HRESULT CMainApp::Ready_Static_Component()
{
	m_pRendererComp = CRenderer::Create(m_pDevice, m_pContext);
	Safe_AddRef(m_pRendererComp);
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_RENDERER, m_pRendererComp)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL,
		CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\Shader\\Shader_VtxModel.hlsl",
			VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL,
		CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\Shader\\Shader_VtxAnimModel.hlsl",
			VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Static_GameObj()
{
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SHOWOBJECT, CShowObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ANIMOBJECT, CAnimObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAM_MD, CCamera_MD::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Open_Level(const LEVEL& _eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_MD::Create(m_pDevice, m_pContext, _eLevel))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMainApp::Create To Failed");
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRendererComp);

	Safe_Release(m_pDataManger_MD);
	CData_Manager_MD::Destroy_Instance();
	
	Safe_Release(m_pImGuiManager);
	CImGui_Manager_MD::Destroy_Instance();

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
