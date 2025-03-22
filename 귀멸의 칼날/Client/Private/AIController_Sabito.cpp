#include "AIController_Sabito.h"
#include "GameInstance.h"

CAIController_Sabito::CAIController_Sabito(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CAI_CharController(_pDevice, _pContext)
{
}

HRESULT CAIController_Sabito::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	m_pSabito = static_cast<CSabito*>(_pGameObject);
	Find_Target();

	m_tIdleWaitTimer = CUSTIMER(5.f);

	return S_OK;
}

void CAIController_Sabito::Tick(const _double& _dTimeDelta)
{
	if (false == m_bStopAI)
	{
		m_OutVoidDatas.clear();
		Judge_State(_dTimeDelta);
	}
	else
	{
		m_iCurStateData = CSabito::CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
	}
}

void CAIController_Sabito::Late_Tick(const _double& _dTimeDelta)
{
	m_dStackUpTime = _dTimeDelta;
}

void CAIController_Sabito::Insert_Datas(VOIDDATAS _pData)
{
	__super::Insert_Datas(_pData);
}

VOIDDATAS CAIController_Sabito::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.reserve(1000);
	// Push State Data
	m_OutVoidDatas.push_back(&m_iCurStateData);
	// Transofrm
	m_OutVoidDatas.push_back(&m_vDirection);
	if (nullptr != m_pTargetObject)
		m_OutVoidDatas.push_back(m_pTargetObject->Get_Transform());
	return __super::Out_Datas();
}

