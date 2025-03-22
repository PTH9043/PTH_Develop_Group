#include "..\Public\Thread_Manager.h"

IMPLEMENT_SINGLETON(CThread_Manager)

CThread_Manager::CThread_Manager() 
{

}
// 스레드를 참가하고  JOIN 하는 함수
HRESULT CThread_Manager::Add_Thread(function<void(void*)> _pData, void* _pArg)
{
	m_Threads.push_back(thread(_pData, _pArg));
	return S_OK;
}
HRESULT CThread_Manager::Join_Thread()
{
	for (auto& iter : m_Threads)
	{
		m_Mutex.lock();
		iter.join();
		m_Mutex.unlock();
	}
	return S_OK;
}
HRESULT CThread_Manager::Detach_Thread()
{
	for (auto& iter : m_Threads)
	{
		m_Mutex.lock();
		iter.detach();
		m_Mutex.unlock();
	}
	return S_OK;
}

HRESULT CThread_Manager::End_Threads()
{
	// 스레드 모두 지움 
	m_Threads.clear();
	return S_OK;
}


void CThread_Manager::Free()
{
	m_Threads.clear();
}

