#include "CData_Manager.h"
#include <direct.h>
#include "CGameInstance.h"

IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager() 
    : CManager()
{
}

_bool CData_Manager::Is_InitLoad_End()
{
    _uint iThreadCount = 0;
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        iThreadCount = pGameInstance->Get_ThreadCount();
    }
    END_SINGLETON(pGameInstance);

    if (m_iInit_ThreadCount == iThreadCount)
        return true;

    return false;
}

HRESULT CData_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    m_pDevice = _pDevice;
    m_pContext = _pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    m_pWorldManager = CWorldObj_Manager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pWorldManager)
        return E_FAIL;

    m_pPlayerManager = CPlayer_Manager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pPlayerManager)
        return E_FAIL;

    m_pMonsterManager = CMonster_Manager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pMonsterManager)
        return E_FAIL;

    m_pItemManager = CItem_Manager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pItemManager)
        return E_FAIL;

    m_pVarialbeManager = CVariable_Manager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVarialbeManager)
        return E_FAIL;

    m_pStageManager = CStageManager::Create(m_pDevice, m_pContext);
    if (nullptr == m_pStageManager)
        return E_FAIL;

    return S_OK;
}


HRESULT CData_Manager::Save_Data(const string& _strFileName)
{
    string strPath = FOLDER_PATH;
    strPath.append("\\");
    strPath.append(_strFileName);
    _mkdir(strPath.c_str());

    if(FAILED(m_pWorldManager->Save_Data(strPath)))
        return E_FAIL;

    if (FAILED(m_pMonsterManager->Save_Data(strPath)))
        return E_FAIL;

    if (FAILED(m_pStageManager->Save_Data(strPath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CData_Manager::Load_Data(const string& _strFilePath, const LEVEL& _eLevel)
{
    string strPath = FOLDER_PATH;
    strPath.append("\\");
    strPath.append(_strFilePath);
    if (FAILED(m_pWorldManager->Load_Data(strPath)))
        return E_FAIL;

    if (FAILED(m_pMonsterManager->Load_Data(strPath)))
        return E_FAIL;

    if (FAILED(m_pStageManager->Load_Data(strPath, _eLevel)))
        return E_FAIL;

    if (FAILED(Clone_Objects(_eLevel)))
        return E_FAIL;

    m_pStageManager->Get_Stage()->Update_Region();
    return S_OK;
}

void CData_Manager::Reset_Data()
{
    if(nullptr!= m_pStageManager)
    m_pStageManager->Reset();
}

HRESULT CData_Manager::Clone_Objects(const LEVEL& _eLevel)
{
    if (FAILED(m_pWorldManager->Clone_Objects(_eLevel)))
        return E_FAIL;

    if (FAILED(m_pMonsterManager->Clone_Objects(_eLevel)))
        return E_FAIL;

    return S_OK;
}

void CData_Manager::Free()
{
    Safe_Release(m_pStageManager);
    Safe_Release(m_pWorldManager);
    Safe_Release(m_pPlayerManager);
    Safe_Release(m_pMonsterManager);
    Safe_Release(m_pItemManager);
    Safe_Release(m_pVarialbeManager);
    __super::Free();
}
