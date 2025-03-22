#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{


}

HRESULT CLevel_Manager::Change_Level(const _uint& _iCurrentLevelIndex, CLevel* pNextLevel)
{
	if (nullptr == pNextLevel)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pCurrentLevel)
	{
		pGameInstance->Clear_Container(m_iCurrentLevelIndex);
	}

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNextLevel;

	m_iCurrentLevelIndex = _iCurrentLevelIndex;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Manager::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	m_pCurrentLevel->Tick(_dTimeDelta);
	m_fDeltaTime += static_cast<_float>(_dTimeDelta);
	return S_OK;
}

HRESULT CLevel_Manager::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	m_pCurrentLevel->Late_Tick(_dTimeDelta);
	return S_OK;
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	m_pCurrentLevel->Render();
	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}



