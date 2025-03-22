#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AMAINLOOP_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AMAINLOOP_H

#include "ACoreObject.h"

BEGIN(Core)
class AService;
class AGameTimer;

class AMainLoop final : public ACoreObject {
public:
	AMainLoop(OBJCON_CONSTRUCTOR, Asio::io_context& _context, SHPTR<AService> _spServerService);
	DESTRUCTOR(AMainLoop)
public:
	void RegisterTimer(_int _iTime);
	void TimerThread(const boost::system::error_code& _error);
private:
	virtual void Free() override;
private:
	Asio::steady_timer 				m_SteadyTimer;

	SHPTR<AGameTimer>			m_spGameTimer;
	WKPTR< AService>		m_wpServerService;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AMAINLOOP_H