void CAIController_Sabito::Judge_State(const _double& _dTimeDelta)
{
	CPlayer* pTargetPlayer = static_cast<CPlayer*>(m_pTargetObject);
	if (nullptr == pTargetPlayer)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_ReceiveVoidDatas.size() > 0)
	{
		m_strPrevAnimationType = m_strCurrentAnimationType;
		m_strCurrentAnimationType = *static_cast<_string*>(m_ReceiveVoidDatas[CSabito::INPUT_ANIMTYPE]);

		if ("Hit" == m_strCurrentAnimationType)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		// 현재 애니메이션 클립을 가져온다. 
		CAnimationClip* pCurClip = static_cast<CAnimationClip*>(m_ReceiveVoidDatas[CSabito::INPUT_CURANIMATION]);
		// 다음 행동이 비어있지 않으면
		if (CSabito::STATE_END != m_iNextStateData)
		{
			// 그 행동을 처리하기 위해 움직인다.
			m_iCurStateData = m_iNextStateData;
			if (m_pCurAnimationClip == pCurClip)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			m_iNextStateData = CSabito::STATE_END;
		}

		if ("wdjjjjjjjjjwjjj" == m_strCurrentAnimationType)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		// 다음 애니메이션 클립을 넣어준다.
		m_pCurAnimationClip = pCurClip;

		if ("Idle" != m_strCurrentAnimationType)
		{
			_uint iRand = rand() % 5 + 2;
			m_tIdleWaitTimer = CUSTIMER((_float)iRand);
			m_tIdleWaitTimer.Reset_Timer();
		}

		// 이전 행동
		if (CSabito::STATE_END != m_iCurStateData)
			m_iPrevStateData = m_iCurStateData;
		// Current State End
		m_iCurStateData = CSabito::STATE_END;
		_float fDistance = m_pSabito->OtherCharacterToDistance(pTargetPlayer);
		_float fDot = m_pSabito->OtherCharacterDirToLookConverter(pTargetPlayer);
		m_iDir = Judge_Direction(fDot);

		Ready_RanPat_Time();

		if (false == Judge_TargetDamgedState(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot)
			&& false == Judge_TargetDashState(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot))
		{
			if (false == Judge_Target_GuardState(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot)
				&& false == Judge_Succes_Throw(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot))
			{
				if ("Idle" == m_strCurrentAnimationType)
					Current_Idle_state(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Move" == m_strCurrentAnimationType)
					Current_Move_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Dash" == m_strCurrentAnimationType)
					Current_Dash_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("JumpDash" == m_strCurrentAnimationType)
					Current_JumpDash_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Attack" == m_strCurrentAnimationType)
					Current_Attack_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Attack_S" == m_strCurrentAnimationType)
					Current_Attack_S_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Skill" == m_strCurrentAnimationType)
					Current_Skill_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("JumpAttack" == m_strCurrentAnimationType)
					Current_JumpAttack_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Step" == m_strCurrentAnimationType)
					Current_Step_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Jump" == m_strCurrentAnimationType)
					Current_Jump_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Charge" == m_strCurrentAnimationType)
					Current_Charge_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Guard" == m_strCurrentAnimationType)
					Current_Guard_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else if ("Hit" == m_strCurrentAnimationType)
					Current_Hit_State(_dTimeDelta, pGameInstance, pTargetPlayer, fDistance, fDot);
				else
				{
					// 아무것도 들어오지 않을 떄 IDLE
					m_iCurStateData = CSabito::STATE_IDLE;
				}
			}
		}
		
		// Prev Enemy State 
		m_strPrevEnemyState = m_strCurrentAnimationType;

		m_vDirection = m_pTargetObject->Get_Transform()->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		m_vDirection.y = 0.f;

		if ("Step" != m_pSabito->Get_Animator()->Get_AnimationType())
		{
			if ((m_iCurStateData % CSabito::STATE_JUDGE) == CSabito::STATE_STEP)
			{
				m_pSabito->Get_Transform()->LookAt(m_pSabito->Get_Transform()->Get_Pos() + m_vDirection.Get_Vector());
			}
		}

	}
	RELEASE_INSTANCE(CGameInstance);
}
#pragma region CSabito::STATE_JUDGE
#pragma region IDLE
void CAIController_Sabito::Current_Idle_state(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();

	if (strType == "MOVE" || strType == "DASH" || strType == "DASH_FREE")
	{
		CurIdle_To_EnemyMove_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "ATTACK" || strType == "AIR_ATTACK" || strType == "CHARGET_ATTACK" || strType == "SKILL_1" 
		|| strType == "SKILL_2" || strType == "SKILL_3" || strType == "AIR_SKILL_1" || strType == "AIR_SKILL_2" || strType == "AIR_SKILL_3"
		|| strType == "THORW")
	{
		CurIdle_To_EnemyAttack_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "GUARD")
	{
		CurIdle_To_EnemyGuard_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "JUMP" || strType == "JUMP_PRE")
	{
		CurIdle_To_EnemyJump_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "AIR_HIT " || strType == "HIT")
	{
		CurIdle_To_EnemyHit_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else 
	{
		CurIdle_To_EnemyIdle_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}

	CurIdle_To_PassTime(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
}
// 적이 가만히 있을 경우
void CAIController_Sabito::CurIdle_To_EnemyIdle_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		if (_fDistance >= DISTANCE_ATK_RANGE)
		{
			_uint iRan = RanCheckTime();
			if (0 == iRan || 1 == iRan)
			{
				iRan = _pGameInstance->Get_RandomCount();
				if(0 == iRan)
					m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
				else
					m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
			}
			else
			{
				m_iCurStateData = Judge_Charge_State();
			}
		}
		else
		{
			// Attack Combo
			m_iCurStateData = Judge_AttackCombo();
		}
	}
	else
	{
		if (_fDistance >= DISTANCE_CHARGE_RANGE)
		{
			m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}
// 적이 움직일 경우
void CAIController_Sabito::CurIdle_To_EnemyMove_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// Dir Forward
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		// 공격 거리 안에 들어왔으면 공격 한다.
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			// 옆이나 오른쪽으로 대쉬
			if (0 == iRan)
			{
				// Dash
				m_iCurStateData = CSabito::STATE_STEP;

				if (_fDot > 0)
					m_iCurStateData += CSabito::STATE_RIGHT;
				else
					m_iCurStateData += CSabito::STATE_LEFT;
			}
			// 공격
			else if (1 == iRan || 2 == iRan)
			{
				// Move			
				m_iCurStateData = Judge_AttackCombo();
			}
			else
			{
				// 대쉬
				m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
			}
		}
		else
		{
			if (_fDistance <= DISTANCE_ATK_RANGE)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if (0 == iRan)
				{
					m_iCurStateData = Judge_StepRLD();
				}
				else
				{
					m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
				}
			}
			else
			{
				m_iCurStateData = Judge_Charge_State();
			}
		}
	}
	// 만약 사비토가 플레이어를 바라 보고 있지 않을 경우
	else
	{
		if (_fDistance >= DISTANCE_CHARGE_RANGE)
		{
			m_iCurStateData = Judge_Charge_State();
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}
// 적의 공격 패턴
void CAIController_Sabito::CurIdle_To_EnemyAttack_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// Dir Forward
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
			{
				// 드래곤 공격
				m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON;
			}
			else
			{
				// 일반 공격
				m_iCurStateData = Judge_AttackCombo();
			}
		}
		else if(_fDistance <= DISTANCE_CHARGE_RANGE)
		{
			// State Jump 
			m_iCurStateData = Judge_StepRLD();
		}
		else
		{
			m_iCurStateData = Judge_Charge_State();
		}
	}
	else
	{
		if (_fDistance <= DISTANCE_CHARGE_RANGE)
		{
			// State Jump 
			m_iCurStateData = Judge_StepRLD();
		}
		else
		{
			m_iCurStateData = Judge_Charge_State();
		}
	}
}
// 적의 방어 패턴
void CAIController_Sabito::CurIdle_To_EnemyGuard_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer,
	const _float& _fDistance, const _float& _fDot)
{
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			// 강 공격
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
	else
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
		else 
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
			{
				m_iCurStateData = Judge_StepRLD();
			}
			else
			{
				m_iCurStateData = Judge_Charge_State();
			}
		}
	}
}
// 적의 점프 패턴
void CAIController_Sabito::CurIdle_To_EnemyJump_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// 정면을 바라볼 때
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			// 적이 떨어지는 상황일 때
			if (_pTargetPlayer->Is_JudgeFallState())
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 4;
				if (0 == iRan || 1 == iRan)
				{
					// 토네이도
					m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
				}
				else
				{
					// 점프 한다.
					m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_BACK;
					m_iNextStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_JUMPATTACK;
				}
			}
			// 적이 떨어지는 상황이 아닐 때
			else
			{
				m_iCurStateData = Judge_StepRLD();
			}
		}
		// 거리가 가까운 상황이 아닐 경우
		else
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
			{
				// Back
				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
				m_iNextStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
			}
			else
			{
				// 점프 한다.
				m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_BACK;
			}
		}
	}
	else
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = Judge_StepRLD();
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
			{
				m_iNextStateData = Judge_StepRLD();
			}
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}
// 적의 맞는 패턴
void CAIController_Sabito::CurIdle_To_EnemyHit_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// 정면을 바라볼 때
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		m_iCurStateData = Judge_Charge_State();
	}
	else
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = Judge_StepRLD();
		}
		else
		{
			m_iCurStateData = Judge_Charge_State();
		}
	}
}

