#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer() 
{
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

void CLayer::Delete_GameObjects()
{
	if (m_lsDelObjList.size() > 0)
	{
		for (auto& iter : m_lsDelObjList)
		{
			for (list<CGameObject*>::iterator it = m_lsObjectList.begin(); it != m_lsObjectList.end();)
			{
				if ((*it) == iter)
				{
					Safe_Release(iter);
					it = m_lsObjectList.erase(it);
				}
				else
					++it;
			}
		}

		m_lsDelObjList.clear();
	}
}

void CLayer::Tick(const _double& _dTimeDelta)
{
	// Tick
	_double dTimeDelta = _dTimeDelta * m_fUpdateDelayTime;
	for (auto& pGameObject : m_lsObjectList)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(dTimeDelta);
	}
}

void CLayer::LateTick(const _double& _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_fLateUpdateDelayTime;
	for (list<CGameObject*>::iterator it = m_lsObjectList.begin(); it != m_lsObjectList.end(); ++it)
	{
		if (nullptr == *it)
			continue;
		(*it)->Late_Tick(_dTimeDelta);
	}
}

void CLayer::Out_DelayTime(_float*& _pUpdate, _float*& _pLateUpdate)
{
	_pUpdate = &m_fUpdateDelayTime;
	_pLateUpdate = &m_fLateUpdateDelayTime;
}

CComponent* CLayer::Get_Component(const _tchar* pComponentTag, const _uint& _iIndex)
{
	if (0 == m_lsObjectList.size())
		return nullptr;

	auto	iter = m_lsObjectList.begin();

	for (_uint i = 0; i < _iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

CGameObject* CLayer::Find_GameObject(const _uint& _iIndex)
{
	if (m_lsObjectList.size() - 1 < _iIndex)
		return nullptr;

	list<CGameObject*>::iterator	iter = m_lsObjectList.begin();
	for (_uint i = 0; i < _iIndex; ++i)
		++iter;
	return (*iter);
}

void CLayer::Add_DeleteObject(CGameObject*& _pGameObject)
{
	if (nullptr == _pGameObject)
		return;

	m_lsDelObjList.push_back(_pGameObject);
}

HRESULT CLayer::Add_GameObject(CGameObject*& pGameObject)
{
	m_lsObjectList.push_back(pGameObject);
	return S_OK;
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for_each(m_lsObjectList.begin(), m_lsObjectList.end(), [&](auto& pair) {Safe_Release(pair); });
	m_lsObjectList.clear();
}

