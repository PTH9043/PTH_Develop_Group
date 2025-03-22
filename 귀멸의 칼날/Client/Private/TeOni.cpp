#include "TeOni.h"
#include "GameInstance.h"
#include "AnimationClip.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"


#include "Battle_Manager.h"

CTeOni::CTeOni(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBoss(_pDevice, _pContext)
{
}

CTeOni::CTeOni(const CTeOni& _rhs)
	: CBoss(_rhs)
{
}

HRESULT CTeOni::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	m_bWrapSampler = true;

	return S_OK;
}

HRESULT CTeOni::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Animator(L"TeOni", m_pModelCom)))
		return E_FAIL;

	if (LEVEL_STORY == g_iCurrentLevel)
		m_pTransformCom->Set_Pos(XMVectorSet(-799.f, 175.f, 304.f, 1.f));
	else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
	{
		m_pTransformCom->Set_Pos(XMVectorSet(0.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Rotate_Fix(m_pTransformCom->Get_Up(), 180.f);
	}

	m_pTransformCom->Update_Navigation_Cell(m_pTransformCom->Get_Pos());

	m_iState = STATE_IDLE;

	// 빛나는 외각선 색상
	m_vEdgeGlowColor = _float4(1.f, 0.f, 0.f, m_fEdge);

	m_tBattleData.fMaxHp = 100;
	m_tBattleData.fCurHp = m_tBattleData.fMaxHp;

	return S_OK;
}

void CTeOni::Tick(const _double& _dTimeDelta)
{
	if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bBattleStart)
	{
		m_bInit = true;
		m_iState = STATE_IDLE + STATE_IDLE_BELLOW;
		bCheck = m_pAnimator->Input_Trigger("BELLOW");
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_BGM(L"BGM_TEONI", 0.5f);
		RELEASE_INSTANCE(CGameInstance);
	}
	if (false == bCheck)
	{
		m_iState = STATE_IDLE + STATE_IDLE_BELLOW;
		bCheck = m_pAnimator->Input_Trigger("BELLOW");
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_BGM(L"BGM_TEONI", 0.5f);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (false == m_bInit)
		return;

	__super::Tick(_dTimeDelta);

	//=-===== 오니 모델 테스트코드 =============
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_1))
	{
		m_iState = STATE_ATTACK + STATE_ATK_L_STONE;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_2))
	{
		m_iState = STATE_ATTACK + STATE_ATK_S_SWING;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_3))
	{
		m_iState = STATE_WALK + STATE_FF;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_4))
	{
		m_iState = STATE_ATTACK + STATE_ATK_S_JUMP;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_5))
	{
		pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
	}

	if (pGameInstance->Get_DIKeyDown(DIK_F3))
		m_bDissolve = !m_bDissolve;
	
	if (m_bDissolve)
		m_fDissolveAmount += (_float)_dTimeDelta * 0.2f;
	else
		m_fDissolveAmount = 0.f;

	RELEASE_INSTANCE(CGameInstance);
	//===========================================
}

