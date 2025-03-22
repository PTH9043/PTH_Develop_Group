#include "CGKWStateMachine.h"
#include "CGameInstance.h"
#include "CGateKeeperWoman.h"

CGKWStateMachine::CGKWStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonsterState(_pDevice, _pContext)
{
}

HRESULT CGKWStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;

	m_iState = STATE_IDLE;
	return S_OK;
}

void CGKWStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CGKWStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CGKWStateMachine::Change_State(const _uint& _iState, CGateKeeperWoman* _pOwnerObject)
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
	m_bArrive = false;

	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
}

void CGKWStateMachine::Idle_State(CGateKeeperWoman* _pOwnerObject)
{
	m_pModel->Change_Animation("TSwordGK_Appearance_N_Loop");
}

void CGKWStateMachine::Find_State(CGateKeeperWoman* _pOwnerObject)
{
	m_pModel->Change_Animation("TSwordGK_Appearance_N");
}

void CGKWStateMachine::Retarget_State(CGateKeeperWoman* _pOwnerObject)
{
	if (true == m_bTurnLeft90)
	{
		m_pModel->Change_Animation("TSwordGK_IdleTurn_L90");
	}
	else if (true == m_bTurnRight90)
	{
		m_pModel->Change_Animation("TSwordGK_IdleTurn_R90");
	}
	else if(true == m_bTurn180)
	{
		_float fDot = _pOwnerObject->SelfToDot_DirToPlayerLookConvert();
		if(fDot < 0)
			m_pModel->Change_Animation("TSwordGK_IdleTurn_L180");
		else
			m_pModel->Change_Animation("TSwordGK_IdleTurn_R180");
	}
	else
	{
		m_pModel->Change_Animation("TSwordGK_Idle_N_Loop");
	}
}

void CGKWStateMachine::Dodge_State(CGateKeeperWoman* _pOwnerObject)
{
	_uint iPat = _pOwnerObject->Get_AttackPattern();
	_float fRot = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();

	switch (_pOwnerObject->Get_AttackPattern())
	{
	case CGateKeeperWoman::PAT_AVOIDBACK:
		m_pModel->Change_Animation("TSwordGK_DodgeNomal_B");
		break;
	case CGateKeeperWoman::PAT_AVOIDLR:
		if (fRot > 0.f)
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
		else
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
		break;
	case CGateKeeperWoman::PAT_AVOIDFOR:
		m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
		break;
	case CGateKeeperWoman::PAT_AVOID_BACK_LEFT_RIGHT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_B");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		else if (2 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_BACK_RIGHT_LEFT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_B");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		else if (2 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_LEFTRIGHT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_RIGHTLEFT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_BACKBACK:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_B");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_SameAnimationSup("TSwordGK_DodgeNomal_B");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_FORFOR:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_SameAnimationSup("TSwordGK_DodgeNomal_F");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_FOR_LEFT_RIGHT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		else if (2 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		break;
	case CGateKeeperWoman::PAT_AVOID_FOR_RIGHT_LEFT:
		if (0 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
			++m_iDogdeCount;
		}
		else if (1 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iDogdeCount;
		}
		else if (2 == m_iDogdeCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			++m_iDogdeCount;
		}
		break;
	}

	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
}

