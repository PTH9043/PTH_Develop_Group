#include "CStageMananger.h"
#include "CGameInstance.h"

CStageManager::CStageManager()
{
}

CStageManager::CStageManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CManager(_pDevice, _pContext)
{
}

HRESULT CStageManager::NativeConstruct()
{
	return S_OK;
}

HRESULT CStageManager::Save_Data(const _string& _strPath)
{
	if (nullptr == m_pStage)
		return E_FAIL;

	_wstring str;
	str.assign(_strPath.begin(), _strPath.end());
	str.append(L"\\Nav");
	_wmkdir(str.c_str());
	m_pStage->SaveFile(str.c_str());
	return S_OK;
}

HRESULT CStageManager::Load_Data(const _string& _strPath, const LEVEL& _eLevel)
{
	if (nullptr != m_pStage)
	{
		Safe_Release(m_pStage);
		m_pStage = CStage::Create(m_pDevice, m_pContext);
	}
	else
	{
		m_pStage = CStage::Create(m_pDevice, m_pContext);
	}

	PATHS paths{ L"Json" };
	FILEGROUP* pFileGroup = nullptr;

	_wstring str;
	str.assign(_strPath.begin(), _strPath.end());
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		pFileGroup = pGameInstance->Find_Folder(&paths);
	}
	END_SINGLETON(pGameInstance);

	FILEGROUP* pFolder = nullptr;
	if (nullptr != pFileGroup)
	{
		for (auto& iter : pFileGroup->lsUnderList)
		{
			if (!lstrcmp(iter->strPath.c_str(), str.c_str()))
			{
				pFolder = iter;
				break;
			}
		}
	}

	if (nullptr == pFolder)
		return S_OK;

	pFolder = pFolder->Find_Group(L"Nav");
	if (nullptr == pFolder)
		return S_OK;
	for (auto& iter : pFolder->vecFileDataList)
	{
		m_pStage->LoadFile(iter.strFilePath.c_str());
	}
	m_pStage->Add_ArroundReigon();
	return S_OK;
}

CStageManager* CStageManager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CStageManager* pInstance = new CStageManager(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CStageManager::Create to Failed");
	}
	return pInstance;
}

void CStageManager::Free()
{
	Safe_Release(m_pStage);
	__super::Free();
}
