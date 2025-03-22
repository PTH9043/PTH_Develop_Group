#include "Level_Battle.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
#include "UI.h"
#include "MapLight.h"
#include "MapObject.h"
#include "Camera_Free.h"
#include "Player.h"
#include "MapGroupObject.h"
#include "Battle_Manager.h"
#include "UI_CharSelect.h"
#include "CutScene_Manager.h"
#include "UI_Lines.h"


CLevel_Battle::CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Battle::NativeConstruct()
{
	CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
	{
		pInstance->Texture_Load(m_pDevice, m_pContext);
	}
	RELEASE_INSTANCE(CCutScene_Manager);

	g_iCurrentLevel = LEVEL_BATTLE;

	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Load_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Lines(LAYER_LINES)))
		return E_FAIL;

	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Init_Effect(0)))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}
	{
		CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
		{
			pInstance->NativeConstruct_StartCutScene();
		}
		RELEASE_INSTANCE(CBattle_Manager);
	}
	return S_OK;
}

void CLevel_Battle::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);


}

void CLevel_Battle::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	SetWindowText(g_hWnd, TEXT("Battle Level"));
}

HRESULT CLevel_Battle::Render()
{
	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
		{
			{
				CPlayer::PLAYERDATA tData;
				tData.eControllerType = CPlayer::CONTROLLER_KEYBOARD;
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
					LAYER_PLAYER, PROTO_GOB_PLAYER_TANJIRO, &tData)))
				{
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}

			{
				CPlayer::PLAYERDATA tData;
				tData.eControllerType = CPlayer::CONTROLLER_AI;
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
					LAYER_PLAYER, PROTO_GOB_PLAYER_SABITO, &tData)))
				{
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}
		}
		else
		{
			_uint iPlayer1 = m_tSelectInfo.PlayerIndex;
			_uint iPlayer2 = m_tSelectInfo.AIIndex;

			CPlayer::PLAYERDATA tData;

			for (_uint i = 0; i < 2; i++)
			{
				_uint iPlayer = 0;
				if (0 == i)
				{
					iPlayer = m_tSelectInfo.PlayerIndex;
					tData.eControllerType = CPlayer::CONTROLLER_KEYBOARD;
				}
				else
				{
					iPlayer = m_tSelectInfo.AIIndex;
					tData.eControllerType = CPlayer::CONTROLLER_GAMEPAD;
				}

				if (0 == iPlayer)
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_PLAYER, PROTO_GOB_PLAYER_TANJIRO, &tData)))
					{
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
				else if (1 == iPlayer)
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_PLAYER, PROTO_GOB_PLAYER_ZENITSU, &tData)))
					{
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
				else if (2 == iPlayer)
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_PLAYER, PROTO_GOB_PLAYER_SABITO, &tData)))
					{
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
				else if (3 == iPlayer)
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_PLAYER, PROTO_GOB_PLAYER_AKAZA, &tData)))
					{
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
			}
		}

	}
	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLevel_Battle::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Manager::Get_Instance()->Change_Level();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCamera::CAMDESC CameraDesc;
		// Free Camera
		{
			ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));
			CameraDesc.v3Eye = _float3(-24.207f, 18.214f, 136.373f);
			CameraDesc.v3At = CameraDesc.v3Eye + _float3(0.195f, -0.022f, -0.978f);
			CameraDesc.fSpeed = 100.f;
			CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
			CameraDesc.fFovy = XMConvertToRadians(60.0f);
			CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
			CameraDesc.fNear = 0.2f;
			CameraDesc.fFar = 3000.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				pLayerTag, PROTO_GOB_CAMERA_FREE, &CameraDesc)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}

		// Light Camera
		{
			ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));
			_float3 vLook = _float3(-0.578f, -0.705f, 0.411f);
			CameraDesc.v3Eye = _float3(720.278f, 1098.954f, -383.865f);

			CBattle_Manager::BATTLEMAP_TYPE mapType = CBattle_Manager::Get_Instance()->Get_BattleInfo().eBattleMap;
			if (CBattle_Manager::MAP_TAMAYO == mapType)
			{
				vLook = _float3(-0.448f, -0.777f, 0.442f);
				CameraDesc.v3Eye = _float3(229.698f, 736.212f, 252.767f);
			}
			else if (CBattle_Manager::MAP_UROKODAKI == mapType)
			{
				vLook = _float3(-0.189f, -0.962f, 0.195f);
				CameraDesc.v3Eye = _float3(239.430f, 1362.743f, -389.490f);
			}
			else if (CBattle_Manager::MAP_HUJIKASANE == mapType)
			{
				vLook = _float3(-0.017f, - 1.000f, 0.003f);
				CameraDesc.v3Eye = _float3(28.637f, 1196.158f, 33.339f);
			}

			CameraDesc.v3At = CameraDesc.v3Eye + vLook;
			CameraDesc.fSpeed = 0.f;
			CameraDesc.fRotSpeed = XMConvertToRadians(0.0f);
			CameraDesc.fFovy = XMConvertToRadians(120.0f);
			CameraDesc.fAspect = _float(8000) / 6000;
			CameraDesc.fNear = 0.2f;
			CameraDesc.fFar = 3000.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				pLayerTag, PROTO_GOB_LIGHT_CAMERA, &CameraDesc)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}

		// CutScene Camera
		{
			CameraDesc.v3Eye = _float3(0.f, 5.f, -5.0f);
			CameraDesc.v3At = _float3(0.f, 5.f, 0.f);
			CameraDesc.fSpeed = 5.f;
			CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
			CameraDesc.fFovy = XMConvertToRadians(60.0f);
			CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
			CameraDesc.fNear = 0.2f;
			CameraDesc.fFar = 5000;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				pLayerTag, PROTO_GOB_CUTSCENE_CAMERA, &CameraDesc)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Battle::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (FAILED(pGameInstance->Clear_Light()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		CBattle_Manager::BATTLEMAP_TYPE mapType = CBattle_Manager::Get_Instance()->Get_BattleInfo().eBattleMap;
		if (CBattle_Manager::MAP_TAMAYO == mapType)
		{
			_wstring strName = CMethod::ConvertSToW("Tamayo1");
			if (FAILED(pGameInstance->Load_LightsData(strName, m_pDevice, m_pContext)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
		else
		{
			LIGHTDESC			LightDesc;
			ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
			LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
			LightDesc.vDirection = _float3(1.f, -1.f, -1.f);
			LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
			LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
			LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

			if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	FileInfo info;

	if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI == false)
	{
		HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_Battle.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD    dwByte = 0;

		while (true)
		{

			ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

			info.CurrentLevel = LEVEL_BATTLE;

			if (0 == dwByte)
			{
				break;
			}
			pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_UI, info.m_PrototypeTag, &info);
		}

		CloseHandle(hFile);
	}
	else
	{
		HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_AIBattle.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD    dwByte = 0;

		while (true)
		{

			ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

			info.CurrentLevel = LEVEL_BATTLE;

			if (0 == dwByte)
			{
				break;
			}
			pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_UI, info.m_PrototypeTag, &info);
		}

		CloseHandle(hFile);
	}
	

	



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Lines(const _tchar* pLayerTag)
{
	if (false == CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Line = CUI_Lines::TEXTURE_LINES;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
		LAYER_LINES, PROTO_GOB_UI_LINES, &Line)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Battle::Load_Map()
{
	SelectInfo info;
	ZeroMemory(&info, sizeof(SelectInfo));
	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Select_Index.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD    dwByte = 0;
	while (true)
	{
		ReadFile(hFile, &info, sizeof(SelectInfo), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
	}
	CloseHandle(hFile);

	m_tSelectInfo = info;
	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
	{
		if (pBattleInstance->Get_BattleInfo().bSabitoAI)
		{
			m_tSelectInfo.MapIndex = 0;
			m_tSelectInfo.AIIndex = 2;
			m_tSelectInfo.PlayerIndex = 0;
		}
		switch (m_tSelectInfo.MapIndex)
		{
		case 0:
			Ready_UrokodakiMap();
			break;
		case 1:
			Ready_HujikasaneMap();
			break;
		case 2:
			Ready_TamayoMap();
			break;
		case 3:
			Ready_Asakusa();
			break;
		default:
			break;
		}
		pBattleInstance->Set_BattleMapType((CBattle_Manager::BATTLEMAP_TYPE)m_tSelectInfo.MapIndex);
	}
	RELEASE_INSTANCE(CBattle_Manager);

	return S_OK;;
}


HRESULT CLevel_Battle::Ready_UrokodakiMap()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// urokodakimap
	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj01.bin";
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
			_float3 vSize{};
			_float3 vRot{};
			ReadFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			ReadFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			CMapObject::MAPOBJDESC MapObjDesc;

			MapObjDesc.fMatrix = fMatrix;
			lstrcpy(MapObjDesc.ModelName, ModelName);
			MapObjDesc.vSize = vSize;

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_BRANCH))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[0].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[1].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[2].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102TREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102TREE02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102TREE03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[6].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102GRASS01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[7].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_102GRASS02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[8].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_MODEL_BROKENROCK))
			{
				if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_BROKENROCK, &fMatrix)))
					{
						MSG_BOX("Load Failed");
						Safe_Release(pGameInstance);
						return E_FAIL;
					}
				}
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BRANCH);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102BUSH03);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102TREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102TREE02);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102TREE03);
			mapdesc.Matrix = m_vecInstanceMatrix[6];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[6].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102GRASS01);
			mapdesc.Matrix = m_vecInstanceMatrix[7];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[7].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_102GRASS02);
			mapdesc.Matrix = m_vecInstanceMatrix[8];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[8].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround01.bin";
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND)))
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_SKY, PROTO_COMP_MODEL_UROKODAKIIWA_SKY)))
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
		// MAPLIGHT
		{
			CMapLight::MAPLIGHTDESC tDesc;
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(719.f, 50.f, 118.f);
			tDesc.vRot = _float3(12.f, 2.f, 0.f);
			tDesc.fAngle = -19.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 30.f);
			tDesc.vPos = _float3(303.f, 50.f, -616.f);
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(785.f, 50.f, -232.f);
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(30.f, 50.f, -926.f);
			tDesc.vRot = _float3(-0.4f, 1.f, 0.f);
			tDesc.fAngle = 60.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 190.f, 20.f);
			tDesc.vPos = _float3(-58.f, 89.f, 318.f);
			tDesc.vRot = _float3(0.f, 1.f, 0.f);
			tDesc.fAngle = -100.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Battle::Ready_TamayoMap()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// urokodakimap
	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj06.bin";
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
			_float3 vSize{};
			_float3 vRot{};
			ReadFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			ReadFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			CMapObject::MAPOBJDESC MapObjDesc;

			MapObjDesc.fMatrix = fMatrix;
			lstrcpy(MapObjDesc.ModelName, ModelName);
			MapObjDesc.vSize = vSize;

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[0].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[1].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[2].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_PLANT))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_FAKEBG))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[6].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_FAR01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[7].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_STREETLIGHT))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[8].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[9].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[10].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM01);
			mapdesc.Matrix = m_vecInstanceMatrix[0];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[0].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM02);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM03);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_PLANT);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_FAKEBG);
			mapdesc.Matrix = m_vecInstanceMatrix[6];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[6].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_FAR01);
			mapdesc.Matrix = m_vecInstanceMatrix[7];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[7].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_STREETLIGHT);
			mapdesc.Matrix = m_vecInstanceMatrix[8];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[8].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[9];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[9].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[10];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[10].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround06.bin";
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_TAMAYO_GROUND)))
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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

