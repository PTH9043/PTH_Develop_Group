#include "ToolDefines.h"
#include "TFogView.h"
#include "UGameInstance.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"
#include "URenderTargetGroup.h"
#include "UTransform.h"
#include "TMainScene.h"
#include "UGrid.h"
#include "UFog.h"

TFogView::TFogView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "FogView"),
	m_stMainDesc{},
	m_stFogView{},
	m_isInitSetting{ false }

{
}

void TFogView::Free()
{
}

HRESULT TFogView::NativeConstruct()
{
	m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });
	m_stFogView = DOCKDESC("Fog Viewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);

	return S_OK;
}





HRESULT TFogView::LoadResource()
{
	

	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		UFog::FOGDESC tFogDesc;
		tFogDesc.wstrFogShader = PROTO_RES_2DFOGSHADER;
		m_stFog = std::static_pointer_cast<UFog>(spGameInstance->CloneActorAdd(PROTO_ACTOR_FOG, { &tFogDesc }));
		
	}

	return S_OK;
}

//================================================================
//**************** 파티클 종류별로 리소스 해제 ****************
//================================================================




HRESULT TFogView::ReleaseResource()
{



	return S_OK;
}


//================================================================
//************ 파티클 종류별로 Rendering 하는 부분 ************
//================================================================

void TFogView::TickActive(const _double& _dTimeDelta)
{
}

void TFogView::LateTickActive(const _double& _dTimeDetla)
{
}

void TFogView::RenderActive()
{
	ImGui::SetNextWindowPos(m_stMainDesc.vPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_stMainDesc.vSize, ImGuiCond_Appearing);
	// Name, OpenPointer, MainDesc 

	ImGui::Begin(GetName().c_str(), GetOpenPointer(), m_stMainDesc.imgWindowFlags);
	{
		m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
		DockBuildInitSetting();
		ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{}, m_stMainDesc.imgDockNodeFlags);
		//여기부터 내 입력

		FogView();
		
		
	}
	ImGui::End();

		
}

void TFogView::RenderMenu()
{
}

void TFogView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
		m_stFogView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.5f, NULL, &dock_main_id);

		// DockBuild		
		ImGui::DockBuilderDockWindow(m_stFogView.strName.c_str(), m_stFogView.iDockSpaceID);
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}



//================================================================
//************ MULTIPLE PARTICLE TOOL VIEW SETTING ************
//================================================================

void TFogView::FogView()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	ImGui::Begin(m_stFogView.strName.c_str(), GetOpenPointer(), m_stFogView.imgWindowFlags);
	{
		if (ImGui::CollapsingHeader("Fog Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Fog View");


			if (true == ImGui::Button("Render Cube")) {


				m_stFog->SetActive(true);

			}

			if (true == ImGui::Button("Stop Rendering Cube"))
			{

				m_stFog->SetActive(false);
			}
			if (true == ImGui::Button("Turn Grid On")) {
				static_pointer_cast<TMainScene>(spGameInstance->GetCurScene())->GetGrid()->SetActive(true);
			}
			if (true == ImGui::Button("Turn Grid Off")) {
				static_pointer_cast<TMainScene>(spGameInstance->GetCurScene())->GetGrid()->SetActive(false);
			}
			FogColorTextureSetting();
			FogScalingSetting();
			FogPosSetting();
			FogDensitySetting();
			FogStartAndEndSetting();
			FogTypeSetting();
			SetRTBackColorAndFogColor();
		}
	}
	ImGui::End();
}

void TFogView::FogColorTextureSetting() {
	ImGui::Text("Fog Color Texture Select");
	if (ImGui::BeginListBox("Fog Color Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stFog->GetColorTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stFog->SetColorTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}



void TFogView::FogScalingSetting() {

	static float Scale = 20.f;
	ImGui::SliderFloat("Scale", &Scale, 1.f, 100.f, "%.2f");

	_float3 ScaleFloat3 = _float3(Scale, Scale, Scale);
	m_stFog->GetTransform()->SetScale(ScaleFloat3);
}

void TFogView::FogPosSetting() {


	static _float posX = 0;
	static _float posY = 0;
	static _float posZ = 0;

	ImGui::InputFloat("Input Fog Position X", &posX);
	ImGui::InputFloat("Input Fog Position Y", &posY);
	ImGui::InputFloat("Input Fog Position Z", &posZ);

	m_stFog->GetTransform()->SetPos(_float3(posX, posY, posZ));
}

void TFogView::FogDensitySetting()
{
	FOGBUFFER* FogBuf = m_stFog->GetFogBuffer();
	ImGui::SliderFloat("FogDensity", &FogBuf->fFogDensity, 0.f, 1.f, "%.02f");
	
}

void TFogView::FogStartAndEndSetting()
{
	FOGBUFFER* FogBuf = m_stFog->GetFogBuffer();
	
		ImGui::InputFloat("Input Fog Start (z)", &FogBuf->fFogStart);
		ImGui::InputFloat("Input Fog End (z)", &FogBuf->fFogEnd);
}

void TFogView::FogTypeSetting()
{
	FOGBUFFER* FogBuf = m_stFog->GetFogBuffer();

	ImGui::Text("Fog Interpolation Type Select");
	if (true == ImGui::Button("Linear Interpolation")) {
		FogBuf->fType = 0;
	}
	if (true == ImGui::Button("Exponential Interpolation")) {
		FogBuf->fType = 1;
	}
	if (true == ImGui::Button("Exponential Squared Interpolation")) {
		FogBuf->fType = 2;
	}
}

void TFogView::SetRTBackColorAndFogColor()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<URenderTargetGroup> RTGroup = spGameInstance->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED);
	FOGBUFFER* FogBuf = m_stFog->GetFogBuffer();


	_bool Default = false;
	if (ImGui::Selectable("Setting RT BackColor And FogColor", Default)) {
	
		ImGui::OpenPopup("Set Color");
	}

	if (ImGui::BeginPopup("Set Color")) {
		

		ImGui::Text("Set RT And Fog Color");
		static _float ColR = 0;
		static _float ColG = 0;
		static _float ColB = 0;

		ImGui::InputFloat("Input RT and FogColor R", &ColR);
		ImGui::InputFloat("Input RT and FogColor G", &ColG);
		ImGui::InputFloat("Input RT and FogColor B", &ColB);

		RTGroup->SetRenderTargetColor(RTOBJID::NONALPHA_DIFFUSE_DEFFERED, _float4(ColR, ColG, ColB, 0.f));

		FogBuf->fFogColor = _float4(ColR, ColG, ColB, 0.f);
		ImGui::EndPopup();
	}
	
	
	
	

}
