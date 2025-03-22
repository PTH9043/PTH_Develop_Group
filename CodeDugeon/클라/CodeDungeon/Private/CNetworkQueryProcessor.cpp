#include "ClientDefines.h"
#include "CNetworkQueryProcessor.h"
#include "UGameInstance.h"
#include "CWarriorPlayer.h"
#include "CNetworkClientController.h"
#include "UMethod.h"
#include "CMummy.h"
#include "CSarcophagus.h"

CNetworkQueryProcessor::CNetworkQueryProcessor(CSHPTRREF<UNetworkBaseController> _spNetworkBaseController)
	: UNetworkQueryProcessing(_spNetworkBaseController)
{
}

void CNetworkQueryProcessor::MakeActors(const NETWORKRECEIVEINITDATA& _NetworkRecvInitData)
{
#ifdef _ENABLE_PROTOBUFF
	switch (_NetworkRecvInitData.iType)
	{
	case TAG_CHAR::TAG_OTHERPLAYER:
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		{
			CWarriorPlayer::CHARACTERDESC CharDesc{ PROTO_RES_FEMAILPLAYERANIMMODEL, PROTO_COMP_NETWORKWARRIORANIMCONTROLLER, true };
			SHPTR<CWarriorPlayer> spWarriorPlayer = std::static_pointer_cast<CWarriorPlayer>(spGameInstance->CloneActorAdd(
				PROTO_ACTOR_WARRIORPLAYER, { &CharDesc }));

			spGameInstance->AddCollisionPawnList(spWarriorPlayer);
			GetNetworkBaseController()->AddCreatedNetworkActor(_NetworkRecvInitData.iNetworkID, spWarriorPlayer);
		}
	}
	break;
	}
#endif
}

void CNetworkQueryProcessor::Free()
{
}
