#include "AI_CharController.h"
#include "Battle_Manager.h"
#include "Method.h"

CAI_CharController::CAI_CharController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CAI_Controller(_pDevice, _pContext)
{
}

HRESULT CAI_CharController::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CAI_CharController::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CAI_CharController::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

VOIDDATAS* CAI_CharController::FillOut_AIControllerData(VOIDDATAS* _pDatas)
{
	if (nullptr == _pDatas)
		return nullptr;

	_pDatas->push_back(&m_bStopAI);
	return _pDatas;
}

void CAI_CharController::Judge_State(const _double& _dTimeDelta)
{
}

void CAI_CharController::Find_Target()
{
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		m_pTargetObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
	}
	RELEASE_INSTANCE(CBattle_Manager);
}

_uint CAI_CharController::Judge_Direction(const _float& _fDot)
{
	_uint eDir = 0;
	// Forward
	if (20.f >= _fDot && _fDot >= -20.f)
		eDir = DIR_FORWARD;
	// Left Forward
	else if (_fDot >= 20.f && _fDot <= 80.f)
		eDir = DIR_FORELEFT | DIR_LEFT;
	else if (_fDot >= 80.f && _fDot <= 120.f)
		eDir = DIR_LEFT;
	else if (_fDot >= 120.f && _fDot <= 160.f)
		eDir = DIR_BACKLEFT | DIR_LEFT;
	else if (_fDot >= 160.f && _fDot <= -160.f)
		eDir = DIR_BACK;
	else if (_fDot >= -160.f && _fDot <= -120.f)
		eDir = DIR_BACKRIGHT | DIR_RIGHT;
	else if (_fDot >= -120.f && _fDot <= -80.f)
		eDir = DIR_RIGHT;
	else if (_fDot >= -80.f && _fDot <= -20.f)
		eDir = DIR_FORRIGHT | DIR_RIGHT;

	if (isnan(_fDot))
		eDir = DIR_FORWARD;

	return eDir;
}

void CAI_CharController::Free()
{
	__super::Free();
}
