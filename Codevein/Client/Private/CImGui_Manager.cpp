#include "CMainView.h"
#include "CObjectTool.h"
#include "CMapTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "CGameObject_Client.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

CGameObject* CImGui_Manager::Get_DebugObject()
{
	return static_cast<CObjectTool*>(m_pMainView->Get_ImGuiClass(CMainView::GUI_OBJ))->Get_DebugObject();
}

_bool CImGui_Manager::Is_RenderingTerrain()
{
	return m_pMainView->Get_ImGuiClass(CMainView::GUI_MAP)->Is_Open();
}

map<const _tchar*, CGameObject_Client*> CImGui_Manager::Get_ProtoObjMap(){
	return m_pMainView->Get_ProtoObjMap();
}

HRESULT CImGui_Manager::Add_Proto(const _tchar* _pTag, CGameObject_Client* _pObject)
{
	if (nullptr == m_pMainView)
		return E_FAIL;

	return m_pMainView->Add_ProtoObjMap(_pTag, _pObject);
}

void CImGui_Manager::Set_DebugObject_Model(CModel* _pModel )
{
	if (nullptr == _pModel)
		return;

	CObjectTool* pObjectTool = static_cast<CObjectTool*>(m_pMainView->Get_ImGuiClass(CMainView::GUI_OBJ));
	if (nullptr == pObjectTool)
		return;

	pObjectTool->Set_DebugObject_Model(_pModel);
}

HRESULT CImGui_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	BEGIN_SINGLETON(CGameInstance, pGameInstnace)
	{
		FILEGROUP* pFileDesc = pGameInstnace->Find_Folder(&PATHS{ L"Font", L"NanumFontSetup_TTF_SQUARE" });
		FILEDATA tData = *pFileDesc->FindData(L"NanumSquareEB");

		string path;
		path.assign(tData.strFilePath.begin(), tData.strFilePath.end());
		io.Fonts->AddFontFromFileTTF(path.c_str(), 18.f, NULL, io.Fonts->GetGlyphRangesKorean());
	}
	END_SINGLETON(pGameInstnace);

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::StyleColorsLight();
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);


	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	_uint		iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FreatureLV;
	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &pDevice, &FreatureLV, &pContext)))
		return E_FAIL;

	ImGui_ImplDX11_Init(pDevice, pContext);
	Safe_Release(pDevice);
	Safe_Release(pContext);

	Safe_AddRef(_pDevice);
	Safe_AddRef(_pContext);
	
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	// Create MainView
	m_pMainView = CMainView::Create(m_pDevice, m_pContext);
	return S_OK;
}

HRESULT CImGui_Manager::NativeConstruct_Tool()
{
	if (FAILED(m_pMainView->NativeConstruct_Tool()))
		return E_FAIL;

	return S_OK;
}

HRESULT CImGui_Manager::Release_Tool()
{
	if (FAILED(m_pMainView->Release_Tool()))
		return E_FAIL;

	return S_OK;
}

void CImGui_Manager::Tick(const _double & _dDeltaSeconds)
{
	if (nullptr != m_pMainView)
		m_pMainView->Tick(_dDeltaSeconds);
}

HRESULT CImGui_Manager::Render()
{      
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	/*  Start ImGui :: Rendering */
	if (nullptr != m_pMainView)
	{
		m_pMainView->Render();
		m_pMainView->Set_Open(m_bToolMode);
	}
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	return S_OK;
}

void CImGui_Manager::Add_DeleteImGui(CIMGUI* _pImGui)
{
	for (auto& iter : m_DeleteImGuiClass)
		if (iter == _pImGui)
			return;

	m_DeleteImGuiClass.push_back(_pImGui);
}

void CImGui_Manager::Delete_ImGui()
{
	for (auto& iter : m_DeleteImGuiClass)
	{
		Safe_Release(iter);
	}
	m_DeleteImGuiClass.clear();
}

void CImGui_Manager::Free()
{
	Safe_Release(m_pMainView);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
