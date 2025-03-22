#include "EngineDefine.h"
#include "UActorManager.h"
#include "UPawn.h"
#include "UActorGroup.h"
#include "URenderer.h"
#include "UGameInstance.h"

UActorManager::UActorManager()
{
}

void UActorManager::Free()
{
	for (auto& iter : m_arrClones)
	{
		iter.clear();
	}
	m_vecTickClones.clear();
	m_ProtoTypes.clear();
}

void UActorManager::Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_vecTickClones) {
		iter->Tick(_dTimeDelta);
	}
}

void UActorManager::LateTick(const _double& _dTimeDelta)
{
	for (auto& iter : m_vecTickClones) {
		iter->LateTick(_dTimeDelta);
	}
}

void UActorManager::NetworkTick(const _double& _dTimeDelta)
{
	for (auto& iter : m_vecTickClones) {
		iter->NetworkTick(_dTimeDelta);
	}
}

void UActorManager::Collision(const _double& _dTimeDelta)
{

}

HRESULT UActorManager::ReadyActorManager(CSHPTRREF<URenderer> _spRenderer)
{
	m_spRenderer = _spRenderer;
	return S_OK;
}

HRESULT UActorManager::AddPrototypes(const _wstring& _wstrName, const SHPTR<UActor>& _spActor)
{
	RETURN_CHECK(nullptr != FindActor(_wstrName), E_FAIL);
	m_ProtoTypes.insert(std::pair<_wstring, SHPTR<UActor>>(_wstrName, _spActor));
	return S_OK;
}

void UActorManager::CloneActor(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	CloneActorAdd(_wstrProto, _stDatas);
}

SHPTR<UActor> UActorManager::CloneActorAdd(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	SHPTR<UActor> pInstance = FindActor(_wstrProto);
	RETURN_CHECK(nullptr == pInstance, nullptr);

	if (nullptr == pInstance->GetCloneData()) {
		return nullptr;
	}

	SHPTR<UActor> pClones{ nullptr };
	{
		//	std::lock_guard<std::mutex> LL{UGameInstance::GetMainMutex()};
		pClones = static_pointer_cast<UActor>(pInstance->Clone(_stDatas));
	}
	RETURN_CHECK(nullptr == pClones, nullptr);

	const _ubyte& bCloneType = pInstance->GetCloneData()->bCloneType;
	const auto& ActorList = m_arrClones[bCloneType].find(pInstance->GetCloneData()->wstrCloneLayer);
	if (ActorList == m_arrClones[bCloneType].end())
	{
		SHPTR<UActorGroup> pGroup = Create<UActorGroup>();
		pGroup->AddActive(pClones);
		m_arrClones[bCloneType].insert(
			std::pair<_wstring, SHPTR<UActorGroup>>(pInstance->GetCloneData()->wstrCloneLayer, pGroup));

		m_vecTickClones.push_back(pGroup);
	}
	else
	{
		ActorList->second->AddActive(pClones);
	}

	return pClones;
}

SHPTR<UActor> UActorManager::CloneActorAddAndNotInLayer(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	SHPTR<UActor> pInstance = FindActor(_wstrProto);
	RETURN_CHECK(nullptr == pInstance, nullptr);

	SHPTR<UActor> pClones = static_pointer_cast<UActor>(pInstance->Clone(_stDatas));
	RETURN_CHECK(nullptr == pClones, nullptr);

	return pClones;
}

void UActorManager::ActosTickDisable()
{
	for (auto& iter : m_arrClones)
	{
		for (auto& value : iter)
			value.second->ActorsTickDisable();
	}
}

void UActorManager::ActorsTickEnable()
{
	for (auto& iter : m_arrClones)
	{
		for (auto& value : iter)
			value.second->ActorsTickEnable();
	}
}

void UActorManager::EnableLayers(const _wstring& _wstrLayer)
{
	for (auto& iter : m_arrClones)
	{
		const auto& Find = iter.find(_wstrLayer);
		if (Find == iter.end()) continue;
		Find->second->ActorsEnable();
	}
}

void UActorManager::DisableLayers(const _wstring& _wstrLayer)
{
	for (auto& iter : m_arrClones)
	{
		const auto& Find = iter.find(_wstrLayer);
		if (Find == iter.end()) continue;
		Find->second->ActorsDisable();
	}
}

void UActorManager::RemoveActor(const SHPTR<UActor>& _spActor)
{
	{
		RETURN_CHECK(nullptr == _spActor, ;);
		const CSHPTRREF<const CLONEDATA> cloneData = _spActor->GetCloneData();
		const _ubyte& iCloneType = cloneData->bCloneType;
		const auto& ActorList = m_arrClones[iCloneType].find(cloneData->wstrCloneLayer);
		ActorList->second->AddDelete(_spActor);
	}
}

void UActorManager::ClearOnceTypeData()
{
	_ubyte bOnce = (_ubyte)CLONETYPE::CLONE_ONCE;
	m_arrClones[bOnce].clear();
	m_vecTickClones.clear();

	for (auto& Layer : m_arrClones[(_ubyte)CLONETYPE::CLONE_STATIC])
	{
		m_vecTickClones.push_back(Layer.second);
	}
}

SHPTR<UActor> UActorManager::FindActor(const _wstring& _wstrPrototypeNames)
{
	const auto& iter = m_ProtoTypes.find(_wstrPrototypeNames);
	if (iter == m_ProtoTypes.end())
		return nullptr;

	return (*iter).second;
}
