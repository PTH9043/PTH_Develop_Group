#include "ToolDefines.h"
#include "TCameraView.h"

TCameraView::TCameraView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "CameraView"),
	m_stMainDesc{},
	m_stCameraView{},
	m_isInitSetting{ false },
	m_dShowDeltaTime{ 0.0 }
{
}

void TCameraView::Free()
{
}

HRESULT TCameraView::NativeConstruct()
{
	m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });
	m_stCameraView = DOCKDESC("CameraViewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);
	return S_OK;
}

HRESULT TCameraView::LoadResource()
{
	return S_OK;
}

HRESULT TCameraView::ReleaseResource()
{
	return S_OK;
}

void TCameraView::TickActive(const _double& _dTimeDelta)
{
}

void TCameraView::LateTickActive(const _double& _dTimeDetla)
{
}

void TCameraView::RenderActive()
{
	ImGui::SetNextWindowPos(m_stMainDesc.vPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_stMainDesc.vSize, ImGuiCond_Appearing);
	// Name, OpenPointer, MainDesc 
	ImGui::Begin(GetName().c_str(), GetOpenPointer(), m_stMainDesc.imgWindowFlags);
	{
		m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
		DockBuildInitSetting();
		ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{}, m_stMainDesc.imgDockNodeFlags);
		CameraView();
	}
	ImGui::End();
}

void TCameraView::RenderMenu()
{
}

void TCameraView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
		m_stCameraView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 1.f, NULL, &dock_main_id);
		// DockBuild
		ImGui::DockBuilderDockWindow(m_stCameraView.strName.c_str(), m_stCameraView.iDockSpaceID);
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}

void TCameraView::CameraView()
{
	ImGui::Begin(m_stCameraView.strName.c_str(), GetOpenPointer(), m_stCameraView.imgWindowFlags);
	{
		ImGui::Text("CameraView");
	}
	ImGui::End();
}
