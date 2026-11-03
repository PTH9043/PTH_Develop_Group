#include "CTimer.h"
#include "CTimer_Manager.h"


IMPLEMENT_SINGLETON(CTimer_Manager);

CTimer_Manager::CTimer_Manager()
{
}

void CTimer_Manager::Free()
{
    for (auto& iter : m_Timers)
        Safe_Free(iter.second);
}

_double CTimer_Manager::Get_TimeDelta(const _wstring& _strTimerTag)
{
   shared_ptr<CTimer> pTimer = Find_Timer(_strTimerTag);
    return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _tchar* _pTimerTag)
{
    shared_ptr<CTimer> pTimer = CreateNew<CTimer>();

    RETURN_CHECK(nullptr == pTimer, E_FAIL);
    m_Timers.emplace(pair<const _tchar*, shared_ptr<CTimer>>(_pTimerTag, pTimer));
    return S_OK;
}

void CTimer_Manager::Change_CurTimer(const _wstring& _strTimerTag)
{
    RETURN_CHECK_RUNVALUES(nullptr == m_pCurTimer, Find_Timer(_strTimerTag); , );
}

shared_ptr<CTimer> CTimer_Manager::Find_Timer(const _wstring& _strTimerTag)
{
    auto	iter = find_if(m_Timers.begin(), m_Timers.end(), [&](auto& pair)->
        bool {if (_strTimerTag == pair.first) return true; return false; });

    if (iter == m_Timers.end())
        return nullptr;

    return iter->second;
}
