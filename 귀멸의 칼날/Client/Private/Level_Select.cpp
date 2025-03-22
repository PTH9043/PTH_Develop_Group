#include "Level_Select.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
#include "UI.h"
#include "Camera.h"
#include "UI_CharSelect.h"

CLevel_Select::CLevel_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Select::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_SELECT;

	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Model(LAYER_UIMODEL)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Select::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_SELECT, LAYER_UI);

	CUI_CharSelect* Select = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_CharSelect*>(*it)->Is_MyName(L"MapTitle") == true)
		{
			Select = dynamic_cast<CUI_CharSelect*>(*it);
			break;
		}
	}


	if (Select != nullptr && Select->Get_StartBattle() == true)
	{
		if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
			LEVEL_BATTLE))))
			Safe_Release(pGameInstance);
		return;
	}


	Safe_Release(pGameInstance);
	
}

void CLevel_Select::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	SetWindowText(g_hWnd, TEXT("대전모드 선택창입니다 "));
}

HRESULT CLevel_Select::Render()
{
	return S_OK;
}

HRESULT CLevel_Select::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	FileInfo info;
	
	//캐릭터 선택창

	HANDLE    hFile2 = CreateFile(TEXT("../../Resources/Data/UI_CharSelect.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile2)
		return E_FAIL;


	DWORD    dwByte2 = 0;

	while (true)
	{

		ReadFile(hFile2, &info, sizeof(FileInfo), &dwByte2, nullptr);

		info.CurrentLevel = LEVEL_SELECT;

		if (0 == dwByte2)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_SELECT, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile2);

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_Select::Ready_Layer_Model(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_int Player1 = 0;
	_int Player2 = 1;
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SELECT,
		LAYER_UIMODEL, PROTO_GOB_UI_SELECTMODEL,&Player1)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SELECT,
		LAYER_UIMODEL, PROTO_GOB_UI_SELECTMODEL,&Player2)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Select::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(1.3f, 11.5f, -17.1f);
		CameraDesc.v3At = _float3(0.f, 10.f, 0.f);
		CameraDesc.fSpeed = 0.f;
		CameraDesc.fRotSpeed = 0.f;//XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SELECT,
			pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/*ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));
		CameraDesc.v3Eye = _float3(-460.f, 1500.f, -70.f);
		CameraDesc.v3At = _float3(0.f, 300.f, 0.f);
		CameraDesc.fSpeed = 0.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(0.0f);
		CameraDesc.fFovy = XMConvertToRadians(120.0f);
		CameraDesc.fAspect = _float(8000) / 6000;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SELECT,
			pLayerTag, PROTO_GOB_LIGHT_CAMERA, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}*/
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CLevel_Select::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (FAILED(pGameInstance->Clear_Light()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float3(0.f, -0.5f, 1.f);
		LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
		LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
		LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Select* CLevel_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Select* pInstance = new CLevel_Select(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Select");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Select::Free()
{
	__super::Free();
}
