
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Zenitsu.h"
#include "MapObject.h"            
#include "Camera_Free.h"
#include "UI.h"
#include "MapGroupObject.h"
#include "UI_Lines.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_GAMEPLAY;

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

	if (FAILED(Ready_Layer_Lines(LAYER_LINES)))
		return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Init_Effect(0)))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

void CLevel_GamePlay::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CLevel_GamePlay::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	{
		// 대전 탄지로
		{
			CPlayer::PLAYERDATA tData;
			tData.eControllerType = CPlayer::CONTROLLER_KEYBOARD;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_PLAYER, PROTO_GOB_PLAYER_TANJIRO, &tData)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
	}
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 손 혈귀
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
			pLayerTag, PROTO_GOB_BOSS_TEONI)))
		{
			RELEASE_INSTANCE(CGameInstance)
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
		CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
		CameraDesc.fSpeed = 100.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
			pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));
		_float3 vLook = _float3(-0.017f, -1.000f, 0.003f);
		CameraDesc.v3Eye = _float3(28.637f, 1196.158f, 33.339f);
		CameraDesc.v3At = CameraDesc.v3Eye + vLook;
		CameraDesc.fSpeed = 0.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(0.0f);
		CameraDesc.fFovy = XMConvertToRadians(120.0f);
		CameraDesc.fAspect = _float(8000) / 6000;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 3000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
			pLayerTag, PROTO_GOB_LIGHT_CAMERA, &CameraDesc)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
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

HRESULT CLevel_GamePlay::Ready_Map()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 툴에서 만든 맵 불러오기 
	// urokodakimap
	  {
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj05.bin";
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

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[0].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01B))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[1].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE03A))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[2].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BUSH02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_TREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[6].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[7].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[8].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
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
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01A);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01B);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE03A);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_TREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK01);
			mapdesc.Matrix = m_vecInstanceMatrix[6];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[6].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK02);
			mapdesc.Matrix = m_vecInstanceMatrix[7];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[7].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[8];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[8].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
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
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround05.bin";
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
				if (0 == _tcscmp(PROTO_COMP_MODEL_TEONI_GROUND, ModelName))
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_TEONI_GROUND)))
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
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
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

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	FileInfo info;

	//전투 HP,스킬 게이지 등

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Story_Boss.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_GAMEPLAY;

		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);


	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Lines(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Line = CUI_Lines::TEXTURE_LINES;
	_uint Name = CUI_Lines::TEXTURE_NAME;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
		LAYER_LINES, PROTO_GOB_UI_LINES, &Line)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,
		LAYER_LINES, PROTO_GOB_UI_LINES, &Name)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
