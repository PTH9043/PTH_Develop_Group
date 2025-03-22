#include "EngineDefine.h"
#include "UWorkThread.h"

UWorkThread::UWorkThread(const THREADFUNC& _threadFunc, void* _pReigsterPointer) : 
	m_spThread{Create<THREAD>(_threadFunc, _pReigsterPointer)}
{
}

void UWorkThread::StartFunc()
{
	m_spThread->detach();
}

void UWorkThread::Free()
{
	m_spThread.reset();
}
