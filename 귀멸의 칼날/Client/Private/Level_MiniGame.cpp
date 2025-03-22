#include "Level_MiniGame.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
#include "UI.h"
#include "MiniGame_Note.h"
#include "MiniGame_Model.h"
#include "Camera.h"
#include "MapObject.h"
#include "UI_MiniGame.h"
#include "MapGroupObject.h"

CLevel_MiniGame::CLevel_MiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MiniGame::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_MINIGAME;
	
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(LAYER_NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	return S_OK;
}

void CLevel_MiniGame::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);



	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_UI);

	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		CUI_MiniGame* pInstance = static_cast<CUI_MiniGame*>(*it);
		if (nullptr == pInstance)
			continue;

		if (pInstance->Is_MyName(L"Mini_End") == true)
		{
			if (pInstance->Get_Return() == true)
			{
				if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
					LEVEL_RATTAN))))
					return;
			}
			break;
		}
	}

	SetWindowText(g_hWnd, TEXT("미니게임입니다"));

	Safe_Release(pGameInstance);


}

void CLevel_MiniGame::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_MiniGame::Render()
{
	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_MiniGame.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_MINIGAME;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_MINIGAME, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	_int GreatEff = CMiniGame_Note::EFFECT_GREAT;
	_int RingEff = CMiniGame_Note::EFFECT_RING;
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
		LAYER_QUEST, PROTO_GOB_UI_NOTE,&GreatEff)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
		LAYER_QUEST, PROTO_GOB_UI_NOTE, &RingEff)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	_int Nurse1 = CMiniGame_Model::NURSE_0;
	_int Nurse2 = CMiniGame_Model::NURSE_1;
	_int Nurse3 = CMiniGame_Model::NURSE_2;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
		LAYER_NPC, PROTO_GOB_UI_MINIMGAME_MODEL, &Nurse1)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
		LAYER_NPC, PROTO_GOB_UI_MINIMGAME_MODEL, &Nurse2)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
		LAYER_NPC, PROTO_GOB_UI_MINIMGAME_MODEL, &Nurse3)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(51.53f, 49.38f, 152.4f);
		CameraDesc.v3At = _float3(-0.31f, -0.199f, -0.926f);
		CameraDesc.fSpeed = 50.f;
		CameraDesc.fRotSpeed = 5.f;//XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
			pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float3(1.f, -1.f, -1.f);
		LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
		LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
		LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 툴에서 만든 맵 불러오기 
	// 등나무 맵 
	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj04.bin";
		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (0 == hFile)
			return E_FAIL;

		DWORD dwByte = 0;
		while (true)
		{
			_float4x4 fMatrix;
			_uint i = 0;
			_tchar ModelName[MAX_PATH] = L"";
			_float3 vSize;
			_float3 vRot;
			ReadFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			ReadFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			CMapObject::MAPOBJDESC MapObjDesc;
			ZeroMemory(&MapObjDesc, sizeof(CMapObject::MAPOBJDESC));

			MapObjDesc.fMatrix = fMatrix;
			lstrcpy(MapObjDesc.ModelName, ModelName);
			MapObjDesc.vSize = vSize;

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_PILLAR))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[0].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[1].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[2].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_LAVENDER))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
					LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_OBJ, &MapObjDesc)))
				{
					MSG_BOX("Load Failed");
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}
		}
		{
			CMapGroupObject::tagMapObjDesc mapdesc;
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_PILLAR);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01A);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01B);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02A);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02B);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_LAVENDER);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
		CloseHandle(hFile);
	}

	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround04.bin";
		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (0 == hFile)
			return E_FAIL;

		DWORD dwByte = 0;
		_bool GroundCheck = false;
		while (true)
		{
			_uint i = 0;
			_tchar ModelName[MAX_PATH] = L"";
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			if (0 == dwByte)
				break;

			if (false == GroundCheck)
			{
				if (0 == _tcscmp(PROTO_COMP_MODEL_RATTAN_GROUND, ModelName))
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_RATTAN_GROUND)))
					{
						MSG_BOX("Load Failed");
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}

				GroundCheck = true;
			}
			else
			{
				if (0 == _tcscmp(PROTO_COMP_MODEL_RATTAN_SKY, ModelName))
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_SKY, PROTO_COMP_MODEL_RATTAN_SKY)))
					{
						MSG_BOX("Load Failed");
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
			}
		}
		CloseHandle(hFile);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_MiniGame* CLevel_MiniGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MiniGame* pInstance = new CLevel_MiniGame(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_MiniGame::Free()
{
	__super::Free();
}