void CTeOni::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bInit)
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CTeOni::Render(void)
{
	if (m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	_bool bDissolve = false;
	if (m_pShaderCom->Set_RawValue("g_bDissolve", &bDissolve, sizeof(_bool)))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

void CTeOni::Extern_ImGuiClass(void)
{
	ImGui::Text("Cur");
	switch (m_iState)
	{
	case STATE_IDLE:
	{
		ImGui::Text("STATE_IDLE");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_S_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_ONEHAND:
	{
		ImGui::Text(u8"STATE_ATK_S_ONEHAND");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_SWING:
	{
		ImGui::Text(u8"STATE_ATK_S_SWING");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_L_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_RUSH:
	{
		ImGui::Text(u8"STATE_ATK_L_RUSH");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_STONE:
	{
		ImGui::Text(u8"STATE_ATK_L_STONE");
	}
	break;
	case STATE_IDLE + STATE_IDLE_BELLOW:
	{
		ImGui::Text(u8"STATE_IDLE_BELLOW");
	}
	break;
	case STATE_IDLE + STATE_IDLE_DAMAGE:
	{
		ImGui::Text(u8"STATE_IDLE_DAMAGE");
	}
	break;
	case STATE_STEP + STATE_FF:
	{
		ImGui::Text(u8"STATE_FF");
	}
	break;
	case STATE_STEP + STATE_RIGHT:
	{
		ImGui::Text(u8"STATE_RIGHT");
	}
	break;
	case STATE_STEP + STATE_LEFT:
	{
		ImGui::Text(u8"STATE_LEFT");
	}
	break;
	default:
	{
		ImGui::Text(u8"_");
	}
	break;
	}

	ImGui::Text("");
	ImGui::Text("Pre");
	switch (m_iPreState)
	{
	case STATE_IDLE:
	{
		ImGui::Text(u8"STATE_IDLE");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_S_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_ONEHAND:
	{
		ImGui::Text(u8"STATE_ATK_S_ONEHAND");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_SWING:
	{
		ImGui::Text(u8"STATE_ATK_S_SWING");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_L_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_RUSH:
	{
		ImGui::Text(u8"STATE_ATK_L_RUSH");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_STONE:
	{
		ImGui::Text(u8"STATE_ATK_L_STONE");
	}
	break;
	case STATE_IDLE + STATE_IDLE_BELLOW:
	{
		ImGui::Text(u8"STATE_IDLE_BELLOW");
	}
	break;
	case STATE_IDLE + STATE_IDLE_DAMAGE:
	{
		ImGui::Text(u8"STATE_IDLE_DAMAGE");
	}
	break;
	case STATE_STEP + STATE_FF:
	{
		ImGui::Text(u8"STATE_FF");
	}
	break;
	case STATE_STEP + STATE_RIGHT:
	{
		ImGui::Text(u8"STATE_RIGHT");
	}
	break;
	case STATE_STEP + STATE_LEFT:
	{
		ImGui::Text(u8"STATE_LEFT");
	}
	break;
	default:
	{
		ImGui::Text(u8"_");
	}
	break;
	}

	ImGui::Text("");
	ImGui::Text("PreAttack");
	switch (m_iPreAtkState)
	{
	case STATE_IDLE:
	{
		ImGui::Text(u8"STATE_IDLE");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_S_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_ONEHAND:
	{
		ImGui::Text(u8"STATE_ATK_S_ONEHAND");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_SWING:
	{
		ImGui::Text(u8"STATE_ATK_S_SWING");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_JUMP:
	{
		ImGui::Text(u8"STATE_ATK_L_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_RUSH:
	{
		ImGui::Text(u8"STATE_ATK_L_RUSH");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_STONE:
	{
		ImGui::Text(u8"STATE_ATK_L_STONE");
	}
	break;
	case STATE_IDLE + STATE_IDLE_BELLOW:
	{
		ImGui::Text(u8"STATE_IDLE_BELLOW");
	}
	break;
	case STATE_IDLE + STATE_IDLE_DAMAGE:
	{
		ImGui::Text(u8"STATE_IDLE_DAMAGE");
	}
	break;
	case STATE_STEP + STATE_FF:
	{
		ImGui::Text(u8"STATE_FF");
	}
	break;
	case STATE_STEP + STATE_RIGHT:
	{
		ImGui::Text(u8"STATE_RIGHT");
	}
	break;
	case STATE_STEP + STATE_LEFT:
	{
		ImGui::Text(u8"STATE_LEFT");
	}
	break;
	default:
	{
		ImGui::Text(u8"_");
	}
	break;
	}

	ImGui::Text("");
	ImGui::Text("MonsterPhase");
	if (false == m_bPhase2)
		ImGui::Text("Phase1");
	else
		ImGui::Text("Phase2");

	ImGui::Text("");
	ImGui::Text("MonsterHP");
	ImGui::Text(CMethod::Convert_Number_S((_int)m_tBattleData.fCurHp).c_str());

	ImGui::Text("");
	ImGui::Text("HitCount");
	ImGui::Text(CMethod::Convert_Number_S(m_iHitCount).c_str());

	ImGui::Text("");
	ImGui::Text("TurnCount");
	ImGui::Text(CMethod::Convert_Number_S(m_iTurnCount).c_str());

	_float fDot = OtherCharacterDirToLookConverter(m_pTargetObject);
	ImGui::Text("");
	ImGui::Text("Dot");
	ImGui::Text(CMethod::Convert_Number_S((_uint)fDot).c_str());

}

HRESULT CTeOni::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	if (m_bDissolve)
	{
		if (m_pDissolveTexture->SetUp_ConstantTable_Index(m_pShaderCom, "g_DissolveTexture", 17))
			return E_FAIL;

		_float4 edgeColor = _float4(0.5f, 0.f, 0.f, 0.f);
		_float noiseTiling = 2.f;
		_float edgeThickness = 0.05f;
		if (m_pShaderCom->Set_RawValue("dissolveAmount", &m_fDissolveAmount, sizeof(_float)))
			return E_FAIL;
		if (m_pShaderCom->Set_RawValue("noiseTiling", &noiseTiling, sizeof(_float)))
			return E_FAIL;
		if (m_pShaderCom->Set_RawValue("edgeThickness", &edgeThickness, sizeof(_float)))
			return E_FAIL;
		if (m_pShaderCom->Set_RawValue("edgeColor", &edgeColor, sizeof(_float4)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTeOni::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_BOSS_TEONI, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (LEVEL_STORY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}
	else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	if (FAILED(Add_Component(L"Com_NoiseTexture", LEVEL_STATIC, L"Prototype_Component_Texture_EffectMoise", (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

void CTeOni::Move_HitCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	//if (true == m_bIsHit)
	//{
	//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//	pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
	//	RELEASE_INSTANCE(CGameInstance);
	//}

	if ((STATE_IDLE != m_iState) || (STATE_IDLE + STATE_IDLE_DAMAGE == m_iPreIdleState))
		return;

	if ((30.f < m_tBattleData.fCurHp && 40.f > m_tBattleData.fCurHp) ||
		(70.f < m_tBattleData.fCurHp && 80.f > m_tBattleData.fCurHp))
	{
		m_iState = STATE_IDLE + STATE_IDLE_DAMAGE;
		//m_iHitCount = 0;
	}
}

void CTeOni::Move_StepCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (STATE_IDLE != m_iState)
		return;

	if (2 < m_iTurnCount)
	{
		m_iState = STATE_ATTACK;
		m_iTurnCount = 0;
		return;
	}

	{
		_float fDot = OtherCharacterDirToLookConverter(m_pTargetObject);

		if (fDot >= 50.f)
			m_iState = STATE_STEP + STATE_LEFT;
		else if (fDot <= -50.f)
			m_iState = STATE_STEP + STATE_RIGHT;
	}
}

void CTeOni::Give_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (STATE_IDLE == m_iState)
	{
		_float fDistance = OtherCharacterToDistance(m_pTargetObject);
		if (90.f <= fDistance) // 멀리 떨어져있을때 
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
				m_iState = STATE_WALK;
			else 
				m_iState = STATE_ATTACK;
		}
		else
		{
			m_iState = STATE_ATTACK;
		}
	}

	//if (STATE_WALK == m_iState)
	//	m_iState = STATE_WALK + STATE_FF;

	if (STATE_WALK == m_iState)
	{
		if (3 < m_iWalkCount)
		{
			m_iState = STATE_ATTACK;
			m_iWalkCount = 0;
		}
		else
		{
			m_iState = STATE_WALK + STATE_FF;
		}
	}

	// attack
	if (STATE_ATTACK == m_iState)
	{
		// 플레이어와 거리 계산 
		_float fDistance = OtherCharacterToDistance(m_pTargetObject);
		if (false == m_bPhase2)
		{
			if (60.f > fDistance)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if (STATE_ATTACK + STATE_ATK_S_ONEHAND != m_iPreAtkState && 0 == iRan)
					m_iState = STATE_ATTACK + STATE_ATK_S_ONEHAND;
				else if (STATE_ATTACK + STATE_ATK_S_JUMP != m_iPreAtkState && 1 == iRan)
					m_iState = STATE_ATTACK + STATE_ATK_S_JUMP;
				else
					m_iState = STATE_ATTACK + STATE_ATK_S_SWING;
			}
			else // 원거리 
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
/*				if (STATE_ATTACK + STATE_ATK_L_STONE != m_iPreAtkState && 0 == iRan)
					m_iState = STATE_ATTACK + STATE_ATK_L_STONE;
				else*/ if (STATE_ATTACK + STATE_ATK_L_JUMP != m_iPreAtkState)
					m_iState = STATE_ATTACK + STATE_ATK_L_JUMP;
				else
					m_iState = STATE_ATTACK + STATE_ATK_S_SWING;
			}
		}
		else // phase2
		{
			if (60.f > fDistance)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if (STATE_ATTACK + STATE_ATK_S_ONEHAND != m_iPreAtkState && 0 == iRan)
					m_iState = STATE_ATTACK + STATE_ATK_S_ONEHAND;
				else if (STATE_ATTACK + STATE_ATK_S_JUMP != m_iPreAtkState)
					m_iState = STATE_ATTACK + STATE_ATK_S_JUMP;
				else
					m_iState = STATE_ATTACK + STATE_ATK_S_SWING;
			}
			else // 원거리 
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if (STATE_ATTACK + STATE_ATK_L_RUSH != m_iPreAtkState && 0 == iRan
					&& (70.f < fDistance))
					m_iState = STATE_ATTACK + STATE_ATK_L_RUSH;
				//else if (STATE_ATTACK + STATE_ATK_L_STONE != m_iPreAtkState && 0 == iRan)
				//	m_iState = STATE_ATTACK + STATE_ATK_L_STONE;
				else if (STATE_ATTACK + STATE_ATK_L_JUMP != m_iPreAtkState)
					m_iState = STATE_ATTACK + STATE_ATK_L_JUMP;
				else
					m_iState = STATE_ATTACK + STATE_ATK_S_SWING;
			}
		}
	}
}

void CTeOni::Animation_Control_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	switch (m_iState % STATE_JUDGE)
	{
	case STATE_IDLE:
	{
		Idel_State(_dTimeDelta, _pGameInstance, m_iState);
	}
	break;
	case STATE_WALK:
	{
		Walk_State(_dTimeDelta, _pGameInstance, m_iState);
	}
	break;
	case STATE_STEP:
	{
		Step_State(_dTimeDelta, _pGameInstance, m_iState);
	}
	break;
	case STATE_ATTACK:
	{
		Attack_State(_dTimeDelta, _pGameInstance, m_iState);
	}
	break;
	case STATE_GUARD:
	{
	}
	break;
	case STATE_HIT:
	{
	}
	break;
	default:
		break;
	}
}

void CTeOni::Behavior_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance)
{
	m_vEdgeGlowColor = _float4(1.f, 0.f, 0.f, m_fEdge);

	_float3 lookpos = m_pTargetObject->Get_Transform()->Get_PosF3();
	_float3 mypos = m_pTransformCom->Get_PosF3();
	_float3 newpos = _float3{ lookpos.x, mypos.y, lookpos.z };
	_vector vlookpos = XMLoadFloat3(&newpos);

	if ("TURN" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
	{
		_vector vec = XMVectorLerp(XMVector3Normalize(m_pTransformCom->Get_Look()), XMVector3Normalize(vlookpos - m_pTransformCom->Get_Pos()), 0.3f);
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vec);
	}

	if ("MOVE" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
	{
		_vector vec = XMVectorLerp(XMVector3Normalize(m_pTransformCom->Get_Look()), XMVector3Normalize(vlookpos - m_pTransformCom->Get_Pos()), 0.3f);
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vec);
	}

	// 점프공격 
	if ("JUMP" == m_pAnimator->Get_AnimationType())
	{
		Jump(_dTimeDetla, "STOP");
	}

	// 돌던지기 공격 
	if ("SKILL_1" == m_pAnimator->Get_AnimationType() ||
		"SKILL_2" == m_pAnimator->Get_AnimationType())
	{
		_vector vec = XMVectorLerp(XMVector3Normalize(m_pTransformCom->Get_Look()), XMVector3Normalize(vlookpos - m_pTransformCom->Get_Pos()), 0.3f);
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vec);

		if ("SKILL_1" == m_pAnimator->Get_AnimationType())
			m_iRLCheck = 0;
		else
			m_iRLCheck = 1;

		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				//_pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_ITEM_TEONIROCK, &m_iRLCheck);
				if (0 == m_iRLCheck)
				{
					if (false == m_bTimeCheck[0])
					{
						CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
						static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.5f, 0.3f);
						RELEASE_INSTANCE(CCamera_Manager);

						m_bTimeCheck[0] = true;
					}
					else
					{
						if (false == m_bCreateCheck[0])
						{
							_pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_ITEM_TEONIROCK, &m_iRLCheck);
							m_bCreateCheck[0] = true;
						}
					}
				}
				else
				{
					if (false == m_bTimeCheck[1])
					{
						CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
						static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.5f, 0.3f);
						RELEASE_INSTANCE(CCamera_Manager);

						m_bTimeCheck[1] = true;
					}
					else
					{
						if (false == m_bCreateCheck[1])
						{
							_pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_PROJECTILE, PROTO_GOB_ITEM_TEONIROCK, &m_iRLCheck);
							m_bCreateCheck[1] = true;
						}
					}
				}
			}
		}
	}

	// 스텝 ff
	if ("STEP" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(1.f, 0.3f);
				RELEASE_INSTANCE(CCamera_Manager);
			}
		}
	}
	// 점프 s공격
	if ("PUSH" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.5f, 1.3f);
				RELEASE_INSTANCE(CCamera_Manager);
			}
		}
	}

	// 러쉬  시작 
	if ("CHARGE" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.05f, 0.7f);
				RELEASE_INSTANCE(CCamera_Manager);
			}
		}

		m_fEdge = 1.f;
	}
	// 러쉬
	if ("CHARGE_ATTACK" == m_pAnimator->Get_AnimationType())
	{
		if ((false == m_bNaviCheck) &&
			"IDLE" != m_pAnimator->Get_AnimationType())
			m_pAnimator->Input_Trigger("STOP");

		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.1f, 1.f);
				RELEASE_INSTANCE(CCamera_Manager);

				if (false == m_bRLCheck)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					pGameInstance->Play_SoundW(TEXT("TEONI_RUSH_1"), 0.5f);
					RELEASE_INSTANCE(CGameInstance);

					m_bRLCheck = true;
				}
				else
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					pGameInstance->Play_SoundW(TEXT("TEONI_RUSH_2"), 0.5f);
					RELEASE_INSTANCE(CGameInstance);

					m_bRLCheck = false;
				}
			}
		}

		m_fEdge = 1.f;
	}

	// 손흔들기 공격 / 포효 / 점프로 내려올때 시간 다르게 주기 
	if ("SKILL_3" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.55f, 2.5f);
				RELEASE_INSTANCE(CCamera_Manager);
			}
		}
	}
	// 포효 
	if ("AWAKE" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(2.4f, 3.f);
				RELEASE_INSTANCE(CCamera_Manager);
			}
		}

		if (true == m_bPhase2)
		{
			m_fEdge = 1.f;
			m_tBattleData.Increase_HP(1.5f);
		}
	}
	// 점프L공격 내려갈때
	if ("BOUND" == m_pAnimator->Get_AnimationType())
	{
		if (nullptr != m_pAnimator->Get_CurrentAnimationClip())
		{
			if (true == m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.4f, 1.5f);
				RELEASE_INSTANCE(CCamera_Manager);

				_float3 vPos = m_pTransformCom->Get_PosF3();
				_float3 vRight = _float3{ 1.f, 0.f, 0.f };
				_float3 vUp = _float3{ 0.f, 1.f, 0.f };
				_float3 vLook = _float3{ 0.f, 0.f, 1.f };
				_float4x4 mat = _float4x4{ 1.f, 0.f, 0.f, 0.f,
											0.f, 1.f, 0.f, 0.f,
											0.f, 0.f, 1.f, 0.f,
											vPos.x, 0.f, vPos.z, 1.f };

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->SetEffectStateCall(L"TEONI_JUMP_G", XMLoadFloat4x4(&mat));
				RELEASE_INSTANCE(CGameInstance);
			}
		}
	}
	// 가드상태
	if ("GUARD" == m_pAnimator->Get_AnimationType())
	{
		m_fEdge = 1.f;
		m_bGuard = true;
	}
	if ("DIE" == m_pAnimator->Get_AnimationType())
	{
		m_dDieTime += _dTimeDetla;
		if (2.f <= m_dDieTime)
		{
			m_bDissolve = true;
		}
	}
}

