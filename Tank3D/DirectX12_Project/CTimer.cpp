#include "CTimer.h"

CTimer::CTimer() : m_dTimeDelta(0.0)
{
    ZeroMemory(&m_CurrentLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_OldLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_OriginLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_StopLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_CPULTime, sizeof(LARGE_INTEGER));
}

CTimer::CTimer(const CTimer& _rhs) : CBase(_rhs), m_dTimeDelta(0.)
{
    ZeroMemory(&m_CurrentLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_OldLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_OriginLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_StopLTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_CPULTime, sizeof(LARGE_INTEGER));
}

void CTimer::Free()
{
}

HRESULT CTimer::NativeConstruct()
{
    //메인보드가 갖고 있는 고해상도 타이머의 누적값을 얻어오는 함수
    QueryPerformanceCounter(&m_CurrentLTime);		// 1000
    QueryPerformanceCounter(&m_OldLTime);			// 1020	
    QueryPerformanceCounter(&m_OriginLTime);			// 1030	
    QueryPerformanceCounter(&m_StopLTime);			// 1030	
    // 고해상도 타이머의 주파수를 얻어오는 함수, 주파수는 cpu 초당 클럭수 주기를 말함
    QueryPerformanceFrequency(&m_CPULTime);			// 1600000
    m_pRunningFunc = &CTimer::Tick;
    return S_OK;
}

void CTimer::Tick()
{
    QueryPerformanceCounter(&m_CurrentLTime);
    if (m_CurrentLTime.QuadPart - m_OriginLTime.QuadPart > m_CPULTime.QuadPart)
    {
        QueryPerformanceFrequency(&m_CPULTime);
        m_OriginLTime = m_CurrentLTime;
    }
    m_dTimeDelta = static_cast<_double>(m_CurrentLTime.QuadPart - m_OldLTime.QuadPart)
        / static_cast<_double>(m_CPULTime.QuadPart);
    m_OldLTime = m_CurrentLTime;
}

void CTimer::Stop()
{
    QueryPerformanceCounter(&m_StopLTime);
    if (m_StopLTime.QuadPart - m_OriginLTime.QuadPart > m_CPULTime.QuadPart)
    {
        QueryPerformanceFrequency(&m_CPULTime);
        m_OriginLTime = m_StopLTime;
    }
    m_dTimeDelta = static_cast<_double>(m_StopLTime.QuadPart - m_OldLTime.QuadPart) / m_CPULTime.QuadPart;
}

void CTimer::Stop_Timer(){
    m_pRunningFunc = &CTimer::Stop;
}

void CTimer::Run_Timer(){
    m_pRunningFunc = &CTimer::Tick;
}
