#include "CGiantDevilStateMachine.h"
#include "CWeapon.h"
#include "CGameObject.h"
#include "CGiantDevil.h"
#include "CGameInstance.h"

CGiantDevilStateMachine::CGiantDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CMonsterState(_pDevice, _pContext)
{
}

HRESULT CGiantDevilStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	CGiantDevil* pGiantDevil = static_cast<CGiantDevil*>(_pOwnerObject);
	if (nullptr != pGiantDevil)
		m_pWeapon = static_cast<CWeapon*>(pGiantDevil->Get_Equip(CEquipment::EQUIP_WEAPON));

	return S_OK;
}

void CGiantDevilStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CGiantDevilStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
	Reset_FindAttackState(_dTimeDelta);
}

void CGiantDevilStateMachine::Change_State(const _uint& _iState, CGiantDevil* _pOwnerObject)
{
	if (nullptr == m_pWeapon)
		return;

	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_FIND == _iState)
		Find_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
}

void CGiantDevilStateMachine::Idle_State(CGiantDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CGiantDevil::INIT_IDLE:
		if(STATE_ATTACK == m_iState)
			m_pModel->Set_Animation("AS_TGiantDevil_TCmn_NoneFightIdleLook03_N_Loop");
		else
			m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleLook03_N_Loop");
		break;
	case CGiantDevil::INIT_CROUCH :
		if (STATE_ATTACK == m_iState)
			m_pModel->Set_Animation("AS_TGiantDevil_TCmn_NoneFightIdleCrouch_N_Loop");
		else
			m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleCrouch_N_Loop");
		break;
	case CGiantDevil::INIT_SIT:
		if (STATE_ATTACK == m_iState)
		m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleSit_N_Loop"); 
		else
			m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleSit_N_Loop");
		break;
	}
}

void CGiantDevilStateMachine::GetUp_State(CGiantDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CGiantDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdle_N_Loop");
		break;
	case CGiantDevil::INIT_CROUCH:
		m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleCrouch_N");
		break;
	case CGiantDevil::INIT_SIT:
		m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdleSit_N");
		break;
	}
	m_pModel->Get_CurAnimation()->Set_FastValue(1.5f);
	m_pModel->Get_NextAnimation()->Set_FastValue(1.5f);
}

void CGiantDevilStateMachine::Find_State(CGiantDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightIdle_N_Loop");
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
}

void CGiantDevilStateMachine::Threat_State(CGiantDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TGiantDevil_TCmn_NoneFightThreat_N_Loop");
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
}

void CGiantDevilStateMachine::Attack_State(CGiantDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_EquipWeaponType())
	{
	case CGiantDevil::EW_SWORD:
		if (CGiantDevil::PAT_N1 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_THammer_AttackNormal01_N");
		}
		else if (CGiantDevil::PAT_N2 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_THammer_AttackSpecial05_N");
			m_bNextAttack = true;
		}
		else if (CGiantDevil::PAT_S1 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_THammer_AttackNormal01_N");
		}
		else if (CGiantDevil::PAT_S2 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_THammer_AttackSpecial05_N");
		}
		break;
	case CGiantDevil::EW_HAMMER:
		if (CGiantDevil::PAT_N1 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_TLSword_AttackStrong02_N");
		}
		else if (CGiantDevil::PAT_N2 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_TLSword_AttackStrong03_N");
		}
		else if (CGiantDevil::PAT_S1 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_TLSword_AttackPowerful01A_N");
			m_bNextAttack = true;
		}
		else if (CGiantDevil::PAT_S2 == _pOwnerObject->Get_AttackPattern())
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
			m_pModel->Change_Animation("AS_TGiantDevil_TLSword_AttackSpecial01_N");
		}
		break;
	}
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	++m_iAttackCount;
}

CGiantDevilStateMachine* CGiantDevilStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CGiantDevilStateMachine* pInstance = new CGiantDevilStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGiantDevilStateMachine::Create To Failed");
	}
	return pInstance;
}

void CGiantDevilStateMachine::Free()
{
	__super::Free();
}
