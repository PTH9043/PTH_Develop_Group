#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGamePad_Device final : public CBase
{
	DECLARE_SINGLETON(CGamePad_Device)
public:
	explicit CGamePad_Device();
	virtual ~CGamePad_Device() = default;

public:
	HRESULT Initailize_Inpuit_Device(const HINSTANCE& _hInst, const HWND& _hWnd);
	HRESULT Update();

	_bool Get_PadDown(const GAMEPADKEYS& _eKeyID);
	_bool Get_PadUp(const GAMEPADKEYS& _eKeyID);
	_bool Get_PadPressing(const GAMEPADKEYS& _eKeyID);

	const _float& Get_Yaw() const { return m_fYaw; }
	const _float& Get_Pitch() const { return m_fPitch; }
	const _float& Get_Theta() const { return m_fTheta; }
	const _float& Get_Phi() const { return m_fPhi; }
	const _float& Get_Radius() const { return m_fRadius; }
protected:
	// Update GamePad
	void Exit_GamePad();
protected:
	// GamePad
	unique_ptr<GamePad>						m_GamePad;
	GamePad::ButtonStateTracker		m_Buttons;
	// Yaw, Pitch  
	_float														m_fYaw = 0.f;
	_float														m_fPitch = 0.f;
	// Theta, Phi, Radius
	_float														m_fTheta = 0.f;
	_float														m_fPhi = 0.f;
	_float														m_fRadius = 0.f;
protected:
	const  _float											ROTATION_GAIN = 0.1f;
	const _float											c_defaultPhi = XM_2PI / 6.0f;
	const _float											c_defaultRadius = 3.3f;
	const _float											c_minRadius = 0.1f;
	const _float											c_maxRadius = 5.f;
public:
	virtual void Free() override;
};

END