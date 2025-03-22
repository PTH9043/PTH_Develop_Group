#include "ToolDefines.h"
#include "TImGuiManager.h"
#include "UDevice.h"
#include "UGpuCommand.h"
#include "UTableDescriptor.h"
#include "TImGuiView.h"
#include "TMainView.h"
#include "TModelView.h"
//#include "TSceneView.h"
//#include "URenderCommand.h"
#include "UGameInstance.h"
#include "TMainScene.h"
#include "TAnimControlView.h"
#include "TCameraView.h"
#include "TParticleView.h"
#include "TRotationEffectView.h"
#include "TBloodEffectView.h"
#include "TFireView.h"
#include "TFogView.h"
#include "TNavigationView.h"
#include "TMapView.h"


IMPLEMENT_SINGLETON(TImGuiManager)


TImGuiManager::TImGuiManager() :
	m_ImGuiObjectContainer{},
	m_spMainView{ nullptr },
	m_spDevice{ nullptr },
	m_spGpuCommand{ nullptr },
	m_spTableDesciptor{ nullptr }
{
}

void TImGuiManager::Free()
{
	m_ImGuiObjectContainer.clear();
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

HRESULT TImGuiManager::ReadyManager(const GRAPHICDESC& _stGraphicDesc, const OUTPUTDATA& _stOutputData)
{
	m_spDevice = _stOutputData.wpDevice.lock();
	m_spGpuCommand = static_pointer_cast<UGpuCommand>(_stOutputData.wpGpuCmd.lock());
	ImGuiContext* pContext = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("..\\..\\Resource\\Font\\NanumSquare_acB.ttf", 15, NULL, io.Fonts->GetGlyphRangesKorean());
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	m_spTableDesciptor = CreateNative<UTableDescriptor>(m_spDevice, 256);
	ComPtr<Dx12DescriptorHeap> cpDescriptor = m_spTableDesciptor->GetDescriptorHeap();
	{
		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(_stGraphicDesc.hWnd);
		ImGui_ImplDX12_Init(m_spDevice->GetDV().Get(), SWAP_CHAIN_END,
			DXGI_FORMAT_R8G8B8A8_UNORM, cpDescriptor.Get(),
			cpDescriptor->GetCPUDescriptorHandleForHeapStart(),
			cpDescriptor->GetGPUDescriptorHandleForHeapStart());
	}
	ImGui_ImplDX12_CreateDeviceObjects();

	RETURN_CHECK_FAILED(ReadyImGuiClass(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->SetImGuiContext(pContext);
	return S_OK;
}

SHPTR<TImGuiView> TImGuiManager::GetImGuiObject(const IMGTAG _eImGuiTag)
{
	const IMGOBJECTS::iterator& it = m_ImGuiObjectContainer.find(_eImGuiTag);
	RETURN_CHECK(it == m_ImGuiObjectContainer.end(), nullptr);
	return it->second;
}

void TImGuiManager::Tick(const _double& _dDeltaTime)
{
	m_spMainView->Tick(_dDeltaTime);
}

void TImGuiManager::LateTick(const _double& _dDeltaTime)
{
	m_spMainView->LateTick(_dDeltaTime);

	
	if (true == m_isResetScene)
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		spGameInstance->RegisterScene(CreateConstructorNative<TMainScene>(m_spDevice));
		m_isResetScene = false;
	}
}

void TImGuiManager::Render()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_spMainView->Render();

	// Rendering
	ImGui::Render();

	m_spTableDesciptor->BindAndClear(m_spGpuCommand);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_spGpuCommand->GetGpuCmdList().Get());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(NULL, (void*)m_spGpuCommand->GetGpuCmdList().Get());
	}
}

HRESULT TImGuiManager::ReadyImGuiClass()
{
	{
		m_spMainView = CreateConstructorNative<TMainView>(m_spDevice);
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::MAIN, m_spMainView));
	}
	{
		SHPTR<TModelView> spModelConverter = CreateConstructorNative<TModelView>(m_spDevice);
		spModelConverter->CloseImGui();
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::MODELCONVERTER, spModelConverter));
		m_spMainView->InsertImGuiView(spModelConverter);
	}
	{
		SHPTR<TAnimControlView> spAnimControlView = CreateConstructorNative<TAnimControlView>(m_spDevice);
		spAnimControlView->CloseImGui();
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::ANIMCONTROLVIEW, spAnimControlView));
		m_spMainView->InsertImGuiView(spAnimControlView);
	}
	{
		SHPTR<TCameraView> spCameraView = CreateConstructorNative<TCameraView>(m_spDevice);
		spCameraView->CloseImGui();
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::CAMERATOOL, spCameraView));
		m_spMainView->InsertImGuiView(spCameraView);
	}
	{

		SHPTR<TParticleView> spParticleView = CreateConstructorNative<TParticleView>(m_spDevice);
		spParticleView->CloseImGui();
		m_ImGuiObjectContainer.insert(std::make_pair(IMGTAG::PARTICLETOOL, spParticleView));
		m_spMainView->InsertImGuiView(spParticleView);
	}
	{

		SHPTR<TRotationEffectView> spRotationEffectView = CreateConstructorNative<TRotationEffectView>(m_spDevice);
		spRotationEffectView->CloseImGui();
		m_ImGuiObjectContainer.insert(std::make_pair(IMGTAG::ROTATIONEFFECTTOOL, spRotationEffectView));
		m_spMainView->InsertImGuiView(spRotationEffectView);
	}
	{

		SHPTR<TBloodEffectView> spRotationEffectView = CreateConstructorNative<TBloodEffectView>(m_spDevice);
		spRotationEffectView->CloseImGui();
		m_ImGuiObjectContainer.insert(std::make_pair(IMGTAG::BLOODEFFECTTOOL, spRotationEffectView));
		m_spMainView->InsertImGuiView(spRotationEffectView);
	}

	{

		SHPTR<TFireView> spFireView = CreateConstructorNative<TFireView>(m_spDevice);
		spFireView->CloseImGui();
		m_ImGuiObjectContainer.insert(std::make_pair(IMGTAG::FIRETOOL, spFireView));
		m_spMainView->InsertImGuiView(spFireView);
	}
	{

		SHPTR<TFogView> spFogView = CreateConstructorNative<TFogView>(m_spDevice);
		spFogView->CloseImGui();
		m_ImGuiObjectContainer.insert(std::make_pair(IMGTAG::FOGTOOL, spFogView));
		m_spMainView->InsertImGuiView(spFogView);
	}
	{

		SHPTR<TNavigationView> spNavigationView = CreateConstructorNative<TNavigationView>(m_spDevice);
		spNavigationView->CloseImGui();
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::NAVITOOL, spNavigationView));
		m_spMainView->InsertImGuiView(spNavigationView);


	}
	{
		SHPTR<TMapView> spMapView = CreateConstructorNative<TMapView>(m_spDevice);
		spMapView->CloseImGui();
		m_ImGuiObjectContainer.insert(MakePair(IMGTAG::MAPTOOL, spMapView));
		m_spMainView->InsertImGuiView(spMapView);
	}
	return S_OK;
}