void CAIController_Sabito::CurIdle_To_PassTime(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (true == m_tIdleWaitTimer.Is_Over(_dTimeDelta))
	{
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 3;
			if (0 == iRan)
			{
				m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
			}
			else
			{
				m_iCurStateData = Judge_AttackCombo();
			}
		}
		else
		{
			m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
	}
}

#pragma endregion IDLE
#pragma region MOVE

void CAIController_Sabito::Current_Move_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();


	_float fDistance = m_pSabito->OtherCharacterToDistance(_pTargetPlayer);
	_float fDot = m_pSabito->OtherCharacterDirToLookConverter(_pTargetPlayer);
	m_iDir = Judge_Direction(fDot);

	if (strType == "MOVE" || strType == "DASH" || strType == "DASH_FREE")
	{
		CurMove_To_EnemyMove_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "ATTACK" || strType == "AIR_ATTACK" || strType == "CHARGET_ATTACK" || strType == "SKILL_1"
		|| strType == "SKILL_2" || strType == "SKILL_3" || strType == "AIR_SKILL_1" || strType == "AIR_SKILL_2" || strType == "AIR_SKILL_3"
		|| strType == "THORW")
	{
		CurMove_To_EnemyAttack_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "GUARD")
	{
		CurMove_To_EnemyGaurd_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "JUMP" || strType == "JUMP_PRE")
	{
		CurMove_To_EnemyJump_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else if (strType == "AIR_HIT " || strType == "HIT")
	{
		CurMove_To_EnemyHit_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}
	else
	{
		CurMove_To_EnemyIdle_Pattern(_dTimeDelta, _pGameInstance, _pTargetPlayer, _fDistance, _fDot);
	}

}
// CurMove To Enemy Idle Pattern
void CAIController_Sabito::CurMove_To_EnemyIdle_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
				m_iCurStateData = Judge_AttackCombo();
			else
				m_iCurStateData = Judge_SkillCombo();
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
	}
	else
	{
		if (_fDistance >= DISTANCE_CHARGE_RANGE)
		{
			m_iCurStateData = CSabito::STATE_IDLE +  CSabito::STATE_RETURN_IDLE;
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}
// CurMove To Enemy Move Pattern
void CAIController_Sabito::CurMove_To_EnemyMove_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
				m_iCurStateData = Judge_AttackCombo();
			else if (1 == iRan)
			{
				if (_fDot > 0)
					m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
				else
					m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
			}
			else
				m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
		else
		{
			if (_fDistance >= DISTANCE_CHARGE_RANGE)
			{
				m_iCurStateData  = Judge_Charge_State();
			}
			else
			{
				m_iCurStateData = Judge_StepRLD();
			}
		}
	}
	else
	{
		if (_fDistance >= DISTANCE_CHARGE_RANGE)
		{
			m_iCurStateData  = Judge_Charge_State();
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}
// CurMove To Enemy Attack Pattern
void CAIController_Sabito::CurMove_To_EnemyAttack_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (_fDistance >= DISTANCE_ATK_RANGE)
	{
		m_iCurStateData = Judge_Charge_State();
	}
	else
	{
		m_iCurStateData = Judge_StepRLD();
		m_iNextStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCH;
	}
}
// CurMove To Enemy Gaurd Pattern
void CAIController_Sabito::CurMove_To_EnemyGaurd_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (_fDistance <=  DISTANCE_ATK_RANGE)
	{
		m_iCurStateData = CSabito::STATE_ATK_STRONG;
		m_iNextStateData = Judge_AttackCombo();
	}
	else
	{
		_uint iRan = _pGameInstance->Get_RandomCount() % 2;
		if (0 == iRan)
		{
			m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
		else
		{
			m_iCurStateData = Judge_Charge_State();
		}
	}
}

