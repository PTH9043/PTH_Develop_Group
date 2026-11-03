#pragma once
#include "CBase.h"

class CTimer : public CBase
{
protected:
	explicit CTimer();
	CTimer(const CTimer& _rhs);
	virtual ~CTimer() = default;
public:
	virtual void Free() override;
public:
	// 초기화
	HRESULT NativeConstruct();
protected:
	void Tick();
	void Stop();
public:
	// 함수 포인터 호출
	void	 Run_Func() { (this->*m_pRunningFunc)(); }
	// Stop, Run을 호출
	void Stop_Timer();
	void Run_Timer();


public: // Get Set 
	const _double& Get_TimeDelta() const { return m_dTimeDelta; }

private:
	LARGE_INTEGER		m_CurrentLTime = LARGE_INTEGER();
	LARGE_INTEGER		m_OldLTime = LARGE_INTEGER();
	LARGE_INTEGER		m_OriginLTime = LARGE_INTEGER();
	LARGE_INTEGER		m_StopLTime = LARGE_INTEGER();
	LARGE_INTEGER		m_CPULTime = LARGE_INTEGER();
private:
	_double						m_dTimeDelta = 0.0;
	_bool							m_bStop = false;
	// Run Function
	void								(CTimer::* m_pRunningFunc)(void);
};

