#pragma once
#include "UNetworkQueryProcessing.h"


BEGIN(Engine)

END

BEGIN(Client)

class CNetworkQueryProcessor final : public UNetworkQueryProcessing{
public:
	CNetworkQueryProcessor(CSHPTRREF<UNetworkBaseController> _spNetworkBaseController);
	DESTRUCTOR(CNetworkQueryProcessor)

public:
	virtual void MakeActors(const NETWORKRECEIVEINITDATA& _NetworkRecvInitData) override;
private:
	virtual void Free() override;
};

END