void CAIController_Sabito::CurMove_To_EnemyJump_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// 정면을 바라볼 때
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			// 적이 떨어지는 상황일 때
			if (_pTargetPlayer->Is_JudgeFallState())
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 4;
				if (0 == iRan || 1 == iRan)
				{
					// 토네이도
					m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
				}
				else
				{
					// 점프 한다.
					m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_BACK;
					m_iNextStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_JUMPATTACK;
				}
			}
			// 적이 떨어지는 상황이 아닐 때
			else
			{
				m_iCurStateData = Judge_StepRLD();
			}
		}
		// 거리가 가까운 상황이 아닐 경우
		else
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
			{
				// Back
				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
				m_iNextStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
			}
			else
			{
				// 점프 한다.
				m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_BACK;
			}
		}
	}
	else
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = Judge_StepRLD();
			_uint iRan = _pGameInstance->Get_RandomCount() % 4;
			if (0 == iRan || 1 == iRan)
			{
				m_iNextStateData = Judge_StepRLD();
			}
		}
		else
		{
			m_iCurStateData = Judge_StepRLD();
		}
	}
}

void CAIController_Sabito::CurMove_To_EnemyHit_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	// 정면을 바라볼 때
	if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
	{
		m_iCurStateData = Judge_Charge_State();
	}
	else
	{
		// 거리가 가까울 경우
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = Judge_StepRLD();
		}
		else
		{
			m_iCurStateData = Judge_Charge_State();
		}
	}
}

#pragma endregion MOVE
#pragma region DASH

void CAIController_Sabito::Current_Dash_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	_string strType = "";
	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	strType = pTargetAnimator->Get_AnimationType();

	if ("GUARD" == strType || "SKILL_1" == strType || "SKILL_2" == strType || "SKILL_3" == strType)
	{
		m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_END;
	}
}

#pragma endregion DASH

#pragma region JUMPDASH

void CAIController_Sabito::Current_JumpDash_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();

}

#pragma endregion JUMPDASH

#pragma region ATTACK

void CAIController_Sabito::Current_Attack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();

	if (_fDistance <= DISTANCE_ATK_RANGE)
	{
		if (strType == "GUARD")
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		}
		else
		{
			if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
				m_iCurStateData = m_iPrevStateData;
			else
			{
				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
			}
		}
	}
	else
	{
		if ((m_iDir & DIR_FORWARD) == DIR_FORWARD)
			m_iCurStateData = m_iPrevStateData;
		else
		{
			if (_fDot > 0)
				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
			else
				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
		}
	}
}

#pragma endregion ATTACK

#pragma region ATTACK_S

void CAIController_Sabito::Current_Attack_S_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{

}
#pragma endregion ATTACK_S

#pragma region SKILL
void CAIController_Sabito::Current_Skill_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{

}
#pragma endregion SKILL

