#include "CLoader.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CTwoHandSword.h"
#include "CSkyBox.h"

CLoader::CLoader(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoader::NativeConstruct(const LEVEL & _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Run_Thread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
		BEGIN_SINGLETON(CData_Manager, pDataManager)
		{

		}
		END_SINGLETON(pDataManager);
		break;
	}

	return S_OK;
}

_uint APIENTRY CLoader::Run_Thread(void * _pArg)
{
	CLoader*		pLoader = (CLoader*)_pArg;
	pLoader->Running();
	return 0;
}

void CLoader::Running()
{	/* 자원로드할꺼야. */

	EnterCriticalSection(&m_CriticalSection);
	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
		Loading_ForToolLevel();
		break;
	case LEVEL_LOGO:
		Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY1:
		Loading_ForGamePlayLevel();
		break;
	case LEVEL_GAMEPLAY2:
		Loading_ForGamePlayLevel2();
		break;
	}
	LeaveCriticalSection(&m_CriticalSection);
}

HRESULT CLoader::Loading_ForToolLevel()
{
#ifdef _DEBUG
	cout << "Loading For ool Level" << endl;
#endif
	/* Tool Defines */
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		{
#pragma region WEAPON
			/* Weapon  */
			{
				_matrix TransformationMatrix = XMMatrixIdentity();
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"HannibalSword" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_HANNIBALSWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"NormalGreatSwordA" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_NORMALGREATSWORDA,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion WEAPON
		}
		{
#pragma region MONKEYDEVIL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				{
					// MonkeyDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"MonkeyDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"MonkeyDevil",  L"Animations_Converter" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext,
								CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_MONKEYDEVIL_V1, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
				{
					// MonkeyDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"MonkeyDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"MonkeyDevil",  L"Animations_Converter" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
								pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_MONKEYDEVIL_V2, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}

				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"MardukHammer" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_MONKEYDEVIL_MARDUKHAMMER,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"BurningSword" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_MONKEYDEVIL_BURNINGSWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion MONKEYDEVIL
		}
		{
#pragma region WOLFDEVIL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				{
					// WolfDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"WolfDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"WolfDevil",  L"Animations_FBX" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext,
								CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_WOLFDEVIL_V1, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
				{
					// WolfDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"WolfDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"WolfDevil",  L"Animations_FBX" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
								pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_WOLFDEVIL_V2, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
			}
#pragma endregion WOLFDEVIL
		}
		{
#pragma region URCHINDEVIL
			_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
			// UrchinDevil
			{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"UrchinDevil" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					PATHS Anim{ L"Game", L"Character", L"UrchinDevil",  L"Animations_FBX" };
					FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext,
							CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
						pModel->Load_Animation(pAnim);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_URCHINDEVIL_V1, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
			{
				// MonkeyDevil
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"UrchinDevil" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					PATHS Anim{ L"Game", L"Character", L"UrchinDevil",  L"Animations_FBX" };
					FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
						pModel->Load_Animation(pAnim);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_URCHINDEVIL_V2, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
			}
#pragma endregion URCHINDEVIL
		}
		{
#pragma region GIANTWHITEDEVIL		
			_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
			// GiantWhiteDevil 
			{
				// HannibalSword
				PATHS Path{ L"Game", L"Character", L"GiantWhiteDevil" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
				PATHS Anim{ L"Game", L"Character", L"GiantWhiteDevil",  L"Animations_FBX" };
				FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
				{
					CModel* pModel = CModel::Create(m_pDevice, m_pContext,
						CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
					pModel->Load_Animation(pAnim);
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_GIANTWHITEDEVIL, pModel)))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
			// GiantHalberd
			{
				// NormalGreat Sword
				PATHS Path{ L"Game", L"Equip", L"Weapon", L"WhiteLargeHalberd" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_GIANTWHITEDEVIL_WHITELARGEHALBERD,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
				{
					END_SINGLETON(pGameInstance);
					return E_FAIL;
				}
			}
#pragma endregion GIANTWHITEDEVIL
		}
		{
#pragma region ACIENTHANNIBAL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.014f, 0.014f, 0.014f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				// ACIENTHANNIBAL
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"AncientHannibal" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_ANCIENTHANNIBAL, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
			}
#pragma endregion ACIENTHANNIBAL
		}
		{
#pragma region GATEKEEPERWOMAN
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				// ACIENTHANNIBAL
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"GateKeeperWoman" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					PATHS Anim{ L"Game", L"Character", L"GateKeeperWoman",  L"Animations_FBX" };
					FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
						pModel->Load_Animation(pAnim);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_GATEKEEPERWOMAN, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
				// GiantHalberd
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"GateKeeperWoman" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_GATEKEEPERWOMAN_SWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion GATEKEEPERWOMAN
		}
	}
	END_SINGLETON(pGameInstance);

	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		pDataManager->Get_WorldMgr()->Load_Stage1();
	}
	END_SINGLETON(pDataManager);
	m_bFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
#ifdef _DEBUG
	cout << "Loading For Logo Level" << endl;
#endif
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager);
		{
			if (false == pDataManager->Is_InitLoadData())
			{
				// 쓰레드 진입
				pGameInstance->Detach_Thread();
				pDataManager->Set_InitLoadData(true);
			}
		}
		END_SINGLETON(pDataManager);

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_LOGO, PROTO_COMP_TEXTURE_LOGO,
			CTexture::Create(m_pDevice, m_pContext, &PATHS{ L"Game", L"Title", L"Textures" }))))
			return E_FAIL;
	}
	END_SINGLETON(pGameInstance);
	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager);
		{
			// Delete WorldManager
			pDataManager->Get_PlayerMgr()->Release_Player();
			pDataManager->Get_WorldMgr()->Release_Data();
		}
		END_SINGLETON(pDataManager);
	}

