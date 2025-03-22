#pragma once

#include "CBase.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	_double Get_TimeDelta(void) {
		return m_dTimeDelta; }

public:
	HRESULT	Initialize();
	void Update(void);

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_dTimeDelta;

public:
	static CTimer* Create();
	virtual void Free() override;
};

END
