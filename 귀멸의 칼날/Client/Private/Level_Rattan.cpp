#include "Level_Rattan.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
#include "UI.h"
#include "Camera.h"
#include "MapObject.h"
#include "NPC.h"
#include "UI_Icon.h"
#include "Animal.h"
#include "MapGroupObject.h"


CLevel_Rattan::CLevel_Rattan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Rattan::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_RATTAN;
	
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(LAYER_NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Quest(LAYER_QUEST)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Animal(LAYER_ANIMAL)))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_STORYPLAYER)))
		return E_FAIL; //네비 찍어야해

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Rattan::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_UI);

	CUI* Minigame = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI*>(*it)->Is_MyName(L"Rattan_Select0") == true)
		{
			Minigame = dynamic_cast<CUI*>(*it);
			break;
		}
	}

	CUI* Storygame = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI*>(*it)->Is_MyName(L"Rattan_Chapter") == true)
		{
			Storygame = dynamic_cast<CUI*>(*it);
			break;
		}
	}

		if (Minigame != nullptr && Minigame->Get_MiniGameStart() == true)
		{
			if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
				LEVEL_MINIGAME))))
				Safe_Release(pGameInstance);
				return;
		}

		if (Storygame != nullptr && Storygame->Get_StoryStart() == true)
		{
			if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
				LEVEL_STORY))))
				Safe_Release(pGameInstance);
				return;
		}
		
		

	

	SetWindowText(g_hWnd, TEXT("등나무 맵입니다"));
	Safe_Release(pGameInstance);


}

void CLevel_Rattan::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Rattan::Render()
{
	return S_OK;

}

HRESULT CLevel_Rattan::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Rattan_Msg.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_RATTAN;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_RATTAN, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Rattan::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// 탄지로
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
			LAYER_STORYPLAYER, PROTO_GOB_STORY_TANJIRO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Rattan::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Aoi = CNPC::NPC_AOI;
	_uint SisterL = CNPC::NPC_SISTER_L;
	_uint SisterR = CNPC::NPC_SISTER_R;
	_uint Zenitsu = CNPC::NPC_ZENITSU;
	_uint Kanawo = CNPC::NPC_KANAWO;
	_uint Candidate = CNPC::RATTAN_CANDIDATE;
	_uint Genya = CNPC::NPC_GENYA;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &Aoi)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &SisterL)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &SisterR)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &Zenitsu)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &Kanawo)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &Candidate)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_NPC, PROTO_GOB_NPC, &Genya)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_Rattan::Ready_Layer_Quest(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Aoi = CUI_Icon::QUEST_AOI;
	_uint Sister = CUI_Icon::QUEST_MAIN;
	_uint ZenitsuIcon = CUI_Icon::ZENITSU_ICON;
	_uint KanawoIcon = CUI_Icon::KANAWO_ICON;
	_uint CandidateIcon = CUI_Icon::CANDIDATE_ICON;
	_uint GenyaIcon = CUI_Icon::GENYA_ICON;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Aoi)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Sister)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &ZenitsuIcon)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &KanawoIcon)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &CandidateIcon)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &GenyaIcon)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Rattan::Ready_Map()
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
					LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_OBJ, &MapObjDesc)))
				{
					MSG_BOX("Load Failed");
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}
		}
		
		// INSTANCEMODEL
		{
			CMapGroupObject::tagMapObjDesc mapdesc;
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_PILLAR);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01A);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA01B);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02A);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_WISTERIA02B);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_LAVENDER);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
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
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
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
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
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

HRESULT CLevel_Rattan::Ready_Layer_Animal(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Dog = CAnimal::ANIMAL_DOG;
	_uint Butterfly = CAnimal::ANIMAL_BUTTERFLY;
	_uint Butterfly2 = CAnimal::ANIMAL_BUTTERFLY2;
	_uint Butterfly3 = CAnimal::ANIMAL_BUTTERFLY3;
	_uint Chicekn = CAnimal::ANIMAL_CHICKEN;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Dog)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Butterfly)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Butterfly2)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Butterfly3)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Chicekn)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Rattan::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(-171.f, 12.13f, -1.05f);
		CameraDesc.v3At = _float3(0.f, 10.f, 0.f);
		CameraDesc.fSpeed = 50.f; //50
		CameraDesc.fRotSpeed = XMConvertToRadians(30.0f); //90
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
			pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Rattan::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (FAILED(pGameInstance->Clear_Light()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_wstring strName = CMethod::ConvertSToW("Rattan");
		if (FAILED(pGameInstance->Load_LightsData(strName, m_pDevice, m_pContext)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Rattan* CLevel_Rattan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Rattan* pInstance = new CLevel_Rattan(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Rattan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Rattan::Free()
{
	__super::Free();
}
