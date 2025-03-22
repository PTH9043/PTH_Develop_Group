#pragma once
#include "Base.h"
#include<thread>
#include<mutex>

BEGIN(Engine)

class CThread_Manager final : public CBase
{
	DECLARE_SINGLETON(CThread_Manager)
private:
	explicit CThread_Manager();
	virtual ~CThread_Manager() = default;
public:
	// Add Thread 
	// 스레드 아이디를 저장할 변수를 리턴함
	HRESULT Add_Thread(function<void(void*)> _pData, void* _pArg);
	// Join Thread
	HRESULT Join_Thread();
	// DetachThread
	HRESULT Detach_Thread();
	// End All Thread
	HRESULT End_Threads();

	_uint Get_ThreadCount() { return (_uint)m_Threads.size(); }
protected:
	typedef list<thread> THREADS;
	list<thread>  m_Threads;
protected:
	// mutex
	mutex	m_Mutex;
public:
	// Free
	virtual void Free() override;
};

END