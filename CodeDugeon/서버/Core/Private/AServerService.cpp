#include "CoreDefines.h"
#include "AServerService.h"
#include "ASession.h"
#include "ACoreInstance.h"
#include "ANavigation.h"

namespace Core {

	AServerService::AServerService(OBJCON_CONSTRUCTOR, const _string& _strNavigationPath) :
		AService(OBJCON_CONDATA, SERVICETYPE::SERVER), 
		m_TcpAcceptor{ GetIOContext(),Asio::ip::tcp::endpoint(Asio::ip::tcp::v4(),TCP_PORT_NUM) }, 
		m_spNavigation{nullptr}
	{

	}

	_bool AServerService::NativeConstruct()
	{
		return true;
	}

	_bool AServerService::Start()
	{
		__super::Start();
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();

		for (_uint i = 0; i < TLS::MAX_WORKTHREAD; ++i)
		{
			spCoreInstance->RegisterFunc(AServerService::ThreadFunc, GetIOConectPointer());
		}
		spCoreInstance->Join();
		return true;
	}

	void AServerService::ThreadFunc(void* _spService)
	{
		RETURN_CHECK(nullptr == _spService, ;);

		IOContext* pService = static_cast<IOContext*>(_spService);
		// Running 
		// ½ÇÇà 
		pService->run();
	}

	void AServerService::Free()
	{
		m_TcpAcceptor.close();
	}
}