#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	enum DIMOUSEMOVE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum DIMOUSEBUTTON { DIMB_L, DIMB_R, DIMB_WHEEL, DIMB_END };
	enum CURSOR_LOCKMODE { CURSOR_LOCKED, CURSOR_CONFINED, CORSOR_NONE, CURSOR_END };
private:
	explicit CInput_Device();
	virtual ~CInput_Device() = default;
public:
	_bool Get_DIKeyDown(const _ubyte& _eKeyID);
	_bool Get_DIKeyUp(const _ubyte& _eKeyID);
	_bool Get_DIKeyPressing(const _ubyte& _eKeyID);
	_bool Get_DIMBtnDown(const DIMOUSEBUTTON& _eMouseBtn);
	_bool Get_DIMBtnUp(const DIMOUSEBUTTON& _eMouseBtn);
	_bool Get_DIMBtnPressing(const DIMOUSEBUTTON& _eMouseBtn);
	_long Get_DIMMoveState(const DIMOUSEMOVE& _eMouseMove) { return *((_long*)&m_MouseState + _eMouseMove); }
	// Get Mouse Position
	_float2 Get_MousePosition();
	_int Judge_MouseOverCenterX(const _float& _fMousePosX, const _float& _fValue = 0.f);
	_int Judge_MouseOverCenterY(const _float& _fMousePosY, const _float& _fValue = 0.f);
	// Set CursorLock
	void Set_CursorLock(const CURSOR_LOCKMODE& eLockMode);
	const CURSOR_LOCKMODE& Get_CursorMode() const { return m_eCursurMode; }
	// Add
	void Add_KeyInputContainers(const _ubyte& _eByte) { m_KeyInputContainer.push_back(_eByte); }
	void Add_MouseInputContainers(const DIMOUSEBUTTON& _eMouseButton) { m_MouseInputContainer.push_back(_eMouseButton); }
	// Get KeyBoard Inputs
	_uint Get_KeyBoardInputs_InContainers(const _ubyte& _eByte);
	// Get Mouse Button Inputs 
	_uint Get_MouseButtonInputs_InContainers(const DIMOUSEBUTTON& _eMouseButton);
	// If SameInputs
	_bool Is_KeyBoardInput_InContainers(const _ubyte& _eByte);
	// if SameInputs
	_bool Is_MouseButtonInputs_InContainers(const DIMOUSEBUTTON& _eMouseButton);
	// Reset Containers
	void Reset_KeyBoardContainers();
	void Reset_MouseButtonContainers();
public:
	HRESULT Initialize_Input_Device(const HINSTANCE& _hInst, const HWND& _hwnd);
	HRESULT Update();
private:
	LPDIRECTINPUT8										m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8							m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8							m_pMouse = nullptr;
private:
	list<_ubyte>							m_KeyInputContainer;
	list<DIMOUSEBUTTON>	m_MouseInputContainer;
private:
	// Key Mouse State 
	_ubyte								m_byKeyState[256] = { 0 };
	DIMOUSESTATE						m_MouseState;

	// Pre Key Mouse State
	_byte								m_byPreKeyState[256] = { 0 };
	DIMOUSESTATE						m_PreMouseState;


	// Boolean State
	//_bool									m_bKeyInput[256] = { 0 };
	//_bool									m_bKeyInputUp[256] = { 0 };
	//_bool									m_bMouseInput[4] = { 0 };
	//_bool									m_bMouseInputUp[4] = { 0 };
	// HWND
	HWND									m_hWnd = NULL;
	// Rect
	RECT									m_tClientRect;
	// Cursor LockMode
	CURSOR_LOCKMODE		m_eCursurMode = CURSOR_END;
public:
	virtual void Free() override;
};

END