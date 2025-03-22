#include "..\Public\CLevel_Manager.h"
#include "CLevel.h"
#include "CGameInstance.h"
#include "CRenderer.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{


}

void CLevel_Manager::Add_Renderer(CRenderer* _pRenderer)
{
	m_pRendererComp = _pRenderer;
	Safe_AddRef(m_pRendererComp);
}

HRESULT CLevel_Manager::Change_Level(const _uint& _iCurrentLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == pNextLevel)
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pCurrentLevel)
	{
		if(nullptr != m_pRendererComp)
			m_pRendererComp->Clear();
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
	Safe_Release(m_pRendererComp);
}



