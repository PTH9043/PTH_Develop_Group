#pragma once
#include "UBase.h"

BEGIN(Engine)
class UTimer;

class UTimerManager : public UBase {
public:
	UTimerManager();
	NO_COPY(UTimerManager)
		DESTRUCTOR(UTimerManager)
public:
	virtual void Free() override;
	HRESULT CreateTimer(const _wstring& _wstrName);
	SHPTR<UTimer> CreateTimerAdd(const _wstring& _wstrName);
	void Tick();
	HRESULT RemoveTimer(const _wstring& _wstrName);
private:
	SHPTR<UTimer> FindTimer(const _wstring& _wstrName);

private:
	using TIMERS = UNORMAP<_wstring, SHPTR<UTimer>>;
	TIMERS			m_Timers;
};

END