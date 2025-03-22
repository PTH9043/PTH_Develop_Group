#include "AI_FSMController.h"
#include  "AI_State.h"

CAI_FSMController::CAI_FSMController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CAI_Controller(_pDevice, _pContext)
{
}

const char* CAI_FSMController::Get_CurAnimation_Name()
{
	if (nullptr == m_pCurState)
		return nullptr;
	return m_pCurState->Get_CurAnimation();
}

_uint CAI_FSMController::Get_CurAnimation_Index()
{
	if (nullptr == m_pCurState)
		return 0;
	return m_pCurState->Get_CurAnimation_Index();
}

_uint CAI_FSMController::Get_State()
{
	if (nullptr == m_pCurState)
		return 0;
	return m_pCurState->Get_State();
}

HRESULT CAI_FSMController::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;


	//CAI_State::AIDESC tDesc;
	//tDesc.iStateID = 0;
	//tDesc.iAnimationIndex = 0;
	//tDesc.strAnimationName = "Animation";
	// m_AIStateList.push_back(CAI_State::Create(&tDesc));
	return S_OK;
}

void CAI_FSMController::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pCurState)
		m_pCurState->Tick(_dTimeDelta, this, m_pOwnerObject, m_pTargetObject);
}

void CAI_FSMController::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pCurState)
		m_pCurState->Late_Tick(_dTimeDelta, this, m_pOwnerObject, m_pTargetObject);
}

void CAI_FSMController::Change_AIState(const _uint& _iID)
{
	if (m_AIStateList.size() <= 0)
		return;

	if (m_AIStateList.size() <= _iID)
		return;

	m_pPrevState = m_pCurState;
	m_pCurState = m_AIStateList[_iID];
}

void CAI_FSMController::Free()
{
	for (auto& iter : m_AIStateList)
		Safe_Release(iter);

	__super::Free();
}
