#include "Akaza.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Akaza_KeyBoard_Controller.h"
#include "Akaza_GamePad_Controller.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "Battle_Manager.h"
#include "WindBall.h"

#include "Level_Story.h"
#include"CutScene_Manager.h"

CAkaza::CAkaza(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPlayer(_pDevice, _pContext)
{
}

CAkaza::CAkaza(const CAkaza& _rhs)
	: CPlayer(_rhs)
{
}

HRESULT CAkaza::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_bWrapSampler = true;

	return S_OK;
}

HRESULT CAkaza::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Animator(L"Akaza", m_pModelCom)))
		return E_FAIL;

	// 컨트롤러 세팅
	PLAYERDATA stPlayerData = *static_cast<PLAYERDATA*>(pArg);
	if (FAILED(SetUp_Controller(stPlayerData.eControllerType)))
		return E_FAIL;

	// 네비게이션 세팅
	if (nullptr != m_pTransformCom && LEVEL_STORY == g_iCurrentLevel)
	{
		if (false == m_pTransformCom->Update_Navigation_Cell(XMVectorSet(-760.f, 175.7f, 351.f, 1.f)))
			return E_FAIL;
	}
	else if (nullptr != m_pTransformCom)
	{
		if (false == m_pTransformCom->Update_Navigation_Cell(XMVectorSet(0.f, 0.f, -50.f, 0.f)))
			return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		PATHS Path = { L"CutScene",  L"Data" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
		if (nullptr != pFileGroup)
		{
			Add_CutScene(pFileGroup->FindData(L"Akaza_AtkSk"));
			Add_CutScene(pFileGroup->FindData(L"Akaza_Throw"));
			Add_CutScene(pFileGroup->FindData(L"Akaza_Battle_End"));
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	// 빛나는 색상 선택
	m_vGlowColor = _float4(0.7f, 0.9f, 0.6f, 0.8f);
	m_vEdgeGlowColor = _float4(0.4f, 0.0f, 0.f, 0.f);
	m_eCharType = CHAR_TYPE::AKAZA;

	

	return S_OK;
}

void CAkaza::Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && !CBattle_Manager::Get_Instance()->IsStoryBattle())
		return;

	// 개방시 개방 이펙트 호출
	if (m_tBattleData.bIsAwake)
	{
		m_fAwakeEffectTimer += (_float)_dTimeDelta;

		if (0.2f < m_fAwakeEffectTimer)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			m_fAwakeEffectTimer = 0.f;


			CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW("StepParticle_Aka").c_str());
			if (nullptr != pEffect)
			{
				pEffect->Get_Transform()->Bind_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
				static_cast<CEffectObject*>(pEffect)->Set_Play(true);
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	__super::Tick(_dTimeDelta);
}

void CAkaza::Late_Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && !CBattle_Manager::Get_Instance()->IsStoryBattle())
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CAkaza::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CAkaza::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
	__super::SetUp_Controller(_eType);
	if (CONTROLLER_GAMEPAD == _eType)
	{
		// 컨트롤러를 생성한다.
		m_pController = CAkaza_GamePad_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	else if (CONTROLLER_KEYBOARD == _eType)
	{	// 컨트롤러를 생성한다.
		m_pController = CAkaza_KeyBoard_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	m_eControllerType = _eType;
	return S_OK;
}

HRESULT CAkaza::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAkaza::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_PLAYER_AKAZA, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CAkaza::Fill_Datas()
{
	m_InsertVoidDatas.clear();
	m_InsertVoidDatas.reserve(1000);
	// Current State
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_AnimationType());
	// TransitionList
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_ActiveTransitionList());
	// Current AnimCLip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_CurrentAnimationClip());
	// Prev Animation Clip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_PrevAnimationClip());
}

