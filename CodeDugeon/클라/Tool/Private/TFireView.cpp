#include "ToolDefines.h"
#include "TFireView.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UGameInstance.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"
#include "UTransform.h"
#include "UFire.h"
#include "UGuard.h"
#include "UBlood.h"
#include "UDust.h"
#include "UMat.h"


TFireView::TFireView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "FireView"),
	m_stMainDesc{},
	m_stFireView{},
	m_stMultiParticleView{},
	m_isInitSetting{ false }, m_MultipleParticle{ nullptr }, m_MultipleParticleParam{ nullptr }, m_MultipleParticleType{ nullptr }
	, m_iMultipleParticleSize{ 0 }, m_iCurActiveMultipleParticle{ 0 }

{
}

void TFireView::Free()
{
}

HRESULT TFireView::NativeConstruct()
{
	m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });
	m_stFireView = DOCKDESC("Fire Viewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);
	m_stMultiParticleView = DOCKDESC("Emit Effect Viewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);
	return S_OK;
}
void TFireView::ResizeMultipleParticleVector(_uint _resizeAmount)
{
	m_iMultipleParticleSize = _resizeAmount;
	m_MultipleParticle.resize(m_iMultipleParticleSize);
	m_MultipleParticleParam.resize(m_iMultipleParticleSize);
	m_MultipleParticleType.resize(m_iMultipleParticleSize);
}

void TFireView::LoadMultipleParticleResource()
{
	// Default를 y값 증가하지않는 원 , AUTO를 y값 증가하는 원
	for (int i = 0; i < m_iMultipleParticleSize; i++) {
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		{
			UParticle::PARTICLEDESC tDesc;
			tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEEMITPARTICLE2DSHADER;
			tDesc.wstrParticleShader = PROTO_RES_PARTICLEFLARE2DSHADER;


			tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
			//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 0.25f;
			tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.6f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 1.f;
			tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 0.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 200;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 0.5f;
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.1f, 0.f);
			tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_FLARE;
			m_MultipleParticle[i] = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
		}
	}
	{
		m_MultipleParticleParam[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleParam();
		m_MultipleParticleType[0] = m_MultipleParticle[0]->GetParticleSystem()->GetParticleTypeParam();
		m_MultipleParticleType[0]->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_MultipleParticleType[0]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_MultipleParticle[0]->SetTexture(L"Particle"); // y값 증가 x 원
	}
	{
		//m_MultipleParticleParam[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleParam();
		//m_MultipleParticleType[1] = m_MultipleParticle[1]->GetParticleSystem()->GetParticleTypeParam();
		//m_MultipleParticleType[1]->fParticleType = PARTICLE_TYPE_AUTO;
		//m_MultipleParticleType[1]->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		//m_MultipleParticle[1]->SetTexture(L"RedBubble");// y값 증가 O 원
	}


	*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 0.15f;
	*m_MultipleParticle[0]->GetParticleSystem()->GetAddParticleAmount() = 1;
	m_MultipleParticle[0]->SetParticleType(PARTICLE_FLARE);
	
	//*m_MultipleParticle[1]->GetParticleSystem()->GetCreateInterval() = 0.05f;
	//*m_MultipleParticle[1]->GetParticleSystem()->GetAddParticleAmount() = 1;
	//m_MultipleParticle[1]->GetParticleSystem()->SetUAVBUFFERPLUS(true);
}




HRESULT TFireView::LoadResource()
{
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		UFire::FIREDESC tFireDesc;
		tFireDesc.wstrFireShader = PROTO_RES_2DFIRESHADER;
		m_stFire = std::static_pointer_cast<UFire>(spGameInstance->CloneActorAdd(PROTO_ACTOR_FIRE, { &tFireDesc }));
	}
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_stGuard = std::static_pointer_cast<UGuard>(spGameInstance->CloneActorAdd(PROTO_ACTOR_GUARD));

	
	m_stBlood = std::static_pointer_cast<UBlood>(spGameInstance->CloneActorAdd(PROTO_ACTOR_BLOOD));
	m_stDust = std::static_pointer_cast<UDust>(spGameInstance->CloneActorAdd(PROTO_ACTOR_DUST));
	m_stMat = std::static_pointer_cast<UMat>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAT));
	m_stGuard->GetTransform()->SetPos(_float3(0, 0, 0));

	ResizeMultipleParticleVector(1);
	LoadMultipleParticleResource();
	m_stFireNoiseBuffer = m_stFire->GetFireNoiseBuffer();
	m_stFireDistortionBuffer = m_stFire->GetFireDistortionBuffer();
	return S_OK;
}

//================================================================
//**************** 파티클 종류별로 리소스 해제 ****************
//================================================================

