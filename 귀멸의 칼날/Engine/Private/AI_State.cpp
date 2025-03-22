#include "AI_State.h"
#include "Model.h"
#include "GameObject.h"
#include "AI_FSMController.h"


CAI_State::CAI_State(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(_pDevice);
	Safe_AddRef_DV(_pContext);
}

HRESULT CAI_State::NativeConstruct_Prototype(void* _pArg)
{
	AIDESC tDesc = *static_cast<AIDESC*>(_pArg);

	m_strAnmationName = tDesc.strAnimationName;
	m_iAnimationIndex = tDesc.iAnimationIndex;
	m_iStateID = tDesc.iStateID;

	return S_OK;
}

void CAI_State::Tick(const _double& _dTimeDelta, CAI_FSMController* _pFSM, CGameObject* _pOwner, CGameObject* _pTarget,
void* _pExtraData, void** _ppOut)
{

}

void CAI_State::Late_Tick(const _double& _dTimeDelta, CAI_FSMController* _pFSM, CGameObject* _pOwner, CGameObject* _pTarget,
void* _pExtraData, void** _ppOut)
{

}

void CAI_State::Change_State(CAI_FSMController* _pFSM, const _uint& _iID)
{
	if (nullptr != _pFSM)
		return;
	// 내부적으로 행동을 변경
	_pFSM->Change_AIState(_iID);
}

void CAI_State::Free()
{
	Safe_Release_DV(m_pContext);
	Safe_Release_DV(m_pDevice);
}
