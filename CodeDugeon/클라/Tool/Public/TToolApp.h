#pragma once

#include "UBase.h"

BEGIN(Engine)
class UGameInstance;
class UTimer;
class UDevice;
class UGpuCommand;
END

BEGIN(Tool)
class TImGuiManager;

class TToolApp : public UBase {
public:
	TToolApp();
	NO_COPY(TToolApp)
		DESTRUCTOR(TToolApp)
public:
	virtual void Free() override;
	HRESULT NativeConstruct(const HWND& _hWnd, const HINSTANCE& _hInst);
	void KeyUpdate(WPARAM _wParam);
	void Tick();
private:
	_uint										m_iTickCount;
	HWND										m_hwnd;
	_double									m_dRenderTick;

	SHPTR<UGameInstance>	m_spGameInstance;
	SHPTR<TImGuiManager>	m_spImGuiManager;

	SHPTR<UTimer>					m_spTickTimer;
	SHPTR<UTimer>					m_spDeltaTimer;
	SHPTR<UTimer>					m_spRenderTimer;

	_double									m_dDeltaTime;
	const _double						TICK_DOUBLE;
	_double									m_dShowTickFPS;

	const _wstring						DELTA_TIMER;
	const _wstring						TICK_TIMER;
	const _wstring						RENDER_TIMER;

	std::thread							m_TickThread;
};

END