void CTeOni::Init_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance)
{
	// 상태 초기화 
	_string type = m_pAnimator->Get_AnimationType();
	_string str = m_pAnimator->Get_MainAnimationName();

	if ("IDLE" == m_pAnimator->Get_AnimationType() &&
		"SK_P1002_V00_C00.ao|(BaseNut01_1) A_P1002_V00_C00_BaseNut01_1" == m_pAnimator->Get_MainAnimationName())
	{
		_string str = m_pAnimator->Get_MainAnimationName();
		if (STATE_IDLE != m_iState)
		{
			m_iPreState = m_iState;
			if (STATE_ATTACK == m_iState % STATE_JUDGE)
				m_iPreAtkState = m_iState;
			if (STATE_IDLE == m_iState % STATE_JUDGE)
				m_iPreIdleState = m_iState;
			if (STATE_STEP == m_iState % STATE_JUDGE)
			{
				m_iTurnCount += 1;
				m_iPreIdleState = m_iState;
			}
			if (STATE_WALK == m_iState % STATE_JUDGE)
			{
				m_iWalkCount += 1;
			}
			m_iState = STATE_IDLE;
			m_bJumpAtkCheck = false;

			//if (false == m_bPhase2)
		}

		// 생성변수 초기화 
		m_bCreateCheck[0] = false;
		m_bTimeCheck[0] = false;
		m_bCreateCheck[1] = false;
		m_bTimeCheck[1] = false;

		m_bGuard = false;
		m_fEdge = 0.f;
	}

	// 페이즈 
	if (false == m_bPhase2 &&
		0 >= m_tBattleData.fCurHp)
	{
		// phase 변신 이벤트 후, 
		//m_fEdge = 1.f;
		m_bPhase2 = true;
		m_iState = STATE_IDLE + STATE_IDLE_BELLOW;
		//m_iHitCount = 0;
		m_tBattleData.fCurHp = 1.f;
	}
	else if (true == m_bPhase2 &&
		0 >= m_tBattleData.fCurHp &&
		(m_iState != STATE_IDLE + STATE_IDLE_BELLOW))
	{
		m_iState = STATE_IDLE + STATE_IDLE_DEAD;
		//m_bDissolve = true;
	}

	if (1.f <= m_fDissolveAmount && false == m_bFadeOutCheck)
	{
		CRenderer* pRenderer = (CRenderer*)Get_Component(COM_RENDERER);
		if (nullptr != pRenderer)
		{
			pRenderer->Start_FadeInOut(0.05f, false, 1);
		}
		m_bFadeOutCheck = true;
	}
}

