#include "CWolfDevilStateMachine.h"
#include "CWolfDevil.h"
#include "CGameInstance.h"

CWolfDevilStateMachine::CWolfDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonsterState(_pDevice, _pContext)
{
}

HRESULT CWolfDevilStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;

	m_iState = STATE_IDLE;
	return S_OK;
}

void CWolfDevilStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CWolfDevilStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CWolfDevilStateMachine::Change_State(const _uint& _iState, CWolfDevil* _pOwnerObject)
{
	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_FIND == _iState)
		Find_State(_pOwnerObject);
	else if (STATE_RETARGET == _iState)
		Retarget_State(_pOwnerObject);
	else if (STATE_DODGE == _iState)
		Dodge_State(_pOwnerObject);
	else if (STATE_RUN == _iState)
		Run_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
	else if (STATE_HIT == _iState)
		Hit_State(_pOwnerObject);
	else if (STATE_DIE == _iState)
		Die_State(_pOwnerObject);

	m_pModel->Get_NextAnimation()->Set_FastValue(2.5f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.5f);
}

void CWolfDevilStateMachine::Idle_State(CWolfDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CWolfDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Idle_N_Loop");
		break;
	case CWolfDevil::INIT_SIT:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_NoneFightIdleSit_N_Loop");
		break;
	case CWolfDevil::INIT_EAT:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_NoneFightThreatEat02_N");
		break;
	case CWolfDevil::INIT_WALK:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_NoneFightIdleSit_N_Loop");
		break;
	}
}

void CWolfDevilStateMachine::Find_State(CWolfDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CWolfDevil::INIT_IDLE:
	case CWolfDevil::INIT_EAT:
	case CWolfDevil::INIT_WALK:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Threat01_N");
		break;
	case CWolfDevil::INIT_SIT:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_NoneFightIdleSit_N_Loop");
		break;
	}
}

void CWolfDevilStateMachine::Retarget_State(CWolfDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Walk_F_Loop");
}

void CWolfDevilStateMachine::Dodge_State(CWolfDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Dodge_B");
}

void CWolfDevilStateMachine::Run_State(CWolfDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Walk_F_Loop");
}

void CWolfDevilStateMachine::Attack_State(CWolfDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CWolfDevil::ATK_NORMAL:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_AttackNormal01_N");
		break;
	case CWolfDevil::ATK_JUMP:
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_AttackJump01_N");
		break;
	case CWolfDevil::ATK_SP:
		BEGIN_SINGLETON(CGameInstance, pGameInstance);
		{
			_uint iRanValue = pGameInstance->Get_RandomCount() % 2;
			if (0 == iRanValue)
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_AttackJump02_L90");
			else
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_AttackJump02_R90");
		}
		END_SINGLETON(pGameInstance);
		break;
	}
}

void CWolfDevilStateMachine::Hit_State(CWolfDevil* _pOwnerObject)
{
	if (CWolfDevilStateMachine::STATE_ATTACK == m_iPrevState)
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);

	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
	CCharacter::DIRECTION eDir = CCharacter::DIR_END;

	if (CCharacter::AS_STRONG == _pOwnerObject->Get_AtkStrongEncounter())
	{
		if (0 == m_iDownCount)
		{
			if (fValue < 0)
			{
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Down_S_Start");
			}
			else
			{
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Down_P_Start");
			}
			++m_iDownCount;
		}
		else if (1 == m_iDownCount)
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			if (fValue < 0)
			{
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Down_S_End");
			}
			else
			{
				m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Down_P_End");
			}
		}
	}
	else 
	{
		if (fValue < 0)
		{
			m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Damage_F");
		}
		else
		{
			m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Damage_B");
		}
	}
}

void CWolfDevilStateMachine::Die_State(CWolfDevil* _pOwnerObject)
{
	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
	if (fValue < 0)
	{
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Death_N");
	}
	else
	{
		m_pModel->Change_Animation("AS_TWolfDevil_TCmn_Death_B");
	}
}

CWolfDevilStateMachine* CWolfDevilStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CWolfDevilStateMachine* pInstance = new CWolfDevilStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CWolfDevilStateMachine::Create To Failed");
	}
	return pInstance;
}

void CWolfDevilStateMachine::Free()
{
	__super::Free();
}