void TFireView::ReleaseMultipleParticleResource()
{
	for (auto& _particle : m_MultipleParticle) {
		_particle.reset();
		GetGameInstance()->RemoveActor(_particle);
	}//문제 유발될 수 있는 코드 RemoveActor의 상세 구현을 살펴보아야함.
}



HRESULT TFireView::ReleaseResource()
{

	ReleaseMultipleParticleResource();


	return S_OK;
}


//================================================================
//************ 파티클 종류별로 Rendering 하는 부분 ************
//================================================================

void TFireView::TickActive(const _double& _dTimeDelta)
{
}

void TFireView::LateTickActive(const _double& _dTimeDetla)
{
}

void TFireView::RenderActive()
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

		FireView();
		MultipleParticleView();
	}
	ImGui::End();
}

void TFireView::RenderMenu()
{
}

void TFireView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
		m_stFireView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.5f, NULL, &dock_main_id);
		m_stMultiParticleView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.f, NULL, &dock_main_id);
		// DockBuild		
		ImGui::DockBuilderDockWindow(m_stFireView.strName.c_str(), m_stFireView.iDockSpaceID);
		ImGui::DockBuilderDockWindow(m_stMultiParticleView.strName.c_str(), m_stMultiParticleView.iDockSpaceID);

		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}



//================================================================
//************ MULTIPLE PARTICLE TOOL VIEW SETTING ************
//================================================================
void TFireView::MultipleParticleView()
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
			MultipleParticleCountSetting();
			MultipleParticleTimeSetting();

			DefaultMultipleParticleSetting();
			AutomaticMultipleParticleSetting();
		}
	}
	ImGui::End();
}


void TFireView::MultipleParticleCountSetting()
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



void TFireView::MultipleParticleTimeSetting()
{
	//파티클의 생성 시간 정하기
	if (ImGui::CollapsingHeader("Multiple Particle Create Time Interval", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::InputFloat("Enter Time Interval\n Min:0.f  Max :6.f", m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval(), 0.1f, 1.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		if (*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() <= 0) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 0.1f;
		}
		else if (*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() > 6) {
			*m_MultipleParticle[0]->GetParticleSystem()->GetCreateInterval() = 6;
		}
	}
}



void TFireView::DefaultMultipleParticleSetting()
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

void TFireView::AutomaticMultipleParticleSetting()
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







void TFireView::MultipleParticleTexSetting()
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
}






















//-----------------
void TFireView::FireView()
{
	ImGui::Begin(m_stFireView.strName.c_str(), GetOpenPointer(), m_stFireView.imgWindowFlags);
	{
		if (ImGui::CollapsingHeader("Fire Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Fire View");


			if (true == ImGui::Button("Start Fire")) {


				m_stFire->SetActive(true);
				m_stGuard->SetActive(true);
				m_stBlood->SetActive(true);
				m_stBlood->SetTimer(1.75f);
			
				m_stDust->SetActive(true);
				m_stDust->SetTimer(2.f);
				m_stDust->GetTransform()->SetPos(_float3(-196, -79, 132));

				m_stMat->SetActive(true);
				m_stMat->SetTimer(50.f);
				m_stMat->GetTransform()->SetPos(_float3(0, 0, 0));

				_float3 firepos = m_stFire->GetTransform()->GetPos();
				m_MultipleParticle[0]->GetTransform()->SetPos(_float3(firepos.x, firepos.y-3.f, firepos.z));
			}

			if (true == ImGui::Button("Stop Fire"))
			{

				m_stFire->SetActive(false);
				m_stGuard->SetActive(false);
				m_stBlood->SetActive(false);
				m_stDust->SetActive(false);
				m_stMat->SetActive(false);
			}

			FireColorTextureSetting();
			FireNoiseTextureSetting();
			FireAlphaTextureSetting();
			
			TextureSetting();
			
			FireScalingSetting();
			FirePosSetting();
			FireDistortionSetting();
		}
	}
	ImGui::End();
}

