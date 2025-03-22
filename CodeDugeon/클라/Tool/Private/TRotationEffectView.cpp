#include "ToolDefines.h"
#include "TRotationEffectView.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UGameInstance.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"

TRotationEffectView::TRotationEffectView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "RotationEffectView"),
	m_stMainDesc{},
	m_stMultiParticleView{},
	m_isInitSetting{ false }
	, m_MultipleParticle{ nullptr }, m_MultipleParticleParam{ nullptr }, m_MultipleParticleType{ nullptr }
	, m_iMultipleParticleSize{ 0 }, m_iCurActiveMultipleParticle{ 0 }
{
}

void TRotationEffectView::Free()
{
}

HRESULT TRotationEffectView::NativeConstruct()
{
	m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });
	m_stMultiParticleView = DOCKDESC("Multiple Rotation Effect Viewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);

	return S_OK;
}

//================================================================
//**** 파티클 종류별로 관리하는 컨테이너(벡터) 크기 미리 할당 ****
//================================================================


void TRotationEffectView::ResizeMultipleParticleVector(_uint _resizeAmount)
{
	m_iMultipleParticleSize = _resizeAmount;
	m_MultipleParticle.resize(m_iMultipleParticleSize);
	m_MultipleParticleParam.resize(m_iMultipleParticleSize);
	m_MultipleParticleType.resize(m_iMultipleParticleSize);
}


//================================================================
//**************** 파티클 종류별로 리소스 할당 ****************
//================================================================

void TRotationEffectView::LoadMultipleParticleResource()
{
	// Default를 y값 증가하지않는 원 , AUTO를 y값 증가하는 원
	for (int i = 0; i < m_iMultipleParticleSize; i++) {
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		{
			UParticle::PARTICLEDESC tDesc;
			tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEROTATIONEFFECT2DSHADER;
			tDesc.wstrParticleShader = PROTO_RES_PARTICLEROTATION2DSHADER;


			tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
			//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 2.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 2.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 3.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.3f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 10.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 15.f;
			tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 102;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 5.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.1f, 0.f);
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_ORIGINAL;
			m_MultipleParticle[i] = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
		}
	}
	{
		m_MultipleParticleParam[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleParam();
		m_MultipleParticleType[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleTypeParam();
		m_MultipleParticleType[0]->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_MultipleParticleType[0]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_MultipleParticle[0]->SetTexture(L"RedDot"); // y값 증가 x 원
	}
	{
		m_MultipleParticleParam[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleParam();
		m_MultipleParticleType[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleTypeParam();
		m_MultipleParticleType[1]->fParticleType = PARTICLE_TYPE_AUTO;
		m_MultipleParticleType[1]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_MultipleParticle[1]->SetTexture(L"RedDot");// y값 증가 O 원
	}


	*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 0.05f;
	*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() = 1;
	*m_MultipleParticle[1]->GetParticleSystem()->GetCreateInterval() = 0.05f;
	*m_MultipleParticle[1]->GetParticleSystem()->GetAddParticleAmount() = 1;
	/*
for (int i = 0; i < m_iMultipleParticleSize; i++) {
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		{
			UParticle::PARTICLEDESC tDesc;
			tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEROTATIONEFFECT2DSHADER;
			tDesc.wstrParticleShader = PROTO_RES_PARTICLEROTATION2DSHADER;


			tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
			//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 15.f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 5.f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 3.5f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.3f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 10.f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 15.f ;
			tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 102 ;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 5.f ;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.1f, 0.f);
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_ORIGINAL;
			m_MultipleParticle[i] = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
		}
	}
		{
			m_MultipleParticleParam[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleParam();
			m_MultipleParticleType[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleTypeParam();
			m_MultipleParticleType[0]->fParticleType = PARTICLE_TYPE_DEFAULT;
			m_MultipleParticleType[0]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
			m_MultipleParticle[0]->SetTexture(L"RedDot"); // y값 증가 x 원
		}
		{
			m_MultipleParticleParam[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleParam();
			m_MultipleParticleType[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleTypeParam();
			m_MultipleParticleType[1]->fParticleType = PARTICLE_TYPE_AUTO;
			m_MultipleParticleType[1]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
			m_MultipleParticle[1]->SetTexture(L"RedDot");// y값 증가 O 원
		}


	*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 0.05f;
	*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() = 1;
	*m_MultipleParticle[1]->GetParticleSystem()->GetCreateInterval() = 0.05f;
	*m_MultipleParticle[1]->GetParticleSystem()->GetAddParticleAmount() = 1;
	*/
}


HRESULT TRotationEffectView::LoadResource()
{

	ResizeMultipleParticleVector(2);
	LoadMultipleParticleResource();

	return S_OK;
}

//================================================================
//**************** 파티클 종류별로 리소스 해제 ****************
//================================================================


void TRotationEffectView::ReleaseMultipleParticleResource()
{
	for (auto& _particle : m_MultipleParticle) {
		_particle.reset();
		GetGameInstance()->RemoveActor(_particle);
	}//문제 유발될 수 있는 코드 RemoveActor의 상세 구현을 살펴보아야함.
}


HRESULT TRotationEffectView::ReleaseResource()
{

	ReleaseMultipleParticleResource();


	return S_OK;
}


//================================================================
//************ 파티클 종류별로 Rendering 하는 부분 ************
//================================================================

void TRotationEffectView::TickActive(const _double& _dTimeDelta)
{
}

void TRotationEffectView::LateTickActive(const _double& _dTimeDetla)
{
}

void TRotationEffectView::RenderActive()
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

		MultipleParticleView();

	}
	ImGui::End();
}

void TRotationEffectView::RenderMenu()
{
}

void TRotationEffectView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
		m_stMultiParticleView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.5f, NULL, &dock_main_id);

		// DockBuild		
		ImGui::DockBuilderDockWindow(m_stMultiParticleView.strName.c_str(), m_stMultiParticleView.iDockSpaceID);
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}



