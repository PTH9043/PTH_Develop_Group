#include "ToolDefines.h"
#include "TToolApp.h"
#include "UGameInstance.h"
#include "UTimer.h"
#include "TImGuiManager.h"
#include "TMainCamera.h"
#include "TMainScene.h"
#include "TProtoMaker.h"

TToolApp::TToolApp() :
	m_iTickCount{ 0 },
	m_hwnd{ NULL },
	m_dRenderTick{ 0.0 },
	m_spGameInstance{ nullptr },
	m_spImGuiManager{ nullptr },
	m_spTickTimer{ nullptr },
	m_spDeltaTimer{ nullptr },
	m_spRenderTimer{ nullptr },
	m_dDeltaTime{ 0 },
	TICK_DOUBLE{ 1 / 180.f },
	m_dShowTickFPS{ 0.0 },
	DELTA_TIMER{ L"DELTA_TIMER" },
	TICK_TIMER{ L"TICK_TIMER" },
	RENDER_TIMER{ L"RENDER_TIMER" }
{
}

void TToolApp::Free()
{
	m_spImGuiManager.reset();
	TImGuiManager::DestroyInstance();
}

HRESULT TToolApp::NativeConstruct(const HWND& _hWnd, const HINSTANCE& _hInst)
{
	m_spGameInstance = GET_INSTANCE(UGameInstance);
	m_spImGuiManager = GET_INSTANCE(TImGuiManager);
	/* Graphic Desc */
	GRAPHICDESC	stGraphicDesc;
	{
		stGraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
		stGraphicDesc.hInst = _hInst;
		stGraphicDesc.hWnd = _hWnd;
		stGraphicDesc.iWinCX = WINDOW_WIDTH;
		stGraphicDesc.iWinCY = WINDOW_HEIGHT;

		m_hwnd = _hWnd;
	}
	OUTPUTDATA stOutputData;
	RETURN_CHECK_FAILED(m_spGameInstance->ReadyInstance(stGraphicDesc, stOutputData), E_FAIL);
	RETURN_CHECK_FAILED(m_spGameInstance->LoadFirstFolder(FIRST_RESOURCE_FOLDER), E_FAIL);

	TProtoMaker::CreateProtoData(m_spGameInstance, stOutputData.wpDevice.lock(), stOutputData.wpGpuCmd.lock());

	// Register 
	m_spGameInstance->RegisterScene(CreateConstructorNative<TMainScene>(stOutputData.wpDevice.lock()));
	// ReadyManager
	RETURN_CHECK_FAILED(m_spImGuiManager->ReadyManager(stGraphicDesc, stOutputData), E_FAIL);


	m_spDeltaTimer = m_spGameInstance->CreateTimerAdd(DELTA_TIMER);
	m_spTickTimer = m_spGameInstance->CreateTimerAdd(TICK_TIMER);
	m_spRenderTimer = m_spGameInstance->CreateTimerAdd(RENDER_TIMER);

	//m_spGameInstance->SoundPlay(L"background_1");
	//m_spGameInstance->SetCurrentContext(ImGui::GetCurrentContext());
	return S_OK;
}



void TToolApp::KeyUpdate(WPARAM _wParam)
{
	m_spGameInstance->OtherFrame(m_dDeltaTime, _wParam);
}


void TToolApp::Tick()
{
	m_spDeltaTimer->Tick();

	m_dDeltaTime += m_spDeltaTimer->GetDeltaTime();
	if (m_dDeltaTime >= TICK_DOUBLE)
	{

		m_spTickTimer->Tick();
		m_dDeltaTime = m_spTickTimer->GetDeltaTime();

		++m_iTickCount;
		m_spGameInstance->AwakeTick();
		// Tick 
		m_spGameInstance->Tick(m_dDeltaTime);
		m_spImGuiManager->Tick(m_dDeltaTime);
		// LateTick
		m_spGameInstance->LateTick(m_dDeltaTime);
		m_spImGuiManager->LateTick(m_dDeltaTime);

		m_spGameInstance->RenderBegin();
		m_spImGuiManager->Render();
		m_spGameInstance->RenderEnd();
		m_dDeltaTime = 0;
	}
}