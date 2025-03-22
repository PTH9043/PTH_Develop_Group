#include "..\Public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}

_bool CInput_Device::Get_DIKeyDown(const _ubyte& _eKeyID)
{
	if (!m_byPreKeyState[_eKeyID] && m_byKeyState[_eKeyID])
		return true;
	return false;
}

_bool CInput_Device::Get_DIKeyUp(const _ubyte& _eKeyID)
{
	if (m_byPreKeyState[_eKeyID] && !m_byKeyState[_eKeyID])
		return true;
	return false;
}

_bool CInput_Device::Get_DIKeyPressing(const _ubyte& _eKeyID)
{
	return ((m_byKeyState[_eKeyID] & 0x80)) > 0;
}

_bool CInput_Device::Get_DIMBtnDown(const DIMOUSEBUTTON& _eMouseBtn)
{
	_float2 v2MosuePos = Get_MousePosition();
	if (m_tClientRect.left <= v2MosuePos.x && m_tClientRect.top <= v2MosuePos.y)
	{
		if (m_tClientRect.right >= v2MosuePos.x && m_tClientRect.bottom >= v2MosuePos.y)
		{
			if (!m_PreMouseState.rgbButtons[_eMouseBtn] && m_MouseState.rgbButtons[_eMouseBtn])
				return true;
			//if (!m_bMouseInput[_eMouseBtn] && (m_MouseState.rgbButtons[_eMouseBtn] & 0x80)) // < 0
			//{
			//	m_bMouseInput[_eMouseBtn] = !m_bMouseInput[_eMouseBtn];
			//	return true;
			//}
			//if (m_bMouseInput[_eMouseBtn] && !(m_MouseState.rgbButtons[_eMouseBtn] & 0x80)) // > 0
			//	m_bMouseInput[_eMouseBtn] = !m_bMouseInput[_eMouseBtn];

		}
	}
	return false;
}

_bool CInput_Device::Get_DIMBtnUp(const DIMOUSEBUTTON& _eMouseBtn)
{
	_float2 v2MosuePos = Get_MousePosition();
	if (m_tClientRect.left <= v2MosuePos.x && m_tClientRect.top <= v2MosuePos.y)
	{
		if (m_tClientRect.right >= v2MosuePos.x && m_tClientRect.bottom >= v2MosuePos.y)
		{
			if (m_PreMouseState.rgbButtons[_eMouseBtn] && !m_MouseState.rgbButtons[_eMouseBtn])
				return true;
			/*if (m_bMouseInputUp[_eMouseBtn] && !(m_MouseState.rgbButtons[_eMouseBtn] & 0x81))
			{
				m_bMouseInputUp[_eMouseBtn] = !m_bMouseInputUp[_eMouseBtn];
				return true;
			}
			if (!m_bMouseInputUp[_eMouseBtn] && (m_MouseState.rgbButtons[_eMouseBtn] & 0x81))
				m_bMouseInputUp[_eMouseBtn] = !m_bMouseInputUp[_eMouseBtn];*/
		}
	}
	return false;
}

_bool CInput_Device::Get_DIMBtnPressing(const DIMOUSEBUTTON& _eMouseBtn)
{
	_float2 v2MosuePos = Get_MousePosition();
	if (m_tClientRect.left <= v2MosuePos.x && m_tClientRect.top <= v2MosuePos.y)
	{
		if (m_tClientRect.right >= v2MosuePos.x && m_tClientRect.bottom >= v2MosuePos.y)
		{
			return (_bool)(m_MouseState.rgbButtons[_eMouseBtn] && 0x80);
		}
	}
	return false;
}

_float2 CInput_Device::Get_MousePosition()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	return _float2(_float(pt.x), _float(pt.y));
}

_int CInput_Device::Judge_MouseOverCenterX(const _float& _fMousePosX, const _float& _fValue)
{
	_float fCenter = static_cast<_float>(m_tClientRect.right - m_tClientRect.left) / 2.f;
	if (fCenter - _fValue > _fMousePosX)
		return -1;

	if (fCenter + _fValue < _fMousePosX)
		return 1;

	return 0;
}

_int CInput_Device::Judge_MouseOverCenterY(const _float& _fMousePosY, const _float& _fValue)
{
	_float fCenter = static_cast<_float>(m_tClientRect.bottom - m_tClientRect.top) / 2.f;
	if (fCenter - _fValue > _fMousePosY)
		return -1;

	if (fCenter + _fValue < _fMousePosY)
		return 1;

	return 0;
}

