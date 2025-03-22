#include "CAncHanibalStateMachine.h"
#include "CGameInstance.h"
#include "CAncientHannibal.h"

CAncHanibalStateMachine::CAncHanibalStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonsterState(_pDevice, _pContext)
{
}

HRESULT CAncHanibalStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;
	m_iState = STATE_IDLE;
	return S_OK;
}

void CAncHanibalStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CAncHanibalStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{

}

void CAncHanibalStateMachine::Change_State(const _uint& _iState, CAncientHannibal* _pOwnerObject)
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
	else if (STATE_MOVE == _iState)
		Move_State(_pOwnerObject);
	else if (STATE_RUN == _iState)
		Run_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
	else if (STATE_HIT == _iState)
		Hit_State(_pOwnerObject);
	else if (STATE_DIE == _iState)
		Die_State(_pOwnerObject);

	m_pModel->Get_NextAnimation()->Set_FastValue(3.f);
}

void CAncHanibalStateMachine::Idle_State(CAncientHannibal* _pOwnerObject)
{
	m_pModel->Change_Animation("TADrago_TCmn_Appearance_N_Loop");
}

void CAncHanibalStateMachine::Find_State(CAncientHannibal* _pOwnerObject)
{
	m_pModel->Change_Animation("TADrago_TCmn_Appearance_N");
}

void CAncHanibalStateMachine::Retarget_State(CAncientHannibal* _pOwnerObject)
{
	if (STATE_ATTACK == m_iPrevState)
	{
		if (CAncientHannibal::ATK_TAIL == _pOwnerObject->Get_AttackIndex())
		{
			m_pModel->Get_CurAnimation()->Set_AnimSupIndex(CAnimation::SUPINDEX_INIT);
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		}
		else
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		}
	}
	else
	{
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
	}

	m_pModel->Change_Animation("TADrago_TCmn_Idle_N_Loop");
}

void CAncHanibalStateMachine::Dodge_State(CAncientHannibal* _pOwnerObject)
{
	if (true == m_bDoubleDogde)
	{
		m_pModel->Repeat_Animation("TADrago_TCmn_AttackStep01_B");
	}
	else
	{
		m_pModel->Change_Animation("TADrago_TCmn_AttackStep01_B");
	}
}

void CAncHanibalStateMachine::Move_State(CAncientHannibal* _pOwnerObject)
{
	if (true == m_bPlayerFindIdleState)
	{
		m_pModel->Change_Animation("TADrago_TCmn_Idle_N_Loop");
	}
	else
	{
		if (true == m_bPlayerNotForward_MoveStation)
		{
			_float fValue = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
			if (fValue >= 0)
			{
				m_pModel->Change_Animation("TADrago_TCmn_IdleTurn_L0");
			}
			else
			{
				m_pModel->Change_Animation("TADrago_TCmn_IdleTurn_R0");
			}
		}
		else
		{
			switch (_pOwnerObject->Get_Direction())
			{
			case CCharacter::DIR_FOR:
				m_pModel->Change_Animation("TADrago_TCmn_Walk_F_Loop");
				break;
			case CCharacter::DIR_LEFT:
				m_pModel->Change_Animation("TADrago_TCmn_Walk_L_Loop");
				break;
			case CCharacter::DIR_RIGHT:
				m_pModel->Change_Animation("TADrago_TCmn_Walk_R_Loop");
				break;
			}
		}
	}
}

void CAncHanibalStateMachine::Run_State(CAncientHannibal* _pOwnerObject)
{
	if (true == m_bPlayerNotForward_MoveStation)
	{
		_float fValue = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
		if (fValue >= 0)
		{
			m_pModel->Change_Animation("TADrago_TCmn_IdleTurn_L0");
		}
		else
		{
			m_pModel->Change_Animation("TADrago_TCmn_IdleTurn_R0");
		}
	}
	else
	{
		m_pModel->Change_Animation("TADrago_TCmn_Run_F_Loop");
	}
}