#pragma region JUMPATTACK
void CAIController_Sabito::Current_JumpAttack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{

}
#pragma endregion JUMPATTACK

#pragma region STEP

void CAIController_Sabito::Current_Step_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();

}

#pragma endregion STEP

#pragma region JUMP

void CAIController_Sabito::Current_Jump_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();

	m_iCurStateData = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
}

#pragma endregion JUMP

#pragma region CHARGE
void CAIController_Sabito::Current_Charge_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{

	vector<CAnimator::TRANSITION>* pTransitionAnimator = static_cast<vector<CAnimator::TRANSITION>*>(m_ReceiveVoidDatas[CSabito::INPUT_TRANSITIONLIST]);
	if (nullptr == pTransitionAnimator)
		return;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return;

	_string strType = "";
	if (nullptr != _pTargetPlayer->Get_Animator())
		strType = _pTargetPlayer->Get_Animator()->Get_AnimationType();


	if (_fDistance >= DISTANCE_CHARGE_RANGE)
	{
		m_iCurStateData = Judge_Charge_State();
	}
}
#pragma endregion CHARGE
#pragma region GUARD
void CAIController_Sabito::Current_Guard_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
}
#pragma endregion GUARD
#pragma region HIT
void CAIController_Sabito::Current_Hit_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
}
#pragma endregion HIT

_bool CAIController_Sabito::Judge_Target_GuardState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;

	if ("GUARD" == pTargetAnimator->Get_AnimationType())
	{
		if (_fDistance < DISTANCE_ATK_RANGE)
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCH;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
		return true;
	}
	return bIsTrue;
}

_bool CAIController_Sabito::Judge_Succes_Throw(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;

	if ("Hit" == pTargetAnimator->Get_AnimationType())
	{
		m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCHSUCCESS;
		return true;
	}

	return bIsTrue;
}

_bool CAIController_Sabito::Judge_Air_Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;

	if (CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR == m_iPrevStateData || CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON == m_iPrevStateData)
	{
		// 애니메이션 타입을 검사한다.
		if ("AIR_DOWN" == pTargetAnimator->Get_AnimationType())
		{
			m_iCurStateData = CSabito::STATE_DASH +  CSabito::STATE_DASH_AIR;
			return true;
		}
	}

	return bIsTrue;
}

_bool CAIController_Sabito::Judge_Air_AttacKCombo(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;

	if (CSabito::STATE_DASH + CSabito::STATE_DASH_AIR == m_iPrevStateData)
	{
		// 애니메이션 타입을 검사한다.
		if ("AIR_HIT" == pTargetAnimator->Get_AnimationType())
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_AIR;
			return true;
		}
	}

	return bIsTrue;
}

_bool CAIController_Sabito::Judge_TargetDamgedState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;

	if ("DOWN" == pTargetAnimator->Get_AnimationType() || "AIR_DOWN" == pTargetAnimator->Get_AnimationType())
	{
		if (_fDistance <= DISTANCE_CHARGE_RANGE)
		{
			if (m_fSaveDistacne - 1.f <= _fDistance && m_fSaveDistacne + 1.f >= _fDistance)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if(0 == iRan)
					m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
				else
					m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
			}
			else
			{

				m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
				m_fSaveDistacne = _fDistance;
			}
		}
		else
		{
			m_iCurStateData = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
		}
		return true;
	}

	return false;
}

_bool CAIController_Sabito::Judge_TargetDashState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
	if (nullptr == _pTargetPlayer)
		return false;

	_bool bIsTrue = false;

	CAnimator* pTargetAnimator = _pTargetPlayer->Get_Animator();
	if (nullptr == pTargetAnimator)
		return false;
	
	if ("DASH" == pTargetAnimator->Get_AnimationType())
	{
		if (_fDistance <= DISTANCE_ATK_RANGE)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
			{
				m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
			}
			else
			{
				m_iCurStateData = Judge_AttackCombo();
			}
		}
		else
		{
			m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
		}
		return true;
	}
	return false;
}

void CAIController_Sabito::Attack_Combo(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot)
{
}

