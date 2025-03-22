#include "ToolDefines.h"
#include "TImGuiView.h"
#include "UGameInstance.h"
#include "TImGuiManager.h"

TImGuiView::TImGuiView(CSHPTRREF<UDevice> _spDevice, const _string& _strName)
	: UObject(_spDevice),
	// Tick, LateTick, Render
	m_pTickFunc{ &TImGuiView::TickActive },
	m_pLateTickFunc{ &TImGuiView::LateTickActive },
	m_pRenderFunc{ &TImGuiView::RenderActive },
	m_strName{ _strName },
	m_spGameInstance{ UGameInstance::GetInstance() },
	m_isOpen{ false },
	m_isActive{ false }
{
}

void TImGuiView::Free()
{
}

void TImGuiView::OpenImGui()
{
	RETURN_CHECK(true == m_isActive, ;);
	m_pTickFunc = &TImGuiView::TickActive;
	m_pLateTickFunc = &TImGuiView::LateTickActive;
	m_pRenderFunc = &TImGuiView::RenderActive;
	LoadResource();
	m_isActive = true;
	m_isOpen = true;
}

void TImGuiView::CloseImGui()
{
	RETURN_CHECK(false == m_isActive, ;);

	m_pTickFunc = &TImGuiView::TickNonActive;
	m_pLateTickFunc = &TImGuiView::LateTickNonActive;
	m_pRenderFunc = &TImGuiView::RenderNonActive;
	ReleaseResource();
	m_isActive = false;
	m_isOpen = false;
}

HRESULT TImGuiView::LoadResource()
{
	return S_OK;

}

HRESULT TImGuiView::ReleaseResource()
{
	return S_OK;
}

void TImGuiView::TickActive(const _double& _dTimeDelta)
{
}

void TImGuiView::LateTickActive(const _double& _dTimeDetla)
{
}

void TImGuiView::RenderActive()
{
}


void TImGuiView::ActiveResetSceneData()
{
	SHPTR<TImGuiManager> pImGuiManager = GET_INSTANCE(TImGuiManager);
	pImGuiManager->SetResetScene(true);
}