#ifdef _DEBUG
	cout << "Loading_ForGamePlayLevel" << endl;
#endif
	/* Tool Defines */
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		{
#pragma region WEAPON
			/* Weapon  */
			{
				_matrix TransformationMatrix = XMMatrixIdentity();
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"HannibalSword" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_HANNIBALSWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"NormalGreatSwordA" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_NORMALGREATSWORDA,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion WEAPON
		}
		{
#pragma region MONKEYDEVIL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				{
					// MonkeyDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"MonkeyDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"MonkeyDevil",  L"Animations_Converter" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext,
								CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_MONKEYDEVIL_V1, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
				{
					// MonkeyDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"MonkeyDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"MonkeyDevil",  L"Animations_Converter" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
								pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_MONKEYDEVIL_V2, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}

				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"MardukHammer" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_MONKEYDEVIL_MARDUKHAMMER,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"BurningSword" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_MONKEYDEVIL_BURNINGSWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion MONKEYDEVIL
		}
		{
#pragma region WOLFDEVIL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				{
					// WolfDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"WolfDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"WolfDevil",  L"Animations_FBX" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext,
								CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_WOLFDEVIL_V1, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
				{
					// WolfDevil
					{
						// HannibalSword
						PATHS Path{ L"Game", L"Character", L"WolfDevil" };
						FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
						FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
						PATHS Anim{ L"Game", L"Character", L"WolfDevil",  L"Animations_FBX" };
						FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
						{
							CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
								pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
							pModel->Load_Animation(pAnim);
							if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_WOLFDEVIL_V2, pModel)))
							{
								END_SINGLETON(pGameInstance);
								return E_FAIL;
							}
						}
					}
				}
			}
