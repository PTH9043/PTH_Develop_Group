#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERAPP_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERAPP_H

#include "ACoreApp.h"

BEGIN(Server)
class CServerAdiminstor;
/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- ServerApp을 관리하는 클래스
*/
class CServerApp : public Core::ACoreApp {
public:
	CServerApp();
	NO_COPY(CServerApp)
public:
	virtual _bool Start() override;
private:
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERAPP_H