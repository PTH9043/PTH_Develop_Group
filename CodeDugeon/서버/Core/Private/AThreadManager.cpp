#include "CoreDefines.h"
#include "AThreadManager.h"
#include "AJobTimer.h"
#include "AMainLoop.h"

namespace Core
{
	AThreadManager::AThreadManager() : m_CurThreadNum{0}
	{
	}
	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	*/
	void AThreadManager::CreateMainLoop(SHPTR<ACoreInstance> _spCoreinstance, Asio::io_service& _service, SHPTR< AService> _spService)
	{
		m_spMainLoop = Create<AMainLoop>(_spCoreinstance, _service, _spService);
	}

	void AThreadManager::RegisterFunc(const THREADFUNC& _CallBack, void* _Data)
	{
		RETURN_CHECK(m_CurThreadNum >= TLS::MAX_THREAD, ;);

		LOCKGUARD<MUTEX> Lock{ m_Mutex };
		m_ThreadContainer.emplace_back(ThreadJoin, _CallBack, _Data, m_CurThreadNum++);
	}

	void AThreadManager::RegisterJob(_int _jobType, CSHPTRREF<AJobTimer> _spJobTimer)
	{
		assert(nullptr != _spJobTimer);
		m_JobThreadContainer.emplace(MakePair(_jobType, _spJobTimer));
	}

	SHPTR<AJobTimer> AThreadManager::FindJobTimer(_int _JobTimer)
	{
		const auto& iter = m_JobThreadContainer.find(_JobTimer);
		RETURN_CHECK(m_JobThreadContainer.end() == iter, nullptr);
		return iter->second;
	}

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	*/
	void AThreadManager::Join()
	{
	//	m_spMainLoop->RegisterTimer(1);

		for (auto& iter : m_JobThreadContainer)
		{
			iter.second->RegisterTimer(1);
		}

		for (auto& iter : m_ThreadContainer)
		{
			iter.join();
		}
	}
	void AThreadManager::Detach()
	{
		for (auto& iter : m_ThreadContainer)
		{
			iter.detach();
		}
	}
	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	*/
	void AThreadManager::ThreadJoin(const THREADFUNC& _CallBack, void* _Data, _uint _CurrentThreadID)
	{
		InitTLS(_CurrentThreadID);
		_CallBack(_Data);
		DestroyTLS();
	}
	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	*/
	void AThreadManager::InitTLS(_uint _CurrentThreadID)
	{
		TLS::g_ThreadID = _CurrentThreadID;
	}
	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	*/
	void AThreadManager::DestroyTLS()
	{

	}

	void AThreadManager::Free()
	{
		m_JobThreadContainer.clear();
		m_ThreadContainer.clear();
	}
}