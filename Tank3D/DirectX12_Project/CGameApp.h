#pragma once
#include "CBase.h"

class CGameApp : public CBase
{
protected:
	explicit CGameApp() = default;
	explicit CGameApp(const CGameApp& _rhs) : CBase(_rhs) {}
	virtual ~CGameApp() = default;
public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct(const HWND& _hWnd, const HINSTANCE& _hInst);
private:
	HRESULT CloneActor();
	HRESULT CreateBuffer();
	HRESULT ClearBuffer(const DWORD& _dwColor);
	HRESULT Present();
public:
	HRESULT Tick(const _double& _dTimeDelta);
	HRESULT Render();
	HRESULT KeyHandler(const UINT& _iMsg, const WPARAM& _wParam);
private:
	HWND																m_hWnd = NULL;
	HINSTANCE													m_hInstance = NULL;
	// GameInstance
	shared_ptr<class CGameInstance>			m_pGameInstance = nullptr;

	RECT																m_rcClient{};

	HDC																	m_hDCFrameBuffer = NULL;
	HBITMAP														m_hBitmapFrameBuffer = NULL;
	HBITMAP														m_hBitmapSelect = NULL;
	// Time Delta
	_double															m_dTimeDelta{ 0. };
	WPARAM														m_wParam{ NULL };
};

