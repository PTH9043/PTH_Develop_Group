#include "CComponent_Manager.h"
#include "CComponent.h"


void CComponent_Manager::Free()
{
	for (auto& iter : m_Prototypes)
		Safe_Free(iter.second);
	m_Prototypes.clear();
}

shared_ptr<CComponent> CComponent_Manager::Find_Comp(const _wstring& _strPrototypes)
{
	const auto& iter = m_Prototypes.find(_strPrototypes);
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Add_Prototypes(const _wstring& _strPrototypes, const shared_ptr<CComponent>& _pComponent)
{
	if (nullptr != Find_Comp(_strPrototypes))
		return;

	m_Prototypes.emplace(pair<_wstring, shared_ptr<CComponent>>(_strPrototypes, _pComponent));
}

shared_ptr<CComponent> CComponent_Manager::Clone(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas)
{
	shared_ptr<CComponent> pComponent = Find_Comp(_strPrototypes);
	if (nullptr == pComponent)
		return nullptr;

	shared_ptr<CComponent> Clones = pComponent->Clone(_tDatas);
	if (nullptr == Clones)
		return nullptr;

	return Clones;
}
