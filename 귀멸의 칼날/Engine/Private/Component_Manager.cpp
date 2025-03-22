#include "..\Public\Component_Manager.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager() : CBase()
{

}

HRESULT CComponent_Manager::Reserve_Container(const _uint& iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(const _uint& iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (iLevelIndex >= m_iNumLevels || nullptr == m_pPrototypes || nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(const _uint& iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CComponent* CComponent_Manager::Get_ProtoComp(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag)
{
	if (_iLevelIndex >= m_iNumLevels || nullptr == m_pPrototypes)
		return nullptr;

	return Find_Prototype(_iLevelIndex, _pPrototypeTag);
}

HRESULT CComponent_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pPrototypes[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pPrototypes[iLevelIndex].clear();

	return S_OK;
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
