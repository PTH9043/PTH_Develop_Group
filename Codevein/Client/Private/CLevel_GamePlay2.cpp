#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CLevel_GamePlay2.h"
#include "CGameInstance.h"
#include "CCamera_Free.h"
#include "CData_Manager.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CSkyBox.h"

CLevel_GamePlay2::CLevel_GamePlay2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay2::NativeConstruct()
{
	if (FAILED(Ready_Light(L"Layer_Lighjts")))
		return E_FAIL;

	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		pDataManager->Reset_Data();
		pDataManager->Load_Data("Stage2", LEVEL_GAMEPLAY2);
		pDataManager->Get_PlayerMgr()->Spawn_Player(LEVEL_GAMEPLAY2);
	}
	END_SINGLETON(pDataManager);

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CSkyBox::SKYBOXDESC tSkyBoxDesc;
		tSkyBoxDesc.strSkyBoxName = L"SkyBox_Stage1";
		pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY2, LAYER_SKYBOX, PROTO_GOB_SKYBOX, &tSkyBoxDesc);
	}
	END_SINGLETON(pGameInstance);

	g_iCurrentLevel = LEVEL_GAMEPLAY2;
	g_bGameRun = true;
	return S_OK;
}

void CLevel_GamePlay2::Tick(const _double& _dTimeDelta)
{
	g_iCurrentLevel = LEVEL_GAMEPLAY2;
}

void CLevel_GamePlay2::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CLevel_GamePlay2::Render()
{
	return E_NOTIMPL;
}

HRESULT CLevel_GamePlay2::Ready_Light(const _tchar* pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.v4Direction = _float4(0.f, -1.f, 0.f, 0.f);
		LightDesc.v4Diffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
		LightDesc.v4Ambient = _float4(0.5f, 0.5f, 0.5f, 1.f);
		LightDesc.v4Specular = _float4(0.2f, 0.2f, 0.2f, 0.2f);

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
			return E_FAIL;
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay2::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		CCamera_Free::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
		CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
		CameraDesc.fSpeed = 10.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY2, pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			END_SINGLETON(pGameInstance)
				return E_FAIL;
		}
	}
	END_SINGLETON(pGameInstance)
		return S_OK;
}

CLevel_GamePlay2* CLevel_GamePlay2::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex)
{
	CLevel_GamePlay2* pInstance = new CLevel_GamePlay2(_pDevice, _pContex);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_GamePlay::Create To Failed");
	}
	return pInstance;
}

void CLevel_GamePlay2::Free()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		pGameInstance->Clear_Light();
	}
	END_SINGLETON(pGameInstance);
	g_bGameRun = false;
	__super::Free();
}