HRESULT CLevel_Battle::Ready_HujikasaneMap()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[9].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE01B);
			mapdesc.Matrix = m_vecInstanceMatrix[1];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[1].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BROKENTREE03A);
			mapdesc.Matrix = m_vecInstanceMatrix[2];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[2].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_TREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK01);
			mapdesc.Matrix = m_vecInstanceMatrix[6];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[6].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_MOUNTROCK02);
			mapdesc.Matrix = m_vecInstanceMatrix[7];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[7].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[8];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[8].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			//lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03);
			//mapdesc.Matrix = m_vecInstanceMatrix[9];
			//mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[9].size();
			//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
			//	LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			//{
			//	MSG_BOX("Load Failed");
			//	Safe_Release(pGameInstance);
			//	return E_FAIL;
			//}
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
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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

HRESULT CLevel_Battle::Ready_Asakusa()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj03.bin";
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
			_float3 vSize{};
			_float3 vRot{};
			ReadFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
			ReadFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			CMapObject::MAPOBJDESC MapObjDesc;

			MapObjDesc.fMatrix = fMatrix;
			lstrcpy(MapObjDesc.ModelName, ModelName);
			MapObjDesc.vSize = vSize;

			if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_PLANT))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[3].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[4].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[5].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH01))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[9].push_back(mat);
			}
			else if (!lstrcmp(ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH02))
			{
				_matrix mat = XMLoadFloat4x4(&fMatrix);
				m_vecInstanceMatrix[10].push_back(mat);
			}
			else
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
					LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_OBJ, &MapObjDesc)))
				{
					MSG_BOX("Load Failed");
					Safe_Release(pGameInstance);
					return E_FAIL;
				}
			}
		}

		{
			/*CMapGroupObject::tagMapObjDesc mapdesc;
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_PLANT);
			mapdesc.Matrix = m_vecInstanceMatrix[3];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[3].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE01);
			mapdesc.Matrix = m_vecInstanceMatrix[4];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[4].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_SMALLTREE03);
			mapdesc.Matrix = m_vecInstanceMatrix[5];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[5].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH01);
			mapdesc.Matrix = m_vecInstanceMatrix[9];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[9].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			lstrcpy(mapdesc.ModelName, PROTO_COMP_INSTANCEMODEL_106BUSH02);
			mapdesc.Matrix = m_vecInstanceMatrix[10];
			mapdesc.InstanceCount = (_uint)m_vecInstanceMatrix[10].size();
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
				LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_INSTANCE, &mapdesc)))
			{
				MSG_BOX("Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}*/
		}
		CloseHandle(hFile);
	}

	{
		_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround03.bin";
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
						LAYER_MAPGROUND, PROTO_GOB_MAPOBJECT_GROUND, PROTO_COMP_MODEL_ASAKUSA_GROUND)))
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
				{
					if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE,
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
		// MAPLIGHT
		{
			CMapLight::MAPLIGHTDESC tDesc;
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(719.f, 50.f, 118.f);
			tDesc.vRot = _float3(12.f, 2.f, 0.f);
			tDesc.fAngle = -19.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 30.f);
			tDesc.vPos = _float3(303.f, 50.f, -616.f);
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(785.f, 50.f, -232.f);
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 100.f, 20.f);
			tDesc.vPos = _float3(30.f, 50.f, -926.f);
			tDesc.vRot = _float3(-0.4f, 1.f, 0.f);
			tDesc.fAngle = 60.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			tDesc.vScale = _float3(0.f, 190.f, 20.f);
			tDesc.vPos = _float3(-58.f, 89.f, 318.f);
			tDesc.vRot = _float3(0.f, 1.f, 0.f);
			tDesc.fAngle = -100.f;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_BATTLE, LAYER_MAPOBJ, PROTO_GOB_MAPOBJECT_LIGHT, &tDesc)))
			{
				MSG_BOX("Light Load Failed");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Battle* CLevel_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Battle* pInstance = new CLevel_Battle(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Battle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Battle::Free()
{
	__super::Free();
}
