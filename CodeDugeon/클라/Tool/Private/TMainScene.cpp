#include "ToolDefines.h"
#include "TMainScene.h"
#include "TMainCamera.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UGrid.h"
#include "UVIBufferGrid.h"
#include "UTransform.h"
#include "UPicking.h"
#include "TGuizmoManager.h"
#include "ULight.h"

TMainScene::TMainScene(CSHPTRREF<UDevice> _spDevice)
	: UScene(_spDevice, (_uint)IMGTAG::MAIN),
	m_spMainCamera{ nullptr }
{
}

void TMainScene::Free()
{
}

HRESULT TMainScene::LoadSceneData()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	// Main Camera Load 
	{
		UCamera::CAMDESC tDesc;
		tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::PERSPECTIVE, _float3(0.f, 0.f, 0.f),
			_float3(0.f, 0.f, 1.f),
			DirectX::XMConvertToRadians(60.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.2f, 1000.f);
		tDesc.stCamValue = UCamera::CAMVALUE(20.f, DirectX::XMConvertToRadians(90.f));
		tDesc.eCamType = CAMERATYPE::MAIN;
		// Actor Add Main Camera

		VOIDDATAS vecDatas;
		vecDatas.push_back(&tDesc);

		m_spMainCamera = std::static_pointer_cast<TMainCamera>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAINCAMERA, vecDatas));
		m_spMainCamera->GetTransform()->SetPos({ -875.66, -135.805, 852. });
	}
	{
		
		
	/*	AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_DIRECTIONAL,LIGHTACTIVE::ISACTIVE, {1.f, 1.f, 1.f, 1.f}, {0.2f, 0.2f,0.2f, 1.f}, {0.15f, 0.15f, 0.15f, 1.f}, {0.f, -1.f, 0.f,}, {0.f, 100.f, 0.f}, 0.f, 0.f ,
			1.f, 20.f });
		*/
		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_POINT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.4f, 0.2f, 0.08f, 1.f}, {0.8f, 0.4f, 0.16f, 1.f}, {0.f, 0.f, 1.f,},
		_float3(-883.653748, -145.2615, 847.1639), 60.f, 0.f ,
		1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f) });
		/*
		* 현재 머티리얼은 Diffuse는 밝은 회색으로 개성있게 나와 곱해도 무방. Ambient, Specular추출정보 대부분 검은색이라, 조명의 Ambient Specular을 곱하면 무시되는 수준이라 적용안하는게 좋아보임.
		* 
		1. Directional Light
		태현 : 필요인자 vDiffuse , vAmbient, vSpecular, fLightPower,fSpecularPowValue
		용빠 : 필요인자 vDiffuse, vAmbient, vSpecular
		
		2.SpotLight
		태현: 필요인자 vDiffuse , vAmbient, vSpecular,vDirection, vPosition, fLightPower, fRange,fAngle, fLightPower, fSpecularPowValue
		용빠: 필요인자 vDiffuse , vAmbient, vSpecular,vDirection, vPosition, fRange,fSpecularPowValue, fFallOff,fPhi,fTheta, 업데이트 버전(vAttenuation 불필요) 전버전: 필요

		2.PointLight
		태현: 필요인자 vDiffuse , vAmbient, vSpecular,vDirection, vPosition, fLightPower, fRange,fAngle, fLightPower, fSpecularPowValue
		용빠: 필요인자 vDiffuse , vAmbient, vSpecular, vPosition, fRange, vAttenuation 

		*/
		//현재 용빠 라이팅이나 태현이 라이팅이나 Directional Light는 Material적용해도, Directional은 Ambient를 그대로 혼합, 용빠는 Specular는 Material이 0으로 결과가 0으로 추출되어
		// 태현이는 Specular 0을 곱하여 결국 똑같이 나온다. 용빠꺼는 LightPower적용은안되어있다. 또한 용빠는 현재 월드공간에서 계산하며, 태현이는 뷰공간에서 계산한다.

		//Diffuse Light는 확산조명으로 보통 물체의 매끄럽지 않은 표면, 거친 표면에서 일어남.
		//확산 표면은 물체의 표면에서 모든 방향으로 균일하게 빛을 반사한다고 가정.

	/*	AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_SPOT,LIGHTACTIVE::ISACTIVE, {0.75f,0.75f,0.75f, 1.f}, {0.4f, 0.2f, 0.08f, 1.f}, {0.15f, 0.125f, 0.11f, 1.f}, {0.f, -1.f, 0.f,},
		_float3(-355,-20,166), 100.f, 0.f ,
		1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f) });*/
		
		/*	AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_SPOT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 0.f}, {0.15f, 0.125f, 0.11f, 1.f}, {1.f, 0.5f, 0.2f, 1.f}, {0.f, -1.f, 0.f,}
			, m_spMainCamera->GetTransform()->GetPos(), 100.f, 60.f ,
			100.f, 32.f, 8.0f,(float)cos(DirectX::XMConvertToRadians(30.f)),(float)cos(DirectX::XMConvertToRadians(15.f)),_float3(1.0f, 0.01f, 0.0001f) });*/
		
		/*
		* SpecularPowValue가 클수록 국소범위 줄어듬 더 좁은면적에서 반사. 권장 32 . 1로갈수록 Specular범위 커짐.
		자연스러운 조명:

		Ambient: 0.2 ~ 0.4 사이의 값. 이 값은 장면 전체의 조명 수준을 조절합니다.
		Diffuse: 0.6 ~ 0.8 사이의 값. 이 값은 직접적인 조명의 강도를 결정합니다.
		Specular: 0.3 ~ 0.5 사이의 값. 이 값은 하이라이트의 강도를 조절합니다.
		강렬한 조명:
		
		Ambient: 0.1 ~ 0.3 사이의 값. 낮은 값으로 조명을 더 직접적이고 대비감 있게 만듭니다.
		Diffuse: 0.8 ~ 1.0 사이의 값. 높은 값으로 표면에 직접적으로 닿는 빛의 강도를 높입니다.
		Specular: 0.5 ~ 0.7 사이의 값. 높은 값으로 하이라이트를 강조합니다.
		부드러운 조명:
		
		Ambient: 0.4 ~ 0.6 사이의 값. 높은 값으로 조명을 부드럽게 만듭니다.
		Diffuse: 0.5 ~ 0.7 사이의 값. 중간 정도의 값으로 조명의 강도를 적당하게 유지합니다.
		Specular: 0.2 ~ 0.4 사이의 값. 낮은 값으로 하이라이트를 부드럽게 유지합니다.
		*/
	
		
		
		/*AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_POINT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.4f, 0.2f, 0.08f, 1.f}, {0.8f, 0.4f, 0.16f, 1.f}, {0.f, 0.f, 1.f,},
			_float3(-555.183f,-32.f,149.312f), 40.f, 0.f ,
			1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f)});

		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_POINT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.4f, 0.2f, 0.08f, 1.f}, {0.15f, 0.125f, 0.11f, 1.f}, {0.f, 0.f, 1.f,},
		_float3(50,0,0), 40.f, 0.f ,
		1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f) });*/
		
	/*	AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_POINT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.2f, 0.1f, 0.04f, 1.f}, {0.15f, 0.125f, 0.11f, 1.f}, {0.f, 0.f, 1.f,},
			_float3(0,0,0), 20.f, 0.f ,
			1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f) });*/
		
		/*AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_FLASHLIGHT, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 1.f,}, {0.f, 50.f, -100.f}, 60.f, 60.f ,
			1.f, 2.f });*/
	}

	{
		_float GridScale = 7.0f;
		m_spGrid = std::static_pointer_cast<UGrid>(spGameInstance->CloneActorAdd(PROTO_ACTOR_GRID));
		m_spGrid->GetTransform()->SetScale({ GridScale, 1.0f, GridScale });
		m_spGrid->GetTransform()->SetPos({ -(float)HALF_GRID_SIZE * GridScale, 0.f, -(float)HALF_GRID_SIZE * GridScale });
	}

	return S_OK;
}