void CTeOni::Idel_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance, const _uint& _iOrder)
{
	_bool bIsSuccess = false;
	switch (m_iState)
	{
	case STATE_IDLE + STATE_IDLE_BELLOW:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("BELLOW");

		if (false == bIsSuccess)
			m_iState = STATE_IDLE + STATE_IDLE_BELLOW;
	}
	break;
	case STATE_IDLE + STATE_IDLE_DAMAGE:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("DAMAGE");
	}
	break;
	case STATE_IDLE + STATE_IDLE_SMILE:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("SMILE");
	}
	break;
	case STATE_IDLE + STATE_IDLE_DEAD:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("DEAD");
	}
	break;
	default:
		break;
	}
}

void CTeOni::Walk_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder)
{
	_bool bIsSuccess = false;
	switch (m_iState)
	{
	case STATE_WALK + STATE_FORWARD:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("WALK");
	}
	break;
	case STATE_WALK + STATE_FF:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("STEP_FF");
	}
	break;
	case STATE_WALK + STATE_STOP:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("STOP");
	}
	break;
	default:
		break;
	}
}

void CTeOni::Step_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder)
{
	_bool bIsSuccess = false;
	switch (m_iState)
	{
	case STATE_STEP + STATE_RIGHT:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("TURN_R");
	}
	break;
	case STATE_STEP + STATE_LEFT:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("TURN_L");
	}
	break;
	default:
		break;
	}
}