//================================================================
//************ MULTIPLE PARTICLE TOOL VIEW SETTING ************
//================================================================

void TRotationEffectView::MultipleParticleView()
{
	ImGui::Begin(m_stMultiParticleView.strName.c_str(), GetOpenPointer(), m_stMultiParticleView.imgWindowFlags);
	{
		if (ImGui::CollapsingHeader("Multiple Particle Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Multiple ParticleView");
			_bool Show = false;

			if (true == ImGui::Button("Start Multiple Particle")) {
				//_uint increment = 1;
				/*ImGui::InputScalar("Enter Create Amount\n Min:0  Max :5", ImGuiDataType_U32, &m_iCurActiveMultipleParticle, &increment, &increment);
				if (m_iCurActiveMultipleParticle >= m_iMultipleParticleSize) m_iCurActiveMultipleParticle = m_iMultipleParticleSize;*/
				for (int i = 0; i < m_iMultipleParticleSize; i++) m_MultipleParticle[i]->SetActive(true);

			}

			if (true == ImGui::Button("Stop Multiple Particle"))
			{
				for (int i = 0; i < m_iMultipleParticleSize; i++)
					m_MultipleParticle[i]->SetActive(false);
			}

			MultipleParticleTexSetting();
			/*	MultipleParticleCountSetting();
				MultipleParticleTimeSetting();
				MultipleParticleTexSetting();
				DefaultMultipleParticleSetting();
				AutomaticMultipleParticleSetting();*/
		}
	}
	ImGui::End();
}

void TRotationEffectView::MultipleParticleCountSetting()
{
	//파티클 한 사이클 생성 갯수.
	if (ImGui::CollapsingHeader("Multiple Particle Count Setting", ImGuiTreeNodeFlags_DefaultOpen)) {

		_uint increment = 1;
		ImGui::InputScalar("Enter Create Amount\n Min:1  Max :1000", ImGuiDataType_U32, m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount(), &increment, &increment);
		if (*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() < 1) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() = 1;
		}
		else if (*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() > 1000) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() = 1000;
		}
	}

}

