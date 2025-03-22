#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CMONSTERJOBTIMER_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CMONSTERJOBTIMER_H

#include "AJobTimer.h"


BEGIN(Server)

class CMonsterJobTimer final : public AJobTimer {
public:
	CMonsterJobTimer(OBJCON_CONSTRUCTOR, Asio::io_context& _context);
	DESTRUCTOR(CMonsterJobTimer)
protected:
	virtual void RegisterTimer(_int _RegisterTimer) override;
	virtual void TickTimer(const TIMEREVENT& _TimerEvent) override;
private:
	virtual void Free() override;
private:

};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CMONSTERJOBTIMER_H