#pragma endregion CSabito::STATE_JUDGE
// Just Move
_uint CAIController_Sabito::Just_Move(const _uint& _iDir, const _float& _fDot, const _uint& _iForwardstate)
{
	_uint iState = 0;
	// Just Move
	// 앞에 나왔으면
	if (DIR_FORWARD == m_iDir)
		// 앞으로 가라 
		iState = _iForwardstate;
	// Left Forward
	else
		iState = CSabito::STATE_STEP | CSabito::STATE_BACK;
	return iState;
}
// Radom Pattern
_uint CAIController_Sabito::Random_Pattern(const _uint _iValue)
{
	if (0 == _iValue)
		return 0;
	_uint iValue = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		iValue = pGameInstance->Get_RandomCount() % _iValue;
	}
	RELEASE_INSTANCE(CGameInstance);
	return iValue;
}
// Judge Attack Combo
_uint CAIController_Sabito::Judge_AttackCombo()
{
	_uint iValue = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		iValue = pGameInstance->Get_RandomCount() % 6;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (0 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON;
	else  if (1 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
	else if (2 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR;
	else if (3 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
	else if (4 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DOWN;
	else if (5 == iValue) 
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;

	if (m_iPrevStateData == iValue)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			iValue = pGameInstance->Get_RandomCount() % 6;
		}
		RELEASE_INSTANCE(CGameInstance);

		if (0 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON;
		else  if (1 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
		else if (2 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR;
		else if (3 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
		else if (4 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DOWN;
		else if (5 == iValue)
			iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
	}

	return iValue;
}
// Judge Skill Combo
_uint CAIController_Sabito::Judge_SkillCombo()
{
	_uint iValue = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		iValue = pGameInstance->Get_RandomCount() % 6;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (0 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON;
	else  if (1 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
	else if (2 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR;
	else if (3 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
	else if (4 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DOWN;
	else if (4 == iValue)
		iValue = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
	return iValue;
}
// Judge Step RLD
_uint CAIController_Sabito::Judge_StepRLD()
{
	_uint iValue = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		iValue = pGameInstance->Get_RandomCount() % 3;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (0 == iValue)
		iValue = CSabito::STATE_STEP + CSabito::STATE_BACK;
	else  if (1 == iValue)
		iValue = CSabito::STATE_STEP + CSabito::STATE_LEFT;
	else if (2 == iValue)
		iValue = CSabito::STATE_STEP + CSabito::STATE_RIGHT;

	return iValue;
}

void CAIController_Sabito::Ready_RanPat_Time()
{
	if (m_dCheck_ChangeTime_ToRanTimeVector <= m_dStackUpTime)
	{
		m_RanPatTimeVector.resize(MAX_RANTIME_VECTOR);
		for (_uint i = 0; i < MAX_RANTIME_VECTOR; ++i)
		{
			m_RanPatTimeVector[i].dTime = m_dStackUpTime + (i * TIME_FACTOR_TO_RANTIME_VECTOR);
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			{
				m_RanPatTimeVector[i].iValue = pGameInstnace->Get_RandomCount() % 4;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		m_iIndex = 0;
		//  Plus Check Change Time
		m_dCheck_ChangeTime_ToRanTimeVector = m_dStackUpTime +  PLUS_CHECK_CHANGETIME;
	}
}

_uint CAIController_Sabito::RanCheckTime()
{
	_uint iValue = 0;
	if (m_iIndex <= m_RanPatTimeVector.size())
	{
		if (m_RanPatTimeVector[m_iIndex].dTime >= m_dStackUpTime)
		{
			iValue = m_RanPatTimeVector[m_iIndex].iValue;
		}
		else
		{
			iValue = m_RanPatTimeVector[m_iIndex].iValue;
			++m_iIndex;
		}
	}
	else
	{
		iValue = m_RanPatTimeVector[m_iIndex - 1].iValue;
	}

	return iValue;
}

_uint CAIController_Sabito::Judge_Charge_State()
{
	_uint iCurstate = 0;
	CPlayer::BATTLEDATA tBattleData = m_pSabito->Get_BattleData();
	if (true == tBattleData.Is_CheckMaxCharge())
	{
		iCurstate = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
	}
	else
	{
		iCurstate = CSabito::STATE_CHARGE;
	}
	return iCurstate;
}

_uint CAIController_Sabito::Judge_CharStateDash(const _string& _strType)
{

	return _uint();
}

// Create
CAIController_Sabito* CAIController_Sabito::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext
, CGameObject* _pGameObject)
{
	CAIController_Sabito* pInstance = new CAIController_Sabito(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CAIController_Sabito::Create To Failed");
#endif
	}
	return pInstance;
}

void CAIController_Sabito::Free()
{
	__super::Free();
}
