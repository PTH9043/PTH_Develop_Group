#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AJOBTIMER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AJOBTIMER_H

#include "ACoreObject.h"

BEGIN(Core)

using TIMEREVENTQUEUE = CONPRIORITYQUEUE<TIMEREVENT>;

/*
@ Date: 2024-01-23, Writer: 박태현
@ Explain
- 일을 생성하는 클래스로 해당 클래스를 통해 몬스터의 AI, 실시간으로 DB에 저장이 필요한 값들을 
저장할 수 있다. 
*/
class CORE_DLL AJobTimer abstract : public ACoreObject {
public:
	AJobTimer(OBJCON_CONSTRUCTOR, Asio::io_context& _context);
	DESTRUCTOR(AJobTimer)
public:
	virtual void RegisterTimer(_int _RegisterTimer);
	void TimerThread(const boost::system::error_code& _error);
	void InsertTimerEvent(TIMEREVENT _TimerEvent);
protected:
	virtual void TickTimer(const TIMEREVENT& _TimerEvent) PURE;
protected: /* get set*/
	Asio::steady_timer& GetSteadyEvent() { return m_SteadyEvent; }
private:
	virtual void Free() override;
private:
	Asio::steady_timer 			m_SteadyEvent;
	TIMEREVENTQUEUE		m_TimerEventQueue;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AJOBTIMER_H