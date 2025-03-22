#include "GamePad_Device.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGamePad_Device)

CGamePad_Device::CGamePad_Device()
{

}

HRESULT CGamePad_Device::Initailize_Inpuit_Device(const HINSTANCE& _hInst, const HWND& _hWnd)
{
	m_GamePad = make_unique<GamePad>();
	if (nullptr != m_GamePad)
	{
		m_GamePad->Resume();
		m_GamePad->Resume();
	}
	return S_OK;
}

HRESULT CGamePad_Device::Update()
{
	auto& pad = m_GamePad->GetState(0);
	if (pad.IsConnected())
	{
		m_Buttons.Update(pad);

		if (pad.IsViewPressed())
		{
			Exit_GamePad();
		}
		if (pad.IsLeftStickPressed()){
			m_fYaw = m_fPitch = 0.f;
			m_fPhi = c_defaultPhi;
			m_fRadius = c_defaultRadius;
		}
		else
		{
			m_fYaw += -pad.thumbSticks.leftX * ROTATION_GAIN;
			m_fPitch += pad.thumbSticks.leftY * ROTATION_GAIN;
			m_fTheta += pad.thumbSticks.rightX * XM_PI * ROTATION_GAIN;
			m_fPhi -= pad.thumbSticks.rightY * XM_PI * ROTATION_GAIN;
			m_fRadius -= pad.thumbSticks.leftY * 5.f * ROTATION_GAIN;
		}
		m_GamePad->SetVibration(0, pad.triggers.left, pad.triggers.right);
	}
	else
	{
		m_Buttons.Reset();
	}

	constexpr _float Limit = XM_PIDIV2 - 0.01f;
	m_fPhi = max(1e-2f, min(Limit, m_fPhi));
	m_fRadius = max(c_minRadius, min(c_maxRadius, m_fRadius));

	if (m_fTheta > XM_PI)
	{
		m_fTheta -= XM_2PI;
	}
	else if (m_fTheta < -XM_PI)
	{
		m_fTheta += XM_2PI;
	}

	return S_OK;
}

_bool CGamePad_Device::Get_PadDown(const GAMEPADKEYS& _eKeyID)
{
	_bool bPress = false;
	if (GAMEPADKEYS::GPK_A == _eKeyID)
		bPress = m_Buttons.a == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_B == _eKeyID)
		bPress = m_Buttons.b == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_Y == _eKeyID)
		bPress = m_Buttons.y == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_X == _eKeyID)
		bPress = m_Buttons.x == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_BACK == _eKeyID)
		bPress = m_Buttons.back == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_DPAD_DOWN == _eKeyID)
		bPress = m_Buttons.dpadDown == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_DPAD_UP == _eKeyID)
		bPress = m_Buttons.dpadUp == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_DPAD_LEFT == _eKeyID)
		bPress = m_Buttons.dpadLeft == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_DPAD_RIGHT == _eKeyID)
		bPress = m_Buttons.dpadRight == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_SHOULDER == _eKeyID)
		bPress = m_Buttons.leftShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_STICK == _eKeyID)
		bPress = m_Buttons.leftStick == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.leftStickDown == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_STICKUP == _eKeyID)
		bPress = m_Buttons.leftStickUp == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.leftStickRight == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.leftStickLeft == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_LEFT_TRIGGER == _eKeyID)
		bPress = m_Buttons.leftTrigger == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_SHOULDER == _eKeyID)
		bPress = m_Buttons.rightShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_STICK == _eKeyID)
		bPress = m_Buttons.rightStick == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.rightStickDown == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKUP == _eKeyID)
		bPress = m_Buttons.rightStickUp == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.rightStickRight == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.rightStickLeft == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_RIGHT_TRIGGER == _eKeyID)
		bPress = m_Buttons.rightTrigger == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_MENU == _eKeyID)
		bPress = m_Buttons.menu == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_START == _eKeyID)
		bPress = m_Buttons.start == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	else if (GAMEPADKEYS::GPK_VIEW == _eKeyID)
		bPress = m_Buttons.view == GamePad::ButtonStateTracker::ButtonState::PRESSED;
	return bPress;
}

