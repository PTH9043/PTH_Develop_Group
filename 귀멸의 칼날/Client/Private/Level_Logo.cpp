#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Logo::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_LOGO;
	
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;
	 
	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;



	return S_OK;
}

void CLevel_Logo::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_LOGO, LAYER_UI);

		for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
		{
			CUI* pInstance = static_cast<CUI*>(*it);
			if (nullptr == pInstance)
				continue;
			if (pInstance->Is_MyName(L"Logo_Smoke_Eff") == true)
			{
				if (pInstance->Get_Start() == true)
				{
					if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
						LEVEL_MODE))))
						return;
				}
				break;
			}
		}


		SetWindowText(g_hWnd, TEXT("로고입니다"));

		Safe_Release(pGameInstance);
	}
}

void CLevel_Logo::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//CCamera::CAMERADESC			CameraDesc;
	//ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	//CameraDesc.vEye = _float3(0.f, 2.f, -2.0f);
	//CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	//CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	//CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	//CameraDesc.fFovy = D3DXToRadian(60.0f);
	//CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	//CameraDesc.fNear = 0.2f;
	//CameraDesc.fFar = 300.f;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);	

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Monster(const _tchar * pLayerTag)
{

	//CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	//Safe_AddRef(pGameInstance);

	////if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Monster"))))
	////	return E_FAIL;

	//Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_Logo.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

    FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_LOGO;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_LOGO, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