void CInput_Device::Set_CursorLock(const CURSOR_LOCKMODE& eLockMode)
{
	if (eLockMode >= CURSOR_END) return;

	if (eLockMode == CURSOR_LOCKED)
	{
		RECT	rcDest;
		GetClientRect(m_hWnd, &rcDest); // get client size(wincx * wincy)

		POINT	ptDest;
		ptDest.x = (_long)((_float)rcDest.right * .5f);
		ptDest.y = (_long)((_float)rcDest.bottom * .5f);

		ClientToScreen(m_hWnd, &ptDest);// client -> window

		rcDest.left = ptDest.x;
		rcDest.top = ptDest.y;
		rcDest.right = ptDest.x;
		rcDest.bottom = ptDest.y;

		//SetCursorPos(ptDest.x, ptDest.y);	
		ClipCursor(&rcDest);			// clamp : input window pos
		ShowCursor(false);
	}
	else if (eLockMode == CURSOR_CONFINED)
	{
		RECT	rcDest;
		GetClientRect(m_hWnd, &rcDest);

		POINT	ptMin, ptMax;
		ptMin.x = rcDest.left;
		ptMin.y = rcDest.top;
		ptMax.x = rcDest.right;
		ptMax.y = rcDest.bottom;

		ClientToScreen(m_hWnd, &ptMin);
		ClientToScreen(m_hWnd, &ptMax);

		rcDest.left = ptMin.x;
		rcDest.top = ptMin.y;
		rcDest.right = ptMax.x;
		rcDest.bottom = ptMax.y;

		ClipCursor(&rcDest);
		ShowCursor(false);
	}
	else if (eLockMode == CORSOR_NONE)
	{
		ClipCursor(nullptr);
		ShowCursor(true);
	}

	m_eCursurMode = eLockMode;
}

_uint CInput_Device::Get_KeyBoardInputs_InContainers(const _ubyte& _eByte)
{
	_uint iCount = 0;
	for (list<_ubyte>::iterator it = m_KeyInputContainer.begin(); it != m_KeyInputContainer.end(); ++it)
	{
		if (*it == _eByte)
			++iCount;
	}
	return iCount;
}

_uint CInput_Device::Get_MouseButtonInputs_InContainers(const DIMOUSEBUTTON& _eMouseButton)
{
	_uint iCount = 0;
	for (list<DIMOUSEBUTTON>::iterator it = m_MouseInputContainer.begin(); it != m_MouseInputContainer.end(); ++it)
	{
		if (*it == _eMouseButton)
		{
			++iCount;
		}
	}
	return iCount;
}

_bool CInput_Device::Is_KeyBoardInput_InContainers(const _ubyte& _eByte)
{
	for (list<_ubyte>::iterator it = m_KeyInputContainer.begin(); it != m_KeyInputContainer.end(); )
	{
		if (*it == _eByte)
		{
			it = m_KeyInputContainer.erase(it);
			return true;
		}
		else
			++it;
	}
	return false;
}

_bool CInput_Device::Is_MouseButtonInputs_InContainers(const DIMOUSEBUTTON& _eMouseButton)
{
	for (list<DIMOUSEBUTTON>::iterator it = m_MouseInputContainer.begin(); it != m_MouseInputContainer.end(); )
	{
		if (*it == _eMouseButton)
		{
			it = m_MouseInputContainer.erase(it);
			return true;
		}
		else
			++it;
	}
	return false;
}

void CInput_Device::Reset_KeyBoardContainers()
{
	m_KeyInputContainer.clear();
}

void CInput_Device::Reset_MouseButtonContainers()
{
	m_MouseInputContainer.clear();
}

HRESULT CInput_Device::Initialize_Input_Device(const HINSTANCE& _hInst, const HWND& _hwnd)
{
	if (FAILED(DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return E_FAIL;

	m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr);
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(_hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboard->Acquire();

	m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr);
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(_hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	m_hWnd = _hwnd;

	GetClientRect(m_hWnd, &m_tClientRect);
	return S_OK;
}

HRESULT CInput_Device::Update()
{
	if (nullptr == m_pKeyboard ||
		nullptr == m_pMouse)
		return E_FAIL;

	memcpy(&m_byPreKeyState, &m_byKeyState, sizeof(m_byKeyState));
	memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));

	m_pKeyboard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	return S_OK;
}

void CInput_Device::Free()
{
	Safe_Release_DV(m_pKeyboard);
	Safe_Release_DV(m_pMouse);
	Safe_Release_DV(m_pInput);
}