_bool CGamePad_Device::Get_PadUp(const GAMEPADKEYS& _eKeyID)
{
	_bool bPress = false;
	if (GAMEPADKEYS::GPK_A == _eKeyID)
		bPress = m_Buttons.a == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_B == _eKeyID)
		bPress = m_Buttons.b == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_Y == _eKeyID)
		bPress = m_Buttons.y == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_X == _eKeyID)
		bPress = m_Buttons.x == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_BACK == _eKeyID)
		bPress = m_Buttons.back == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_DPAD_DOWN == _eKeyID)
		bPress = m_Buttons.dpadDown == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_DPAD_UP == _eKeyID)
		bPress = m_Buttons.dpadUp == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_DPAD_LEFT == _eKeyID)
		bPress = m_Buttons.dpadLeft == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_DPAD_RIGHT == _eKeyID)
		bPress = m_Buttons.dpadRight == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_SHOULDER == _eKeyID)
		bPress = m_Buttons.leftShoulder == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_STICK == _eKeyID)
		bPress = m_Buttons.leftStick == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.leftStickDown == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_STICKUP == _eKeyID)
		bPress = m_Buttons.leftStickUp == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.leftStickRight == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.leftStickLeft == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_LEFT_TRIGGER == _eKeyID)
		bPress = m_Buttons.leftTrigger == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_SHOULDER == _eKeyID)
		bPress = m_Buttons.rightShoulder == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_STICK == _eKeyID)
		bPress = m_Buttons.rightStick == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.rightStickDown == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKUP == _eKeyID)
		bPress = m_Buttons.rightStickUp == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.rightStickRight == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.rightStickLeft == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_RIGHT_TRIGGER == _eKeyID)
		bPress = m_Buttons.rightTrigger == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_MENU == _eKeyID)
		bPress = m_Buttons.menu == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_START == _eKeyID)
		bPress = m_Buttons.start == GamePad::ButtonStateTracker::ButtonState::UP;
	else if (GAMEPADKEYS::GPK_VIEW == _eKeyID)
		bPress = m_Buttons.view == GamePad::ButtonStateTracker::ButtonState::UP;
	return bPress;
}

_bool CGamePad_Device::Get_PadPressing(const GAMEPADKEYS& _eKeyID)
{
	_bool bPress = false;
	if (GAMEPADKEYS::GPK_A == _eKeyID)
		bPress = m_Buttons.a == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_B == _eKeyID)
		bPress = m_Buttons.b == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_Y == _eKeyID)
		bPress = m_Buttons.y == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_X == _eKeyID)
		bPress = m_Buttons.x == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_BACK == _eKeyID)
		bPress = m_Buttons.back == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_DPAD_DOWN == _eKeyID)
		bPress = m_Buttons.dpadDown == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_DPAD_UP == _eKeyID)
		bPress = m_Buttons.dpadUp == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_DPAD_LEFT == _eKeyID)
		bPress = m_Buttons.dpadLeft == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_DPAD_RIGHT == _eKeyID)
		bPress = m_Buttons.dpadRight == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_SHOULDER == _eKeyID)
		bPress = m_Buttons.leftShoulder == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_STICK == _eKeyID)
		bPress = m_Buttons.leftStick == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.leftStickDown == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_STICKUP == _eKeyID)
		bPress = m_Buttons.leftStickUp == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.leftStickRight == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.leftStickLeft == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_LEFT_TRIGGER == _eKeyID)
		bPress = m_Buttons.leftTrigger == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_SHOULDER == _eKeyID)
		bPress = m_Buttons.rightShoulder == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_STICK == _eKeyID)
		bPress = m_Buttons.rightStick == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKDOWN == _eKeyID)
		bPress = m_Buttons.rightStickDown == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKUP == _eKeyID)
		bPress = m_Buttons.rightStickUp == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKRIGHT == _eKeyID)
		bPress = m_Buttons.rightStickRight == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_STICKLEFT == _eKeyID)
		bPress = m_Buttons.rightStickLeft == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_RIGHT_TRIGGER == _eKeyID)
		bPress = m_Buttons.rightTrigger == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_MENU == _eKeyID)
		bPress = m_Buttons.menu == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_START == _eKeyID)
		bPress = m_Buttons.start == GamePad::ButtonStateTracker::ButtonState::HELD;
	else if (GAMEPADKEYS::GPK_VIEW == _eKeyID)
		bPress = m_Buttons.view == GamePad::ButtonStateTracker::ButtonState::HELD;
	return bPress;
}

void CGamePad_Device::Exit_GamePad()
{

}

void CGamePad_Device::Free()
{

}
