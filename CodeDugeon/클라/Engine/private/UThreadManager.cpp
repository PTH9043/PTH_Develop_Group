#include "EngineDefine.h"
#include "UThreadManager.h"

UThreadManager::UThreadManager() : m_RegisterThreadContainer{}/*, m_BackgroundThreadContainer{}*/
{
}

void UThreadManager::RegisterFuncToRegister(const THREADFUNC& _CallBack, void* _pData)
{
	m_RegisterThreadContainer.emplace_back(_CallBack, _pData);
}

void UThreadManager::JoinRegister()
{
	for (auto& iter : m_RegisterThreadContainer) {
		iter.join();
	}
}

void UThreadManager::DetachRegister()
{
	for (auto& iter : m_RegisterThreadContainer) {
		iter.detach();
	}
}


void UThreadManager::Free()
{
}