#pragma endregion WOLFDEVIL
		}
		{
#pragma region URCHINDEVIL
			_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
			// UrchinDevil
			{
				// HannibalSword
				PATHS Path{ L"Game", L"Character", L"UrchinDevil" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
				PATHS Anim{ L"Game", L"Character", L"UrchinDevil",  L"Animations_FBX" };
				FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
				{
					CModel* pModel = CModel::Create(m_pDevice, m_pContext,
						CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
					pModel->Load_Animation(pAnim);
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_URCHINDEVIL_V1, pModel)))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
			{
				// MonkeyDevil
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"UrchinDevil" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					PATHS Anim{ L"Game", L"Character", L"UrchinDevil",  L"Animations_FBX" };
					FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[1], TransformationMatrix);
						pModel->Load_Animation(pAnim);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_URCHINDEVIL_V2, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
			}
#pragma endregion URCHINDEVIL
		}
		{
#pragma region GIANTWHITEDEVIL		
			_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
			// GiantWhiteDevil 
			{
				// HannibalSword
				PATHS Path{ L"Game", L"Character", L"GiantWhiteDevil" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
				PATHS Anim{ L"Game", L"Character", L"GiantWhiteDevil",  L"Animations_FBX" };
				FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
				{
					CModel* pModel = CModel::Create(m_pDevice, m_pContext,
						CModel::TYPE_ANIM, pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
					pModel->Load_Animation(pAnim);
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_GIANTWHITEDEVIL, pModel)))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
			// GiantHalberd
			{
				// NormalGreat Sword
				PATHS Path{ L"Game", L"Equip", L"Weapon", L"WhiteLargeHalberd" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
				if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_GIANTWHITEDEVIL_WHITELARGEHALBERD,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
				{
					END_SINGLETON(pGameInstance);
					return E_FAIL;
				}
			}
#pragma endregion GIANTWHITEDEVIL
		}
		{
#pragma region ACIENTHANNIBAL
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.014f, 0.014f, 0.014f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				// ACIENTHANNIBAL
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"AncientHannibal" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY1, PROTO_COMP_MODEL_ANCIENTHANNIBAL, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
			}
#pragma endregion ACIENTHANNIBAL
		}
	}
	END_SINGLETON(pGameInstance);
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager)
		{
			pDataManager->Get_WorldMgr()->Load_Stage1();
		}
		END_SINGLETON(pDataManager);
	}
	m_bFinished = true;

	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		pDataManager->Set_InitLoadData(false);
	}
	END_SINGLETON(pDataManager);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel2()
{
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager);
		{
			// Delete WorldManager
			pDataManager->Get_PlayerMgr()->Release_Player();
			pDataManager->Get_WorldMgr()->Release_Data();
		}
		END_SINGLETON(pDataManager);
	}

#ifdef _DEBUG
	cout << "Loading_ForGamePlayLevel" << endl;
#endif
	/* Tool Defines */
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		{
#pragma region WEAPON
			/* Weapon  */
			{
				_matrix TransformationMatrix = XMMatrixIdentity();
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"HannibalSword" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY2, PROTO_COMP_MODEL_HANNIBALSWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"NormalGreatSwordA" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY2, PROTO_COMP_MODEL_NORMALGREATSWORDA,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front(),
							TransformationMatrix))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion WEAPON
		}

		{
#pragma region GATEKEEPERWOMAN
			{
				_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
				// ACIENTHANNIBAL
				{
					// HannibalSword
					PATHS Path{ L"Game", L"Character", L"GateKeeperWoman" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					PATHS Anim{ L"Game", L"Character", L"GateKeeperWoman",  L"Animations_FBX" };
					FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
					{
						CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
							pFileGroup, &pFiles->vecFileDataList[0], TransformationMatrix);
						pModel->Load_Animation(pAnim);
						if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY2, PROTO_COMP_MODEL_GATEKEEPERWOMAN, pModel)))
						{
							END_SINGLETON(pGameInstance);
							return E_FAIL;
						}
					}
				}
				// GiantHalberd
				{
					// NormalGreat Sword
					PATHS Path{ L"Game", L"Equip", L"Weapon", L"GateKeeperWoman" };
					FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
					FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX_Converter");
					if (FAILED(pGameInstance->Add_ProtoType(LEVEL_GAMEPLAY2, PROTO_COMP_MODEL_GATEKEEPERWOMAN_SWORD,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &pFiles->vecFileDataList.front()))))
					{
						END_SINGLETON(pGameInstance);
						return E_FAIL;
					}
				}
			}
#pragma endregion GATEKEEPERWOMAN
		}
	}
	END_SINGLETON(pGameInstance);
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager)
		{
			pDataManager->Get_WorldMgr()->Load_Stage1();
		}
		END_SINGLETON(pDataManager);
	}
	m_bFinished = true;

	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		pDataManager->Set_InitLoadData(false);
	}
	END_SINGLETON(pDataManager);

	return S_OK;
}
CLoader * CLoader::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eNextLevel)
{
	CLoader* pInstance = new CLoader(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLoader::Create To Failed");
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
	m_hThread = NULL;

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
