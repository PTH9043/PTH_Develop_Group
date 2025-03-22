#include "Zako.h"
#include "GameInstance.h"
#include "Level_Story.h"
#include "Player.h"
#include "Battle_Manager.h"
#include "AnimationClip.h"

_bool CZako::m_bSkillCheck = false;

CZako::CZako(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: COni(_pDevice, _pContext)
{
}

CZako::CZako(const CZako& _rhs)
	: COni(_rhs)
{
}

HRESULT CZako::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float3 fPos = *reinterpret_cast<_float3*>(pArg);
		m_pTransformCom->Set_PosF3(fPos);
	}

	if (FAILED(SetUp_Animator(L"ZakoOni01", m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Update_Navigation_Cell(m_pTransformCom->Get_Pos());

	//_float3 vOriginalPos = m_pTransformCom->Get_PosF3();
	//m_pTransformCom->Set_PosF3(_float3{ vOriginalPos.x, vOriginalPos.y /*+ 200.f*/, vOriginalPos.z });

	m_vMyPos = m_pTransformCom->Get_PosF3();

	m_tBattleData.fMaxHp = 50;
	m_tBattleData.fCurHp = m_tBattleData.fMaxHp;

	// 빛나는 외각선 색상
	m_vEdgeGlowColor = _float4(1.f, 0.f, 0.f, m_fEdge);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iRan = pGameInstance->Get_RandomCount() % 5 + 3;
	m_Timer.fStandardTime = iRan * 0.1f + 1.f;
	RELEASE_INSTANCE(CGameInstance);

	m_iState = ONISTATE_IDLE;

	return S_OK;
}

void CZako::Tick(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (pGameInstance->Get_DIKeyDown(DIK_F2))
			m_bMSkillCheck = false;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (true == m_bTickCheck)
		__super::Tick(_dTimeDelta);
	else
	{
		CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
		m_iOniLayerNum = pBattleManager->Get_CurrentOniLayer();
		_bool bCheck = pBattleManager->IsStoryBattle();
		RELEASE_INSTANCE(CBattle_Manager);

		if (1 == m_iOniLayerNum && true == bCheck)
		{
			m_bActive = true;
			m_bTickCheck = true;
		}
	}
}

void CZako::Late_Tick(const _double& _dTimeDelta)
{
	if (true == m_bTickCheck)
		__super::Late_Tick(_dTimeDelta);
}

HRESULT CZako::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CZako::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_ONI_ZAKO, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (LEVEL_STORY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_ONI1, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	return S_OK;
}

void CZako::Idle_Oni(const _double& _dTimeDelta)
{
}

void CZako::Recognize_Oni(const _double& _dTimeDelta)
{
}

void CZako::Battle_Oni(const _double& _dTimeDelta)
{
	if (ONISTATE_BATTLE != m_iOniState)
		return;

	if (false == m_bFirstCheck)
		m_bFirstCheck = true;

	if (true == m_bSkillEndCheck)
		m_bSkillEndCheck = false;

	// m_iOniState 현재 실행할 상태 / m_iPatternCount 다음에 실행할 상태
	{
		if (0 == m_iPatternCount)
		{
			{
				// 플레이어의 오른쪽이라면 오른쪽으로, 왼쪽이라면 왼쪽으로 가게하기 
				_uint j = rand() % 4;
				if (0 == j || 1 == j)
				{
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
					m_iPatternCount = ONISTATE_BATTLE_BACK;
				}
				else
				{
					_vector vCross = XMVector3Cross(m_pTargetObject->Get_Transform()->Get_Look(), m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos());
					if (0 > XMVectorGetY(vCross))
						m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R;
					else
						m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L;

					// 다음 패턴카운트 지정 
					_uint i = rand() % 11;
					/*if (0 == i || 1 == i || 6 == i)
					m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
					else*/
					if ((2 == i || 3 == i || 9 == i /*|| 7 == i*/) && false == m_bSkillCheck)
					{
						m_iPatternCount = ONISTATE_BATTLE_SKILL1;
					}
					else if (10 == i)
						m_iPatternCount = 0;
					else
						m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
				}
			}

		}
		else if (ONISTATE_BATTLE_ATTACK1 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
		else if (ONISTATE_BATTLE_BACK == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_BACK;
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_SKILL1 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL1;
			m_iPatternCount = ONISTATE_BATTLE_NUT;
		}
		else if (ONISTATE_BATTLE_NUT == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_NUT;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
		else if (ONISTATE_BATTLE_ATTACK2 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
	}
}

void CZako::Idle_State(const _double& _dTimeDelta)
{
	//m_pTransformCom->Set_PosF3(m_vMyPos);
}

void CZako::Recognize_State(const _double& _dTimeDelta)
{
	m_iPatternCount = 0;

	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	{
		// 스킬 끝나면 스텝으로 할것
		if (true == m_bSkillEndCheck)
		{
			m_pAnimator->Input_Trigger("STEP_F");
		}
		else
		{
			if (false == m_bFirstCheck && m_bActive &&
				m_Timer.Is_Over(_dTimeDelta))
			{
				// 점프 등장
				m_pAnimator->Input_Trigger("SURPRISE");
				m_pTransformCom->LookAt(vlookpos);
			}
			else if (true == m_bFirstCheck)
			{
				m_pAnimator->Input_Trigger("WALK_F");
			}
		}

		if ("MOVE" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->Chase(vlookpos, _dTimeDelta, 20.f, 10.f);
		}
		//if ("JUMP" == m_pAnimator->Get_AnimationType())
		//{
		//	//m_pTransformCom->Translate_Dir(-m_pTransformCom->Get_Up(), _dTimeDelta, 1.f);
		//	//if (m_pTransformCom->Get_PosF3().y <= m_pNavigationCom->Get_CurCell()->Compute_Height(m_pTransformCom->Get_Pos()))
		//	//	m_bIsAir = false;
		//}
		if ("STEP" == m_pAnimator->Get_AnimationType() && "SK_E0001_V00_C00.ao|(ZakoOni_BaseStepF01) A_E0001_V00_C00_BaseStepF01" == m_pAnimator->Get_MainAnimationName())
		{
			m_pTransformCom->LookAt(vlookpos);
		}
	}
}

void CZako::Battle_State(const _double& _dTimeDelta)
{
	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	_vector vDir = m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos();
	_float fLenght = XMVectorGetX(XMVector3Length(vDir));

	{
		m_dBattleTime += _dTimeDelta;
		switch (m_iOniState)
		{
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1:
		{
			m_pAnimator->Input_Trigger("WALK_F");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2:
		{
			m_pAnimator->Input_Trigger("WALK_F");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R:
		{
			m_pAnimator->Input_Trigger("WALK_R");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L:
		{
			m_pAnimator->Input_Trigger("WALK_L");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_BACK:
		{
			m_pAnimator->Input_Trigger("WALK_B");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL1:
		{
			// 여기 추가 
			if (false == m_bSkillCheck)
				m_pAnimator->Input_Trigger("SKILL1");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL1_END:
		{
			m_pAnimator->Input_Trigger("SKILL1_ATTACK");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_NUT:
		{
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_TURN:
		{
			m_pAnimator->Input_Trigger("TURN_L");
		}
		break;
		default:
			break;
		}

		// 다음상태 세팅
		{
			_uint i = rand() % 5 + 3;
			if (_double(i) <= m_dBattleTime)
			{
				if ("SKILL_1" == m_pAnimator->Get_AnimationType())
				{
					//m_bSkillCheck = true;
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL1_END;
				}
				else
				{
					m_iOniState = ONISTATE_RECOGNIZE;
					m_dBattleTime = 0.0;
				}
			}
		}

		// 이동값 주는 함수 
		{
			if ("MOVE" == m_pAnimator->Get_AnimationType())
			{
				m_pTransformCom->Chase(vlookpos, _dTimeDelta, 25.f, 10.f);

				if (20.f >= fLenght)
				{
					if (ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1 == m_iOniState)
						m_pAnimator->Input_Trigger("ATTACK1");
					else
						m_pAnimator->Input_Trigger("ATTACK2");
				}
			}
			if ("DASH" == m_pAnimator->Get_AnimationType())
			{
				m_pTransformCom->Chase(vlookpos, _dTimeDelta, 50.f, 10.f);

				if (20.f >= fLenght)
				{
					m_pAnimator->Input_Trigger("ATTACK2");
				}
			}
		}
		{
			// 옆으로 이동할때 네비에 닿으면 / 뒤로갈때 네비에 닿으면 
			if ("PUSH" == m_pAnimator->Get_AnimationType() || "GUARD" == m_pAnimator->Get_AnimationType())
			{
				m_pTransformCom->LookAt(vlookpos);

				if ((false == m_bNaviCheck) &&
					"IDLE" != m_pAnimator->Get_AnimationType())
				{
					m_pAnimator->Input_Trigger("STOP");
					m_iOniState = ONISTATE_IDLE;
				}
			}

			if ("STEP" == m_pAnimator->Get_AnimationType() && "SK_E0001_V00_C00.ao|(ZakoOni_BaseStepF01) A_E0001_V00_C00_BaseStepF01" == m_pAnimator->Get_MainAnimationName())
			{
				m_pTransformCom->LookAt(vlookpos);
			}
		}
		{
			if ("ULTIMATE" == m_pAnimator->Get_AnimationType())
			{
				if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
				{
					if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
					{
						m_bSkillCheck = true;
						m_bMSkillCheck = true;
						//m_fEdge = 1.f;
					}
				}
			}

			if ("SKILL_2" == m_pAnimator->Get_AnimationType())
			{
				//if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
				//{
				//	if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
				//	{
						m_bSkillCheck = false;
						m_bMSkillCheck = false;
						m_bSkillEndCheck = true;
						//m_fEdge = 0.f;
				//	}
				//}
			}

			if ("TURN" == m_pAnimator->Get_AnimationType())
			{
				m_pTransformCom->LookAt(vlookpos);
			}
		}
	}
}

void CZako::Collision_Oni(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> OniList = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_ONI1);
	for (auto& Oni : OniList)
	{
		CCollider* OniCollider = static_cast<CCollider*>(Oni->Get_Component(COM_COLLIDER_SPHERE));
		if ((Oni != this) && m_pColliderCom->Is_Collision(OniCollider))
		{
			_vector vDir = m_pTransformCom->Get_Pos() - Oni->Get_Transform()->Get_Pos();
			_float fLength = XMVectorGetX(XMVector3Length(vDir));

			m_pTransformCom->Translate_Dir(XMVector3Normalize(vDir), _dTimeDelta, 5.f * (m_pColliderCom->Get_Scale().x + m_pColliderCom->Get_Scale().x - fLength));
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CZako::Behavior_State(const _double& _dTimeDelta)
{
	if ("DOWN" == m_pAnimator->Get_AnimationType())
	{
		m_bSkillCheck = false;
		m_bMSkillCheck = false;
	}

	if (true == m_bMSkillCheck)
		m_fEdge = 1.f;
	else
		m_fEdge = 0.f;

	__super::Behavior_State(_dTimeDelta);
}

HRESULT CZako::Initialize_Sound(void)
{
	{
		Add_Sound(L"HIT_S_VOICE", L"ZAKO1_T_HIT1");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO1_T_HIT2");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO1_T_HIT3");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO1_T_HIT4");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO1_T_HIT5");

		Add_Sound(L"HIT_VOICE", L"ZAKO1_T_HIT1");
		Add_Sound(L"HIT_VOICE", L"ZAKO1_T_HIT2");
		Add_Sound(L"HIT_VOICE", L"ZAKO1_T_HIT3");
		Add_Sound(L"HIT_VOICE", L"ZAKO1_T_HIT4");
		Add_Sound(L"HIT_VOICE", L"ZAKO1_T_HIT5");
	}
	return S_OK;
}

CZako* CZako::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CZako* pInstance = new CZako(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CZako::Clone(void* pArg)
{
	CZako* pInstance = new CZako(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako::Clone To Failed");
#endif
	}
	return pInstance;
}

void CZako::Free(void)
{
	__super::Free();
}
