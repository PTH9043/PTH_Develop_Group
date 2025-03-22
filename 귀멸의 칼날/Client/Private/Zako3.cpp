#include "Zako3.h"
#include "GameInstance.h"
#include "Level_Story.h"
#include "Player.h"
#include "Battle_Manager.h"
#include "AnimationClip.h"

CZako3::CZako3(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: COni(_pDevice, _pContext)
{
}

CZako3::CZako3(const CZako3& _rhs)
	: COni(_rhs)
{
}

HRESULT CZako3::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako3::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float3 fPos = *reinterpret_cast<_float3*>(pArg);
		m_pTransformCom->Set_PosF3(fPos);
	}

	if (FAILED(SetUp_Animator(L"ZakoOni03", m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Update_Navigation_Cell(m_pTransformCom->Get_Pos());

	_float3 vOriginalPos = m_pTransformCom->Get_PosF3();

	m_tBattleData.fMaxHp = 40;
	m_tBattleData.fCurHp = m_tBattleData.fMaxHp;

	m_iPatternCount = 0;

	return S_OK;
}

void CZako3::Tick(const _double& _dTimeDelta)
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

		if (3 == m_iOniLayerNum && true == bCheck)
		{
			m_bActive = true;
			m_bTickCheck = true;
		}
	}
}

void CZako3::Late_Tick(const _double& _dTimeDelta)
{
	if (true == m_bTickCheck)
		__super::Late_Tick(_dTimeDelta);
}

HRESULT CZako3::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

void CZako3::Extern_ImGuiClass(void)
{
	ImGui::Text("Oni State");
	switch (m_iOniState)
	{
	case ONISTATE_RECOGNIZE:
	{
		ImGui::Text("ONISTATE_RECOGNIZE");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_MOVELOOK_R");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_MOVELOOK_L");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_BURST:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_BURST");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_RUN:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_RUN");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_BACK:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_BACK");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_STEP_B");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_F:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_STEP_F");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK1");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK2");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK3:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK3");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_SKILL2");
	}
	break;
	case ONISTATE_BATTLE + ONISTATE_BATTLE_HIT:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_HIT");
	}
	break;
	default:
	{
		ImGui::Text(u8"_");
	}
	break;
	}

	ImGui::Text("");
	ImGui::Text("Pattern Count");
	switch (m_iPatternCount)
	{
	case 0:
	{
		ImGui::Text(u8"0");
	}
	break;
	case ONISTATE_BATTLE_ATTACK1:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK1");
	}
	break;
	case ONISTATE_BATTLE_ATTACK2:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK2");
	}
	break;
	case ONISTATE_BATTLE_ATTACK3:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_ATTACK3");
	}
	break;
	case ONISTATE_BATTLE_STEP_F:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_STEP_F");
	}
	break;
	case ONISTATE_BATTLE_STEP_B:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_STEP_B");
	}
	break;
	case ONISTATE_BATTLE_BACK:
	{
		ImGui::Text(u8"ONISTATE_BATTLE_BACK");
	}
	break;
	default:
	{
		ImGui::Text(u8"_");
	}
	break;
	}

	ImGui::Text("");
	ImGui::Text("TimeAcc");
	ImGui::Text(CMethod::Convert_Number_S(m_dBattleTime, 3).c_str());

	ImGui::Text("");
	ImGui::Text("Anim");
	ImGui::Text(m_pAnimator->Get_MainAnimationName().c_str());


	ImGui::Text("");
	ImGui::Text("HP");
	ImGui::Text(CMethod::Convert_Number_S((_int)m_tBattleData.fCurHp).c_str());


}

HRESULT CZako3::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZako3::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_ONI_ZAKO3, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (LEVEL_STORY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_ONI3, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	return S_OK;
}

void CZako3::Idle_Oni(const _double& _dTimeDelta)
{
}

void CZako3::Recognize_Oni(const _double& _dTimeDelta)
{
}

