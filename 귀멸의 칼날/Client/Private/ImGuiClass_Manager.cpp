#include "ImGuiClass.h"
#include "ImGuiClass_Manager.h"
#include "GameInstance.h"
#include "MainView.h"

IMPLEMENT_SINGLETON(CImGuiClass_Manager)

CImGuiClass_Manager::CImGuiClass_Manager()
{
}

CImGuiClass* CImGuiClass_Manager::Get_ImGuiClass(const _uint& _iIndex)
{	
	// ImGui Class 숫자 넘어가면 리턴
	if (_iIndex >= m_iImGuiClassCount)
	{
#ifdef _DEBUG
		MSG_BOX("CImGuiClass_Manager::Insert Failed -> Result : 할당 할 수 있는 최대 숫자를 넘어갔음");
#endif
		return nullptr;
	}
	return m_ImGuiClassList[_iIndex];
}

HRESULT CImGuiClass_Manager::NativeConstruct(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext
	, const _uint& _iImGuiClassCount)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Font Load
	io.Fonts->AddFontFromFileTTF("..\\Bin\\Font\\NanumSquare_acB.ttf", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	io.BackendFlags = ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasMouseCursors 
		| ImGuiBackendFlags_HasMouseHoveredViewport;
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(_hWnd);
	// ImGui Class Render Load
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;

	_uint		iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	ImGui_ImplDX11_Init(_pDevice, _pContext);

	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef_DV(_pDevice);
	Safe_AddRef_DV(_pContext);

	m_iImGuiClassCount = _iImGuiClassCount;
	m_ImGuiClassList.resize(m_iImGuiClassCount);
	Insert_ImGuiClass(IMGCLASS_MAIN, CMainView::Create(m_pDevice, m_pContext));
	Safe_AddRef(m_ImGuiClassList[0]);

	CDebug_Manager* pInstance = GET_INSTANCE(CDebug_Manager);
	{
		m_pRefDebugActor = static_cast<CRefDebugActor*>(pInstance->Get_DebugActor(CDebug_Manager::DEBUG_REFACTOR));
		Safe_AddRef(m_pRefDebugActor);
	}
	RELEASE_INSTANCE(CDebug_Manager);

	return S_OK;
}

void CImGuiClass_Manager::Insert_ImGuiClass(const _uint& _iIndex, CImGuiClass* _pImGuiClass)
{
	if (nullptr == _pImGuiClass)
		return;

	// ImGui Class 숫자 넘어가면 리턴
	if (_iIndex >= m_iImGuiClassCount)
	{
#ifdef _DEBUG
		MSG_BOX("CImGuiClass_Manager::Insert Failed -> Result : 할당 할 수 있는 최대 숫자를 넘어갔음");
#endif
		return;
	}

	// 현재 ImGui Class가 비어있지 않았다면
	if (nullptr != m_ImGuiClassList[_iIndex])
	{
#ifdef _DEBUG
		MSG_BOX("CImGuiClass_Manager::Insert Failed -> Result : 현재 위치에 ImGui 클래스가 이미 있음");
#endif
		return;
	}

	if (Is_FindSame(_pImGuiClass))
	{
#ifdef _DEBUG
		MSG_BOX("CImGuiClass_Manager::Insert Failed -> Result : 같은 ImGui 클래스가 할당 되어져 있음");
#endif
		return;
	}

	m_ImGuiClassList[_iIndex] = _pImGuiClass;
}

void CImGuiClass_Manager::Tick(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (true == pGameInstance->Get_DIKeyDown(DIK_F1))
		{
			m_bImGui_Active = !m_bImGui_Active;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	if (true == m_bImGui_Active)
	{
		if (nullptr != m_ImGuiClassList[0])
			m_ImGuiClassList[0]->Tick(_dTimeDelta);
	}
	else
	{
#ifdef _DEBUG
		if (nullptr != m_pRefDebugActor)
			m_pRefDebugActor->Clear_Explain();
#endif
	}
}

HRESULT CImGuiClass_Manager::Late_Tick(const _double& _dTimeDelta)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	if (true == m_bImGui_Active)
	{
		if (nullptr != m_ImGuiClassList[0])
			m_ImGuiClassList[0]->Late_Tick(_dTimeDelta);
	}

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

HRESULT CImGuiClass_Manager::Render()
{
	if (true == m_bImGui_Active)
	{
		if (nullptr != m_ImGuiClassList[0])
			m_ImGuiClassList[0]->Render();
	}
	return S_OK;
}

_bool CImGuiClass_Manager::Is_FindSame(CImGuiClass* _pImGuiClass)
{
	const auto& iter = find_if(m_ImGuiClassList.begin(), m_ImGuiClassList.end(), [&](auto& pair)->bool
	{
		if (pair == _pImGuiClass)
			return true;

		return false;
	});

	if (m_ImGuiClassList.end() == iter)
		return false;

	return true;
}

void CImGuiClass_Manager::Free()
{
	for (auto& iter : m_ImGuiClassList)
		Safe_Release(iter);

	Safe_Release(m_ImGuiClassList[0]);

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pRefDebugActor);
}
