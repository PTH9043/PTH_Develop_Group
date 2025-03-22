#pragma once
#include "UBase.h"

BEGIN(Engine)

class UInputManager : public UBase {
public:
	UInputManager();
	NO_COPY(UInputManager)
	DESTRUCTOR(UInputManager)
public:
	void SetCursorLock(const CURSOR_LOCKMODE& _eLockMode);
	const CURSOR_LOCKMODE& GetCursorMode() const { return m_eCursorLockMode; }
public:
	// Override
	virtual void Free() override;
public:
	HRESULT ReadyInpuDevice(CSHPTRREF<GRAPHICDESC> _spGraphicDesc);
	void MouseTick();
	void KeyTick();
	void InvokeKeyMethods(const _ubyte& _bFirstKeyID, const _double& _dDeltaTime);

	void AddKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName, const KEYACTIONFUNC& _keyMethod);
	void DeleteKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName);

	_bool GetDIKeyDown( _ubyte _bKeyID);
	_bool GetDIKeyUp( _ubyte _bKeyID);
	_bool GetDIKeyPressing(const _ubyte& _bKeyID);
	_bool GetDIMBtnDown(_ubyte _eMouseBtn);
	_bool GetDIMBtnUp(_ubyte _eMouseBtn);
	_bool GetDIMBtnPressing(_ubyte _eMouseBtn);
	_long GetDIMMoveState(_ubyte _eMouseMove) { return *((_long*)&m_eMouseState + _eMouseMove); }
	// Get Mouse Position
	_float2 GetMousePosition();

	_bool	IsMouseInWindowSize();
private:
	RECT GetClientAsRect();
private:
	using	KEYARR = ARRAY<_ubyte, MAX_KEYSIZE>;
	using	KEYMETHODS = UNORMAP<_wstring, KEYACTIONFUNC>;
	using	KEYBINDS = ARRAY< KEYMETHODS, MAX_KEYSIZE>;

	LPDIRECTINPUT8								m_pInput;
	LPDIRECTINPUTDEVICE8				m_pKeyboard;
	LPDIRECTINPUTDEVICE8				m_pMouse;

	KEYARR												m_arrKeyState;
	DIMOUSESTATE								m_eMouseState;
	KEYARR												m_arrPrevKeyState;
	DIMOUSESTATE								m_ePrevMouseState;

	// Using Press Key 
	KEYBINDS											m_arrKeyBinds;

	CURSOR_LOCKMODE						m_eCursorLockMode;
	SHPTR<GRAPHICDESC>					m_spGraphicDesc;
	RECT													m_stWindowRect;
};

END
