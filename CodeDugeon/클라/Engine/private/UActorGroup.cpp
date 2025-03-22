#include "EngineDefine.h"
#include "UActorGroup.h"
#include "UActor.h"
#include "UGameInstance.h"

UActorGroup::UActorGroup()
{
}

void UActorGroup::Free()
{
	m_lsDeleteActors.clear();
	m_lsActiveActors.clear();
}

void UActorGroup::Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_lsDeleteActors)
	{
		const auto& findIter = m_lsActiveActors.find(iter);
		m_lsActiveActors.erase(findIter);
	}
	m_lsDeleteActors.clear();

	for (auto& iter : m_lsActiveActors)
		iter->AwakeTick(_dTimeDelta);

	for (auto& iter : m_lsActiveActors)
		iter->Tick(_dTimeDelta);
}

void UActorGroup::LateTick(const _double& _dTimeDelta)
{
	for (auto& iter : m_lsActiveActors)
		iter->LateTick(_dTimeDelta);
}

void UActorGroup::NetworkTick(const _double& _dTimeDelta)
{
	for (auto& iter : m_lsActiveActors)
		iter->NetworkTick(_dTimeDelta);
}

void UActorGroup::AddActive(CSHPTRREF<UActor> _pActor)
{
	m_lsActiveActors.insert(_pActor);
}

void UActorGroup::AddDelete(CSHPTRREF<UActor> _pActor)
{
	m_lsDeleteActors.insert(_pActor);
}

void UActorGroup::ActorsTickDisable()
{
	for (auto& iter : m_lsActiveActors)
		iter->SetTickActive(false);
}

void UActorGroup::ActorsTickEnable()
{
	for (auto& iter : m_lsActiveActors)
		iter->SetTickActive(true);
}

void UActorGroup::ActorsDisable()
{
	for (auto& iter : m_lsActiveActors)
		iter->SetActive(false);
}

void UActorGroup::ActorsEnable()
{
	for (auto& iter : m_lsActiveActors)
		iter->SetActive(true);
}
