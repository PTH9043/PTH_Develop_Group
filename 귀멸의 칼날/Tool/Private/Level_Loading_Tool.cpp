#include "Level_Loading_Tool.h"
#include"GameInstance.h"
#include "Loader_Tool.h"
#include "Level_Main_Tool.h"

CLevel_Loading_Tool::CLevel_Loading_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eLevel)
    : CLevel(_pDevice, _pContext), m_eNextLevel(_eLevel)
{
}

HRESULT CLevel_Loading_Tool::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

	m_pLoader = CLoader_Tool::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

    return S_OK;
}

void CLevel_Loading_Tool::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (true == m_pLoader->Is_Finished())
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CLevel* pLevel = nullptr;
		switch (m_eNextLevel)
		{
		case LEVEL_TOOL:
			g_iCurrentLevel = LEVEL_TOOL;
			pLevel = CLevel_Main_Tool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_REMOVE:
			m_eNextLevel = LEVEL_TOOL;
			g_iCurrentLevel = LEVEL_TOOL;
			pLevel = CLevel_Main_Tool::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
		{
			Safe_Release(pGameInstance);
			return;
		}
		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);
	}
}

void CLevel_Loading_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CLevel_Loading_Tool::Render()
{
    return S_OK;
}

CLevel_Loading_Tool* CLevel_Loading_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel)
{
    CLevel_Loading_Tool* pInstance = new CLevel_Loading_Tool(_pDevice, _pContext, _eNextLevel);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CLevel_Loading_Tool::Create To Failed");
    }
    return pInstance;
}

void CLevel_Loading_Tool::Free()
{
	Safe_Release(m_pLoader);

    __super::Free();
}