void CTeOni::Attack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder)
{
	_bool bIsSuccess = false;
	switch (m_iState)
	{
	case STATE_ATTACK + STATE_ATK_S_JUMP:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("S_JUMP");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_SWING:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("S_SWING");
	}
	break;
	case STATE_ATTACK + STATE_ATK_S_ONEHAND:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("S_ONEHAND");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_STONE:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("L_STONE");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_RUSH:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("L_RUSH");
	}
	break;
	case STATE_ATTACK + STATE_ATK_L_JUMP:
	{
		bIsSuccess = m_pAnimator->Input_Trigger("L_JUMP");
	}
	break;
	default:
		break;
	}
}

void CTeOni::Jump(const _double& _dTimeDelta, const _string& _strTrigger)
{
	if (false == m_bJumpAtkCheck)
	{
		_vector vPlayerPos = m_pTargetObject->Get_Transform()->Get_Pos();

		m_vJumpDir = vPlayerPos - m_pTransformCom->Get_Pos();
		m_fJumpLength = XMVectorGetX(XMVector3Length(m_vJumpDir));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->SetEffectStateCall(L"TEONI_LJUMP_1", m_pTargetObject->Get_Transform()->Get_WorldMatrix());
		RELEASE_INSTANCE(CGameInstance);

		m_bJumpAtkCheck = true;
	}

	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, XMVector3Normalize(m_vJumpDir), m_fJumpLength, 40.f, 0.5f, 2.f);

	_vector vOniPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vOniPos);
	if (fCurCellYPos >= XMVectorGetY(vOniPos))
	{
		m_dFallTimeAcc = 0.0;
		vOniPos = XMVectorSetY(vOniPos, fCurCellYPos);
		m_pAnimator->Input_Trigger(_strTrigger);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	m_pTransformCom->Move_Pos_NotNavigateY(vOniPos);
}

HRESULT CTeOni::Initialize_Sound(void)
{
	__super::Initialize_Sound();

	{
		Add_Sound(L"HIT_S_VOICE", L"TEONI_T_HIT1");
		Add_Sound(L"HIT_S_VOICE", L"TEONI_T_HIT2");
		Add_Sound(L"HIT_S_VOICE", L"TEONI_T_HIT3");
		Add_Sound(L"HIT_S_VOICE", L"TEONI_T_HIT4");

		Add_Sound(L"HIT_VOICE", L"TEONI_T_HIT1");
		Add_Sound(L"HIT_VOICE", L"TEONI_T_HIT2");
		Add_Sound(L"HIT_VOICE", L"TEONI_T_HIT3");
		Add_Sound(L"HIT_VOICE", L"TEONI_T_HIT4");
	}
	return S_OK;
}

CTeOni* CTeOni::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTeOni* pInstance = new CTeOni(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CTeOni::Clone(void* pArg)
{
	CTeOni* pInstance = new CTeOni(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni::Clone To Failed");
#endif
	}
	return pInstance;
}

void CTeOni::Free(void)
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
}
