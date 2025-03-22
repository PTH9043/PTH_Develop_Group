

#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "Animator.h"
//#include "Monster.h"
//#include "Terrain.h"
//#include "Player.h"
//#include "Effect.h"
//#include "Sky.h"
//#include "UI_Default.h"
#include "Battle_Manager.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

unsigned int APIENTRY ThreadEntryFunc(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;
	EnterCriticalSection(pLoader->Get_CriticalSection());
	//g_iCurrentLevel = pLoader->Get_NextLevelID();
	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_MODE:
		pLoader->Loading_ForModeLevel();
		break;
	case LEVEL_SELECT:
		pLoader->Loading_ForSelectLevel();
		break;
	case LEVEL_BATTLE:
		pLoader->Loading_ForBattleLevel();
		break;
	case LEVEL_RATTAN:
		pLoader->Loading_ForRattanLevel();
		break;
	case LEVEL_STORY:
		pLoader->Loading_ForStoryLevel();
		break;
	case LEVEL_MINIGAME:
		pLoader->Loading_ForMiniGameLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_ENDING:
		pLoader->Loading_ForEndingLevel();
		break;
	}
	LeaveCriticalSection(pLoader->Get_CriticalSection());
	return 0;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	{
		pGameInstance->Stop_All();
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLoader::Add_Thread(void* _pArg)
{
	if (nullptr == _pArg) return;

	CLoader* pInstance = static_cast<CLoader*>(_pArg);
	pInstance->Loading_Data();
}

void CLoader::Loading_Data()
{
	if (0 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_GamePlay1();
	}
	else if (1 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_GamePlay2();
	}
}

HRESULT CLoader::Load_GamePlay1()
{
	// Zenitsu
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Zenitsu" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_PLAYER_ZENITSU,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Zenitsu", Pivot))))
				return E_FAIL;
		}
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Zenitsu", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_ZENITSU_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Zenitsu_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_ZENITSU_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Zenitsu_Sheath", Pivot))))
				return E_FAIL;
		}
	}

	{
		if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Tanjiro_Mask" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_PLAYER_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Tanjiro_Mask", Pivot))))
				return E_FAIL;
		}
		else
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Tanjiro" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_PLAYER_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Tanjiro", Pivot))))
				return E_FAIL;
		}

		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Tanjiro", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_TANJIRO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_TANJIRO_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sheath", Pivot))))
				return E_FAIL;
		}
	}

	// Akaza
	{
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Akaza" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_PLAYER_AKAZA,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Akaza", Pivot))))
				return E_FAIL;
		}
	}

	{
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Sabito" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_PLAYER_SABITO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Sabito", Pivot))))
				return E_FAIL;
		}

		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Sabito", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_SABITO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Sabito_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_SABITO_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Sabito_Sheath", Pivot))))
				return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Load_GamePlay2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		// 모델 인스턴스 적용, 우로코다키, 타마요맵 
		//story
		_matrix Pivot = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		PATHS MapModelPath = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE01A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE01B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE03A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_TREE01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
		//tamayo
		MapModelPath = { L"Model", L"MapModel", L"106_shinryojo_niwa", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_PLANT))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_FAKEBG))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_FAR01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_STREETLIGHT))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_106BUSH01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_106BUSH02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
		// urokodaki 
		MapModelPath = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BRANCH))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102BUSH01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102BUSH02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102BUSH03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102TREE01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102TREE02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102TREE03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102GRASS01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_102GRASS02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
		// 아사쿠사
		MapModelPath = { L"Model", L"MapModel", L"003_tokyofuasakusa", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// hokusei
		MapModelPath = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// ground
		MapModelPath = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// sky
		MapModelPath = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, XMMatrixScaling(0.003f, 0.003f, 0.003f)))))
				return E_FAIL;
		}
	}

	{
		// maplight
		PATHS MapModelPath = { L"Model", L"MapModel", L"MapLight", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
		_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(90.f));
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_MODEL_MAPLIGHT,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
	}

	{
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_NAVIGATION_UROKODAKI,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation01.bin")))))
				return E_FAIL;
		}

		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_NAVIGATION_TAMAYO,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation06.bin")))))
				return E_FAIL;
		}

		{
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_NAVIGATION_STORY_TEONI,
					CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation01.bin")))))
					return E_FAIL;
			}
		}
	}
	{
		{
			PATHS Path = { L"Trail", L"Noise" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_TEXTURE_NOISE,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}
		{
			PATHS Path = { L"Trail", L"Texture" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_BATTLE, PROTO_COMP_TEXTURE_TRAIL,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Load_GamePlay3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	PATHS Path = { L"Sound", L"Battle" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Load_GamePlay4()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	PATHS Path = { L"Sound", L"Logo" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	lstrcpy(m_szLoading, L"로고 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForModeLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	PATHS Path = { L"Sound", L"Mode" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	lstrcpy(m_szLoading, L"모드 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForSelectLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	


	_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Model" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_MENU,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Model", Pivot))))
		return E_FAIL;

	Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Tanjiro" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_TANJIRO,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Tanjiro", Pivot))))
		return E_FAIL;

	Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Zenitsu" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_ZENITSU,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Zenitsu", Pivot))))
		return E_FAIL;

	Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Sabito" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_SABITO,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Sabito", Pivot))))
		return E_FAIL;

	Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Shinobu" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_SHINOBU,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Shinobu", Pivot))))
		return E_FAIL;

	//Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	//Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Kyoujuro" };

	//if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_KYOUJURO,
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Kyoujuro", Pivot))))
	//	return E_FAIL;

	Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(-35.0f));
	Path = { L"AnimationModel", L"PlayerCharacter", L"Menu_Akaza" };

	if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MODEL_UI_AKAZA,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Menu_Akaza", Pivot))))
		return E_FAIL;

	//무기 
	{
		Pivot = XMMatrixScaling(0.06f, 0.06f, 0.06f);
		Path = { L"Model", L"Item", L"Menu_Tanjiro", L"FBX_Converter" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_TANJIRO_SWORD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Tanjiro_Sword", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Path = { L"Model", L"Item", L"Menu_Zenitsu", L"FBX_Converter" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_ZENITSU_SWORD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Zenitsu_Sword", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Path = { L"Model", L"Item", L"Menu_Sabito", L"FBX_Converter" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_SABITO_SWORD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Sabito_Sword", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.07f, 0.07f, 0.07f);
		Path = { L"Model", L"Item", L"Menu_Shinobu", L"FBX_Converter" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_SHINOBU_SWORD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Shinobu_Sword", Pivot))))
			return E_FAIL;

		/*	Pivot = XMMatrixScaling(0.06f, 0.06f, 0.06f);
			Path = { L"Model", L"Item", L"Menu_Kyoujuro", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_KYOUJURO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Kyoujuro_Sword", Pivot))))
				return E_FAIL;*/

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Path = { L"Model", L"Item", L"Menu_Sabito", L"FBX_Converter" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_SELECT, PROTO_COMP_MENU_SABITO_SHEATH,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Menu_Sabito_Sheath", Pivot))))
			return E_FAIL;
	}

	Path = { L"Sound", L"Select" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	lstrcpy(m_szLoading, L"대전모드 선택창 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForBattleLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	CoInitializeEx(nullptr, 0);
	Load_GamePlay1();
	Load_GamePlay2();
	Load_GamePlay3();
	Load_GamePlay4();

	lstrcpy(m_szLoading, L"대전모드 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStoryLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	{
		// urokodaki 
		_matrix Pivot = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		PATHS MapModelPath = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// hokusei
		MapModelPath = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// ground
		MapModelPath = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// sky
		MapModelPath = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, XMMatrixScaling(0.003f, 0.003f, 0.003f)))))
				return E_FAIL;
		}
		// storymap
		MapModelPath = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_TREE01)
				|| !lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_TREE02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_GRASS))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_GRASSSTRIP))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CLIFF02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE01A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH03A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH03B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH03C))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH04))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH05))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH06))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH10))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH12))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK01))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK02))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_SMALLTREE03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE05A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE02B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_GRASS01B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_GRASS02A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_GRASS03A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_DRYTREE))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CLIFFJUMP))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CLIFF03))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_CLIFF10))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
	}

	// Navigation
	{
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_NAVIGATION_STORY,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation02.bin")))))
				return E_FAIL;
		}
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_NAVIGATION_STORY_ONI1,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Monster1_Navigation02.bin")))))
				return E_FAIL;
		}
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_NAVIGATION_STORY_ONI2,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Monster2_Navigation02.bin")))))
				return E_FAIL;
		}
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_NAVIGATION_STORY_ONI3,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Monster3_Navigation02.bin")))))
				return E_FAIL;
		}
	}

	// Story_Tanjiro
	{
		{
			_matrix Pivot = XMMatrixScaling(0.08f, 0.08f, 0.08f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Story_Tanjiro" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_STORY_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Story_Tanjiro", Pivot))))
				return E_FAIL;
		}
	}

	// Tanjiro
	{
		{
			_matrix Pivot = XMMatrixScaling(0.08f, 0.08f, 0.08f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Tanjiro_Mask" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_PLAYER_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Tanjiro_Mask", Pivot))))
				return E_FAIL;
		}
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Tanjiro", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_TANJIRO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_TANJIRO_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sheath", Pivot))))
				return E_FAIL;
		}
	}

	{
		{
			PATHS Path = { L"Trail", L"Noise" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_TEXTURE_NOISE,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}
		{
			PATHS Path = { L"Trail", L"Texture" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_TEXTURE_TRAIL,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}

	}

	{
		// oni
		{
			_matrix Pivot = XMMatrixScaling(0.09f, 0.09f, 0.09f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni01" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_ONI_ZAKO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"ZakoOni01", Pivot))))
				return E_FAIL;
		}
		{
			_matrix Pivot = XMMatrixScaling(0.09f, 0.09f, 0.09f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni02" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_ONI_ZAKO2,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"ZakoOni02", Pivot))))
				return E_FAIL;
		}
		{
			_matrix Pivot = XMMatrixScaling(0.09f, 0.09f, 0.09f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni03" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_ONI_ZAKO3,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"ZakoOni03", Pivot))))
				return E_FAIL;
		}

		// Boss

		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"BossCharacter", L"TeOni" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_BOSS_TEONI,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"TeOni", Pivot))))
				return E_FAIL;
		}
	}

	{
		//동물칭구들
		_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Deer" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_DEER,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Deer", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Butterfly" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_BUTTERFLY,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Butterfly", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Cat" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_CAT,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Cat", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Bird" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_BIRD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Bird", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Dog" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_DOG,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Dog", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Mouse" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_MOUSE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Mouse", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Chicken" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_CHICKEN,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Chicken", Pivot))))
			return E_FAIL;
	}

	{
		//NPC
		_matrix Pivot = XMMatrixScaling(0.08f, 0.08f, 0.08f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Candidate" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_CANDIDATE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Candidate", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.12f, 0.12f, 0.12f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Candidate2" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_NPC_SIT_CANDIDATE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Candidate2", Pivot))))
			return E_FAIL;

		
		Pivot = XMMatrixScaling(0.12f, 0.12f, 0.12f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Candidate3" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STORY, PROTO_COMP_MODEL_NPC_DEAD_CANDIDATE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Candidate3", Pivot))))
			return E_FAIL;
	}

	PATHS Path = { L"Sound", L"Story" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;

	}
	
	lstrcpy(m_szLoading, L"스토리모드 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForMiniGameLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	{
		//NPC
		_matrix Pivot = XMMatrixScaling(0.12f, 0.12f, 0.12f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Nurse1" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE1,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Nurse1", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.12f, 0.12f, 0.12f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Nurse2" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE2,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Nurse2", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.12f, 0.12f, 0.12f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Nurse3" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE3,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Nurse3", Pivot))))
			return E_FAIL;
	}

	// Navigation
	{
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, PROTO_COMP_NAVIGATION_RATTAN,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation04.bin")))))
				return E_FAIL;
		}
	}

	{
		// urokodaki 
		_matrix Pivot = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		PATHS MapModelPath = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// hokusei
		MapModelPath = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// ground
		MapModelPath = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// sky
		MapModelPath = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, XMMatrixScaling(0.003f, 0.003f, 0.003f)))))
				return E_FAIL;
		}
		MapModelPath = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_PILLAR))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA01A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA01B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA02A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA02B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_LAVENDER))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_MINIGAME, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
	}

	PATHS Path = { L"Sound", L"MiniGame" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;

	}
	
	lstrcpy(m_szLoading, L"미니게임 로딩 완료");

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	PATHS Path = { L"Sound", L"Story" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	{
		{
			// urokodaki 
			_matrix Pivot = XMMatrixScaling(0.04f, 0.04f, 0.04f);
			PATHS MapModelPath = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
			FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
			for (auto& iter : filegroup->vecFileDataList)
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			// hokusei
			MapModelPath = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
			filegroup = pGameInstance->Find_Folder(&MapModelPath);
			for (auto& iter : filegroup->vecFileDataList)
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			// ground
			MapModelPath = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
			filegroup = pGameInstance->Find_Folder(&MapModelPath);
			for (auto& iter : filegroup->vecFileDataList)
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			// sky
			MapModelPath = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
			filegroup = pGameInstance->Find_Folder(&MapModelPath);
			for (auto& iter : filegroup->vecFileDataList)
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, XMMatrixScaling(0.003f, 0.003f, 0.003f)))))
					return E_FAIL;
			}
			// storymap
			MapModelPath = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
			filegroup = pGameInstance->Find_Folder(&MapModelPath);
			for (auto& iter : filegroup->vecFileDataList)
			{
				if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE01A))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE01B))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BROKENTREE03A))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH01))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_BUSH02))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_TREE01))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK01))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_MOUNTROCK02))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_SMALLTREE01))
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
				else
				{
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, iter.strFileName.c_str(),
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
						return E_FAIL;
				}
			}
		}
		// teoni 던지는 item
		{
			_matrix Pivot = XMMatrixScaling(0.32f, 0.32f, 0.32f);
			PATHS Path = { L"Model", L"Item", L"TeOni" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_ITEM_TEONIROCK,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Rock", Pivot))))
				return E_FAIL;

			Pivot = XMMatrixScaling(0.2f, 0.2f, 0.2f);
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_ITEM_TEONITE,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"OniTe", Pivot))))
				return E_FAIL;
		}
	}

	// Navigation
	{
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_NAVIGATION_STORY_TEONI,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation01.bin")))))
				return E_FAIL;
		}
	}

	// Tanjiro
	{

		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Tanjiro_Mask" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_PLAYER_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Tanjiro_Mask", Pivot))))
				return E_FAIL;
		}

		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Tanjiro", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_TANJIRO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_TANJIRO_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sheath", Pivot))))
				return E_FAIL;
		}

	}
	{
		{
			PATHS Path = { L"Trail", L"Noise" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_TEXTURE_NOISE,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}
		{
			PATHS Path = { L"Trail", L"Texture" };
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_TEXTURE_TRAIL,
				CTexture::Create(m_pDevice, m_pContext, &Path))))
				return E_FAIL;
		}

	}

	{
		// Boss
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"BossCharacter", L"TeOni" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY, PROTO_COMP_MODEL_BOSS_TEONI,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"TeOni", Pivot))))
				return E_FAIL;
		}
	}

	lstrcpy(m_szLoading, L"게임플레이 로딩 완료");
	RELEASE_INSTANCE(CGameInstance);
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_ForRattanLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;
	{
		// urokodaki 
		_matrix Pivot = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		PATHS MapModelPath = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// hokusei
		MapModelPath = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// ground
		MapModelPath = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
				return E_FAIL;
		}
		// sky
		MapModelPath = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, XMMatrixScaling(0.003f, 0.003f, 0.003f)))))
				return E_FAIL;
		}
		MapModelPath = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
		filegroup = pGameInstance->Find_Folder(&MapModelPath);
		for (auto& iter : filegroup->vecFileDataList)
		{
			if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_PILLAR))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA01A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA01B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA02A))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_WISTERIA02B))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else if (!lstrcmp(iter.strFileName.c_str(), PROTO_COMP_INSTANCEMODEL_LAVENDER))
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, iter.strFileName.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &MapModelPath, iter.strFileName, Pivot))))
					return E_FAIL;
			}
		}
	}

	{
		//NPC
		_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Aoi" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_AOI,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Aoi", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_SisterL" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_SISTER_L,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_SisterL", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_SisterR" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_SISTER_R,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_SisterR", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.095f, 0.095f, 0.095f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Zenitsu" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_ZENITSU,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Zenitsu", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.095f, 0.095f, 0.095f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Kanawo" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_KANAWO,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Kanawo", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.095f, 0.095f, 0.095f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Candidate" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_CANDIDATE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Candidate", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.095f, 0.095f, 0.095f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"NPC_Genya" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_GENYA,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"NPC_Genya", Pivot))))
			return E_FAIL;
	}

	// Navigation
	{
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_NAVIGATION_RATTAN,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/MapData/Navigation/Navigation04.bin")))))
				return E_FAIL;
		}
	}

	// Story_Tanjiro
	{
		{
			_matrix Pivot = XMMatrixScaling(0.095f, 0.095f, 0.095f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Story_Tanjiro" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_STORY_TANJIRO,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Story_Tanjiro", Pivot))))
				return E_FAIL;
		}
		{
			_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
			PATHS Path = { L"Model", L"Item", L"Tanjiro", L"FBX_Converter" };

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_TANJIRO_SWORD,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sword", Pivot))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_TANJIRO_SHEATH,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Tanjiro_Sheath", Pivot))))
				return E_FAIL;
		}
	}

	{
		//아이템
		_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		PATHS Path = { L"Model", L"Item", L"Item_LanternL" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_LANTERN_L,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Item_LanternL", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Path = { L"Model", L"Item", L"Item_LanternR" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_LANTERN_R,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, &Path, L"Item_LanternR", Pivot))))
			return E_FAIL;
	}

	{
		//동물칭구들
		_matrix Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		PATHS Path = { L"AnimationModel", L"PlayerCharacter", L"Deer" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_DEER,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Deer", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Butterfly" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_BUTTERFLY,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Butterfly", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Cat" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_CAT,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Cat", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Bird" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_BIRD,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Bird", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Dog" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_DOG,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Dog", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Mouse" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_MOUSE,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Mouse", Pivot))))
			return E_FAIL;

		Pivot = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		Path = { L"AnimationModel", L"PlayerCharacter", L"Chicken" };

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_RATTAN, PROTO_COMP_MODEL_CHICKEN,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, &Path, L"Chicken", Pivot))))
			return E_FAIL;
	}



	PATHS Path = { L"Sound", L"Rattan" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);

		lstrcpy(m_szLoading, L"등나무 로딩 완료");


		m_isFinished = true;
		return S_OK;
	
}

HRESULT CLoader::Loading_ForEndingLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;

	PATHS Path = { L"Sound", L"Ending" };
	if (FAILED(pGameInstance->Load_SoundResource(&Path)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	lstrcpy(m_szLoading, L"Thanks to");

	m_isFinished = true;
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
	m_hThread = NULL;
}

