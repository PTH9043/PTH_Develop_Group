#include "CUrchinDevilStateMachine.h"
#include "CUrchinDevil.h"
#include "CGameInstance.h"

CUrchinDevilStateMachine::CUrchinDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonsterState(_pDevice, _pContext)
{
}

HRESULT CUrchinDevilStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;
	return S_OK;
}

void CUrchinDevilStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CUrchinDevilStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CUrchinDevilStateMachine::Change_State(const _uint& _iState, CUrchinDevil* _pOwnerObject)
{
	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_FIND == _iState)
		Find_State(_pOwnerObject);
	else if (STATE_RETARGET == _iState)
		Retarget_State(_pOwnerObject);
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

void CUrchinDevilStateMachine::Idle_State(CUrchinDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CUrchinDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TVarShell_TFist_Idle_N_Loop");
		break;
	case CUrchinDevil::INIT_WALK:

		break;
	}
}

void CUrchinDevilStateMachine::Find_State(CUrchinDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TVarShell_TCmn_NoneFightThreat_N");
}

void CUrchinDevilStateMachine::Retarget_State(CUrchinDevil* _pOwnerObject)
{
	m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
	m_pModel->Change_Animation("AS_TVarShell_TFist_Idle_N_Loop");
}

void CUrchinDevilStateMachine::Run_State(CUrchinDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TVarShell_TCmn_Run_F_Loop");
}

void CUrchinDevilStateMachine::Attack_State(CUrchinDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CUrchinDevil::ATK_1:
		if (0 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_Start");
			++m_iAttackMandibleIndex;
		}
		else if (1 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_Loop");
			++m_iAttackMandibleIndex;
		}
		else if (2 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_End");
			m_iAttackMandibleIndex = 0;
		}
		break;
	case CUrchinDevil::ATK_2:
		m_pModel->Change_Animation("AS_TVarShellS_TFist_AttackBlindCombo_N");
		break;
	case CUrchinDevil::ATK_3:
		m_pModel->Change_Animation("AS_TVarShellS_TFist_AttackBlindCombo_N");
		break;
	case CUrchinDevil::ATK_SP:
		if (0 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_Start");
			++m_iAttackMandibleIndex;
		}
		else if (1 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_Loop");
			++m_iAttackMandibleIndex;
		}
		else if (2 == m_iAttackMandibleIndex)
		{
			m_pModel->Change_Animation("AS_TVarShell_TFist_AttackMandible01_N_End");
			++m_iAttackMandibleIndex;
		}
		break;
	}
}

void CUrchinDevilStateMachine::Hit_State(CUrchinDevil* _pOwnerObject)
{
	if (CUrchinDevilStateMachine::STATE_ATTACK == m_iPrevState)
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);

	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfRight();
	CCharacter::DIRECTION eDir = CCharacter::DIR_END;

	if (CCharacter::AS_STRONG == _pOwnerObject->Get_AtkStrongEncounter())
	{
		if (fValue < 0)
		{
			m_pModel->Change_Animation("AS_TVarShell_TCmn_Down_S_End");
		}
		else
		{
			m_pModel->Change_Animation("AS_TVarShell_TCmn_Down_P_End");
		}
	}
	else
	{
		if (fValue < 0)
		{
			m_pModel->Change_Animation("AS_TVarShell_TCmn_Damage_F");
		}
		else
		{
			m_pModel->Change_Animation("AS_TVarShell_TCmn_Damage_B");
		}
	}
}

void CUrchinDevilStateMachine::Die_State(CUrchinDevil* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TVarShell_TCmn_Death_N");
}

CUrchinDevilStateMachine* CUrchinDevilStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CUrchinDevilStateMachine* pInstance = new CUrchinDevilStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUrchinDevilStateMachine::Create To Failed");
	}
	return pInstance;
}

void CUrchinDevilStateMachine::Free()
{
	__super::Free();
}