void CGKWStateMachine::Move_State(CGateKeeperWoman* _pOwnerObject)
{
	if (true == m_bChangeMoveState)
	{
		switch (_pOwnerObject->Get_Direction())
		{
		case CCharacter::DIR_BACK:
			m_pModel->Change_Animation("TSwordGK_Walk_B_Loop");
			break;
		case CCharacter::DIR_BACKLEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_BL_Loop");
			break;
		case CCharacter::DIR_BACKRIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_BR_Loop");
			break;
		case CCharacter::DIR_FOR:
			m_pModel->Change_Animation("TSwordGK_Walk_F_Loop");
			break;
		case CCharacter::DIR_FORLEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_FL_Loop");
			break;
		case CCharacter::DIR_FORRIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_FR_Loop");
			break;
		case CCharacter::DIR_LEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_L_Loop");
			break;
		case CCharacter::DIR_RIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_R_Loop");
			break;
		}
		m_bChangeMoveState = false;
	}
	else
	{
		switch (_pOwnerObject->Get_AttackPattern())
		{
		case CGateKeeperWoman::PAT_MOVEFOR:
			m_pModel->Change_Animation("TSwordGK_Walk_F_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVEBACK:
			m_pModel->Change_Animation("TSwordGK_Walk_B_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVELEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_L_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVERIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_R_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVE_BACKLEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_BL_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVE_BACKRIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_BR_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVE_FORLEFT:
			m_pModel->Change_Animation("TSwordGK_Walk_FL_Loop");
			break;
		case CGateKeeperWoman::PAT_MOVE_FORRIGHT:
			m_pModel->Change_Animation("TSwordGK_Walk_FR_Loop");
			break;
		}
	}

	m_pModel->Get_NextAnimation()->Set_FastValue(2.5f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.5f);
}

void CGKWStateMachine::Run_State(CGateKeeperWoman* _pOwnerObject)
{
}

void CGKWStateMachine::Attack_State(CGateKeeperWoman* _pOwnerObject)
{
	m_fNextAnimationCalculationTime = 2.5f;
	m_bDamaged = false;
	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CGateKeeperWoman::ATK_ATKSLASH_1:
	case CGateKeeperWoman::ATK_ATKSLASH_2:
	case CGateKeeperWoman::ATK_ATKSLASH_3:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_ATTACKEXPLOSION:
		m_pModel->Change_Animation("TSwordGK_KetsugiExplosion01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKJUMP:
		m_pModel->Change_Animation("TSwordGK_AttackJump01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKNORMAL_1:
	case CGateKeeperWoman::ATK_ATTACKNORMAL_2:
	case CGateKeeperWoman::ATK_ATTACKNORMAL_3:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_ATTACKRANGESHOOT:
		m_pModel->Change_Animation("TSwordGK_KetsugiAllrangeShoot01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKROTATION:
		m_pModel->Change_Animation("TSwordGK_AttackRotation01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKSHOOT:
		m_pModel->Change_Animation("TSwordGK_KetsugiAimShoot01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKSTORM:
		m_pModel->Change_Animation("TSwordGK_KetsugiStorm01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKSTORNG:
		m_pModel->Change_Animation("TSwordGK_AttackStrong01_N");
		break;
	case CGateKeeperWoman::ATK_ATTACKTHRUST:
		m_pModel->Change_Animation("TSwordGK_AttackThrust01_N");
		break;
	case CGateKeeperWoman::ATK_ATTAKAVOID:
		m_pModel->Change_Animation("TSwordGK_AttackAvoid01_B");
		break;
	case CGateKeeperWoman::ATK_BARRIER:
		m_pModel->Change_Animation("TSwordGK_KetsugiBarrier01_N");
		break;
	case CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKNORMAL2:
	case CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2:
	case CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKNORMAL3:
	case CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3:
		if (0 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 1.5f;
			m_pModel->Change_Animation("TSwordGK_AttackThrust01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (3 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_STRONG_AFTER_ATKNORMAL2:
	case CGateKeeperWoman::ATK_STRONG_AFTER_ATKNORMAL3:
	case CGateKeeperWoman::ATK_STRONG_AFTER_ATKSLASH2:
	case CGateKeeperWoman::ATK_STRONG_AFTER_ATKSLASH3:
		if (0 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 1.5f;
			m_pModel->Change_Animation("TSwordGK_AttackStrong01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (3 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_FAVOID_ATKNORMAL2:
	case CGateKeeperWoman::ATK_FAVOID_ATKSLASH2:
	case CGateKeeperWoman::ATK_FAVOID_ATKNORMAL3:
	case CGateKeeperWoman::ATK_FAVOID_ATKSLASH3:
		if (0 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 1.5f;
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (3 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_ATKNORMAL2_AFTER_ROTATIONATK:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackRotation01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_ATKSLASH2_AFTER_ROTATIONATK:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackRotation01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_BACK_ATTACKJUMP:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackAvoid01_B");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackJump01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_AVOIDFOR_ATTACKJUMP:
		if (0 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_DodgeNomal_F");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackJump01_N");
			++m_iAttackCount;
		}
	case  CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2: 
	case CGateKeeperWoman::ATK_AVOIDR_ATKNORMAL2:
	case CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2_STRONG:
	case CGateKeeperWoman::ATK_AVOIDR_ATKNORMAL2_STRONG:
	case CGateKeeperWoman::ATK_AVOIDR_ATKNORMAL2_ROTATE:
	case CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2_ROTATE:
		if (0 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 1.5f;
			if (CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2 == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2_STRONG == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2_ROTATE == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			else
				m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackNormal01_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_pModel->Change_Animation("TSwordGK_AttackNormal02_N");
			++m_iAttackCount;
		}
		else if (3 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_AVOIDR_ATKNORMAL2_STRONG == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_AVOIDL_ATKNORMAL2_STRONG == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackStrong01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackRotation01_N");
			++m_iAttackCount;
		}
		break;
	case CGateKeeperWoman::ATK_AVOIDR_ATKSLASH2_STRONG:
	case CGateKeeperWoman::ATK_AVOIDL_ATKSLASH2_STRONG:
	case CGateKeeperWoman::ATK_AVOIDR_ATKSLASH2_ROTATE:
	case CGateKeeperWoman::ATK_AVOIDL_ATKSLASH2_ROTATE:
		if (0 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 1.5f;
			if (CGateKeeperWoman::ATK_AVOIDL_ATKSLASH2_STRONG == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_AVOIDL_ATKSLASH2_ROTATE == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_DodgeNomal_L");
			else
				m_pModel->Change_Animation("TSwordGK_DodgeNomal_R");
			++m_iAttackCount;
		}
		else if (1 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackSlash01_N");
			++m_iAttackCount;
		}
		else if (2 == m_iAttackCount)
		{
			m_fNextAnimationCalculationTime = 2.75f;
			m_pModel->Change_Animation("TSwordGK_AttackSlash02_N");
			++m_iAttackCount;
		}
		else if (3 == m_iAttackCount)
		{
			if (CGateKeeperWoman::ATK_AVOIDL_ATKSLASH2_STRONG == _pOwnerObject->Get_AttackIndex()
				|| CGateKeeperWoman::ATK_AVOIDR_ATKSLASH2_STRONG == _pOwnerObject->Get_AttackIndex())
				m_pModel->Change_Animation("TSwordGK_AttackStrong01_N");
			else
				m_pModel->Change_Animation("TSwordGK_AttackRotation01_N");
			++m_iAttackCount;
		}
		break;
	}
}

void CGKWStateMachine::Hit_State(CGateKeeperWoman* _pOwnerObject)
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
			m_pModel->Change_Animation("TSwordGK_Damage_FL");
			break;
		case 2:
			m_pModel->Change_Animation("TSwordGK_Damage_BL");
			break;
		case 3:
			m_pModel->Change_Animation("TSwordGK_Damage_BR");
			break;
		case 4:
			m_pModel->Change_Animation("TSwordGK_Damage_FR");
			break;
		}
	}
	else
	{
		switch (m_iTiredCount)
		{
		case 0:
			m_pModel->Change_Animation("TSwordGK_Down_P_Start");
			++m_iTiredCount;
			break;
		case 1:
			m_pModel->Change_Animation("TSwordGK_Down_P_Loop");
			++m_iTiredCount;
			break;
		case 2:
			m_pModel->Change_Animation("TSwordGK_Down_P_End");
			m_bNextPage = true;
			break;
		}
	}

	m_bDamaged = true;
}

void CGKWStateMachine::Die_State(CGateKeeperWoman* _pOwnerObject)
{
	m_pModel->Change_Animation("TSwordGK_Death_N");
}

CGKWStateMachine* CGKWStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CGKWStateMachine* pInstance = new CGKWStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGKStateMachine::Create To Failed");
	}
	return pInstance;
}

void CGKWStateMachine::Free()
{
	__super::Free();
}