// 상태에 따른 움직임을 제어한다.
void CAkaza::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Behavior_State(_dTimeDelta, _Datas);

	if ("SKILL_1" == m_strAnimType)
	{
		Skill_1(_dTimeDelta, _Datas);
	}
	else if ("SKILL_1_AIR" == m_strAnimType)
	{
		Skill_1_Air(_dTimeDelta, _Datas);
	}
	else if ("SKILL_2" == m_strAnimType)
	{
		Skill_2(_dTimeDelta, _Datas);
	}
	else if ("SKILL_3" == m_strAnimType)
	{
		Skill_3(_dTimeDelta, _Datas);
	}
	else if ("SKILL_3_DETECT" == m_strAnimType)
	{
		if (true == m_pAnimator->Is_Gravity())
		{
			// 프레임당 이동값을 계산한다.
			_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

			// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
			_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
			_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
			if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
			{
				m_dFallTimeAcc = 0.0;
				vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
				m_pAnimator->Input_Trigger("FALL_END");
				m_bIsAir = false;
			}
			else
				m_bIsAir = true;

			// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
			m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
		}
	}
}

void CAkaza::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	RETURN_IFNOT(_Datas.size() <= 0, ;);

	// 이번 프레임에 들어온 트리거 리스트
	vector<_string> triggerList = *static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
	// 움직이는 방향값
	m_vDirection = *static_cast<_float3*>(_Datas[OUTDATA::OUT_DIRECTION]);

	// IDLE이 아니라면 차지 타이머를 리셋한다.
	if ("IDLE" != m_strAnimType)
		m_tChargeTimer.Reset_Timer();

	if (nullptr != m_pAnimator)
	{
		m_strAnimType = m_pAnimator->Get_AnimationType();

		for (auto& trigger : triggerList)
		{
			// 기본 상태일 때
			if ("IDLE" == m_strAnimType)
			{
				// 방향값이 0이 아니라면 MOVE 상태로 진행한다.
				if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
				{
					if ("MOVE" == trigger)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							// 이동방향쪽을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
							break;
						}
					}
				}
				else if ("ATTACK" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();

						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if ("JUMP" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;

						if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
							m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
						break;
					}
				}
				// 트리거가 가드일 때
				else if ("GUARD" == trigger)
				{
					// 가드 트리거가 성공하면 Look으로 Target을 바라본다.
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 상대방을 바라본다.
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 대쉬 사용시
				else if ("DASH" == trigger)
				{
					if (true == Input_AirDash_Trigger("AIR_DASH"))
						break;
					else
					{
						if (true == Input_Dash_Trigger(trigger))
							break;
					}
				}
				else if ("STEP" == trigger)
				{
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == m_pAnimator->Input_Trigger(strStepTrigger))
					{
						// 스텝시 Look을 설정합니다.
						m_strCurrentTrigger = trigger;
						Set_StepLook(strStepTrigger);
						break;
					}
				}
				// SKILL_1 사용시
				else if ("SKILL_1" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);

							// 상대방을 바라본다.
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 개방 사용시
				else if ("AWAKE" == trigger)
				{
					if (m_tBattleData.iUltimateCount > 0)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Active_AwakeMode();
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 오의 사용시
				else if ("ULTIMATE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 상대방을 바라본다.
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if ("CHARGE_ATTACK" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();

						// 상대방을 바라본다.
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						m_vBehaviorDir = XMVector3Normalize(Get_DirToTarget());
						break;
					}
				}
				// 스킬3 시전중일때 스킬3의 시전이 가능하다.
				else if ("SKILL_3" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);

							// 상대방을 바라본다.
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}

				// 차지로 넘어간다.
				// 방향값이 0이 아니라면 MOVE 상태로 진행한다.
				if (CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
				{
					if (false == m_tBattleData.Is_CheckMaxCharge() && true == m_tBattleData.tChargeTimer.bPass)
					{
						if (m_tChargeTimer.Is_Over(_dTimeDelta))
						{
							if (true == m_pAnimator->Input_Trigger("CHARGE"))
								m_tChargeTimer.Reset_Timer();
						}
					}
				}
			}
			// 현재 재생중인 애니메이션 타입이 MOVE일 경우
			else if ("MOVE" == m_strAnimType)
			{
				// 움직이고있다면 움직여야하는 방향을 쳐다본다.
				if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
					m_pTransformCom->LookAt(Get_MoveDirection());

				// 달리기를 멈춘다.
				if ("MOVE_END" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						break;
					}
				}
				else if ("JUMP" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_vBehaviorDir = m_pTransformCom->Get_Look();//m_pTransformCom->LookAt(m_vDirection.Get_Vector());
						break;
					}
				}
				else if ("ATTACK" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if ("STEP" == trigger)
				{
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == m_pAnimator->Input_Trigger(strStepTrigger))
					{
						// 스텝시 Look을 설정합니다.
						m_strCurrentTrigger = trigger;
						Set_StepLook(strStepTrigger);
						break;
					}
				}
				else if ("GUARD" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 개방 사용시
				else if ("AWAKE" == trigger)
				{
					if (m_tBattleData.iUltimateCount > 0)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Active_AwakeMode();
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 스킬2 시전
				else if ("SKILL_2" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							// 상대방을 바라본다.
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 스킬3 시전
				else if ("SKILL_3" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 상대방을 바라본다.
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 오의 사용시
				else if ("ULTIMATE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 상대방을 바라본다.
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
			}
			// 현재 재생중인 애니메이션의 타입이 공격일 경우
			else if ("ATTACK" == m_strAnimType)
			{
				// 상단 공격시
				if (trigger == "ATTACK_UPPER")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 하단 공격시
				else if (trigger == "ATTACK_DOWN")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 일반 공격시
				else if (trigger == "ATTACK")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 개방 공격 시
				else if (trigger == "ATTACK_AWAKE")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력게이지가 차기위한 시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 점프 시
				else if (trigger == "JUMP")
				{
					// 기력게이지를 20 소모할 수 있다면 점프를 시전한다.
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);

							m_strCurrentTrigger = trigger;

							if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
								m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
							break;
						}
					}
				}
				// 대쉬인 경우
				else if (trigger == "DASH")
				{
					if (true == Input_AirDash_Trigger("AIR_DASH"))
						break;

					if (true == Check_ChargeValue())
					{
						if (true == Input_Dash_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							break;
						}
					}
				}
				else if (trigger == "STEP")
				{
					// 기력게이지를 20 소모할 수 있다면 스텝을 시전한다.
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(strStepTrigger))
						{
							m_tBattleData.Decrease_Charge(20.f);

							m_strCurrentTrigger = trigger;
							// 스텝시 Look을 설정합니다.
							Set_StepLook(strStepTrigger);
							break;
						}
					}
				}
				// 개방 사용시
				else if ("AWAKE" == trigger)
				{
					if (m_tBattleData.iUltimateCount > 0)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Active_AwakeMode();
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 스킬2 시전
				else if ("SKILL_2" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 스킬3 시전
				else if ("SKILL_3" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 오의 사용시
				else if ("ULTIMATE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
			}
			// 공중 공격시 할 수 있는 행동들
			else if ("AIR_ATTACK" == m_strAnimType)
			{
				// 일반 공격시
				if (trigger == "ATTACK")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if (trigger == "DASH")
				{
					if (true == Check_ChargeValue())
					{
						if (true == Input_AirDash_Trigger("AIR_DASH"))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							break;
						}
					}
				}
				else if (trigger == "STEP")
				{
					// 기력게이지를 20 소모할 수 있다면 스텝을 시전한다.
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(strStepTrigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							// 스텝시 Look을 설정합니다.
							Set_StepLook(strStepTrigger);
							break;
						}
					}
				}
				else if (trigger == "SKILL_2")
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger("SKILL_2_AIR"))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
			}
			// 현재 재생중인 애니메이션 타입이 스텝인 경우
			else if ("STEP" == m_strAnimType)
			{
				if (trigger == "STEP")
				{
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == m_pAnimator->Input_Trigger(strStepTrigger))
					{
						m_strCurrentTrigger = trigger;
						// 스텝시 Look을 설정합니다.
						Set_StepLook(strStepTrigger);
						break;
					}
				}
			}
			// 현재 재생중인 애니메이션 타입이 차지 공격인 경우
			else if ("CHARGE_ATTACK" == m_strAnimType)
			{
				if (trigger == "CHARGE_ATTACK_END")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						break;
					}
				}
			}
			// 현재 재생중인 애니메이션의 타입이 점프인 경우
			else if ("JUMP" == m_strAnimType)
			{
				if (trigger == "ATTACK")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 기력 회복시간을 초기화한다.
						m_tBattleData.tChargeTimer.Reset_Timer();
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if (trigger == "STEP")
				{
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == m_pAnimator->Input_Trigger(strStepTrigger))
					{
						m_strCurrentTrigger = trigger;
						// 스텝시 Look을 설정합니다.
						Set_StepLook(strStepTrigger);
						m_dFallTimeAcc = 0.0;
						break;
					}
				}
				else if (trigger == "DASH")
				{
					// 상대방이 공중 추적대쉬가 가능할 경우 공중 추적대쉬를 사용한다.
					// 점프 중일때는 일반 대쉬는 사용 불가능하다.
					if (true == Input_AirDash_Trigger("AIR_DASH"))
					{
						m_strCurrentTrigger = trigger;
						break;
					}
				}
				else if (trigger == "SKILL_1")
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger("SKILL_1_AIR"))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				else if (trigger == "SKILL_2")
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger("SKILL_2_AIR"))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
			}
			// 현재 재생중인 애니메이션이 가드인 경우
			else if ("GUARD" == m_strAnimType)
			{
				if (trigger == "GUARD_END")
				{
					m_strCurrentTrigger = trigger;
					m_pAnimator->Input_Trigger(trigger);
					break;
				}
				// 밀쳐내기
				else if (trigger == "PUSH")
				{
					// 밀치기를 수행하면 상대방을 바라본다.
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 반격
				else if (trigger == "PARING")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 스킬3 사용
				else if (trigger == "SKILL_3")
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 잡기
				else if (trigger == "THROW")
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 대쉬
				else if (trigger == "DASH")
				{
					if (true == Input_Dash_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						break;
					}
				}
			}
			// 현재 재생중인 애니메이션 타입이 DASH인 경우
			else if ("DASH" == m_strAnimType)
			{
				// 공격 사용시
				if ("ATTACK" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 스텝 사용시
				else if ("STEP" == trigger)
				{
					// 스텝의 방향별 트리거를 받아온다.
					_string strStepTrigger = Get_StepDir();

					if (true == m_pAnimator->Input_Trigger(strStepTrigger))
					{
						m_strCurrentTrigger = trigger;
						// 스텝시 Look을 설정합니다.
						Set_StepLook(strStepTrigger);
						break;
					}
				}
				// 방어 사용시
				else if ("GUARD" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				// 점프 사용시
				else if ("JUMP" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 대쉬도중 점프를 사용하면 대쉬를 하던 방향으로 점프를 한다.
						m_vBehaviorDir = m_pTransformCom->Get_Look();
						break;
					}
				}
				// 개방 사용시
				else if ("AWAKE" == trigger)
				{
					if (m_tBattleData.iUltimateCount > 0)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Active_AwakeMode();
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				// 오의 사용시
				else if ("ULTIMATE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
			}
			// 다운 된 경우
			else if ("DOWN" == m_strAnimType)
			{
				// 다운일 때 움직이면 ROLL로 전환한다.
				if ("MOVE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger("ROLL"))
					{
						m_strCurrentTrigger = trigger;
						break;
					}
				}
				// 낙법
				else if ("PASSIVE" == trigger)
				{
					// 낙법 시에는 뒤쪽으로 점프해야한다 ( 포물선 이동 해야한다 )
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
			}
			else if ("CHARGE" == m_strAnimType)
			{
				// 공격 사용시
				if ("ATTACK" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if ("MOVE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						// 이동방향쪽을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
						break;
					}
				}
				else if ("JUMP" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;

						if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
							m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
						break;
					}
				}
				else if ("GUARD" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
				else if ("AWAKE" == trigger)
				{
					if (m_tBattleData.iUltimateCount > 0)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Active_AwakeMode();
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				else if ("SKILL_1" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				else if ("SKILL_3" == trigger)
				{
					if (true == Check_ChargeValue())
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							m_tBattleData.Decrease_Charge(20.f);
							m_strCurrentTrigger = trigger;
							// 상대방을 바라본다.
							m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
							break;
						}
					}
				}
				else if ("ULTIMATE" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
					{
						m_strCurrentTrigger = trigger;
						// 상대방을 바라본다.
						m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
						break;
					}
				}
			}
		}
	}
	//__super::Animation_Control_State(_dTimeDelta, _Datas);
}

void CAkaza::Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (nullptr == m_pAnimator)
		return;

	// 본 콜라이더를 받아온다.
	_vector vLeftHandPos = m_BonesCollider[1]->Get_Collider()->Get_CurPos();
	_vector vRightHandPos = m_BonesCollider[2]->Get_Collider()->Get_CurPos();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);

	// 상대방을 포인터로 가져온다.
	CCharacter* pEnemy = pBattleManager->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
	if (nullptr == pEnemy)
		return;

	// 이벤트 체크를 받아온다.
	CAnimationClip::EVENTCHECKDESC eventCheckDesc = m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc();

	// 이벤트 이름을 받아온다.
	string strEventName = eventCheckDesc.strEventName;

	if (true == eventCheckDesc.bIsActive)
	{
		// 오른손 공격일 경우
		if ("SKILL_1_RIGHT" == strEventName)
		{
			CProjectile::PROJECTILEDESC projectileDesc;

			projectileDesc.pOwner = this;
			projectileDesc.pTarget = pEnemy;
			projectileDesc.pTargetCollider = static_cast<CCollider*>(pEnemy->Get_Component(COM_COLLIDER_HIT));
			projectileDesc.pNavigation = m_pNavigationCom;
			projectileDesc.fDamage = 6.f;
			projectileDesc.fLifeTime = 2.f;
			projectileDesc.fSpeed = 100.f;
			projectileDesc.vCreatePos = vRightHandPos;
			projectileDesc.vMoveDirection = XMVector3Normalize(pEnemy->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos());

			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());

			// 오른손에서 투사체 발사
			if (FAILED(pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_WATERBALL, &projectileDesc)))
				return;
		}
		// 왼손 공격일 경우
		else
		{
			CProjectile::PROJECTILEDESC projectileDesc;

			projectileDesc.pOwner = this;
			projectileDesc.pTarget = pEnemy;
			projectileDesc.pTargetCollider = static_cast<CCollider*>(pEnemy->Get_Component(COM_COLLIDER_HIT));
			projectileDesc.pNavigation = m_pNavigationCom;
			projectileDesc.fDamage = 6.f;
			projectileDesc.fLifeTime = 2.f;
			projectileDesc.fSpeed = 100.f;
			projectileDesc.vCreatePos = vLeftHandPos;
			projectileDesc.vMoveDirection = XMVector3Normalize(pEnemy->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos());

			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());

			// 오른손에서 투사체 발사
			if (FAILED(pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_WATERBALL, &projectileDesc)))
				return;
		}
	}

	RELEASE_INSTANCE(CBattle_Manager);
	RELEASE_INSTANCE(CGameInstance);

	__super::Skill_1(_dTimeDelta, _Datas);
}
void CAkaza::Skill_1_Air(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (nullptr == m_pAnimator)
		return;

	// 본 콜라이더를 받아온다.
	_vector vLeftHandPos = m_BonesCollider[1]->Get_Collider()->Get_CurPos();
	_vector vRightHandPos = m_BonesCollider[2]->Get_Collider()->Get_CurPos();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);

	// 상대방을 포인터로 가져온다.
	CCharacter* pEnemy = pBattleManager->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
	if (nullptr == pEnemy)
		return;

	// 이벤트 체크를 받아온다.
	CAnimationClip::EVENTCHECKDESC eventCheckDesc = m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc();

	// 이벤트 이름을 받아온다.
	string strEventName = eventCheckDesc.strEventName;

	if (true == eventCheckDesc.bIsActive)
	{
		// 오른손 공격일 경우
		if ("SKILL_1_RIGHT" == strEventName)
		{
			CProjectile::PROJECTILEDESC projectileDesc;

			projectileDesc.pOwner = this;
			projectileDesc.pTarget = pEnemy;
			projectileDesc.pTargetCollider = static_cast<CCollider*>(pEnemy->Get_Component(COM_COLLIDER_HIT));
			projectileDesc.pNavigation = m_pNavigationCom;
			projectileDesc.fDamage = 6.f;
			projectileDesc.fLifeTime = 2.f;
			projectileDesc.fSpeed = 100.f;
			projectileDesc.vCreatePos = vRightHandPos;
			projectileDesc.vMoveDirection = XMVector3Normalize(pEnemy->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos());

			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());

			// 오른손에서 투사체 발사
			if (FAILED(pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_WATERBALL, &projectileDesc)))
				return;
		}
		// 왼손 공격일 경우
		else
		{
			CProjectile::PROJECTILEDESC projectileDesc;

			projectileDesc.pOwner = this;
			projectileDesc.pTarget = pEnemy;
			projectileDesc.pTargetCollider = static_cast<CCollider*>(pEnemy->Get_Component(COM_COLLIDER_HIT));
			projectileDesc.pNavigation = m_pNavigationCom;
			projectileDesc.fDamage = 6.f;
			projectileDesc.fLifeTime = 2.f;
			projectileDesc.fSpeed = 100.f;
			projectileDesc.vCreatePos = vLeftHandPos;
			projectileDesc.vMoveDirection = XMVector3Normalize(pEnemy->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos());

			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());

			// 오른손에서 투사체 발사
			if (FAILED(pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_WATERBALL, &projectileDesc)))
				return;
		}
	}

	// 프레임당 이동값을 계산한다.
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

	RELEASE_INSTANCE(CBattle_Manager);
	RELEASE_INSTANCE(CGameInstance);
}

