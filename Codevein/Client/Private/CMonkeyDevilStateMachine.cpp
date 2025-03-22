#include "CMonkeyDevilStateMachine.h"
#include "CMonkeyDevil.h"
#include "CGameInstance.h"
#include "CHammer.h"
#include "COneHandSword.h"

CMonkeyDevilStateMachine::CMonkeyDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonsterState(_pDevice, _pContext)
{
}

HRESULT CMonkeyDevilStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	CMonkeyDevil* pMonkeyDevil = static_cast<CMonkeyDevil*>(_pOwnerObject);
	if (nullptr != pMonkeyDevil)
		m_pWeapon = static_cast<CWeapon*>(pMonkeyDevil->Get_Equip(CEquipment::EQUIP_WEAPON));



	return S_OK;
}

void CMonkeyDevilStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
	if (true == m_tPattern1Timer.Is_Over(_dTimeDelta))
		m_tPattern1Timer.Reset_Timer();
}

void CMonkeyDevilStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{

}

void CMonkeyDevilStateMachine::Change_State(const _uint& _iState, CMonkeyDevil* _pOwnerObject)
{
	if (nullptr == m_pWeapon)
		return;

	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_DOWN == _iState)
		Down_State(_pOwnerObject);
	else if (STATE_FIND == _iState)
		Find_State(_pOwnerObject);
	else if (STATE_DODGE == _iState)
		Dodge_State(_pOwnerObject);
	else if (STATE_RETARGET == _iState)
		Retarget_State(_pOwnerObject);
	else if (STATE_MOVE == _iState)
		Walk_State(_pOwnerObject);
	else if (STATE_RUN == _iState)
		Run_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
	else if (STATE_HIT == _iState)
		Hit_State(_pOwnerObject);
	else if (STATE_DIE == _iState)
		Die_State(_pOwnerObject);

	m_pModel->Get_NextAnimation()->Set_FastValue(2.0f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.0f);
}

void CMonkeyDevilStateMachine::Idle_State(CMonkeyDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CMonkeyDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_NoneFightIdle_N_Loop");
		break;
	case CMonkeyDevil::INIT_FALL:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_NoneFightIdleSit02_N_Loop");
		break;
	case CMonkeyDevil::INIT_WALK:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_NoneFightWalk_F_Loop");
		break;
	case CMonkeyDevil::INIT_WALL:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_WallFallLanding_N_Loop");
		break;
	case CMonkeyDevil::INIT_SIT:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_PlafondFallLanding_N_Start");
		break;
	}
}

void CMonkeyDevilStateMachine::Find_State(CMonkeyDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CMonkeyDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_WallClimb_N");
		break;
	case CMonkeyDevil::INIT_FALL:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_NoneFightIdleSit02_N");
		break;
	case CMonkeyDevil::INIT_WALK:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_NoneFightWalk_F_Loop");
		break;
	case CMonkeyDevil::INIT_WALL:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_WallFallLanding_N_Loop");
		break;
	case CMonkeyDevil::INIT_SIT:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_PlafondFallLanding_N_Start");
		break;
	}
}

void CMonkeyDevilStateMachine::Down_State(CMonkeyDevil* _pOwnerObject)
{
	switch (m_iDownCount)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		m_iDownCount = 0;
		break;
	}
}

void CMonkeyDevilStateMachine::Dodge_State(CMonkeyDevil* _pOwnerObject)
{
	_float fDistance = _pOwnerObject->PlayerToDistance();
	if (fDistance <= 5.f)
	{
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Dodge_B");
	}
	else
	{
		_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
		if (fValue > 0)
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Dodge_R");
		}
		else
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Dodge_L");
		}
	}
}

void CMonkeyDevilStateMachine::Retarget_State(CMonkeyDevil* _pOwnerObject)
{
	if (STATE_ATTACK == m_iState)
		m_pModel->Get_CurAnimation()->Set_SelectAnimRot(CAnimation::ROT_DEST);


	m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Walk_B_Loop");
}
void CMonkeyDevilStateMachine::Walk_State(CMonkeyDevil* _pOwnerObject)
{
}

void CMonkeyDevilStateMachine::Run_State(CMonkeyDevil* _pOwnerObject)
{
	if(STATE_ATTACK == m_iPrevState)
		m_pModel->Set_Animation("AS_TMonkeyDevil_TCmn_Run_F_Loop");
	else
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Run_F_Loop");
}

void CMonkeyDevilStateMachine::Attack_State(CMonkeyDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CMonkeyDevil::ATK_JUMP:
		if (CMonkeyDevil::EW_HAMMER == _pOwnerObject->Get_EquipWeaponType())
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_THammer_AttackJump01_N");
		}
		else
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_TSword_AttackJump02_N");
		}
		break;
	case CMonkeyDevil::ATK_NORMAL:
		if (CMonkeyDevil::EW_HAMMER == _pOwnerObject->Get_EquipWeaponType())
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_THammer_AttackNormal01_N");
		}
		else
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_TSword_AttackNormal01_N");
		}
		break;
	case CMonkeyDevil::ATK_SP:
		if (CMonkeyDevil::EW_HAMMER == _pOwnerObject->Get_EquipWeaponType())
		{
			if (0 == m_iAttackHammerSPCount)
			{
				++m_iAttackHammerSPCount;
				m_pModel->Change_Animation("AS_TMonkeyDevil_THammer_AttackSpecial01_N_Start");
			}
			else if (1 == m_iAttackHammerSPCount)
			{
				++m_iAttackHammerSPCount;
				m_pModel->Change_Animation("AS_TMonkeyDevil_THammer_AttackSpecial01_N_Loop");
			}
			else if (2 == m_iAttackHammerSPCount)
			{
				m_pModel->Change_Animation("AS_TMonkeyDevil_THammer_AttackSpecial01_N_End");
			}
		}
		else
		{
			m_pModel->Change_Animation("AS_TMonkeyDevil_TSword_AttackNormal02_N");
		}
		break;
	case CMonkeyDevil::ATK_THROW:
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_AttackFangShoot01_N");
		break;
	}
}

void CMonkeyDevilStateMachine::Hit_State(CMonkeyDevil* _pOwnerObject)
{
	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
	if (fValue < 0)
	{
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Damage_F");
	}
	else
	{
		m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Damage_B");
	}
}

void CMonkeyDevilStateMachine::Die_State(CMonkeyDevil* _pOwnerObject)
{
	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
	m_pModel->Change_Animation("AS_TMonkeyDevil_TCmn_Death_N");
}

CMonkeyDevilStateMachine* CMonkeyDevilStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CMonkeyDevilStateMachine* pInstance = new CMonkeyDevilStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMonkeyDevilStateMachine::Create To Failed");
	}
	return pInstance;
}

void CMonkeyDevilStateMachine::Free()
{
	__super::Free();
}