void TMainScene::Tick(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<ULight> SpotLight;
	/*OutLight(LIGHTTYPE::TYPE_SPOT, 0, SpotLight);
	SpotLight->SetLightPos(m_spMainCamera->GetTransform()->GetPos());*/
//	SpotLight->SetDirection(m_spMainCamera->GetTransform()->GetLook());


	/*SHPTR<ULight> DirLight;
	OutLight(LIGHTTYPE::TYPE_DIRECTIONAL, 0, DirLight);
	if (pGameInstance->GetDIKeyPressing(DIK_4))
		DirLight->SetLightVersion(LIGHTVERSION::TYPE_ORIGINAL);
	if (pGameInstance->GetDIKeyPressing(DIK_5))
		DirLight->SetLightVersion(LIGHTVERSION::TYPE_YONGBBA);


	SHPTR<ULight> PointLight;
	OutLight(LIGHTTYPE::TYPE_POINT, 0, PointLight);
	if (pGameInstance->GetDIKeyPressing(DIK_6))
		PointLight->SetLightVersion(LIGHTVERSION::TYPE_ORIGINAL);
	if (pGameInstance->GetDIKeyPressing(DIK_7))
		PointLight->SetLightVersion(LIGHTVERSION::TYPE_YONGBBA);
	_float3 pos = m_spMainCamera->GetTransform()->GetPos();
	_float3 Look = m_spMainCamera->GetTransform()->GetLook();
	PointLight->SetLightPos(_float3(-546, -32, 131));*/

	/*SHPTR<ULight> PointLight2;
	OutLight(LIGHTTYPE::TYPE_POINT, 1, PointLight2);
	PointLight2->SetLightPos(_float3(-360, -20,253));*/

}

void TMainScene::LateTick(const _double& _dTimeDelta)
{

}
