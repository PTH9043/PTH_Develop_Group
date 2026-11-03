#pragma once
#include "CBase.h"

class CTimer;

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager);
protected:
	typedef std::unordered_map<const _tchar*, shared_ptr<CTimer>> TIMRES;

protected:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;
public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct() { return S_OK; }
	// Get Timer Delta
	_double Get_TimeDelta(const _wstring& _strTimerTag);
	HRESULT Add_Timer(const _tchar* _pTimerTag);
	void Change_CurTimer(const _wstring& _strTimerTag);
	// Find Timer
	shared_ptr<CTimer> Find_Timer(const _wstring& _strTimerTag);
private:
	TIMRES								m_Timers;
	shared_ptr<CTimer>		m_pCurTimer = nullptr;
};