void CAkaza::Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (true == m_pAnimator->Is_EventPlay())
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());

	// 프레임당 이동값을 계산한다.
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

	__super::Skill_2(_dTimeDelta, _Datas);
}
void CAkaza::Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 스킬3 사용도중 특정 범위안에 상대방이 들어오면 DETECT 트리거를 애니메이터로 전달해야한다.
	if (nullptr == m_pTargetTransform)
		return;

	// 제한 거리
	_float fLimitDistance = 80.f;

	_vector vDistance = m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos();
	vDistance = XMVectorSetY(vDistance, 0.f);
	// 상대방과 나와의 거리를 구한다.
	_float fLength = XMVectorGetX(XMVector3Length(vDistance));
	// 내가 지정한 제한거리보다 가깝다면 순간이동한다.   
	_float3 vDirTargetToThis = XMVector3Normalize(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos());
	vDirTargetToThis.y = 0.f;
	vDirTargetToThis *= 10.f;

	if (fLimitDistance >= fLength)
	{
		if (true == m_pAnimator->Input_Trigger("DETECT"))
			m_pTransformCom->Move_Pos(m_pTargetTransform->Get_Pos() + XMLoadFloat3(&vDirTargetToThis));
	}

	__super::Skill_3(_dTimeDelta, _Datas);
}