void TRotationEffectView::MultipleParticleTimeSetting()
{
	//파티클의 생성 시간 정하기
	if (ImGui::CollapsingHeader("Multiple Particle Create Time Interval", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::InputFloat("Enter Time Interval\n Min:0.f  Max :6.f", m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval(), 0.1f, 1.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		if (*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() <= 0) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 0.0;
		}
		else if (*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() > 6) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 6;
		}
	}
}

void TRotationEffectView::MultipleParticleTexSetting()
{

	ImGui::Text("Multiple Particle Texture Select");
	if (ImGui::BeginListBox("Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_MultipleParticle[0]->GetTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_MultipleParticle[0]->SetTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Text("Multiple2 Particle Texture Select");
	if (ImGui::BeginListBox("Texture2 List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_MultipleParticle[1]->GetTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_MultipleParticle[1]->SetTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}

void TRotationEffectView::DefaultMultipleParticleSetting()
{
	_bool Default = false;
	if (ImGui::Selectable("Set Multiple Particle Type : Default", Default)) {
		m_MultipleParticleType[0]->fParticleType = PARTICLE_TYPE_DEFAULT;
		ImGui::OpenPopup("Default Particle");
	}

	if (ImGui::BeginPopup("Default Particle")) {
		ImGui::SeparatorText("Particle Settings");
		ImGui::SeparatorText("Current Particle Type : Default");

		ImGui::SliderFloat("EndScale", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fEndScaleParticle, 1.f, 40.f, "%.2f");
		ImGui::SliderFloat("StartScale", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fStartScaleParticle, 1.f, 20.f, "%.2f");
		ImGui::SliderFloat("MaxLifeTime", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMaxLifeTime, 3.f, 10.f, "%.2f");
		ImGui::SliderFloat("MinLifeTime", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMinLifeTime, 0.f, 5.f, "%.2f");
		ImGui::SliderFloat("MaxSpeed", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMaxSpeed, 10.f, 100.f, "%.2f");
		ImGui::SliderFloat("MinSpeed", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMinSpeed, 10.f, 100.f, "%.2f");
		ImGui::SliderFloat("DirectionX", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fParticleDirection.x, -1.f, 1.f, "%.2f");
		ImGui::SliderFloat("DirectionY", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fParticleDirection.y, -1.f, 1.f, "%.2f");
		ImGui::SliderFloat("DirectionZ", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fParticleDirection.z, -1.f, 1.f, "%.2f");
		ImGui::SliderFloat("Thickness", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fParticleThickness, 0.f, 50.f, "%.2f");
		ImGui::EndPopup();
	}
}

void TRotationEffectView::AutomaticMultipleParticleSetting()
{
	_bool Automatic = false;
	if (ImGui::Selectable("Set Multiple Particle Type : Automatic", Automatic)) {
		m_MultipleParticleType[0]->fParticleType = PARTICLE_TYPE_AUTO;
		ImGui::OpenPopup("Automatic Particle");
	}



	if (ImGui::BeginPopup("Automatic Particle")) {
		ImGui::SeparatorText("Particle Settings");
		ImGui::SeparatorText("Current Particle Type : Automatic");

		ImGui::SliderFloat("EndScale", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fEndScaleParticle, 1.f, 40.f, "%.2f");
		ImGui::SliderFloat("StartScale", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fStartScaleParticle, 1.f, 20.f, "%.2f");
		ImGui::SliderFloat("MaxSpeed", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMaxSpeed, 10.f, 100.f, "%.2f");
		ImGui::SliderFloat("MinSpeed", &m_MultipleParticleParam[0]->stGlobalParticleInfo.fMinSpeed, 10.f, 100.f, "%.2f");


		ImGui::EndPopup();
	}
}



