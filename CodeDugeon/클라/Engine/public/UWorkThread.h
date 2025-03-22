#pragma once

#include "UBase.h"


BEGIN(Engine)

using THREAD = std::thread;
/*
@ Date: 2024-02-02, Writer: 박태현
@ Explain
- Background 쓰레드나 기타 다른 백 그라운드 작업에 필요한 클래스를 정의
*/
class UWorkThread final : public UBase {
public:
	UWorkThread(const THREADFUNC& _threadFunc, void* _pReigsterPointer);
	NO_COPY(UWorkThread)
	DESTRUCTOR(UWorkThread)
public:
	void StartFunc();
private:
	virtual void Free() override;
private:
	SHPTR<THREAD>	m_spThread;
};

END

