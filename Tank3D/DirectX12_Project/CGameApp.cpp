#include "CGameApp.h"
#include "CGameInstance.h"
#include "CCamera.h"
#include "CCube.h"
#include "CTank.h"
#include "CGameScene.h"
#include "CLogoScene.h"
#include <time.h>

void CGameApp::Free()
{
	CGameInstance::Destroy_Instance();
	Safe_Free(m_pGameInstance);
}

HRESULT CGameApp::NativeConstruct(const HWND& _hWnd, const HINSTANCE& _hInst)
{
	m_pGameInstance = GET_INSTNACE(CGameInstance);

	m_hWnd = _hWnd;
	m_hInstance = _hInst;

	if (FAILED(m_pGameInstance->NativConstruct(_hWnd, _hInst))) {
		return E_FAIL;
	}

	srand(_uint(time(NULL)));

	CloneActor();
	CreateBuffer();
	return S_OK;
}

HRESULT CGameApp::CloneActor()
{
	m_pGameInstance->RegisterScene(CreateNew<CLogoScene>());
	return S_OK;
}

HRESULT CGameApp::CreateBuffer()
{
	::GetClientRect(m_hWnd, &m_rcClient);

	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
	return S_OK;
}

HRESULT CGameApp::ClearBuffer(const DWORD& _dwColor)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, _dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(_dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
	return S_OK;
}

HRESULT CGameApp::Present()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, 
		m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
	return S_OK;
}

HRESULT CGameApp::Tick(const _double& _dTimeDelta)
{
	m_pGameInstance->RunningKeyMethods(static_cast<_ubyte>(m_wParam), _dTimeDelta);
	m_dTimeDelta = _dTimeDelta;
	m_pGameInstance->Tick(_dTimeDelta);
	m_pGameInstance->Late_Tick(_dTimeDelta);
	m_wParam = 0;
	return S_OK;
}

HRESULT CGameApp::Render()
{
	ClearBuffer(RGB(200, 200, 200));
	m_pGameInstance->Render(m_hDCFrameBuffer);
	Present();
	return S_OK;
}

HRESULT CGameApp::KeyHandler(const UINT& _iMsg, const WPARAM& _wParam)
{
	switch (_iMsg) {
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_wParam = _wParam;
		break;
	}
	return S_OK;
}
