#include "Input_Controller.h"
#include "GameInstance.h"

CInput_Controller::CInput_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CController(_pDevice, _pContext)
{
}

HRESULT CInput_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}


void CInput_Controller::Tick(const _double& _dTimeDelta)
{
	// 이전 프레임에서 담겨있던 Input 정보들을 비워준다.
	__super::Tick(_dTimeDelta);
	Input_Defines();
}

void CInput_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CInput_Controller::Insert_Datas(VOIDDATAS _pData)
{
}

VOIDDATAS CInput_Controller::Out_Datas()
{
	return VOIDDATAS();
}

void CInput_Controller::Input_Defines()
{
}

void CInput_Controller::Free()
{
	__super::Free();
}
