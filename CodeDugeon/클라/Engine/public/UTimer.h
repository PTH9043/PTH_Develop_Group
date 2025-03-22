#pragma once
#include "UBase.h"

BEGIN(Engine)

class UTimer : public UBase {
public:
	UTimer();
	NO_COPY(UTimer)
		DESTRUCTOR(UTimer)
public:
	const _double& GetDeltaTime() const { return m_dTimeDelta; }
public:
	virtual void Free() override;
	HRESULT NativeConstruct();
public:
	// 함수 포인터 호출
	void	 Tick() { (this->*m_pRunningFunc)(); }
	// Stop, Run을 호출
	void StopTimer();
	void RunTimer();
protected:
	void TickActive();
	void Stop();
private:
	// Timers 
	LARGE_INTEGER			m_stCurTimer;
	LARGE_INTEGER			m_stOldLTime;
	LARGE_INTEGER			m_stOriginLTime;
	LARGE_INTEGER			m_stStopLTime;
	LARGE_INTEGER			m_stCPULTime;
private:
	// Timers Defines
	void									(UTimer::* m_pRunningFunc)(void);
	_double							m_dTimeDelta;
	_bool								m_isStop;
};

END
