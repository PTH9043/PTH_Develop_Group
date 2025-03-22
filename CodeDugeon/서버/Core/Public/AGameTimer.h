#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AGAMETIMER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AGAMETIMER_H

#include "ACoreBase.h"

BEGIN(Core)

class CORE_DLL AGameTimer final : public ACoreBase {
public:
	AGameTimer();
	DESTRUCTOR(AGameTimer)
public:
	// 함수 포인터 호출
	void	 Tick() { (this->*m_pRunningFunc)(); }
	// Stop, Run을 호출
	void StopTimer();
	void RunTimer();
public: /* get set*/
	const _double GetDeltaTime() const { return m_dTimeDelta; }
protected:
	void TickActive();
	void Stop();
private:
	virtual void Free() override;
private:
	// Timers 
	LARGE_INTEGER			m_stCurTimer;
	LARGE_INTEGER			m_stOldLTime;
	LARGE_INTEGER			m_stOriginLTime;
	LARGE_INTEGER			m_stStopLTime;
	LARGE_INTEGER			m_stCPULTime;

	// Timers Defines
	void									(AGameTimer::* m_pRunningFunc)(void);
	_double							m_dTimeDelta;
	_bool								m_isStop;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AGAMETIMER_H
