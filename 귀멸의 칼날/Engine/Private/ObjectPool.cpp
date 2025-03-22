#include "ObjectPool.h"
#include "GameInstance.h"
#include "GameObject.h"

CObjectPool::CObjectPool()
{
}

HRESULT CObjectPool::NativeConstruct(const _tchar* _pProtoTag, const _tchar* _pCloneLayer, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	m_pProtoTag = _pProtoTag;
	m_pCloneLayer = _pCloneLayer;
	m_iObjectLevelIndex = _iLevelIndex;
	m_iMaxIndex = _iMaxIndex;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		// Max Index
		for (_uint i = 0; i < m_iMaxIndex; ++i)
		{
			CGameObject* pGameObject = pGameInstance->Clone_GameObject_Add(_iLevelIndex, m_pCloneLayer, m_pProtoTag, _Data);
			m_ObjectList.push_back(pGameObject);
			Safe_AddRef(pGameObject);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CObjectPool::NativeConstruct(const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect)
{
	m_iObjectLevelIndex = _iLevelIndex;
	m_iMaxIndex = _iMaxIndex;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		// Max Index
		for (_uint i = 0; i < m_iMaxIndex; ++i)
		{
			CGameObject* pGameObject = pGameInstance->Load_Effect2(_FileName, iSelect);
			m_ObjectList.push_back(pGameObject);
			Safe_AddRef(pGameObject);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CObjectPool::Make_CloneList(const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		// Max Index
		for (auto& iter : m_ObjectList)
		{
			pGameInstance->Add_DeleteObject(m_iObjectLevelIndex, m_pCloneLayer, iter);
			Safe_Release(iter);
		}
		m_iObjectLevelIndex = _iLevelIndex;
		m_iMaxIndex = _iMaxIndex;
		m_ObjectList.clear();
		// Max Index
		for (_uint i = 0; i < m_iMaxIndex; ++i)
		{
			CGameObject* pGameObject = pGameInstance->Clone_GameObject_Add(_iLevelIndex, m_pCloneLayer, m_pProtoTag, _Data);
			m_ObjectList.push_back(pGameObject);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CObjectPool::Clear_List()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		// Max Index
		for (auto& iter : m_ObjectList)
		{
			pGameInstance->Add_DeleteObject(m_iObjectLevelIndex, m_pCloneLayer, iter);
			Safe_Release(iter);
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	m_ObjectList.clear();
	return S_OK;
}

CGameObject* CObjectPool::Active_Object()
{
	//m_ObjectList[m_iActiveIndex++]->Set_Active(false);

	m_iActiveIndex++;

	if (m_iActiveIndex >= m_iMaxIndex)
		m_iActiveIndex = 0;

	m_ObjectList[m_iActiveIndex]->Set_Active(true);
	return m_ObjectList[m_iActiveIndex];
}

CGameObject* CObjectPool::Active_ObjectList(vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount)
{
	if (m_iMaxIndex >= _iActiveCount)
		return nullptr;

	if (Is_Nullptr(_pActiveObject))
		return nullptr;

	if (m_iActiveIndex >= m_iMaxIndex)
		m_iActiveIndex = 0;

	for (_uint i = 0; i < _iActiveCount; ++i)
	{
		//m_ObjectList[m_iActiveIndex++]->Set_Active(false);

		m_iActiveIndex++;

		if (m_iActiveIndex >= m_iMaxIndex)
			m_iActiveIndex = 0;

		m_ObjectList[m_iActiveIndex]->Set_Active(true);
	}
	return m_ObjectList[m_iActiveIndex];
}

CGameObject* CObjectPool::Cur_Object()
{
	if (Is_Nullptr(m_ObjectList[m_iActiveIndex]))
		return nullptr;

	m_ObjectList[m_iActiveIndex]->Set_Active(true);

	return m_ObjectList[m_iActiveIndex];
}

CObjectPool* CObjectPool::Create(const _tchar* _pProtoTag, const _tchar* _pCloneLayer,
	const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _pArg)
{
	CObjectPool* pInstance = new CObjectPool();
	if (FAILED(pInstance->NativeConstruct(_pProtoTag, _pCloneLayer, _iLevelIndex, _iMaxIndex, _pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CObjectPool::Create To Failed");
#endif
	}
	return pInstance;
}

CObjectPool* CObjectPool::Create(const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect)
{
	CObjectPool* pInstance = new CObjectPool();
	if (FAILED(pInstance->NativeConstruct(_iLevelIndex, _iMaxIndex, _FileName, iSelect)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CObjectPool::Create To Failed");
#endif
	}
	return pInstance;
}

void CObjectPool::Free()
{
	for (auto& iter : m_ObjectList)
		Safe_Release(iter);
}
