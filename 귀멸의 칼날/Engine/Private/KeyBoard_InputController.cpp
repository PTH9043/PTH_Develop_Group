#include "KeyBoard_InputController.h"
#include "GameInstance.h"

CKeyBoard_InputController::CKeyBoard_InputController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CInput_Controller(_pDevice, _pContext)
{
	m_eInputConType = INPUTCON_TYPE::KEYBOAR_MOUSE;
}

HRESULT CKeyBoard_InputController::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CKeyBoard_InputController::Tick(const _double& _dTimeDelta)
{	
	// 이전 프레임에서 담겨있던 Input 정보들을 비워준다.
	__super::Tick(_dTimeDelta);
}

void CKeyBoard_InputController::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CKeyBoard_InputController::Insert_Datas(VOIDDATAS _pData)
{
}

VOIDDATAS CKeyBoard_InputController::Out_Datas()
{
	return VOIDDATAS();
}

void CKeyBoard_InputController::Input_Defines()
{
	m_InputList.clear();

	INPUTDESC input;
	// PRESSING 체크
	{
		input.Push_Info(PRESSING, DIK_W);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_S);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_A);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_D);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_P);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_L);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_O);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_K);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_I);
		Check_Input(input);

		input.Push_Info(PRESSING, DIK_LSHIFT);
		Check_Input(input);

	}

	// DOWN 체크
	{
		input.Push_Info(DOWN, DIK_W);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_S);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_A);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_D);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_J);
		Check_Input(input);

		//input.Push_Info(DOWN, DIK_E);
		//Check_Input(input);

		input.Push_Info(DOWN, DIK_P);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_Q);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_K);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_L);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_O);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_I);
		Check_Input(input);

		input.Push_Info(DOWN, DIK_LSHIFT);
		Check_Input(input);
	}

	// UP 체크
	{
		input.Push_Info(UP, DIK_J);
		Check_Input(input);

		input.Push_Info(UP, DIK_O);
		Check_Input(input);
	}
}

void CKeyBoard_InputController::Check_Input(INPUTDESC inputDesc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 눌렸는지 체크하는 불변수
	_bool bIsCheck = false;

	switch (inputDesc.inputType)
	{
	case DOWN:
		bIsCheck = pGameInstance->Get_DIKeyDown(inputDesc.inputKey);
		m_DownKey[inputDesc.inputKey] = bIsCheck;
		break;

	case UP:
		bIsCheck = pGameInstance->Get_DIKeyUp(inputDesc.inputKey);
		m_UpKey[inputDesc.inputKey] = bIsCheck;
		break;

	case PRESSING:
		bIsCheck = pGameInstance->Get_DIKeyPressing(inputDesc.inputKey);
		m_PressingKey[inputDesc.inputKey] = bIsCheck;
		break;
	}

	if (true == bIsCheck)
		m_InputList.push_back(inputDesc);

	RELEASE_INSTANCE(CGameInstance);
}

void CKeyBoard_InputController::Free()
{
	__super::Free();
}

void CKeyBoard_InputController::tagInputDesc::Push_Info(INPUTTYPE inputType, const _ubyte& inputKey)
{
	this->inputType = inputType;
	this->inputKey = inputKey;
}

_bool CKeyBoard_InputController::tagInputDesc::Compare_Info(tagInputDesc* input)
{
	if (this->inputType == input->inputType && this->inputKey == input->inputKey)
		return true;

	return false;
}