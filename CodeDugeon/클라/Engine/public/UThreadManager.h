#pragma once

#include "UBase.h"

BEGIN(Engine)

using THREAD = std::thread;
using THREADVECTOR = VECTOR<THREAD>;
using THREADLIST = LIST<THREAD>;
/*
@ Date: 2024-02-02, Writer: 박태현
@ Explain
- Thread를 관리하는 매니저 클래스, 등록된 함수들을 JOIN 혹은 Detach로 실행한다. 
*/
class UThreadManager final : public UBase {
public:
	UThreadManager();
	NO_COPY(UThreadManager)
	DESTRUCTOR(UThreadManager)
public:
	void RegisterFuncToRegister(const THREADFUNC& _CallBack, void* _pData);
	void JoinRegister();
	void DetachRegister();
private:
	virtual void Free() override;
private:
	THREADVECTOR		m_RegisterThreadContainer;

};

END