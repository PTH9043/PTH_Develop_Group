#include "Zako2.h"
#include "GameInstance.h"
#include "Player.h"
#include "Level_Story.h"
#include "Battle_Manager.h"
#include "AnimationClip.h"

_bool CZako2::m_bSkillCheck = false;

CZako2::CZako2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: COni(_pDevice, _pContext)
{
}

CZako2::CZako2(const CZako2& _rhs)
	: COni(_rhs)
{
}

HRESULT CZako2::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako2::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_vOriginalPos = *reinterpret_cast<_float3*>(pArg);
		m_pTransformCom->Set_PosF3(m_vOriginalPos);
	}

	if (FAILED(SetUp_Animator(L"ZakoOni02", m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Update_Navigation_Cell(m_pTransformCom->Get_Pos());

	m_tBattleData.fMaxHp = 40;
	m_tBattleData.fCurHp = m_tBattleData.fMaxHp;

	// 빛나는 외각선 색상
	m_vEdgeGlowColor = _float4(0.2f, 0.4f, 1.f, m_fEdge);

	return S_OK;
}

void CZako2::Tick(const _double& _dTimeDelta)
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

		if (2 == m_iOniLayerNum && true == bCheck)
		{
			m_bActive = true;
			m_bTickCheck = true;
		}
	}
}

void CZako2::Late_Tick(const _double& _dTimeDelta)
{
	if (true == m_bTickCheck)
		__super::Late_Tick(_dTimeDelta);
}

HRESULT CZako2::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	//if (FAILED(m_pNavigationCom->Render_Debug()))
	//	return E_FAIL;

	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CZako2::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako2::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_ONI_ZAKO2, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (LEVEL_STORY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_ONI2, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	return S_OK;
}

void CZako2::Idle_Oni(const _double& _dTimeDelta)
{
}

void CZako2::Recognize_Oni(const _double& _dTimeDelta)
{
}

void CZako2::Battle_Oni(const _double& _dTimeDelta)
{
	if (ONISTATE_BATTLE != m_iOniState)
	{
		return;
	}

	if (false == m_bFirstCheck)
		m_bFirstCheck = true;

	if (true == m_bSkillEndCheck)
		m_bSkillEndCheck = false;

	{
		_vector vCross = XMVector3Cross(m_pTargetObject->Get_Transform()->Get_Look(), m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos());
		if (0 == m_iPatternCount)
		{
			_uint j = rand() % 4;
			if (0 == j)
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
				m_iPatternCount = ONISTATE_BATTLE_STEP_B;
			}
			else if (1 == j)
			{
				if (0 > XMVectorGetY(vCross))
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R;
				else
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L;
				m_iPatternCount = 0;
			}
			else
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_NUT;

				// 다음 패턴카운트 지정 
				_uint i = rand() % 11;
				if ((0 == i || 1 == i || 6 == i) /*&& false == m_bSkillCheck*/)
				{
					m_iPatternCount = ONISTATE_BATTLE_SKILL2;
					//m_bSkillCheck = true;
				}
				else if ((2 == i || 3 == i))
				{
					m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
				}
				else if (4 == i || 5 == i )
				{
					if (0 > XMVectorGetY(vCross))
						m_iPatternCount = ONISTATE_BATTLE_MOVELOOK_R;
					else
						m_iPatternCount = ONISTATE_BATTLE_MOVELOOK_L;
				}
				else if (7 == i || 8 == i)
				{
					if (0 > XMVectorGetY(vCross))
						m_iPatternCount = ONISTATE_BATTLE_STEP_R;
					else
						m_iPatternCount = ONISTATE_BATTLE_STEP_L;
				}
				else if (9 == i)
				{
					m_iPatternCount = ONISTATE_BATTLE_BURST;
				}
				else /*if (10 == i)*/
					m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
			}
		}
		else if (ONISTATE_BATTLE_MOVELOOK_R == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R; // 옆으로 돌기
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_MOVELOOK_L == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L; // 옆으로 돌기 
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_ATTACK2 == m_iPatternCount) // 필살기2
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2;
			m_iPatternCount = ONISTATE_BATTLE_STEP_B;
		}
		else if (ONISTATE_BATTLE_ATTACK1 == m_iPatternCount) // 기본공격
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
			m_iPatternCount = ONISTATE_BATTLE_STEP_B;
		}
		else if (ONISTATE_BATTLE_STEP_B == m_iPatternCount) // 뒤로 스텝
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B;
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_SKILL2 == m_iPatternCount) // 필살기
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2;
			m_iPatternCount = ONISTATE_BATTLE_STEP_B;
		}
		else if (ONISTATE_BATTLE_STEP_R == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_R;
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_STEP_L == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_L;
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_BURST == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_BURST;
			m_iPatternCount = 0;
		}
	}
}

void CZako2::Idle_State(const _double& _dTimeDelta)
{
	m_pTransformCom->Set_PosF3(m_vOriginalPos);
}

void CZako2::Recognize_State(const _double& _dTimeDelta)
{
	m_iPatternCount = 0;

	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	if (false == m_bFirstCheck && m_bActive)
	{
		// 점프등장 
		m_pAnimator->Input_Trigger("SURPRISE");
		m_pTransformCom->LookAt(vlookpos);
	}
	else
	{
		m_pAnimator->Input_Trigger("WALK_F");
		m_pTransformCom->LookAt(vlookpos);
	}

	if ("MOVE" == m_pAnimator->Get_AnimationType())
	{
		m_pTransformCom->Chase(vlookpos, _dTimeDelta, 25.f, 25.f);
	}
}

