
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Level_Logo.h"
#include "Level_Mode.h"
#include "Level_GamePlay.h"
#include "Level_Select.h"
#include "Level_Battle.h"
#include "Level_Story.h"
#include "Level_MiniGame.h"
#include "Level_Rattan.h"
#include "Level_Ending.h"
#include "UI.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel)
{
	/*CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Stop_All();
	pGameInstance->Stop_Sound(CSound_Manager::CHANNELID::BGM);

	Safe_Release(pGameInstance);*/


	g_iCurrentLevel = LEVEL_LOADING;

	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_LOADING, LAYER_UI);
	
	if (Prototype_GameObjects.size() == 0)
		return;

	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI*>(*it)->Is_MyName(L"Load_Text") == true)
		{
			CUI* Text = dynamic_cast<CUI*>(*it);

		     if (m_eNextLevel == LEVEL_SELECT)
				Text->Set_Texture(L"Loading_Text4");
			 else if (m_eNextLevel == LEVEL_MODE)
				 Text->Set_Texture(L"Loading_Text1");
			else if (m_eNextLevel == LEVEL_BATTLE)
				Text->Set_Texture(L"Loading_Text2");
			else if (m_eNextLevel == LEVEL_STORY)
				Text->Set_Texture(L"Loading_Text0");
			else if (m_eNextLevel == LEVEL_MINIGAME)
				 Text->Set_Texture(L"Loading_Text5");
			else if (m_eNextLevel == LEVEL_GAMEPLAY)
				 Text->Set_Texture(L"Loading_Text1");
			else if (m_eNextLevel == LEVEL_RATTAN)
				 Text->Set_Texture(L"Loading_Text3");
			break;
		}
	}
	Safe_Release(pGameInstance);
}

void CLevel_Loading::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
	
	/* 로더에게 로딩(다음레벨을 위한 자원로딩) 이 끝났다면이라는 정보를받아와서.  */

	if(true == m_pLoader->isFinished())
	{ 
		{
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CLevel*		pLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MODE:
				pLevel = CLevel_Mode::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_SELECT:
				pLevel = CLevel_Select::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_BATTLE:
				pLevel = CLevel_Battle::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_RATTAN:
				pLevel = CLevel_Rattan::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_STORY:
				pLevel = CLevel_Story::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MINIGAME:
				pLevel = CLevel_MiniGame::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_ENDING:
				pLevel = CLevel_Ending::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
				return;

			Safe_Release(pGameInstance);

		}
	}

	
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_Loading.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_LOADING;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_LOADING, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
