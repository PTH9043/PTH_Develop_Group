#include "EngineDefine.h"
#include "UTimerManager.h"
#include "UTimer.h"
#include "UGameInstance.h"

UTimerManager::UTimerManager()
{
}

void UTimerManager::Free()
{
    m_Timers.clear();
}

HRESULT UTimerManager::CreateTimer(const _wstring& _wstrName)
{
    RETURN_CHECK(nullptr == CreateTimerAdd(_wstrName), E_FAIL);
    return S_OK;
}

SHPTR<UTimer> UTimerManager::CreateTimerAdd(const _wstring& _wstrName)
{
    RETURN_CHECK(FindTimer(_wstrName) != nullptr, nullptr);
    SHPTR<UTimer> pTimer{ CreateNative<UTimer>() };
    m_Timers.insert(std::pair<_wstring, SHPTR<UTimer>>(_wstrName, pTimer));
    return pTimer;
}

void UTimerManager::Tick()
{
    for (auto [name, timer] : m_Timers)
    {
        timer->Tick();
    }
}

HRESULT UTimerManager::RemoveTimer(const _wstring& _wstrName)
{
    RETURN_CHECK(0 == m_Timers.erase(_wstrName), E_FAIL);
    return S_OK;
}

SHPTR<UTimer> UTimerManager::FindTimer(const _wstring& _wstrName)
{
    TIMERS::iterator it = m_Timers.find(_wstrName);
    if (it == m_Timers.end())
        return nullptr;

    return (*it).second;
}
