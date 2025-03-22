#include "CoreDefines.h"
#include "AStressClientService.h"
#include "ASession.h"
#include "ACoreInstance.h"
namespace Core {

	AStressClientService::AStressClientService(OBJCON_CONSTRUCTOR) :
		AService(OBJCON_CONDATA, SERVICETYPE::CLIENT), 
		m_isRunningThread{true}
	{

	}

	_bool AStressClientService::NativeConstruct()	{
		return true;
	}

	_bool AStressClientService::Start()	{
		return __super::Start();
	}

	void AStressClientService::RunningThread(void* _pService)
	{
		RETURN_CHECK(nullptr == _pService, ;);
		AStressClientService* spClientService = static_cast<AStressClientService*>(_pService);
		while (true)
		{
			spClientService->Tick();
		}
	}

	SESSIONID AStressClientService::GiveID()
	{
		if (m_RemainIDQueue.empty())
		{
			return __super::GiveID();
		}
		SESSIONID SessionID{ 0 };
		m_RemainIDQueue.try_pop(SessionID);
		return SessionID;
	}

	void AStressClientService::RemoveDisconnectSockets()
	{
	}

	void AStressClientService::Free()
	{
	}

}