void CAkaza::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Move_State(_dTimeDelta, _Datas);
}

// 공중에 떠서 공격하는 경우 아래로 내려준다.
void CAkaza::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_State(_dTimeDelta, _Datas);
}

void CAkaza::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Upper_State(_dTimeDelta, _Datas);
}

void CAkaza::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Down_State(_dTimeDelta, _Datas);
}

void CAkaza::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Awake_State(_dTimeDelta, _Datas);
}

void CAkaza::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Ultimate_State(_dTimeDelta, _Datas);
}

// 강공격
void CAkaza::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Charge_Attack_State(_dTimeDelta, _Datas);
}

// 공중에 떠서 스텝할 경우 아래로 내려준다.
void CAkaza::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Step_State(_dTimeDelta, _Datas);
}
// 지상 추격 대쉬 사용시
void CAkaza::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Dash_Ground_State(_dTimeDelta, _Datas);
}
// 공중 추격 대쉬 사용시
void CAkaza::Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Dash_Air_State(_dTimeDelta, _Datas);
}
void CAkaza::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Jump_State(_dTimeDelta, _Datas);
}
void CAkaza::Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 자신으로부터 상대방을 바라보는 방향벡터의 반대방향으로 점프한다.
	__super::Passive_State(_dTimeDelta, _Datas);
}
void CAkaza::Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Roll_State(_dTimeDelta, _Datas);
}