void TFireView::FireColorTextureSetting() {
	ImGui::Text("Fire Color Texture Select");
	if (ImGui::BeginListBox("Fire Color Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stFire->GetColorTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stFire->SetColorTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}

void TFireView::TextureSetting()
{
	ImGui::Text("Guard Color Texture Select");
	if (ImGui::BeginListBox("Guard Noise Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stGuard->GetTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stGuard->SetColorTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Text("Mat Color Texture Select");
	if (ImGui::BeginListBox("Mat Noise Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stMat->GetTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stMat->SetColorTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}


void TFireView::FireNoiseTextureSetting() {
	ImGui::Text("Fire Noise Texture Select");
	if (ImGui::BeginListBox("Fire Noise Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stFire->GetNoiseTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stFire->SetNoiseTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}

void TFireView::FireAlphaTextureSetting() {
	ImGui::Text("Fire Alpha Texture Select");
	if (ImGui::BeginListBox("Fire Alpha Texture List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{

		using TEXNAMES = UNORMAP<_wstring, _uint>;
		TEXNAMES m_TextureNames = m_stFire->GetAlphaTextureGroup()->GetTextureNames();
		for (auto& Texture : m_TextureNames)
		{
			if (ImGui::Selectable(UMethod::ConvertWToS(Texture.first)))
			{
				m_stFire->SetAlphaTexture(Texture.second);
			}
		}
		ImGui::EndListBox();
	}
}



void TFireView::FireScalingSetting() {

	static float ScaleX = 3.f;
	static float ScaleY = 6.65f;
	//static float ScaleX = 50.f;
	//static float ScaleY = 50.f;
	ImGui::SliderFloat("ScaleX", &ScaleX, 1.f, 300, "%.2f");
	ImGui::SliderFloat("ScaleY", &ScaleY, 1.f, 300, "%.2f");

	_float3 ScaleFloat3 = _float3(ScaleX, ScaleY, 1);
	m_stFire->GetTransform()->SetScale(ScaleFloat3);
}

void TFireView::FirePosSetting() {

	//static _float posX = -555.2f;
	//static _float posY = -32.310f;
	//static _float posZ = 149.3f;
	static _float posX = 0.f;
	static _float posY = 0.f;
	static _float posZ = 0.f;

	ImGui::InputFloat("Input Fire Position X", &posX);
	ImGui::InputFloat("Input Fire Position Y", &posY);
	ImGui::InputFloat("Input Fire Position Z", &posZ);

	m_stFire->GetTransform()->SetPos(_float3(posX, posY, posZ));
}



void TFireView::FireDistortionSetting() {

	static _float fDistortion1X = 0.1;
	static _float fDistortion1Y = 0.2f;

	ImGui::InputFloat("Input Fire Distortion1.X", &fDistortion1X);
	ImGui::InputFloat("IInput Fire Distortion1.Y", &fDistortion1Y);
	m_stFireDistortionBuffer->fDistortion1.x = fDistortion1X;
	m_stFireDistortionBuffer->fDistortion1.y = fDistortion1Y;

	static _float fDistortion2X = 0.1f;
	static _float fDistortion2Y = 0.3f;

	ImGui::InputFloat("Input Fire Distortion2.X", &fDistortion2X);
	ImGui::InputFloat("IInput Fire Distortion2.Y", &fDistortion2Y);

	m_stFireDistortionBuffer->fDistortion2.x = fDistortion2X;
	m_stFireDistortionBuffer->fDistortion2.y = fDistortion2Y;

	static _float fDistortion3X = 0.1f;
	static _float fDistortion3Y = 0.1f;

	ImGui::InputFloat("Input Fire Distortion3.X", &fDistortion3X);
	ImGui::InputFloat("IInput Fire Distortion3.Y", &fDistortion3Y);

	m_stFireDistortionBuffer->fDistortion3.x = fDistortion3X;
	m_stFireDistortionBuffer->fDistortion3.y = fDistortion3Y;

	static _float fDistortionScale = 0.8f;
	static _float fDistortionBias = 0.5f;

	ImGui::InputFloat("Input Fire Distortion Scale", &m_stFireDistortionBuffer->fDistortionScale);
	ImGui::InputFloat("Input Fire Distortion Bias", &m_stFireDistortionBuffer->fDistortionBias);

	m_stFireDistortionBuffer->fDistortionScale = fDistortionScale;
	m_stFireDistortionBuffer->fDistortionBias = fDistortionBias;

	static _float fScrollSpeedsx = 1.3;
	static _float fScrollSpeedsy = 2.1;
	static _float fScrollSpeedsz = 2.3;

	ImGui::InputFloat("Input Fire ScrollSpeeds.x", &fScrollSpeedsx);
	ImGui::InputFloat("IInput Fire ScrollSpeeds.y", &fScrollSpeedsy);
	ImGui::InputFloat("IInput Fire ScrollSpeeds.z", &fScrollSpeedsz);

	m_stFireNoiseBuffer->fScrollSpeeds.x = fScrollSpeedsx;
	m_stFireNoiseBuffer->fScrollSpeeds.y = fScrollSpeedsy;
	m_stFireNoiseBuffer->fScrollSpeeds.z = fScrollSpeedsz;

	static _float fScalesx = 1;
	static _float fScalesy = 2;
	static _float fScalesz = 3;

	ImGui::InputFloat("IInput Fire Scales.x", &fScalesx);
	ImGui::InputFloat("IInput Fire Scales.y", &fScalesy);
	ImGui::InputFloat("IInput Fire Scales.z", &fScalesz);
	m_stFireNoiseBuffer->fScales.x = fScalesx;
	m_stFireNoiseBuffer->fScales.y = fScalesy;
	m_stFireNoiseBuffer->fScales.z = fScalesz;

}