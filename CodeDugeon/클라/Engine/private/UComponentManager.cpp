#include "EngineDefine.h"
#include "UComponentManager.h"
#include "UComponent.h"

UComponentManager::UComponentManager()
{
}

void UComponentManager::Free()
{
	m_Prototypes.clear();
}

HRESULT UComponentManager::AddPrototype(const _wstring& _wstrPrototypes, const SHPTR<UComponent>& _spComponent)
{
	if (nullptr != FindComp(_wstrPrototypes))
		return E_FAIL;

	m_Prototypes.insert(std::pair<_wstring, SHPTR<UComponent>>(_wstrPrototypes, _spComponent));
	return S_OK;
}

SHPTR<UComponent> UComponentManager::CloneComp(const _wstring& _wstrPrototypes, const VOIDDATAS& _stDatas)
{
	SHPTR<UComponent> pComponent = FindComp(_wstrPrototypes);
	if (nullptr == pComponent)
		return nullptr;

	SHPTR<UComponent> Clones = static_pointer_cast<UComponent>(pComponent->Clone(_stDatas));
	if (nullptr == Clones)
		return nullptr;

	return Clones;
}

SHPTR<UComponent> UComponentManager::FindComp(const _wstring& _wstrPrototypes)
{
	const auto& iter = m_Prototypes.find(_wstrPrototypes);
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}