void CZako3::Battle_Oni(const _double& _dTimeDelta)
{
	// 배틀상태 (자세한 상태값이 안들어왔을때만 이함수에서 지정해줄것 / 이미 상태가 있다면 나간다)
	if (ONISTATE_BATTLE != m_iOniState)
	{
		return;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bFirstCheck)
		m_bFirstCheck = true;

	if (true == m_bSkillEndCheck)
		m_bSkillEndCheck = false;

	{
		// ATTACK1 / RUN -> / STEP_B / WALK_B / 
		// ATTACK2 / RUN -> / WALK_B /
		// ATTACK3 / STEP_F -> / WALK_B /
		// SKILL2 / 바로 / STEP_B / WALK_B / 
		// MOVELOOK / 바로 WALK_옆 / 
		// BELLOW / 바로 / 
		// IDLE / 

		if (0 == m_iPatternCount)
		{
			_uint iRan = pGameInstance->Get_RandomCount() % 6;
			if (0 == iRan || 1 == iRan)
			{
				_vector vCross = XMVector3Cross(m_pTargetObject->Get_Transform()->Get_Look(), m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos());
				if (0 > XMVectorGetY(vCross))
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R;
				else
					m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L;

				m_iPatternCount = 0;
			}
			else if (2 == iRan)
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2;
				m_iPatternCount = ONISTATE_BATTLE_STEP_B;
			}
			else if (3 == iRan)
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
				m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
			}
			else if (4 == iRan)
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
				m_iPatternCount = ONISTATE_BATTLE_ATTACK1;
			}
			else
			{
				m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_F;
				m_iPatternCount = ONISTATE_BATTLE_ATTACK3;
			}

			{
				//switch (iRan)
			//{
			//case 0:
			//{
			//	m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
			//	m_iPatternCount = ONISTATE_BATTLE_ATTACK1;
			//}
			//break;
			//case 1:
			//{
			//	m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
			//	m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
			//}
			//break;
			//case 2:
			//{
			//	m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_F;
			//	m_iPatternCount = ONISTATE_BATTLE_ATTACK3;
			//}
			//break;
			//case 3:
			//{
			//	// !!!1 플레이어와 좀 거리가 있을때만 예외처리 해주기 
			//	m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2;
			//	m_iPatternCount = ONISTATE_BATTLE_STEP_B;
			//}
			//break;
			//case 4:
			//{
			//	_vector vCross = XMVector3Cross(m_pTargetObject->Get_Transform()->Get_Look(), m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos());
			//	if (0 > XMVectorGetY(vCross))
			//		m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_R;
			//	else
			//		m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_MOVELOOK_L;

			//	m_iPatternCount = 0;
			//}
			//break;
			//default:
			//	break;
			//}
			}

		}
		else if (ONISTATE_BATTLE_ATTACK1 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
			m_iPatternCount = ONISTATE_BATTLE_STEP_B;
		}
		else if (ONISTATE_BATTLE_ATTACK2 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
		else if (ONISTATE_BATTLE_ATTACK3 == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK3;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
		else if (ONISTATE_BATTLE_STEP_B == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B;
			m_iPatternCount = ONISTATE_BATTLE_BACK;
		}
		else if (ONISTATE_BATTLE_BACK == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_BACK;
			m_iPatternCount = 0;
		}
		else if (ONISTATE_BATTLE_RUN == m_iPatternCount)
		{
			m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
			m_iPatternCount = ONISTATE_BATTLE_ATTACK1;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CZako3::Idle_State(const _double& _dTimeDelta)
{
}

void CZako3::Recognize_State(const _double& _dTimeDelta)
{
	//m_iPatternCount = 0;

	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	{
		if (false == m_bFirstCheck && m_bActive)
		{
			//등장
			m_pAnimator->Input_Trigger("SURPRISE");
		}
		else if (true == m_bFirstCheck)
		{
			m_pAnimator->Input_Trigger("WALK_F");
		}
	}
}

void CZako3::Battle_State(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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
		case ONISTATE_BATTLE + ONISTATE_BATTLE_RUN:
		{
			m_pAnimator->Input_Trigger("RUN");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_F:
		{
			m_pAnimator->Input_Trigger("STEP_F");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_SKILL2:
		{
			m_pAnimator->Input_Trigger("SKILL2");
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
		case ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B:
		{
			m_pAnimator->Input_Trigger("STEP_B");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_BACK:
		{
			m_pAnimator->Input_Trigger("WALK_B");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1:
		{
			m_pAnimator->Input_Trigger("ATTACK1");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK2:
		{
			m_pAnimator->Input_Trigger("ATTACK2");
		}
		break;
		case ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK3:
		{
			m_pAnimator->Input_Trigger("ATTACK3");
		}
		break;
		default:
			break;
		}

		// 뒤로가는 back이면
		if ("GUARD" == m_pAnimator->Get_AnimationType())
		{
			// 콜라이더 벗어날때까지 
			CCollider* pPlayerCollider = static_cast<CCollider*>(m_pTargetObject->Get_Component(COM_COLLIDER_HIT));
			if (false == m_pAttackColliderCom->Is_Collision(pPlayerCollider))
			{
				m_iOniState = ONISTATE_RECOGNIZE;
				m_dBattleTime = 0.0;
			}
		}
		else if ("PUSH" == m_pAnimator->Get_AnimationType())
		{
			_uint iRan = pGameInstance->Get_RandomCount() % 5 + 5;
			if (_double(iRan) <= m_dBattleTime)
			{
				m_iOniState = ONISTATE_RECOGNIZE;
				m_dBattleTime = 0.0;
			}
		}
		else
		{
			if ("IDLE" == m_pAnimator->Get_AnimationType() &&
				"SK_E0001_V00_C10.ao|(ZakoOni_BaseNut01_1) A_E0001_V00_C00_BaseNut01_1" == m_pAnimator->Get_MainAnimationName())
			{
				if (ONISTATE_BATTLE + ONISTATE_BATTLE_HIT == m_iOniState)
				{
					//m_iOniState = ONISTATE_BATTLE;
					_float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos()));
					if (20.f >= fDistance)
					{
						_uint iRan = pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan)
						{
							//m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
							m_iPatternCount = ONISTATE_BATTLE_ATTACK1;
							m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK1;
						}
						else if (1 == iRan)
						{
							//m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK3;
							m_iPatternCount = ONISTATE_BATTLE_ATTACK3;
							m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_ATTACK3;
						}
						else if (2 == iRan)
						{
							//m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B;
							m_iPatternCount = ONISTATE_BATTLE_STEP_B;
							m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_STEP_B;
						}
					}
					else
					{
						m_iPatternCount = ONISTATE_BATTLE_RUN;
						m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_RUN;
					}
				}
				else
				{
					m_iOniState = ONISTATE_RECOGNIZE;
				}

				m_dBattleTime = 0.0;
			}
		}

		// 공격전, 이동트리거였다면 
		{
			if ("DASH" == m_pAnimator->Get_AnimationType())
			{
				m_pTransformCom->Chase(vlookpos, _dTimeDelta, 50.f, 10.f);
				if (20.f >= fLenght)
				{
					if (ONISTATE_BATTLE_ATTACK1 == m_iPatternCount)
					{
						m_iPatternCount = ONISTATE_BATTLE_ATTACK1;
						m_iOniState = 3;
					}
					else if (ONISTATE_BATTLE_ATTACK2 == m_iPatternCount)
					{
						m_iPatternCount = ONISTATE_BATTLE_ATTACK2;
						m_iOniState = 3;
					}
				}
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CZako3::Behavior_State(const _double& _dTimeDelta)
{
	if ("DOWN" == m_pAnimator->Get_AnimationType())
	{
		m_bMSkillCheck = false;
	}

	if (true == m_bMSkillCheck)
		m_fEdge = 1.f;
	else
		m_fEdge = 0.f;

	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	{
		if ("SKILL_2" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->LookAt(vlookpos);
		}
		if ("PUSH" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->LookAt(vlookpos);
		}
		if ("JUMP" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->LookAt(vlookpos);
		}
		if ("MOVE" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->Chase(vlookpos, _dTimeDelta, 20.f, 10.f);
		}
		if ("STEP" == m_pAnimator->Get_AnimationType())
		{
			m_pTransformCom->LookAt(vlookpos);
		}
		//if ("DASH" == m_pAnimator->Get_AnimationType())
		//{
		//	m_pTransformCom->Chase(vlookpos, _dTimeDelta, 50.f, 10.f);
		//}
		// 옆으로 이동할때 네비에 닿으면 / 뒤로갈때 네비에 닿으면 
		if ("PUSH" == m_pAnimator->Get_AnimationType() || "GUARD" == m_pAnimator->Get_AnimationType())
		{
			if ((false == m_bNaviCheck) &&
				"IDLE" != m_pAnimator->Get_AnimationType())
			{
				m_pAnimator->Input_Trigger("STOP");
				m_iOniState = ONISTATE_IDLE;
			}
		}

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

	__super::Behavior_State(_dTimeDelta);
}

void CZako3::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
	__super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

	m_iOniState = ONISTATE_BATTLE + ONISTATE_BATTLE_HIT;
}

void CZako3::Collision_Oni(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> OniList = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_ONI3);
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

HRESULT CZako3::Initialize_Sound(void)
{
	{
		Add_Sound(L"HIT_S_VOICE", L"ZAKO3_T_HIT1");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO3_T_HIT2");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO3_T_HIT3");
		Add_Sound(L"HIT_S_VOICE", L"ZAKO3_T_HIT4");

		Add_Sound(L"HIT_VOICE", L"ZAKO3_T_HIT1");
		Add_Sound(L"HIT_VOICE", L"ZAKO3_T_HIT2");
		Add_Sound(L"HIT_VOICE", L"ZAKO3_T_HIT3");
		Add_Sound(L"HIT_VOICE", L"ZAKO3_T_HIT4");
	}
	return S_OK;
}

CZako3* CZako3::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CZako3* pInstance = new CZako3(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako3::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CZako3::Clone(void* pArg)
{
	CZako3* pInstance = new CZako3(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZako3::Clone To Failed");
#endif
	}
	return pInstance;
}

void CZako3::Free(void)
{
	__super::Free();
}
