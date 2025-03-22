#include "Level_Story.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
#include "UI.h"
#include "MapObject.h"
#include "Camera_Cinematic.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Animal.h"
#include "NPC.h"
#include "UI_Icon.h"
#include "MapGroupObject.h"

CLevel_Story::CLevel_Story(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Story::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_STORY;

	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Animal(LAYER_ANIMAL)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(LAYER_NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Quest(LAYER_QUEST)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(LAYER_MAP)))
		return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		/*if (FAILED(pGameInstance->Load_Effect()))
			return E_FAIL;*/

		if (FAILED(pGameInstance->Init_Effect(0)))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

void CLevel_Story::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CLevel_Story::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	SetWindowText(g_hWnd, TEXT("스토리 모드입니다 "));
}

HRESULT CLevel_Story::Render()
{
	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 대전 탄지로
	{
		CPlayer::PLAYERDATA tData;
		tData.eControllerType = CPlayer::CONTROLLER_KEYBOARD;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
			LAYER_PLAYER, PROTO_GOB_PLAYER_TANJIRO, &tData)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	// 스토리 탄지로
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
			LAYER_STORYPLAYER, PROTO_GOB_STORY_TANJIRO)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 자코 오니  
	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapMonster02.bin";
		//MapMonster02_one
		//_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapMonster02_one.bin";
		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			CloseHandle(hFile);
			RELEASE_INSTANCE(CGameInstance)
				return E_FAIL;
		}
		if (0 == hFile)
		{
			RELEASE_INSTANCE(CGameInstance)
				return E_FAIL;
		}

		DWORD dwByte = 0;
		while (true)
		{
			_uint i = 0;
			_tchar ModelName[MAX_PATH] = L"";
			_float3 vPosition{};
			ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			if (0 == dwByte)
				break;
			if (!lstrcmp(ModelName, PROTO_GOB_ONI_ZAKO))
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_ONI1, ModelName, &vPosition)))
				{
					RELEASE_INSTANCE(CGameInstance)
						return E_FAIL;
				}
			}
			else if (!lstrcmp(ModelName, PROTO_GOB_ONI_ZAKO2))
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_ONI2, ModelName, &vPosition)))
				{
					RELEASE_INSTANCE(CGameInstance)
						return E_FAIL;
				}
			}
			else if (!lstrcmp(ModelName, PROTO_GOB_ONI_ZAKO3))
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_ONI3, ModelName, &vPosition)))
				{
					RELEASE_INSTANCE(CGameInstance)
						return E_FAIL;
				}
			}
		}
		CloseHandle(hFile);
	}

	RELEASE_INSTANCE(CGameInstance)
		return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
		CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
		CameraDesc.fSpeed = 100.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(60.0f);
		CameraDesc.fFovy = XMConvertToRadians(40.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
			pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		// 오니 시네마틱1
		{
			CCamera_Cinematic* pCamera = static_cast<CCamera_Cinematic*>(pGameInstance->Clone_GameObject_Add(LEVEL_STORY,
				pLayerTag, PROTO_GOB_CAMERA_CINEMATIC, &CameraDesc));
			if (nullptr == pCamera)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			if (FAILED(CCamera_Manager::Get_Instance()->Add_Camera(CM_CAMERA_ONI1, pCamera)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			pCamera->Load_Data(_wstring(L"Oni1Final"));
		}

		// 오니 시네마틱2
		{
			CCamera_Cinematic* pCamera = static_cast<CCamera_Cinematic*>(pGameInstance->Clone_GameObject_Add(LEVEL_STORY,
				pLayerTag, PROTO_GOB_CAMERA_CINEMATIC, &CameraDesc));
			if (nullptr == pCamera)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			if (FAILED(CCamera_Manager::Get_Instance()->Add_Camera(CM_CAMERA_ONI2, pCamera)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			pCamera->Load_Data(_wstring(L"Oni2Final"));
		}

		// 오니 시네마틱3
		{
			CCamera_Cinematic* pCamera = static_cast<CCamera_Cinematic*>(pGameInstance->Clone_GameObject_Add(LEVEL_STORY,
				pLayerTag, PROTO_GOB_CAMERA_CINEMATIC, &CameraDesc));
			if (nullptr == pCamera)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			if (FAILED(CCamera_Manager::Get_Instance()->Add_Camera(CM_CAMERA_ONI3, pCamera)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			pCamera->Load_Data(_wstring(L"Oni3Final"));
		}

		// Light Camera
		{
			ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

			_float3 vLook = _float3(0.285f, - 0.951f, 0.117f);
			CameraDesc.v3Eye = _float3(-1727.455f, 1779.116f, 706.110f);
			CameraDesc.v3At = CameraDesc.v3Eye + vLook;
			CameraDesc.fSpeed = 30.f;
			CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
			CameraDesc.fFovy = XMConvertToRadians(120.0f);
			CameraDesc.fAspect = _float(8000) / 6000;
			CameraDesc.fNear = 0.2f;
			CameraDesc.fFar = 3000.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				pLayerTag, PROTO_GOB_LIGHT_CAMERA, &CameraDesc)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Story::Ready_Light(void)
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
		LightDesc.vDirection = _float3(-1.f, -1.f, 1.f);
		LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
		LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
		LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	FileInfo info;

	//퀘스트

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_StoryQuest.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	DWORD    dwByte2 = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte2, nullptr);

		info.CurrentLevel = LEVEL_STORY;

		if (0 == dwByte2)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_STORY, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	//===============================

	//퀘스트 텍스트

	hFile = CreateFile(TEXT("../../Resources/Data/Story_QuestMsg.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	dwByte2 = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte2, nullptr);

		info.CurrentLevel = LEVEL_STORY;

		if (0 == dwByte2)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_STORY, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	//독백 텍스트

	hFile = CreateFile(TEXT("../../Resources/Data/Story_Msg.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	dwByte2 = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte2, nullptr);

		info.CurrentLevel = LEVEL_STORY;

		if (0 == dwByte2)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_STORY, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Animal(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Deer = CAnimal::ANIMAL_DEER;
	_uint Butterfly = CAnimal::ANIMAL_BUTTERFLY;
	_uint Cat = CAnimal::ANIMAL_CAT;
	_uint Bird = CAnimal::ANIMAL_BIRD;
	_uint Mouse = CAnimal::ANIMAL_MOUSE;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Deer)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Butterfly)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Cat)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Bird)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_ANIMAL, PROTO_GOB_ANIMAL, &Mouse)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Candidate = CNPC::NPC_CANDIDATE;
	_uint Candidate2 = CNPC::NPC_SIT_CANDIDATE;
	_uint Candidate3 = CNPC::NPC_DEAD_CANDIDATE;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_NPC, PROTO_GOB_NPC, &Candidate)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_NPC, PROTO_GOB_NPC, &Candidate2)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_NPC, PROTO_GOB_NPC, &Candidate3)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Quest(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Candidate = CUI_Icon::QUEST_CANDIDATE;
	_uint Candidate2 = CUI_Icon::QUEST_CANDIDATE2;
	_uint Targetting_Icon = CUI_Icon::TARGETING_ICON;
	_uint Targetting_Ring = CUI_Icon::TARGETTING_RING;
	_uint Targetting_Base = CUI_Icon::TARGETING_BASE;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Candidate)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Candidate2)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Targetting_Icon)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Targetting_Ring)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
		LAYER_QUEST, PROTO_GOB_UI_ICON, &Targetting_Base)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Story::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	FileInfo info;

	//방향 맵

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_DirMap.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	DWORD    dwByte2 = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte2, nullptr);

		info.CurrentLevel = LEVEL_STORY;

		if (0 == dwByte2)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_STORY, LAYER_MAP, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Story::Ready_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 툴에서 만든 맵 불러오기 
	// urokodakimap
	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj02.bin";
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

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_TREE01)
				|| !lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_TREE02))
			{
				// 인스턴스용 나무 모델들 숫자 세기 + 메트릭스 받아두기 
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[0].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[1].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[2].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_GRASS))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_GRASSSTRIP))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[6].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[7].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[8].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[9].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03C))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[10].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH04))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[11].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH05))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[12].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH06))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[13].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH10))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[14].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH12))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[15].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[16].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[17].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[18].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE05A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[19].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE02B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[20].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_GRASS01B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[21].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_GRASS02A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[22].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_GRASS03A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[23].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_DRYTREE))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[24].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CLIFFJUMP))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[25].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[26].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF10))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[27].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_OBJ, &MapObjDesc)))
				{
					MSG_BOX("Load Failed");
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}
		}

		// instancemodel
		{
			//// 트리 
			CMapGroupObject::tagMapObjDesc mapdesc;
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_TREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			mapdesc.ColliderSize = _float3{ 6.f, 68.f, 6.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			// bush01
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			mapdesc.ColliderSize = _float3{ 3.4f, 2.9f, 3.5f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			// bush02
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			mapdesc.ColliderSize = _float3{ 3.2f, 1.9f, 3.8f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			// smalltree
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			mapdesc.ColliderSize = _float3{ 5.6f, 11.f, 7.6f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			// grass
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_GRASS);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			// grasstrip
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_GRASSSTRIP);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF02);
			mapdesc.Matrix = m_vecInstanceMatrix[6];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[6].size();
			mapdesc.ColliderSize = _float3{ 47.f, 47.8f, 27.4f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01A);
			mapdesc.Matrix = m_vecInstanceMatrix[7];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[7].size();
			mapdesc.ColliderSize = _float3{ 4.6f, 28.6f, 4.7f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03A);
			mapdesc.Matrix = m_vecInstanceMatrix[8];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[8].size();
			mapdesc.ColliderSize = _float3{ 2.1f, 0.9f, 2.3f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03B);
			mapdesc.Matrix = m_vecInstanceMatrix[9];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[9].size();
			mapdesc.ColliderSize = _float3{ 2.1f, 0.6f, 2.3f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH03C);
			mapdesc.Matrix = m_vecInstanceMatrix[10];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[10].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH04);
			mapdesc.Matrix = m_vecInstanceMatrix[11];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[11].size();
			mapdesc.ColliderSize = _float3{ 7.4f, 0.7f, 3.8f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH05);
			mapdesc.Matrix = m_vecInstanceMatrix[12];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[12].size();
			mapdesc.ColliderSize = _float3{ 8.8f, 3.7f, 5.7f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH06);
			mapdesc.Matrix = m_vecInstanceMatrix[13];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[13].size();
			mapdesc.ColliderSize = _float3{ 1.4f, 0.7f, 1.2f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH10);
			mapdesc.Matrix = m_vecInstanceMatrix[14];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[14].size();
			mapdesc.ColliderSize = _float3{ 5.f, 3.5f, 4.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH12);
			mapdesc.Matrix = m_vecInstanceMatrix[15];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[15].size();
			mapdesc.ColliderSize = _float3{ 5.6f, 1.7f, 2.4f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK01);
			mapdesc.Matrix = m_vecInstanceMatrix[16];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[16].size();
			mapdesc.ColliderSize = _float3{ 25.5f, 10.8f, 11.8f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK02);
			mapdesc.Matrix = m_vecInstanceMatrix[17];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[17].size();
			mapdesc.ColliderSize = _float3{ 8.5f, 6.7f, 10.3f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03);
			mapdesc.Matrix = m_vecInstanceMatrix[18];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[18].size();
			mapdesc.ColliderSize = _float3{ 8.f, 26.4f, 8.5f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE05A);
			mapdesc.Matrix = m_vecInstanceMatrix[19];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[19].size();
			mapdesc.ColliderSize = _float3{ 24.7f, 1.7f, 3.4f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE02B);
			mapdesc.Matrix = m_vecInstanceMatrix[20];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[20].size();
			mapdesc.ColliderSize = _float3{ 6.2f, 19.1f, 3.6f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_GRASS01B);
			mapdesc.Matrix = m_vecInstanceMatrix[21];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[21].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_GRASS02A);
			mapdesc.Matrix = m_vecInstanceMatrix[22];	
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[22].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_GRASS03A);
			mapdesc.Matrix = m_vecInstanceMatrix[23];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[23].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_DRYTREE);
			mapdesc.Matrix = m_vecInstanceMatrix[24];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[24].size();
			mapdesc.ColliderSize = _float3{ 3.f, 22.8f, 2.4f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CLIFFJUMP);
			mapdesc.Matrix = m_vecInstanceMatrix[25];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[25].size();
			mapdesc.ColliderSize = _float3{ 0.f, 0.f, 0.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF03);
			mapdesc.Matrix = m_vecInstanceMatrix[26];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[26].size();
			mapdesc.ColliderSize = _float3{ 35.3f, 49.1f, 37.2f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CLIFF10);
			mapdesc.Matrix = m_vecInstanceMatrix[27];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[27].size();
			mapdesc.ColliderSize = _float3{ 126.4f, 60.6f, 29.f };
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
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
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround02.bin";
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
				if (0 == _tcscmp(PROTO_COMP_MODEL_STORY_GROUND, ModelName))
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_STORY_GROUND)))
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
				if (0 == _tcscmp(PROTO_COMP_MODEL_STORY_SKY, ModelName))
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_SKY, PROTO_COMP_MODEL_STORY_SKY)))
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

	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY, L"Layer_MapCollider", PROTO_GOB_STORYMAP_COLLIDER)))
		{
			MSG_BOX("StoryMap Collider Failed");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Story* CLevel_Story::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CLevel_Story* pInstance = new CLevel_Story(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Story");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Story::Free()
{
	__super::Free();
}
