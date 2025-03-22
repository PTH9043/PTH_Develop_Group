#pragma once

#include "UBase.h"

BEGIN(Engine)
class UGameInstance;
class UTimer;
class UDevice;
class UGpuCommand;
END

BEGIN(Client)
class CDataManager;

class CClientApp final : public UBase {
public:
	CClientApp();
	NO_COPY(CClientApp)
	DESTRUCTOR(CClientApp)
public:
	virtual void Free() override;
	HRESULT NativeConstruct(const HINSTANCE& _hInst, const _uint& _iCmdShow);

	static void ClientThread(void* _pData);

	// Tick
	void Render();
	void KeyUpdate(WPARAM _wParam);
private:
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
private:
	HINSTANCE									m_hInst;
	_uint												m_iCmdShow;
	_uint												m_iTickCount;
	HWND												m_hwnd;
	_double											m_dRenderTick;

	SHPTR<UGameInstance>			m_spGameInstance;

	SHPTR<UTimer>							m_spTickTimer;
	SHPTR<UTimer>							m_spDeltaTimer;
	SHPTR<UTimer>							m_spRenderTimer;
	SHPTR<UTimer>							m_spRenderDeltaTimer;

	_double											m_dDeltaTime;
	_double											m_dShowTickFPS;
	const _double								TICK_DOUBLE;

	const _wstring								DELTA_TIMER;
	const _wstring								TICK_TIMER;
	const _wstring								RENDER_TIMER;
	const _wstring								RENDER_DELETATIMER;

	_bool												m_isTickThread;

	SHPTR<CDataManager>  m_spDataManager;
};

END
