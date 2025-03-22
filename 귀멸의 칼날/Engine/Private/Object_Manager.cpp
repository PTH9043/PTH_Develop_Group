 #include "..\Public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "Level_Manager.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager() 
{

}

CComponent* CObject_Manager::Get_Component(const _uint& _iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, const  _uint& _iIndex)
{
	CLayer* pLayer = Find_Layer(_iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(pComponentTag, _iIndex);
}

HRESULT CObject_Manager::Reserve_Objects(const _uint& _iLevelCount)
{
	if (nullptr != m_pLayers)
		return E_FAIL;
	if (0 >= _iLevelCount)
		return E_FAIL;

	m_pLayers = new LAYERS[_iLevelCount];
	this->m_iNumLevels = _iLevelCount;

	return S_OK;
}

HRESULT CObject_Manager::Add_ProtoType(const _tchar*& _pProtoTypeTag, CGameObject*& _pGameObject)
{
	if (nullptr != Find_ProtoType(_pProtoTypeTag))
		return E_FAIL;

	m_mPrototypes.emplace(_pProtoTypeTag, _pGameObject);
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _tchar*& _pProtoTypeTag, void*& _pArg)
{
	if (nullptr == m_pLayers || m_iNumLevels <= _iLevelIndex)
		return E_FAIL;

	CGameObject* pGameObject = Find_ProtoType(_pProtoTypeTag);
	if (nullptr == pGameObject)
		return E_FAIL;

	CGameObject* pCloneObject = pGameObject->Clone(_pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	CLayer* pLayers = Find_Layer(_iLevelIndex, _pLayerTypeTag);
	if (nullptr == pLayers)
	{
		pLayers = CLayer::Create();
		if (nullptr == pLayers)
			return E_FAIL;

		pLayers->Add_GameObject(pCloneObject);
		m_pLayers[_iLevelIndex].emplace(_pLayerTypeTag, pLayers);
	}
	else
		pLayers->Add_GameObject(pCloneObject);

	return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject_Add(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _tchar*& _pProtoTypeTag, void*& _pArg)
{
	if (nullptr == m_pLayers || m_iNumLevels <= _iLevelIndex)
		return nullptr;

	CGameObject* pGameObject = Find_ProtoType(_pProtoTypeTag);
	if (nullptr == pGameObject)
		return nullptr;

	CGameObject* pCloneObject = pGameObject->Clone(_pArg);
	if (nullptr == pCloneObject)
		return nullptr;

	CLayer* pLayers = Find_Layer(_iLevelIndex, _pLayerTypeTag);
	if (nullptr == pLayers)
	{
		pLayers = CLayer::Create();
		if (nullptr == pLayers)
			return nullptr;

		pLayers->Add_GameObject(pCloneObject);
		m_pLayers[_iLevelIndex].emplace(_pLayerTypeTag, pLayers);
	}
	else
		pLayers->Add_GameObject(pCloneObject);

	return pCloneObject;
}

CGameObject* CObject_Manager::Find_GameObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _uint& _iIndex)
{
	if (0 > _iIndex)
		return nullptr;

	CLayer* pLayers = Find_Layer(_iLevelIndex, _pLayerTypeTag);
	if (nullptr == pLayers)
		return nullptr;

	CGameObject* pGameObject = pLayers->Find_GameObject(_iIndex);
	return pGameObject;
}

list<CGameObject*> CObject_Manager::Find_GameObjectList(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag)
{
	if (0 > _iLevelIndex)
		return NULL_LIST;
	// 
	CLayer* pLayers = Find_Layer(_iLevelIndex, _pLayerTypeTag);
	if (nullptr == pLayers)
		return NULL_LIST;
	// ObjectList
	return pLayers->Get_ObjectList();
}

HRESULT CObject_Manager::Add_DeleteObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, CGameObject*& _pGameObject)
{
	if (0 > _iLevelIndex)
		return E_FAIL;
	CLayer* pLayers = Find_Layer(_iLevelIndex, _pLayerTypeTag);
	if (nullptr == pLayers)
		return E_FAIL;

	pLayers->Add_DeleteObject(_pGameObject);
	return S_OK;
}

HRESULT CObject_Manager::Clear(const _uint& _iLevelIndex)
{
	if (nullptr == m_pLayers)
		return E_FAIL;
	if (0 >= _iLevelIndex || _iLevelIndex >= m_iNumLevels)
		return E_FAIL;
	// Level Reset
	for_each(m_pLayers[_iLevelIndex].begin(), m_pLayers[_iLevelIndex].end(), [&](auto& pair)
	{
		Safe_Release(pair.second);
	});
	m_pLayers[_iLevelIndex].clear();
	return S_OK;
}

void CObject_Manager::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pLayers)
		return;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Delete_GameObjects();
		}
	}

	_double dTimeDelta = _dTimeDelta * m_fUpdateDelayTime;
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Tick(dTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pLayers)
		return;

	_double dTimeDelta = _dTimeDelta * m_fLateUpdateDelayTime;
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->LateTick(dTimeDelta);
		}
	}
}

void CObject_Manager::Out_LayerTags(const _uint& _iLevelIndex, list<const _tchar*>* _pOut)
{
	if (nullptr == _pOut)
		return;

	if (0 >= m_pLayers[_iLevelIndex].size())
		return;

	for (auto& iter : m_pLayers[_iLevelIndex])
	{
		_pOut->push_back(iter.first);
	}
}

_uint CObject_Manager::Cur_LayerSize(const _uint& _iLevel)
{
	return _uint(m_pLayers[_iLevel].size());
}

void CObject_Manager::Out_LevelUpdateTime(_float*& _pUpdate, _float*& _pLateUpdate)
{
	_pUpdate = &m_fUpdateDelayTime;
	_pLateUpdate = &m_fLateUpdateDelayTime;
}

void CObject_Manager::Out_LayerUpdateTime(const _uint& _iLevel, const _tchar*& _pLayerTypeTag, _float*& _pUpdate, _float*& _fLateUpdate)
{
	CLayer* pLayers = Find_Layer(_iLevel, _pLayerTypeTag);
	if (nullptr == pLayers)
		return;

	pLayers->Out_DelayTime(_pUpdate, _fLateUpdate);
}

CGameObject* CObject_Manager::Find_ProtoType(const _tchar*& _pProtoTypeTag)
{
	auto	iter = find_if(m_mPrototypes.begin(), m_mPrototypes.end(), CTag_Finder(_pProtoTypeTag));

	if (iter == m_mPrototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(const _uint& _iLevelIndex, const _tchar* pLayerTag)
{
	if (nullptr == m_pLayers)
		return nullptr;

	auto	iter = find_if(m_pLayers[_iLevelIndex].begin(), m_pLayers[_iLevelIndex].end(), CTag_Finder(pLayerTag));

	if (iter == m_pLayers[_iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	if (nullptr != m_pLayers)
	{
		for (_uint i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
				Safe_Release(Pair.second);

			m_pLayers[i].clear();
 		}

		Safe_Delete_Array(m_pLayers);
	}

	for (auto& Pair : m_mPrototypes)
		Safe_Release(Pair.second);

	m_mPrototypes.clear();
}
