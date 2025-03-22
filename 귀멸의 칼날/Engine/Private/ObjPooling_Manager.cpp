#include "ObjPooling_Manager.h"
#include "ObjectPool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CObjPooling_Manager)

CObjPooling_Manager::CObjPooling_Manager()
{
}

HRESULT CObjPooling_Manager::Add_ObjectPool(const _tchar* _pTag, const _tchar* _pProtoTag,
	const _tchar* _pCloneLayer, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	CObjectPool* pObjectPool = CObjectPool::Create(_pProtoTag, _pCloneLayer, _iLevelIndex, _iMaxIndex, _Data);
	m_ObjectPoolingMap.emplace(pair<const _tchar*, CObjectPool*>(_pTag, pObjectPool));
	return S_OK;
}

HRESULT CObjPooling_Manager::Add_ObjectPool(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect)
{
	CObjectPool* pObjectPool = CObjectPool::Create(_iLevelIndex, _iMaxIndex, _FileName, iSelect);
	m_ObjectPoolingMap.emplace(pair<const _tchar*, CObjectPool*>(_pTag, pObjectPool));
	return S_OK;
}

HRESULT CObjPooling_Manager::Make_CloneList(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (Is_Nullptr(pObjectPool))
		return E_FAIL;

	pObjectPool->Make_CloneList(_iLevelIndex, _iMaxIndex, _Data);
	return S_OK;
}

HRESULT CObjPooling_Manager::Clear_List(const _tchar* _pTag)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (nullptr == pObjectPool)
		return S_OK;

	pObjectPool->Clear_List();
	auto& iter = m_ObjectPoolingMap.begin();
	for (; iter != m_ObjectPoolingMap.end();)
	{
		if (!lstrcmp(iter->first, _pTag))
		{
			m_ObjectPoolingMap.erase(iter);
			break;
		}
		else
			iter++;
	}
	return S_OK;
}

CGameObject* CObjPooling_Manager::Active_Object(const _tchar* _pTag)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (nullptr == pObjectPool)
		return nullptr;

	return pObjectPool->Active_Object();
}

CGameObject* CObjPooling_Manager::Active_ObjectList(const _tchar* _pTag, vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (Is_Nullptr(pObjectPool))
		return nullptr;

	return pObjectPool->Active_ObjectList(_pActiveObject, _iActiveCount);
}

vector<CGameObject*>* CObjPooling_Manager::Get_ObjectList(const _tchar* _pTag)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (Is_Nullptr(pObjectPool))
		return nullptr;

	return pObjectPool->Get_ObjectList();
}

CGameObject* CObjPooling_Manager::Get_CurObject(const _tchar* _pTag)
{
	CObjectPool* pObjectPool = Find_ObjectPool(_pTag);
	if (nullptr == pObjectPool)
		return nullptr;

	return pObjectPool->Cur_Object();
}

CObjectPool* CObjPooling_Manager::Find_ObjectPool(const _tchar* _pTag)
{
	if (0 >= m_ObjectPoolingMap.size())
		return nullptr;

	const auto& iter = find_if(m_ObjectPoolingMap.begin(), m_ObjectPoolingMap.end(), [&](auto& Pair)
		{
			if (!lstrcmp(_pTag, Pair.first))
				return true;

			return false;
		});

	if (iter == m_ObjectPoolingMap.end())
		return nullptr;

	return (*iter).second;
}

void CObjPooling_Manager::Free()
{
	for (auto& iter : m_ObjectPoolingMap)
	{
		if (iter.second != nullptr)
			Safe_Release(iter.second);
	}
}
