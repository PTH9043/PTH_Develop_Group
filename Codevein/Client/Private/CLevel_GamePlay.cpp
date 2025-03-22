#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CLevel_GamePlay.h"
#include "CGameInstance.h"
#include "CCamera_Free.h"
#include "CData_Manager.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CSkyBox.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
{

	if (FAILED(Ready_Light(L"Layer_Lighjts")))
		return E_FAIL;

	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		pDataManager->Load_Data("Stage1", LEVEL_GAMEPLAY1);
		pDataManager->Get_PlayerMgr()->Spawn_Player(LEVEL_GAMEPLAY1);
	}
	END_SINGLETON(pDataManager);

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CSkyBox::SKYBOXDESC tSkyBoxDesc;
		tSkyBoxDesc.strSkyBoxName = L"SkyBox_Stage1";
		pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY1, LAYER_SKYBOX, PROTO_GOB_SKYBOX, &tSkyBoxDesc);
	}
	END_SINGLETON(pGameInstance);

	g_iCurrentLevel = LEVEL_GAMEPLAY1;
	g_bGameRun = true;
	return S_OK;
}

void CLevel_GamePlay::Tick(const _double & _dTimeDelta)
{
	g_iCurrentLevel = LEVEL_GAMEPLAY1;

	if (nullptr == m_pPlayer)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel,
				LAYER_PLAYER, 0));
		}
		END_SINGLETON(pGameInstance);
	}
	else
	{
		if (true == m_pPlayer->Is_LevelEnd())
		{
			BEGIN_SINGLETON(CGameInstance, pGameInstance)
			{
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY2))))
				{
					END_SINGLETON(pGameInstance);
					return;
				}
			}
			END_SINGLETON(pGameInstance);
		}
	}
}

void CLevel_GamePlay::Late_Tick(const _double & _dTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light(const _tchar* pLayerTag)
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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
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
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY1, pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			END_SINGLETON(pGameInstance)
				return E_FAIL;
		}
	}
	END_SINGLETON(pGameInstance)
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_GamePlay::Create To Failed");
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		pGameInstance->Clear_Light();
	}
	END_SINGLETON(pGameInstance);
	g_bGameRun = false;
	__super::Free();
}
