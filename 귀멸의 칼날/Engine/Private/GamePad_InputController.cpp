#include "GamePad_InputController.h"
#include "GameInstance.h"

CGamePad_InputController::CGamePad_InputController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CInput_Controller(_pDevice, _pContext)
{
	m_eInputConType = INPUTCON_TYPE::GAMEPAD;
}

HRESULT CGamePad_InputController::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CGamePad_InputController::Tick(const _double& _dTimeDelta)
{	
	// 이전 프레임에서 담겨있던 Input 정보들을 비워준다.
	__super::Tick(_dTimeDelta);
}

void CGamePad_InputController::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CGamePad_InputController::Insert_Datas(VOIDDATAS _pData)
{
}

VOIDDATAS CGamePad_InputController::Out_Datas()
{
	return VOIDDATAS();
}

void CGamePad_InputController::Input_Defines()
{
	m_InputList.clear();

	INPUTDESC input;
	// PRESSING 체크
	{
		input.Push_Info(PRESSING, GPK_X);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_A);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_Y);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_B);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_BACK);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_DPAD_DOWN);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_DPAD_UP);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_DPAD_LEFT);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_DPAD_RIGHT);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_SHOULDER);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_STICK);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_STICKUP);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_LEFT_STICKLEFT);
		Check_Input(input);
		//input.Push_Info(PRESSING, GPK_LEFT_TRIGGER);
		//Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_SHOULDER);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_STICK);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_STICKUP);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_STICKLEFT);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_RIGHT_TRIGGER);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_MENU);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_START);
		Check_Input(input);
		input.Push_Info(PRESSING, GPK_VIEW);
		Check_Input(input);
	}

	// DOWN 체크
	{
		input.Push_Info(DOWN, GPK_X);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_A);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_Y);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_B);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_BACK);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_DPAD_DOWN);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_DPAD_UP);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_DPAD_LEFT);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_DPAD_RIGHT);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_SHOULDER);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_STICK);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_STICKUP);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_LEFT_STICKLEFT);
		Check_Input(input);
		//input.Push_Info(DOWN, GPK_LEFT_TRIGGER);
		//Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_SHOULDER);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_STICK);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_STICKUP);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_STICKLEFT);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_RIGHT_TRIGGER);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_MENU);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_START);
		Check_Input(input);
		input.Push_Info(DOWN, GPK_VIEW);
		Check_Input(input);
	}

	// UP 체크
	{
		input.Push_Info(UP, GPK_X);
		Check_Input(input);
		input.Push_Info(UP, GPK_A);
		Check_Input(input);
		input.Push_Info(UP, GPK_Y);
		Check_Input(input);
		input.Push_Info(UP, GPK_B);
		Check_Input(input);
		input.Push_Info(UP, GPK_BACK);
		Check_Input(input);
		input.Push_Info(UP, GPK_DPAD_DOWN);
		Check_Input(input);
		input.Push_Info(UP, GPK_DPAD_UP);
		Check_Input(input);
		input.Push_Info(UP, GPK_DPAD_LEFT);
		Check_Input(input);
		input.Push_Info(UP, GPK_DPAD_RIGHT);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_SHOULDER);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_STICK);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_STICKUP);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(UP, GPK_LEFT_STICKLEFT);
		Check_Input(input);
		//input.Push_Info(UP, GPK_LEFT_TRIGGER);
		//Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_SHOULDER);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_STICK);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_STICKDOWN);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_STICKUP);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_STICKRIGHT);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_STICKLEFT);
		Check_Input(input);
		input.Push_Info(UP, GPK_RIGHT_TRIGGER);
		Check_Input(input);
		input.Push_Info(UP, GPK_MENU);
		Check_Input(input);
		input.Push_Info(UP, GPK_START);
		Check_Input(input);
		input.Push_Info(UP, GPK_VIEW);
		Check_Input(input);
	}
}

void CGamePad_InputController::Check_Input(INPUTDESC inputDesc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 눌렸는지 체크하는 불변수
	_bool bIsCheck = false;

	switch (inputDesc.inputType)
	{
	case DOWN:
		bIsCheck = pGameInstance->Get_PadDown((GAMEPADKEYS)inputDesc.inputKey);
		m_GamePadDown[inputDesc.inputKey] = bIsCheck;
		break;

	case UP:
		bIsCheck = pGameInstance->Get_PadUp((GAMEPADKEYS)inputDesc.inputKey);
		m_GamePadUp[inputDesc.inputKey] = bIsCheck;
		break;

	case PRESSING:
		bIsCheck = pGameInstance->Get_PadPressing((GAMEPADKEYS)inputDesc.inputKey);
		m_GamePadPressing[inputDesc.inputKey] = bIsCheck;
		break;
	}

	if (true == bIsCheck)
		m_InputList.push_back(inputDesc);

	RELEASE_INSTANCE(CGameInstance);
}

void CGamePad_InputController::Free()
{
	__super::Free();
}

void CGamePad_InputController::tagInputDesc::Push_Info(INPUTTYPE inputType, const _ubyte& inputKey)
{
	this->inputType = inputType;
	this->inputKey = inputKey;
}

_bool CGamePad_InputController::tagInputDesc::Compare_Info(tagInputDesc* input)
{
	if (this->inputType == input->inputType && this->inputKey == input->inputKey)
		return true;

	return false;
}