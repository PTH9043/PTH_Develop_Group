#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ATHREADMANAGER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ATHREADMANAGER_H

#include "ACoreBase.h"


BEGIN(Core)
class AJobTimer;
class AMainLoop;
class ACoreInstance;
class AService;

using THREAD = std::thread;
using THRAEDVECTOR = VECTOR<THREAD>;
using JOBTIMERCONTAINER = CONUNORMAP<_int, SHPTR<AJobTimer>>;

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- Thread를 관리하는 매니저 클래스 
*/
class  AThreadManager final : public ACoreBase {
public:
	AThreadManager();
	NO_COPY(AThreadManager)
	DESTRUCTOR(AThreadManager)

public:
	void CreateMainLoop(SHPTR<ACoreInstance> _spCoreinstance, Asio::io_service& _service, SHPTR< AService> _spService);
	void RegisterFunc(const THREADFUNC& _CallBack, void* _Data);
	void RegisterJob(_int _jobType, CSHPTRREF<AJobTimer> _spJobTimer);
	SHPTR<AJobTimer> FindJobTimer(_int _JobTimer);
	void Join();
	void Detach();
public: 
	SHPTR<AMainLoop> GetMainLoop() const { return m_spMainLoop; }
private:
	static void ThreadJoin(const THREADFUNC& _CallBack, void* _Data, _uint _CurrentThreadID);
	static void InitTLS(_uint _CurrentThreadID);
	static void DestroyTLS();

	// 초기화 함수 -> UCoreBase에 기록됨 
	virtual void Free() override;
private:
	MUTEX								m_Mutex;
	THRAEDVECTOR				m_ThreadContainer;
	JOBTIMERCONTAINER		m_JobThreadContainer;

	SHPTR< AMainLoop>		m_spMainLoop;
	// Thread Num
	std::atomic< _uint>			m_CurThreadNum;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ATHREADMANAGER_H