// 지상에서 Hit된 경우 ( 높이가 없음 )
void CAkaza::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Hit_State(_dTimeDelta, _Datas);
}
void CAkaza::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Hit_State(_dTimeDelta, _Datas);
}

void CAkaza::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Down_State(_dTimeDelta, _Datas);
}
void CAkaza::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Strike_State(_dTimeDelta, _Datas);
}

void CAkaza::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Bound_State(_dTimeDelta, _Datas);
}

HRESULT CAkaza::Initialize_Sound()
{
	__super::Initialize_Sound();

	// HIT 사운드
	{

	}

	// Dmg 사운드
	{
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg1");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg2");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg3");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg4");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg5");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg6");
		Add_Sound(L"HIT_VOICE", L"Akaza_Dmg7");

		Add_Sound(L"HIT_S_VOICE", L"Akaza_DmgS1");
		Add_Sound(L"HIT_S_VOICE", L"Akaza_DmgS2");
		Add_Sound(L"HIT_S_VOICE", L"Akaza_DmgS3");
		Add_Sound(L"HIT_S_VOICE", L"Akaza_DmgS4");
	}

	return S_OK;
}

void CAkaza::Check_Attack_Type(_string _strHitType)
{
	__super::Check_Attack_Type(_strHitType);
}

void CAkaza::Success_Attack_Upper()
{
}

void CAkaza::Success_Attack_Down()
{
}

void CAkaza::Success_SKILL_1()
{
}

void CAkaza::Success_SKILL_2()
{
}

void CAkaza::Success_SKILL_3()
{
}

void CAkaza::Success_Throw()
{
	CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
	{
		pInstance->Set_CutSceneModul(m_CutSceneModulMap["AKAZA_THROW"]);
	}
	RELEASE_INSTANCE(CCutScene_Manager);
}

void CAkaza::Success_Attack_Awake()
{
	CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
	{
		pInstance->Set_CutSceneModul(m_CutSceneModulMap["AKAZA_ATKSK"]);
	}
	RELEASE_INSTANCE(CCutScene_Manager);
}

void CAkaza::Success_Ultimate()
{
}

CAkaza* CAkaza::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CAkaza* pInstance = new CAkaza(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CAkaza::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CAkaza::Clone(void* pArg)
{
	CAkaza* pInstance = new CAkaza(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CAkaza::Clone To Failed");
#endif
	}
	return pInstance;
}

void CAkaza::Free()
{
	__super::Free();
}