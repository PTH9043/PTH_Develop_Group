#include "Level_Ending.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI.h"


CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Ending::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_ENDING;
	
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Ending::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	SetWindowText(g_hWnd, TEXT("모두 수고하셨습니다"));

}

void CLevel_Ending::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Ending::Render()
{
	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/UI_End.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	FileInfo info;

	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

		info.CurrentLevel = LEVEL_ENDING;
		if (0 == dwByte)
		{
			break;
		}
		pGameInstance->Clone_GameObject(LEVEL_ENDING, LAYER_UI, info.m_PrototypeTag, &info);
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Ending* pInstance = new CLevel_Ending(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLevel_Ending");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
