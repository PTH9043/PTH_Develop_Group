#include "AI_Controller.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Animator.h"

CAI_Controller::CAI_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CController(_pDevice, _pContext)
{

}

HRESULT CAI_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CAI_Controller::Tick(const _double& _dTimeDelta)
{
}

void CAI_Controller::Late_Tick(const _double& _dTimeDelta)
{
}

void CAI_Controller::Set_TargetObject(CGameObject* _pGameObject)
{
	this->m_pTargetObject = _pGameObject;
}

VOIDDATAS* CAI_Controller::FillOut_AIControllerData(VOIDDATAS* _pDatas)
{
	return nullptr;
}

void CAI_Controller::Free()
{
	__super::Free();
}
