#pragma once
#include "UBase.h"
#include "UProcessedData.h"

BEGIN(Engine)
class UActor;
class UNetworkBaseController;

using NETWORJOBKQUERY = LIST<UProcessedData>;
using NETWORKJOBQUERYCONTAINER = ARRAY<NETWORJOBKQUERY, MAX_THREAD_CNT>;

using NETWORKINITQUERY= LIST<NETWORKRECEIVEINITDATA>;
using NETWORKINITQUERYCONTAINER = ARRAY<NETWORKINITQUERY, MAX_THREAD_CNT>;

class UNetworkQueryProcessing abstract : public UBase {
	using NETWORKACTORCONTAINER = CONUNOMAP<_int, SHPTR<UActor>>;
public:
	UNetworkQueryProcessing(CSHPTRREF<UNetworkBaseController> _spNetworkBaseController);
	DESTRUCTOR(UNetworkQueryProcessing)
public:
	void InsertQueryData(UProcessedData&& _ProcessedData);
	void ProcessQueryData();

	void InsertNetworkInitData(NETWORKRECEIVEINITDATA _NetworkRecvInitData);
	void ProcessNetworkInitData();
	void AddCreatedNetworkActor(_int _NetworkID, CSHPTRREF<UActor> _spActor);

	virtual void MakeActors(const NETWORKRECEIVEINITDATA& _NetworkRecvInitData) PURE;
public:
	SHPTR<UNetworkBaseController> GetNetworkBaseController()  { return m_wpNetworkBaseController.lock(); }
private:
	virtual void Free() override;
private:
	NETWORKJOBQUERYCONTAINER			m_NetworkJobQueryContainer;
	NETWORKINITQUERYCONTAINER			m_NetworkInitQueryContainer;
	NETWORKACTORCONTAINER					m_NetworkActorContainer;

	std::atomic_int											m_NetworkJobQueryIndex;
	std::atomic_int											m_NetworkInitQueryIndex;

	WKPTR<UNetworkBaseController>		m_wpNetworkBaseController;
};

END