void CZako2::Battle_State(const _double& _dTimeDelta)
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
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1:
		{
			m_pAnimator->Input_Trigger("RUN");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2:
		{
			m_pAnimator->Input_Trigger("RUN");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B:
		{
			m_pAnimator->Input_Trigger("STEP_B");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_R:
		{
			m_pAnimator->Input_Trigger("STEP_R");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_L:
		{
			m_pAnimator->Input_Trigger("STEP_L");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2:
		{
			m_pAnimator->Input_Trigger("SKILL2");
			//m_bSkillCheck = false;
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_NUT:
		{
			m_pAnimator->Input_Trigger("IDLE");
			m_pTransformCom->LookAt(vlookpos);
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_BURST:
		{
			m_pAnimator->Input_Trigger("BURST");
		}
		break;
		default:
			break;
		}
		{
			{
				if ("IDLE" == m_pAnimator->Get_AnimationType() && 
					"SK_E0001_V01_C00.ao|(ZakoOniB_BaseNut01_1) A_E0001_V01_C00_BaseNut01_1" == m_pAnimator->Get_MainAnimationName())
				{
					_uint i = 1;
					if (_double(i) <= m_dBattleTime)
					{
						m_iOniState = ONISTATE_RECOGNIZE;
						m_dBattleTime = 0.0;
					}
				}
				else if ("STEP" == m_pAnimator->Get_AnimationType())
				{
					if ("SK_E0001_V01_C00.ao|(ZakoOniB_BaseStepB01) A_E0001_V01_C00_BaseStepB01" == m_pAnimator->Get_MainAnimationName())
					{
						m_pTransformCom->LookAt(vlookpos);
					}
					_uint i = 1;
					if (_double(i) <= m_dBattleTime)
					{
						m_iOniState = ONISTATE_RECOGNIZE;
						m_dBattleTime = 0.0;
					}
				}
				else if ("SKILL_2" == m_pAnimator->Get_AnimationType())
				{
					m_pTransformCom->LookAt(vlookpos);
				}
				else if("PUSH" == m_pAnimator->Get_AnimationType())
				{
					_uint i = rand() % 5 + 5;
					if (_double(i) <= m_dBattleTime)
					{
						m_iOniState = ONISTATE_RECOGNIZE;
						m_dBattleTime = 0.0;
					}
				}
				else
				{
					_float i = 2.5f;
					if (_double(i) <= m_dBattleTime)
					{
						m_iOniState = ONISTATE_RECOGNIZE;
						m_dBattleTime = 0.0;
					}
				}
			}

			{
				if ("DASH" == m_pAnimator->Get_AnimationType())
				{
					m_pTransformCom->Chase(vlookpos, _dTimeDelta, 50.f, 10.f);

					if (20.f >= fLenght)
					{
						if(ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2 == m_iOniState)
							m_pAnimator->Input_Trigger("ATTACK2");
						else if (ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1 == m_iOniState)
							m_pAnimator->Input_Trigger("ATTACK1");
					}
				}
			}
			{
				// 옆으로 이동할때 네비에 닿으면 / 뒤로갈때 네비에 닿으면 
				if ("PUSH" == m_pAnimator->Get_AnimationType() || "GUARD" == m_pAnimator->Get_AnimationType())
				{
					if ((false == m_bNaviCheck) &&
						"IDLE" != m_pAnimator->Get_AnimationType())
					{
						m_pAnimator->Input_Trigger("STOP");
						m_iOniState = ONISTATE_IDLE;
					}

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
							m_bMSkillCheck = true;
						}
					}
				}

				if ("AWAKE" == m_pAnimator->Get_AnimationType())
				{
					//if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
					//{
					//	if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
					//	{
							m_bMSkillCheck = false;
					//	}
					//}
				}
			}
		}
	}
}

void CZako2::Collision_Oni(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> OniList = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_ONI2);
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

void CZako2::Behavior_State(const _double& _dTimeDelta)
{
	if ("DOWN" == m_pAnimator->Get_AnimationType())
	{
		m_bMSkillCheck = false;
	}

	if (true == m_bMSkillCheck)
		m_fEdge = 1.f;
	else
		m_fEdge = 0.f;

	__super::Behavior_State(_dTimeDelta);
}

HRESULT CZako2::Initialize_Sound(void)
{
	{
		Add_Sound(L"HIT_S_VOICE", L"ZAKO2_T_HIT1");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO2_T_HIT2");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO2_T_HIT3");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO2_T_HIT4");

		Add_Sound(L"HIT_VOICE", L"ZAKO2_T_HIT1");
		Add_Sound(L"HIT_VOICE", L"ZAKO2_T_HIT2");
		Add_Sound(L"HIT_VOICE", L"ZAKO2_T_HIT3");
		Add_Sound(L"HIT_VOICE", L"ZAKO2_T_HIT4");
	}
	return S_OK;
}

CZako2* CZako2::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CZako2* pInstance = new CZako2(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako2::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CZako2::Clone(void* pArg)
{
	CZako2* pInstance = new CZako2(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako2::Clone To Failed");
#endif
	}
	return pInstance;
}

void CZako2::Free(void)
{
	__super::Free();
}
