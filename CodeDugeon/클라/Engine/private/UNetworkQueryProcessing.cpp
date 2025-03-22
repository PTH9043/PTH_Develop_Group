#include "EngineDefine.h"
#include "UNetworkQueryProcessing.h"
#include "UNetworkBaseController.h"
#include "UActor.h"

UNetworkQueryProcessing::UNetworkQueryProcessing(CSHPTRREF<UNetworkBaseController>_spNetworkBaseController) : 
	m_wpNetworkBaseController{_spNetworkBaseController},
	m_NetworkJobQueryIndex{0},	m_NetworkInitQueryIndex{0}
{

}

void UNetworkQueryProcessing::InsertQueryData(UProcessedData&& _ProcessedData)
{
	_int ThreadID = m_NetworkJobQueryIndex.load();
	ThreadID = ThreadID == 0 ? 1 : 0;

	m_NetworkJobQueryContainer[ThreadID].push_back(_ProcessedData);
}

void UNetworkQueryProcessing::ProcessQueryData()
{
	_int ThreadID = m_NetworkJobQueryIndex.load();
	m_NetworkJobQueryIndex = ThreadID == 0 ? 1 : 0;
	ThreadID = m_NetworkInitQueryIndex.load();

	SHPTR<UNetworkBaseController> spNetworkBaseController = m_wpNetworkBaseController.lock();
	{
		auto& QueryContainer = m_NetworkJobQueryContainer[ThreadID];
		for (auto& iter : QueryContainer)
		{
			const auto& ActorPair = m_NetworkActorContainer.find(iter.GetDataID());
			if (m_NetworkActorContainer.end() != ActorPair)
			{
				ActorPair->second->ReceiveNetworkProcessData(std::move(iter));
			}
		}
		QueryContainer.clear();
	}
}

void UNetworkQueryProcessing::InsertNetworkInitData(NETWORKRECEIVEINITDATA _NetworkRecvInitData)
{
	_int ThreadID = m_NetworkInitQueryIndex.load();
	ThreadID = ThreadID == 0 ? 1 : 0;
	m_NetworkInitQueryContainer[ThreadID].push_back(_NetworkRecvInitData);
}

void UNetworkQueryProcessing::ProcessNetworkInitData()
{
	_int ThreadID = m_NetworkInitQueryIndex.load();
	m_NetworkInitQueryIndex = ThreadID == 0 ? 1 : 0;
	ThreadID = m_NetworkInitQueryIndex.load();
	{
		auto& InitContainer = m_NetworkInitQueryContainer[ThreadID];
		for (auto& iter : InitContainer)
		{
			MakeActors(iter);
		}
		InitContainer.clear();
	}
}

void UNetworkQueryProcessing::AddCreatedNetworkActor(_int _NetworkID, CSHPTRREF<UActor> _spActor)
{
	assert(nullptr != _spActor);
	m_NetworkActorContainer.insert(MakePair(_NetworkID, _spActor));
}

void UNetworkQueryProcessing::MakeActors(const NETWORKRECEIVEINITDATA& _NetworkRecvInitData)
{
}

void UNetworkQueryProcessing::Free()
{
}
