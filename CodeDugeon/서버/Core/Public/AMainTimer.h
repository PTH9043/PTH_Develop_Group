#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AMAINTIMER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AMAINTIMER_H

#include "AJobTimer.h"

BEGIN(Core)

using JOBCONTAINER = VECTOR<WKPTR<AJobTimer>>;

class AMainTimer final : public AJobTimer {
public:
	AMainTimer(OBJCON_CONSTRUCTOR, Asio::io_service& _service);
	DESTRUCTOR(AMainTimer)
protected:
	virtual void TickTimer(const TIMEREVENT& _TimerEvent) override;
private:
	virtual void Free() override;
private:

};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AMAINTIMER_H