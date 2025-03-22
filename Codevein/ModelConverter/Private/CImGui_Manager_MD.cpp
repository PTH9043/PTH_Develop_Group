#include"imgui.h"
#include"imgui_internal.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "CImGui_Manager_MD.h"
#include "CMainView_MD.h"
#include "CCamTool_MD.h"

IMPLEMENT_SINGLETON(CImGui_Manager_MD)

CImGui_Manager_MD::CImGui_Manager_MD()
{
}

HRESULT CImGui_Manager_MD::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
#ifdef _DEBUG
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	{
		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pContext = nullptr;
		_uint		iFlag = 0;
		iFlag = D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL			FreatureLV;
		/* 그래픽 장치를 초기화한다. */
		if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &pDevice, &FreatureLV, &pContext)))
			return E_FAIL;

		ImGui_ImplDX11_Init(pDevice, pContext);
		Safe_Release(pDevice);
		Safe_Release(pContext);
	}
#endif
    this->m_pDevice = _pDevice;
    this->m_pContext = _pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

	m_pMainView =  CMainView_MD::Create(m_pDevice, m_pContext);
	m_pCamTool = CCamTool_MD::Create(m_pDevice, m_pContext);
    return S_OK;
}

void CImGui_Manager_MD::Tick(const _double& _dDeltaSeconds)
{
	m_pMainView->Tick(_dDeltaSeconds);
	m_pCamTool->Tick(_dDeltaSeconds);
}

HRESULT CImGui_Manager_MD::Render()
{	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	m_pMainView->Render();
	m_pCamTool->Render();
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

void CImGui_Manager_MD::Free()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pMainView);
	Safe_Release(m_pCamTool);
    __super::Free();
}
