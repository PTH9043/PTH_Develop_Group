#include "Level_Mode.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI.h"

CLevel_Mode::CLevel_Mode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Mode::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_MODE;
	
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;


	return S_OK;
}

void CLevel_Mode::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	
		list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MODE, LAYER_UI);

		for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
		{
			CUI* pInstance = static_cast<CUI*>(*it);
			if (nullptr == pInstance)
				continue;
			if (pInstance->Is_MyName(L"Mode_Bar") == true)
			{
				if (pInstance->Get_Mode() == true && pInstance->Get_ModeIndex()==0)
				{
					if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
						LEVEL_RATTAN))))
						return;
				}

				else if (pInstance->Get_Mode() == true && pInstance->Get_ModeIndex() == 1)
				{
					if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
						LEVEL_SELECT))))
						return;
				}
				break;
			}
		}
	
	

	SetWindowText(g_hWnd, TEXT("모드선택입니다람쥐"));

	Safe_Release(pGameInstance);
}

void CLevel_Mode::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Mode::Render()
{
	return S_OK;
}

HRESULT CLevel_Mode::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_Mode.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_MODE;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_MODE, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Mode* CLevel_Mode::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Mode* pInstance = new CLevel_Mode(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Mode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Mode::Free()
{
	__super::Free();
}