void CAncHanibalStateMachine::Attack_State(CAncientHannibal* _pOwnerObject)
{

	if (STATE_DODGE == m_iPrevState)
	{
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
	}

	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CAncientHannibal::ATK_FIRSTORM:
		m_pModel->Change_Animation("TADrago_TCmn_AttackFireStorm01_N");
		break;
	case CAncientHannibal::ATK_GILD_N:
		m_pModel->Change_Animation("TADrago_TCmn_AttackGlide01_N");
		break;
	case CAncientHannibal::ATK_GILD_SP:
		m_pModel->Change_Animation("TADrago_TCmn_AttackGlide02_N");
		break;
	case CAncientHannibal::ATK_HALFTURN:
		m_pModel->Change_Animation("TADrago_TCmn_AttackHalfTurn01_N");
		break;
	case CAncientHannibal::ATK_ONEHAND:
		m_pModel->Change_Animation("TADrago_TCmn_AttackSlashL02_N");
		break;
	case CAncientHannibal::ATK_HOOK:
		m_pModel->Change_Animation("TADrago_TCmn_AttackHookLR_N");
		break;
	case CAncientHannibal::ATK_JUMP:
		m_pModel->Change_Animation("TADrago_TCmn_AttackJump01_N");
		break;
	case CAncientHannibal::ATK_POINTSHOOT:
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			_uint iRan = pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
				m_pModel->Change_Animation("TADrago_TCmn_AttackPointShoot01_N");
			else if (1 == iRan)
				m_pModel->Change_Animation("TADrago_TCmn_AttackPointShoot02_N");
		}
		END_SINGLETON(pGameInstance);
	}
		break;
	case CAncientHannibal::ATK_SLASH:
		m_pModel->Change_Animation("TADrago_TCmn_AttackSlashL01_N");
		break;
	case CAncientHannibal::ATK_STAMP:
		m_pModel->Change_Animation("TADrago_TCmn_AttackStamp01_N");
		break;
	case CAncientHannibal::ATK_STEP:
		m_pModel->Change_Animation("TADrago_TCmn_AttackStep01_B");
		break;
	case CAncientHannibal::ATK_TAIL:
		m_pModel->Change_Animation("TADrago_TCmn_AttackTail01_N");
		break;
	case CAncientHannibal::ATK_WEAPONCOMBO1:
		m_pModel->Change_Animation("TADrago_TCmn_AttackWeaponComb01_N");
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		break;
	case CAncientHannibal::ATK_SNAKE:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TADrago_TCmn_AttackWeaponSlide01_N_Start");
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TADrago_TCmn_AttackWeaponSlide01_N_Loop");
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("ATADrago_TCmn_AttackWeaponSlide01_N_End");
			++m_iAttackCount;
		}
		break;
	}
}

void CAncHanibalStateMachine::Hit_State(CAncientHannibal* _pOwnerObject)
{
	if (false == m_bTiredState)
	{
		_float fDot = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
		_uint iJudge = 0;
		if (90.f >= fDot && fDot >= 0)
			iJudge = 1;
		else if (180.f >= fDot && fDot >= 90.f)
			iJudge = 2;
		else if (-90 >= fDot && fDot >= -180.f)
			iJudge = 3;
		else if (-0 >= fDot && fDot >= -90)
			iJudge = 4;

		switch (iJudge)
		{
		case 1:
			m_pModel->Change_Animation("TADrago_TCmn_Damage_FL");
			break;
		case 2:
			m_pModel->Change_Animation("TADrago_TCmn_Damage_BL");
			break;
		case 3:
			m_pModel->Change_Animation("TADrago_TCmn_Damage_BR");
			break;
		case 4:
			m_pModel->Change_Animation("TADrago_TCmn_Damage_FR");
			break;
		}
	}
	else
	{
		m_bNextPage = true;
		m_pModel->Change_Animation("TADrago_TCmn_Down_N");
		m_bTiredState = false;
	}
}

void CAncHanibalStateMachine::Die_State(CAncientHannibal* _pOwnerObject)
{
	m_pModel->Change_Animation("TADrago_TCmn_Death_N");
}

CAncHanibalStateMachine* CAncHanibalStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CAncHanibalStateMachine* pInstance = new CAncHanibalStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAncHannibalStateMachine::Create To Failed");
	}
	return pInstance;
}

void CAncHanibalStateMachine::Free()
{
	__super::Free();
}
