#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CGateKeeperWoman.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "COneHandSword.h"
#include "CGKWStateMachine.h"
#include "CGkwEpController.h"
#include "CMonsterEPController.h"

CGateKeeperWoman::CGateKeeperWoman(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CGateKeeperWoman::CGateKeeperWoman(const CGateKeeperWoman& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CGateKeeperWoman::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGateKeeperWoman::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		GATEKEEPERWOMANDESC tDesc = *static_cast<GATEKEEPERWOMANDESC*>(pArg);
		m_tGobDesc = tDesc.tGobDesc;
		m_tMonsterDesc = tDesc.tMonsterDesc;
		m_tPawnStatus = tDesc.tPawnStatus;

		m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
		m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
		m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
		m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_ANCIENTHANNIBAL;
		m_tCharInfo.UpdateInfo(m_tPawnStatus);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Others()))
		return E_FAIL;

	m_fCurSpeed = 4.f;
	m_fRotationStandardValue = XMConvertToRadians(90.f) / 2.f;
	m_fRotValue = m_fRotationStandardValue;

	m_pGKWStateMachine->Change_State(CAncHanibalStateMachine::STATE_IDLE, this);
//	m_pGKWStateMachine->Change_State(CAncHanibalStateMachine::STATE_RETARGET, this);
	if (nullptr != m_pCollCollider)
		m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));

	m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);

	m_tMoveTimer = CUSTIMER(10.f);

	Load_BonesCollider(m_pModelCom, &m_vecBones);

	if (nullptr != m_pNavigation)
	{
		m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
	}
	m_pCollCollider->Set_Scale(_float3(20.f, 0.f, 0.f));
	m_iAttackPattern = PAT_END;

	m_tCharInfo.fMaxHp = m_tCharInfo.fCurHp = 1.f;
	return S_OK;
}

void CGateKeeperWoman::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Animation(_dTimeDelta);

#ifdef _DEBUG
	if (false == g_bGameRun)
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}
#endif
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		Find_Player(pGameInstance);

		if (nullptr != m_pTargetPlayer)
		{
			Bind_Supplement_Position();
			IDLE_STATE(_dTimeDelta, pGameInstance);
			FIND_STATE(_dTimeDelta, pGameInstance);
			RETARGET_STATE(_dTimeDelta, pGameInstance);
			DODGE_STATE(_dTimeDelta, pGameInstance);
			MOVE_STATE(_dTimeDelta, pGameInstance);
			RUN_STATE(_dTimeDelta, pGameInstance);
			ATTACK_STATE(_dTimeDelta, pGameInstance);
			DAMAGED_STATE(_dTimeDelta, pGameInstance);
			DIE_STATE(_dTimeDelta, pGameInstance);
			m_pGKWStateMachine->Tick(_dTimeDelta, this);
			Calculate_OffsetPosition();
			// Attack 
			Add_AroundPossibleAttackPlayer();
			Attack_Move(_dTimeDelta);
			Avoid_Move(_dTimeDelta);
			m_pGkwEpController->Update(_dTimeDelta, m_pGKWStateMachine, this);
		}
	}
	END_SINGLETON(pGameInstance);
}

void CGateKeeperWoman::Late_Tick(const _double& _dTimeDelta)
{
	if (true == g_bGameRun)
	{
		m_pGKWStateMachine->Late_Tick(_dTimeDelta, this);
		Restrict_AnimationToMoveOffset(m_pGKWStateMachine);
		m_pGkwEpController->Late_Update(_dTimeDelta, m_pGKWStateMachine, this);
	}
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CGateKeeperWoman::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGateKeeperWoman::Damaged(CPawn* _pDamageOwner)
{
	if (CGKWStateMachine::STATE_DIE == m_pGKWStateMachine->Get_State() ||
		CGKWStateMachine::STATE_DODGE == m_pGKWStateMachine->Get_State())
		return E_FAIL;

	if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
		if (nullptr == pWeapon)
			return E_FAIL;;

		m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());

		if (m_tCharInfo.Is_CheckDie())
		{
			m_pGKWStateMachine->Change_State(CGKWStateMachine::STATE_DIE, this);
		}
		else
		{
			if (false == m_pGKWStateMachine->Is_NextPage())
			{
				if (nullptr != m_pMonsterEPController)
				{
					CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
					_float3 v3Pos;
					_vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
					XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
					_vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
					m_pMonsterEPController->Show_Blood(vChestPos, vDir);
				}

				if (m_tCharInfo.fCurHp <= m_tCharInfo.fMaxHp / 2.f)
				{
					m_tHurtTimer.Reset_Timer();
					m_pGKWStateMachine->Set_TiredState(true);
				}

			}
			if (true == m_tHurtTimer.bPass)
			{
				m_tHurtTimer.Reset_Timer();
				if (CGKWStateMachine::STATE_HIT == m_pGKWStateMachine->Get_State() || 
					CGKWStateMachine::STATE_ATTACK == m_pGKWStateMachine->Get_State())
					return E_FAIL;

				if (nullptr != m_pMonsterEPController)
				{
					CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
					_float3 v3Pos;
					_vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
					XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
					_vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
					m_pMonsterEPController->Show_Blood(vChestPos, vDir);
				}

				m_pGKWStateMachine->Change_State(CGKWStateMachine::STATE_HIT, this);
			}
		}
	}
	else  if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
	{
	}
	return S_OK;
}

HRESULT CGateKeeperWoman::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_ANIMMODEL, g_iCurrentLevel, PROTO_COMP_MODEL_GATEKEEPERWOMAN, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Reset_TransformData()))
		return E_FAIL;

	{
		// CollCollider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(0.9f, 1.3f, 0.9f);
		tCollDesc.vTranslation = _float3(0.f, 1.3f, 0.f);
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// First Check
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(0.9f, 1.3f, 0.9f);
		tCollDesc.vTranslation = _float3(0.f, 1.3f, 0.f);
		if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
			(CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(5.f, 1.3f, 2.5f);
		tCollDesc.vTranslation = _float3(0.f, 1.3f, 0.f);
		if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pRangeCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(12.f, 1.3f, 10.f);
		tCollDesc.vTranslation = _float3(0.f, 1.3f, 0.f);
		if (FAILED(__super::Add_Component(COM_ATTACKDISCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pAttackNowRangeCollider, &tCollDesc)))
			return E_FAIL;
	}

	if (FAILED(Find_OffsetNode()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGateKeeperWoman::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGateKeeperWoman::Ready_Others()
{
	if (FAILED(__super::Ready_Others()))
		return E_FAIL;

	m_pGKWStateMachine = CGKWStateMachine::Create(m_pDevice, m_pContext, this);
	if (nullptr == m_pGKWStateMachine)
		return E_FAIL;

	CGkwEpController::GKWEPDESC tDesc;
	tDesc.pInstance = this;
	m_pGkwEpController = CGkwEpController::Create(m_pDevice, m_pContext, &tDesc);
	if (nullptr == m_pGkwEpController)
		return E_FAIL;

	_float3 v3PivotPos = _float3(0.f, 1.f, 0.21f);
	_float3 v3PivotScale = _float3(0.11f, 0.11f, 0.71f);
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		COneHandSword::ONEHANDSWRODDESC tDesc;
		tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_GATEKEEPERWOMAN_SWORD);
		tDesc.tEquipDesc.pOwnerObject = this;
		tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach";
		tDesc.tWeaponDesc.v3PivotColliderPos = v3PivotPos;
		tDesc.tWeaponDesc.v3PivotColliderSize = v3PivotScale;
		CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
			PROTO_GOB_ONEHANDSWORD, &tDesc);
		m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon->ReturnSettingToValue(this);
		Safe_AddRef(pWeapon);
	}
	END_SINGLETON(pGameInstance);

	return S_OK;
}

void CGateKeeperWoman::Calculate_OffsetPosition()
{
	if (CGWhiteDevilStateMachine::STATE_ATTACK == m_pGKWStateMachine->Get_State() ||
		CGWhiteDevilStateMachine::STATE_DODGE == m_pGKWStateMachine->Get_State()
		|| CGWhiteDevilStateMachine::STATE_HIT == m_pGKWStateMachine->Get_State())
	{
		_matrix matWorld =
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();

		if (nullptr != m_pAttackPosCell)
		{
			_vector vPos = m_pTransformCom->Get_Pos();
			vPos = XMVectorSetY(vPos, m_pAttackPosCell->Compute_Height(XMLoadFloat3(&m_v3OffsetPosition)));
			XMStoreFloat3(&m_v3OffsetPosition, vPos);
			vPos = XMVectorSetY(m_pTransformCom->Get_Pos(), m_v3OffsetPosition.y);
			m_pTransformCom->Set_Pos(vPos);
		}
		else
		{
			_float3 v3Pos;
			XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
			m_v3OffsetPosition.y = v3Pos.y;
		}

		matWorld.r[3] = XMLoadFloat3((_float3*)&m_v3OffsetPosition);
		XMStoreFloat4x4(&m_DrawAnimMatrix, matWorld);
		Coll_Tick(matWorld);
	}
	else
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		XMStoreFloat4x4(&m_DrawAnimMatrix, m_pTransformCom->Get_WorldMatrix());
		XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
		m_bRestrictAttackPos = false;
	}
}

void CGateKeeperWoman::Bind_Retarget()
{
	if (false == m_bBindRetarget)
		return;

	if (1 == m_byJudgeTurn)
	{
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Look());
	}
	else if(2 == m_byJudgeTurn)
	{
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Right());
	}
	else if (3 == m_byJudgeTurn)
	{
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Right());
	}
	m_bBindRetarget = false;
}

void CGateKeeperWoman::Bind_Dodge_Position()
{
	if (false == m_bChangeDodgeMatrix)
		return;

	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_bChangeDodgeMatrix = false;
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CGateKeeperWoman::Bind_Supplement_Position()
{
	if (false == m_bChangeSuppleCount)
		return;

	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_bChangeSuppleCount = false;
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if(CMonsterState::STATE_ATTACK == m_pGKWStateMachine->Get_State())
		m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
}

void CGateKeeperWoman::Add_AroundPossibleAttackPlayer()
{
	if (CGKWStateMachine::STATE_ATTACK == m_pGKWStateMachine->Get_State())
	{
		if (nullptr != m_pCollCollider)
		{
			CWeapon* pWeapon = static_cast<CWeapon*>(m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
			if (nullptr != pWeapon)
			{
				pWeapon->Add_CheckColliderList(m_pTargetPlayer);
			}
		}

		CCollider* pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
		if (nullptr != pCollider)
		{
			if (true == m_pGkwEpController->Is_Collision_Shoot(pCollider))
				m_pTargetPlayer->Damaged(this);
			else	 if (true == m_pGkwEpController->Is_Collision_UpEffect(pCollider))
				m_pTargetPlayer->Damaged(this);
		}
	}
}

void CGateKeeperWoman::Restrict_AnimationToMoveOffset(CMonsterState* _pState)
{
}

void CGateKeeperWoman::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CMonsterState::STATE_IDLE != m_pGKWStateMachine->Get_State())
		return;

	CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

	if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
		return;

	if (false == m_bFindPlayer)
	{
		if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
		{
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_FIND, this);
			m_bFindPlayer = true;
		}
	}
	else
	{
		m_pGKWStateMachine->Change_State(CMonsterState::STATE_FIND, this);
	}
}

void CGateKeeperWoman::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstnace)
{
	if (CMonsterState::STATE_FIND != m_pGKWStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
	}
}

void CGateKeeperWoman::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	Bind_Retarget();

	if (CMonsterState::STATE_RETARGET != m_pGKWStateMachine->Get_State())
		return;

	if (true == m_pGKWStateMachine->Is_Turn180())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bBindRetarget = true;
			m_byJudgeTurn = 1;
			m_pGKWStateMachine->Set_DogdeCount(0);
			m_pModelCom->Get_CurAnimation()->Set_SelectAnimRot(CAnimation::ROT_DEST);
			m_pGKWStateMachine->Set_Turn180(false);
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
	}
	else if (true == m_pGKWStateMachine->Is_TurnLeft90())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bBindRetarget = true;
			m_byJudgeTurn = 2;
			m_pGKWStateMachine->Set_DogdeCount(0);
			m_pModelCom->Get_CurAnimation()->Set_SelectAnimRot(CAnimation::ROT_DEST);
			m_pGKWStateMachine->Set_TurnLeft90(false);
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
	}
	else if (true == m_pGKWStateMachine->Is_TurnRight90())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bBindRetarget = true;
			m_byJudgeTurn = 3;
			m_pGKWStateMachine->Set_DogdeCount(0);
			m_pModelCom->Get_CurAnimation()->Set_SelectAnimRot(CAnimation::ROT_DEST);
			m_pGKWStateMachine->Set_TurnRight90(false);
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
	}
	else if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(20.f))
	{
		_float fDistance = PlayerToDistance();
		_float fDot = PlayerToDot_DirToSelfLookConvert();

		if (fDistance <= 7.f)
		{
			_uint RadomCount = _pGameInstance->Get_RandomCount() % 8;

			if (RadomCount <= 3)
			{
				m_iAttackIndex = ATK_ATTACKSTORM;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
			else 	if (RadomCount <= 5)
			{
				m_iAttackIndex = ATK_ATTACKRANGESHOOT;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
			else if(RadomCount == 6)
			{
				m_iAttackPattern = PAT_AVOID_BACKBACK;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
			else if (RadomCount == 7)
			{
				m_iAttackPattern = PAT_AVOID_FORFOR;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
		}

		// 앞에 봤을 때
		if (fDot <= 20.f && fDot >= -20.f)
		{
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
				|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						if (fDistance <= 6.f)
							m_iAttackPattern = PAT_AVOIDBACK;
					}
				}
				else
				{
					if (fDistance <= 6.f)
						m_iAttackPattern = PAT_AVOIDBACK;
				}
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
			else
			{
				_uint iRandomMove = _pGameInstance->Get_RandomCount(100) % 5;
				if (0 == iRandomMove)
				{
					_uint iRan = _pGameInstance->Get_RandomCount(100) % 3;
					if (iRan == 0)
					{
						m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 1)
					{
						m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 2)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					m_pGKWStateMachine->Set_RandomMove(true);
				}

				// 패턴이 PAT_AVOIDBACK 이였다면
				if (PAT_AVOIDBACK == m_iAttackPattern)
				{
					if (fDistance <= 6.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 4;

						if (0 == iRan)
							m_iAttackIndex = ATK_ATKTHRUSH_AFTER_ATKKNORMAL2;
						else if (1 == iRan)
							m_iAttackIndex = ATK_ATKTHRUSH_AFTER_ATKKNORMAL3;
						else  if (2 == iRan)
							m_iAttackIndex = ATK_ATKTHRUSH_AFTER_ATKKSLASH2;
						else if (3 == iRan)
							m_iAttackIndex = ATK_ATKTHRUSH_AFTER_ATKKSLASH3;

						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);

						m_iAttackPattern = PAT_END;
					}
					else if (fDistance <= 9.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 4;

						if (0 == iRan)
							m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL2;
						else if (1 == iRan)
							m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL3;
						else  if (2 == iRan)
							m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH2;
						else if (3 == iRan)
							m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH3;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);

						m_iAttackPattern = PAT_END;
					}
					else
					{
						m_iAttackPattern = PAT_END;
					}
				}
				else
				{
					// 거리가 가까울 때
					if (fDistance <= 7.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 6;
						if (iRan == 0)
						{
							m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else if (iRan == 1)
						{
							m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else if (iRan == 2)
						{
							m_iAttackIndex = ATK_ATTACKSTORM;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						if (iRan == 3)
						{
							m_iAttackPattern = PAT_AVOID_BACKBACK;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else if (iRan == 4)
						{
							m_iAttackIndex = ATK_ATTACKSHOOT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						else if (iRan == 5)
						{
							m_iAttackIndex = ATK_ATTACKRANGESHOOT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					else if (fDistance <= 14.f)
					{
						// 공격 당했었더라면
						if (true == m_pGKWStateMachine->Is_Damaged())
						{
							_uint iRan = _pGameInstance->Get_RandomCount() % 6;
							if (iRan == 0)
							{
								m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
							}
							else if (iRan == 1)
							{
								m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
							}
							else if (iRan == 2)
							{
								m_iAttackIndex = ATK_ATTACKSHOOT;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							else if (iRan == 3)
							{
								m_iAttackIndex = ATK_ATTACKSTORM;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							else if (iRan == 4)
							{
								m_iAttackIndex = ATK_ATTACKRANGESHOOT;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							else if (iRan == 5)
							{
								m_iAttackIndex = ATK_ATTACKRANGESHOOT;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							m_pGKWStateMachine->Set_Damaged(false);
						}
						else
						{
							_uint iRan = _pGameInstance->Get_RandomCount() % 15;

							if (0 == iRan)
								m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2;
							else if (1 == iRan)
								m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2;
							else if (2 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH2;
							else if (3 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL2;
							else if (4 == iRan)
								m_iAttackIndex = ATK_ATTACKJUMP;
							else if (5 == iRan)
								m_iAttackIndex = ATK_ATTACKNORMAL_2;
							else if (6 == iRan)
								m_iAttackIndex = ATK_ATKSLASH_2;
							else if (7 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKNORMAL2;
							else if (8 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKSLASH2;
							else
							{
								if (9 == iRan)
								{
									m_iAttackPattern = PAT_AVOID_LEFTRIGHT;
								}
								else if (10 == iRan)
								{
									m_iAttackPattern = PAT_AVOID_RIGHTLEFT;
								}
								else if (iRan == 11)
								{
									m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
									m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
								}
								else if (iRan == 12)
								{
									m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;
									m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
								}
								else if (iRan == 13)
								{
									m_iAttackPattern = PAT_AVOIDBACK;
									m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
								}
								else if (iRan == 14)
								{
									m_iAttackPattern = PAT_AVOID_BACKBACK;
									m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
								}
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
							}
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					// 거리가 중간으로 가까울 때
					else if (fDistance <= 21.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 10;

						if (8 == iRan || 9 == iRan)
						{
							if (8 == iRan)
								m_iAttackPattern = PAT_MOVELEFT;
							else if (9 == iRan)
								m_iAttackPattern = PAT_MOVERIGHT;
						}
						else
						{
							if (0 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH2;
							else if (1 == iRan)
								m_iAttackIndex = ATK_ATKSLASH_3;
							else if (2 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL2;
							else if (3 == iRan)
								m_iAttackIndex = ATK_ATTACKNORMAL_3;
							else if (4 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKNORMAL2;
							else if (5 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKSLASH2;
							else if (6 == iRan)
								m_iAttackIndex = ATK_ATKNORMAL2_AFTER_ROTATIONATK;
							else if (7 == iRan)
								m_iAttackIndex = ATK_ATKSLASH2_AFTER_ROTATIONATK;

							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					else if (fDistance <= 28.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 6;
						if (4 == iRan || 5 == iRan)
						{
							if (4 == iRan)
							{
								if (fDot > 0)
									m_iAttackPattern = PAT_MOVE_FORRIGHT;
								else
									m_iAttackPattern = PAT_MOVE_FORLEFT;

								m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
							}
							else
							{
								m_iAttackPattern = PAT_MOVEFOR;
								m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
							}
						}
						else
						{
							if (0 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH3;
							else if (1 == iRan)
								m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL3;
							else if (2 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKNORMAL3;
							else if (3 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKSLASH3;

							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					// 거리가 멀때
					else if(fDistance <= 35.f)
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 5;
						if (0 == iRan)
						{
							m_iAttackPattern = PAT_AVOID_FORFOR;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else if (1 == iRan)
						{
							m_iAttackPattern = PAT_AVOID_FOR_RIGHT_LEFT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else if (2 == iRan)
						{
							m_iAttackPattern = PAT_AVOID_FOR_LEFT_RIGHT;
							m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						else
						{
							if (3 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKNORMAL3;
							else if (4 == iRan)
								m_iAttackIndex = ATK_FAVOID_ATKSLASH3;

							m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					else
					{
						m_iAttackPattern = PAT_AVOID_FORFOR;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
				}
			}
		}
		// 뒤에 봤을 떄
		else if (fDot < -130.f || fDot > 130.f)
		{
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan)
						{
							if (fDistance <= 5.f)
								m_iAttackPattern = PAT_AVOIDBACK;
							else
								m_iAttackPattern = PAT_AVOIDFOR;
						}
						else
						{
							m_iAttackPattern = PAT_AVOIDLR;
						}
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
					{
						if (fDistance <= 5.f)
							m_iAttackPattern = PAT_AVOIDBACK;
						else
							m_iAttackPattern = PAT_AVOIDFOR;
					}
					else
					{
						m_iAttackPattern = PAT_AVOIDLR;
					}
				}
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
			else
			{
				if (fDistance >= 10.f)
				{
					m_pGKWStateMachine->Set_Turn180(true);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 1)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (iRan == 2)
					{
						m_iAttackIndex = ATK_ATTACKRANGESHOOT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else
					{
						m_iAttackPattern = PAT_AVOID_FORFOR;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}

				}
			}
		}
		// 오른쪽
		else if (fDot >= 50 && fDot <= 130)
		{
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan)
							m_iAttackPattern = PAT_AVOIDBACK;
						else if (1 == iRan)
							m_iAttackPattern = PAT_AVOIDFOR;
						else
							m_iAttackPattern = PAT_AVOIDLR;
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_AVOIDFOR;
					else
						m_iAttackPattern = PAT_AVOIDLR;
				}
			}
			else
			{
				if (fDistance <= 8.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 1)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (iRan == 2)
					{
						m_iAttackIndex = ATK_ATTACKRANGESHOOT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else
					{
						m_iAttackPattern = PAT_AVOID_FORFOR;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
				}
				else
				{
					m_pGKWStateMachine->Set_TurnRight90(true);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
			}
		}
		// 왼쪽
		else if (fDot >= -130.f && fDot <= -50.f)
		{
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan)
							m_iAttackPattern = PAT_AVOIDBACK;
						else if (1 == iRan)
							m_iAttackPattern = PAT_AVOIDFOR;
						else
							m_iAttackPattern = PAT_AVOIDLR;
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_AVOIDFOR;
					else
						m_iAttackPattern = PAT_AVOIDLR;
				}
			}
			else
			{
				if (fDistance <= 8.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 1)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (iRan == 2)
					{
						m_iAttackIndex = ATK_ATTACKRANGESHOOT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else
					{
						m_iAttackPattern = PAT_AVOID_FORFOR;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
				}
				else
				{
					m_pGKWStateMachine->Set_TurnLeft90(true);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
			}
		}
		//  앞의 오른쪽
		else if (20 < fDot && fDot < 50)
		{
			// 현재 플레이어가 공격 상태 일 때
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 2;
						if (0 == iRan)
							m_iAttackPattern = PAT_AVOIDBACK;
						else if (1 == iRan)
							m_iAttackPattern = PAT_AVOIDFOR;
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 2;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_AVOIDFOR;
				}
			}
			else
			{
				// 거리가 10보다 작을 때
				if (fDistance <= 10.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 7;
					if (0 == iRan)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					if (1 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if(2 == iRan)
					{
						m_iAttackPattern = PAT_AVOIDBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (3 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (4 == iRan)
					{
						m_iAttackPattern = PAT_MOVERIGHT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (iRan == 5)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (iRan == 6)
					{
						m_iAttackIndex = ATK_ATTACKRANGESHOOT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 6;
					if (0 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDR_ATKSLASH2_STRONG;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (1 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDR_ATKSLASH2_ROTATE;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (2 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2_STRONG;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (3 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2_ROTATE;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (4== iRan)
					{
						m_iAttackPattern = PAT_MOVE_BACKRIGHT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (5 == iRan)
					{
						m_iAttackPattern = PAT_MOVEBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
				}
			}
		}
		// 앞 왼
		else if (-20 > fDot && fDot > -50)
		{
			// 현재 플레이어가 공격 상태 일 때
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
				if (nullptr != pWeapon)
				{
					// 무기가 총일 때
					if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						m_iAttackPattern = PAT_AVOIDLR;
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 2;
						if (0 == iRan)
							m_iAttackPattern = PAT_AVOIDBACK;
						else if (1 == iRan)
							m_iAttackPattern = PAT_AVOIDFOR;
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 2;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_AVOIDFOR;
				}
			}
			else
			{
				// 거리가 10보다 작을 때
				if (fDistance <= 10.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 7;
					if (0 == iRan)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					if (1 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (2 == iRan)
					{
						m_iAttackPattern = PAT_AVOIDBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (3 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_BACKBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (4 == iRan)
					{
						m_iAttackPattern = PAT_MOVEBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (iRan == 5)
					{
						m_iAttackIndex = ATK_ATTACKSTORM;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (iRan == 6)
					{
						m_iAttackIndex = ATK_ATTACKRANGESHOOT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 6;
					if (0 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDL_ATKSLASH2_STRONG;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (1 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDL_ATKSLASH2_ROTATE;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (2 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2_STRONG;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (3 == iRan)
					{
						m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2_ROTATE;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (4 == iRan)
					{
						m_iAttackPattern = PAT_MOVEBACK;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (5 == iRan)
					{
						m_iAttackPattern = PAT_MOVE_BACKLEFT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
				}
			}
		}
	}
}

void CGateKeeperWoman::DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDodge)
	{
		Bind_Dodge_Position();
		m_bEndDodge = false;
	}

	if (CMonsterState::STATE_DODGE != m_pGKWStateMachine->Get_State())
		return;

	if (PAT_AVOID_BACK_LEFT_RIGHT == m_iAttackPattern || PAT_AVOID_BACK_RIGHT_LEFT == m_iAttackPattern
		|| PAT_AVOID_FOR_LEFT_RIGHT == m_iAttackPattern || PAT_AVOID_FOR_RIGHT_LEFT == m_iAttackPattern)
	{
		if (1 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (!strcmp("TSwordGK_DodgeNomal_L", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_DodgeNomal_R", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
			}
		}
		else if (3 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				if (true == m_pGKWStateMachine->Is_RandomMove())
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 8;
					if (0 == iRan)
						m_iAttackPattern = PAT_MOVEBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_MOVEFOR;
					else if (2 == iRan)
						m_iAttackPattern = PAT_MOVELEFT;
					else if (3 == iRan)
						m_iAttackPattern = PAT_MOVERIGHT;
					else if (4 == iRan)
						m_iAttackPattern = PAT_MOVE_BACKLEFT;
					else if (5 == iRan)
						m_iAttackPattern = PAT_MOVE_BACKRIGHT;
					else if (6 == iRan)
						m_iAttackPattern = PAT_MOVE_FORRIGHT;
					else if (7 == iRan)
						m_iAttackPattern = PAT_MOVE_FORRIGHT;

					m_pGKWStateMachine->Set_RandomMove(false);
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
				}
				else
				{
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	else if (PAT_AVOID_BACKBACK == m_iAttackPattern || PAT_AVOID_FORFOR == m_iAttackPattern)
	{
		if (1 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				if (true == m_pGKWStateMachine->Is_RandomMove())
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 8;
					if (0 == iRan)
						m_iAttackPattern = PAT_MOVEBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_MOVEFOR;
					else if (2 == iRan)
						m_iAttackPattern = PAT_MOVELEFT;
					else if (3 == iRan)
						m_iAttackPattern = PAT_MOVERIGHT;
					else if (4 == iRan)
						m_iAttackPattern = PAT_MOVE_BACKLEFT;
					else if (5 == iRan)
						m_iAttackPattern = PAT_MOVE_BACKRIGHT;
					else if (6 == iRan)
						m_iAttackPattern = PAT_MOVE_FORRIGHT;
					else if (7 == iRan)
						m_iAttackPattern = PAT_MOVE_FORRIGHT;

					m_pGKWStateMachine->Set_RandomMove(false);
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
				}
				else
				{
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	else if (PAT_AVOID_LEFTRIGHT == m_iAttackPattern)
	{
		if (1 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (!strcmp("TSwordGK_DodgeNomal_R", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
				{
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
				else
				{
					Calculate_OffsetMatrix();
				}
			}
		}
	}
	else if (PAT_AVOID_RIGHTLEFT == m_iAttackPattern)
	{
		if (1 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_DogdeCount())
		{
			if (!strcmp("TSwordGK_DodgeNomal_L", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
				{
					m_bChangeDodgeMatrix = true;
					m_bEndDodge = true;
					m_pGKWStateMachine->Set_DogdeCount(0);
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
				else
				{
					Calculate_OffsetMatrix();
				}
			}
		}
	}
	else
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeDodgeMatrix = true;
			m_bEndDodge = true;
			m_pGKWStateMachine->Set_DogdeCount(0);
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
}

void CGateKeeperWoman::MOVE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CMonsterState::STATE_MOVE != m_pGKWStateMachine->Get_State())
		return;

	_float fDistance = PlayerToDistance();

	if (PAT_MOVELEFT != m_iAttackPattern || PAT_MOVERIGHT != m_iAttackPattern)
	{
		_float fRot = PlayerToDot_DirToSelfLookConvert();
		if (fDistance >= 8.f && fRot <= 10.f && fRot >= -10.f)
		{
			if (true == m_tMoveTimer.Is_Over(_dTimeDelta))
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 12;

				if (0 == iRan)
					m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2;
				else if (1 == iRan)
					m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2;
				else if (2 == iRan)
					m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH2;
				else if (3 == iRan)
					m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL2;
				else if (4 == iRan)
					m_iAttackIndex = ATK_ATTACKJUMP;
				else if (5 == iRan)
					m_iAttackIndex = ATK_ATTACKNORMAL_2;
				else if (6 == iRan)
					m_iAttackIndex = ATK_ATKSLASH_2;
				else if (7 == iRan)
					m_iAttackIndex = ATK_FAVOID_ATKNORMAL2;
				else if(8 == iRan)
					m_iAttackIndex = ATK_FAVOID_ATKSLASH2;
				else
				{
					if (9 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_LEFTRIGHT;
					}
					else if (10 == iRan)
					{
						m_iAttackPattern = PAT_AVOID_RIGHTLEFT;
					}
					else if (iRan == 11)
					{
						m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (iRan == 12)
					{
						m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}

					m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				m_tMoveTimer.Reset_Timer();
			}
			else
			{
				m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
				if (PAT_MOVELEFT == m_iAttackPattern)
				{
					if (strcmp("TSwordGK_Walk_L_Loop", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
					{
						m_eDirection = DIR_LEFT;
						m_pGKWStateMachine->Set_ChangeMoveState(true);
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					m_pTransformCom->Move_Left(_dTimeDelta, m_fCurSpeed * 1.5f);
				}
				else
				{
					if (strcmp("TSwordGK_Walk_R_Loop", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
					{
						m_eDirection = DIR_RIGHT;
						m_pGKWStateMachine->Set_ChangeMoveState(true);
						m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					m_pTransformCom->Move_Right(_dTimeDelta, m_fCurSpeed * 1.5f);
				}
			}
		}
		else
		{
			if (strcmp("TSwordGK_Walk_B_Loop", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				m_eDirection = DIR_BACK;
				m_pGKWStateMachine->Set_ChangeMoveState(true);
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
			}

			if (false == m_tMoveTimer.Is_Over(_dTimeDelta))
			{
				if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta))
					m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed * 1.5f);
				else
					m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed * 1.5f);
			}
			else
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 8;

				if (0 == iRan)
					m_iAttackIndex = ATK_AVOIDL_ATKNORMAL2_STRONG;
				else if (1 == iRan)
					m_iAttackIndex = ATK_AVOIDR_ATKNORMAL2_ROTATE;
				else if (2 == iRan)
					m_iAttackIndex = ATK_AVOIDR_ATKSLASH2_ROTATE;
				else if (3 == iRan)
					m_iAttackIndex = ATK_AVOIDR_ATKSLASH2_ROTATE;
				else if (4 == iRan)
					m_iAttackIndex = ATK_ATKSLASH_3;
				else if (5 == iRan)
					m_iAttackIndex = ATK_ATTACKNORMAL_3;
				else if (6 == iRan)
					m_iAttackIndex = ATK_FAVOID_ATKNORMAL3;
				else if (7 == iRan)
					m_iAttackIndex = ATK_FAVOID_ATKSLASH3;

				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
	}
	else
	{
		_float fRot = PlayerToDot_DirToSelfLookConvert();

		if (fRot >= 20.f && fRot <= -20.f)
		{
			if (true == m_tMoveTimer.Is_Over(_dTimeDelta))
			{
				if (5 >= fDistance)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_AVOID_BACKBACK;
					else if (2 == iRan)
						m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
					else if (3 == iRan)
						m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;

					m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
				else if (10 >= fDistance)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 6;

					if (0 == iRan)
						m_iAttackIndex = ATK_ATTACKNORMAL_2;
					else if (1 == iRan)
						m_iAttackIndex = ATK_ATTACKNORMAL_3;
					else if (2 == iRan)
						m_iAttackIndex = ATK_STRONG_AFTER_ATKSLASH2;
					else if (3 == iRan)
						m_iAttackIndex = ATK_STRONG_AFTER_ATKNORMAL2;
					else if (4 == iRan)
						m_iAttackIndex = ATK_ATKSLASH_2;
					else if (5 == iRan)
						m_iAttackIndex = ATK_ATKSLASH_3;

					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 6;

					if (0 == iRan)
						m_iAttackIndex = ATK_FAVOID_ATKNORMAL2;
					else if (1 == iRan)
						m_iAttackIndex = ATK_FAVOID_ATKNORMAL3;
					else if (2 == iRan)
						m_iAttackIndex = ATK_FAVOID_ATKSLASH2;
					else if (3 == iRan)
						m_iAttackIndex = ATK_FAVOID_ATKSLASH3;
					else if (4 == iRan)
						m_iAttackIndex = ATK_ATKNORMAL2_AFTER_ROTATIONATK;
					else if (5 == iRan)
						m_iAttackIndex = ATK_ATKSLASH2_AFTER_ROTATIONATK;

					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}

				m_tMoveTimer.Reset_Timer();
			}
			else
			{
				m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
				switch (m_iAttackPattern)
				{
				case CGateKeeperWoman::PAT_MOVEFOR:
					m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVEBACK:
					m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_BACKLEFT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Right() -
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_BACKRIGHT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Right() -
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_FORLEFT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Right() +
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_FORRIGHT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Right() +
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				}
			}
		}
		else
		{
			if (5 >= fDistance)
			{
				switch (m_iAttackPattern)
				{
				case CGateKeeperWoman::PAT_MOVEFOR:
				case CGateKeeperWoman::PAT_MOVE_FORLEFT:
				case CGateKeeperWoman::PAT_MOVE_FORRIGHT:
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
						m_iAttackPattern = PAT_AVOIDBACK;
					else if(1 == iRan)
						m_iAttackPattern = PAT_AVOID_BACKBACK;
					else if(2 == iRan)
						m_iAttackPattern = PAT_AVOID_BACK_LEFT_RIGHT;
					else if (3 == iRan)
						m_iAttackPattern = PAT_AVOID_BACK_RIGHT_LEFT;

					m_pGKWStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					return;
					break;
				}
			}
			if (false == m_tMoveTimer.Is_Over(_dTimeDelta))
			{
				switch (m_iAttackPattern)
				{
				case CGateKeeperWoman::PAT_MOVEFOR:
					m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVEBACK:
					m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_BACKLEFT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Right() -
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_BACKRIGHT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Right() -
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_FORLEFT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Right() +
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				case CGateKeeperWoman::PAT_MOVE_FORRIGHT:
					m_pTransformCom->Translate_Pos(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Right() +
						m_pTransformCom->Get_Look(), _dTimeDelta, m_fCurSpeed * 0.8f);
					break;
				}
			}
			else
			{
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
		}
	}
}

void CGateKeeperWoman::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
}

void CGateKeeperWoman::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndAttack)
	{
		Bind_Attack_Postion();
		m_bEndAttack = false;
	}

	if (CMonsterState::STATE_ATTACK != m_pGKWStateMachine->Get_State())
		return;

	if (ATK_END == m_iAttackIndex)
	{
		m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		return;
	}
	// 2타
	if (ATK_ATKSLASH_2 == m_iAttackIndex || ATK_ATTACKNORMAL_2 == m_iAttackIndex)
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
				{
					m_bChangeAttackMatrix = true;
					m_bEndAttack = true;
					m_pGKWStateMachine->Set_AttackCount(0);
					m_iAttackIndex = ATK_END;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
				else
				{
					Calculate_OffsetMatrix();
				}
			}
		}
	}
	// 2타
	else if (ATK_BACK_ATTACKJUMP == m_iAttackIndex || ATK_AVOIDFOR_ATTACKJUMP == m_iAttackIndex)
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackJump01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
				{
					m_bChangeAttackMatrix = true;
					m_bEndAttack = true;
					m_pGKWStateMachine->Set_AttackCount(0);
					m_iAttackIndex = ATK_END;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
				}
				else
				{
					Calculate_OffsetMatrix();
				}
			}
		}
	}
	// 3타
	else if (ATK_ATTACKNORMAL_3 == m_iAttackIndex || ATK_ATKSLASH_3 == m_iAttackIndex
		 || ATK_ATKNORMAL2_AFTER_ROTATIONATK == m_iAttackIndex || ATK_ATKSLASH2_AFTER_ROTATIONATK == m_iAttackIndex)
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (3 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_bEndAttack = true;
				m_pGKWStateMachine->Set_AttackCount(0);
				m_iAttackIndex = ATK_END;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	// 3타
	else if (ATK_ATKTHRUSH_AFTER_ATKKNORMAL2 == m_iAttackIndex || ATK_ATKTHRUSH_AFTER_ATKKSLASH2 == m_iAttackIndex
		 || ATK_STRONG_AFTER_ATKNORMAL2 == m_iAttackIndex || ATK_STRONG_AFTER_ATKSLASH2 == m_iAttackIndex || ATK_FAVOID_ATKNORMAL2  == m_iAttackIndex )
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{     
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (3 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_bEndAttack = true;
				m_pGKWStateMachine->Set_AttackCount(0);
				m_iAttackIndex = ATK_END;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	// 3타
	else if (ATK_AVOIDL_ATKNORMAL2 == m_iAttackIndex || ATK_AVOIDR_ATKNORMAL2 == m_iAttackIndex)
	{
	if (1 == m_pGKWStateMachine->Get_AttackCount())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
		{
			Calculate_OffsetMatrix();
			m_bChangeSuppleCount = true;
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
	}
	else if (2 == m_pGKWStateMachine->Get_AttackCount())
	{
		if (!strcmp("TSwordGK_AttackNormal01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
	}
	else if (3 == m_pGKWStateMachine->Get_AttackCount())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pGKWStateMachine->Set_AttackCount(0);
			m_iAttackIndex = ATK_END;
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
	}
	// 4타
	else if (ATK_ATKTHRUSH_AFTER_ATKKNORMAL3 == m_iAttackIndex || ATK_ATKTHRUSH_AFTER_ATKKSLASH3 == m_iAttackIndex
	|| ATK_STRONG_AFTER_ATKNORMAL3 == m_iAttackIndex || ATK_STRONG_AFTER_ATKSLASH3 == m_iAttackIndex
	|| ATK_FAVOID_ATKNORMAL3 == m_iAttackIndex || ATK_FAVOID_ATKSLASH3 == m_iAttackIndex)
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (3 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (4 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_bEndAttack = true;
				m_pGKWStateMachine->Set_AttackCount(0);
				m_iAttackIndex = ATK_END;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	// 4타
	else if (ATK_AVOIDL_ATKNORMAL2_STRONG == m_iAttackIndex || ATK_AVOIDR_ATKNORMAL2_STRONG == m_iAttackIndex
	|| ATK_AVOIDR_ATKNORMAL2_ROTATE == m_iAttackIndex || ATK_AVOIDL_ATKNORMAL2_ROTATE == m_iAttackIndex

	|| ATK_AVOIDR_ATKSLASH2_STRONG == m_iAttackIndex || ATK_AVOIDL_ATKSLASH2_STRONG == m_iAttackIndex
	|| ATK_AVOIDR_ATKSLASH2_ROTATE == m_iAttackIndex || ATK_AVOIDL_ATKSLASH2_ROTATE == m_iAttackIndex)
	{
		if (1 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleCount = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
			}
		}
		else if (2 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (3 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (!strcmp("TSwordGK_AttackSlash02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str())
				|| !strcmp("TSwordGK_AttackNormal02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pGKWStateMachine->Get_NextAnimationCaluclateValue()))
				{
					Calculate_OffsetMatrix();
					m_bChangeSuppleCount = true;
					m_pGKWStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
		}
		else if (4 == m_pGKWStateMachine->Get_AttackCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_bEndAttack = true;
				m_pGKWStateMachine->Set_AttackCount(0);
				m_iAttackIndex = ATK_END;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	else
	{
		// 연속 공격 아닌 것들
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pGKWStateMachine->Set_AttackCount(0);
			if (ATK_ATTACKSTORM == m_iAttackIndex)
				m_pGkwEpController->Boss_UpEffect_End();
			else 	if (ATK_ATTACKSHOOT == m_iAttackIndex)
				m_pGkwEpController->Boss_Shoot_End();
			else 	if (ATK_ATTACKRANGESHOOT == m_iAttackIndex)
				m_pGkwEpController->Boss_RangeShoot_End();
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else
		{
			if (!strcmp("TSwordGK_KetsugiStorm01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f) &&
					false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					m_pGkwEpController->Boss_UpEffect(m_pTransformCom->Get_Pos());
					m_pModelCom->Get_CurAnimation()->Set_FastValue(1.f);
				}
				else
				{
					m_pModelCom->Get_CurAnimation()->Set_FastValue(2.5f);
				}
			}
			else if (!strcmp("TSwordGK_KetsugiAimShoot01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.0f) &&
					false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					m_pGkwEpController->Boss_Shoot(Get_BonePosition("LeftHandAttach_end"), m_pTargetPlayer->Get_OffsetPos() - m_pTransformCom->Get_Pos());
				}
			}
			else if (!strcmp("TSwordGK_KetsugiAllrangeShoot01_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.0f) &&
					false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					m_pGkwEpController->Boss_RangeShoot(Get_BonePosition("LeftHandAttach_end"), 
						XMVector3Normalize(m_pTransformCom->Get_Right()));
				}
			}
			Calculate_OffsetMatrix();
		}
	}
}

void CGateKeeperWoman::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDamagedPosition)
	{
		Bind_Damaged_Position();
		m_bEndDamagedPosition = false;
	}

	if (CMonsterState::STATE_HIT != m_pGKWStateMachine->Get_State())
		return;

	if (false == m_pGKWStateMachine->Is_TiredState())
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeDamagedPosition = true;
			m_bEndDamagedPosition = true;
			m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
	else
	{
		if (1 == m_pGKWStateMachine->Get_TiredCount())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_HIT, this);
				m_bChangeSuppleCount = true;
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
		else if (2 == m_pGKWStateMachine->Get_TiredCount())
		{
			if (!strcmp("TSwordGK_Down_P_Loop", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_HIT, this);
				m_bChangeSuppleCount = true;
			}
		}
		else if (3 == m_pGKWStateMachine->Get_TiredCount())
		{
			if (!strcmp("TSwordGK_Down_P_End", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
			{
				m_bEndDamagedPosition = true;
				m_bChangeDamagedPosition = true;
				m_pGKWStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
			}
		}
	}
}

void CGateKeeperWoman::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
}

void CGateKeeperWoman::Attack_Move(const _double& _dTimeDelta)
{
	if (CMonsterState::STATE_ATTACK != m_pGKWStateMachine->Get_State())
		return;

	if (m_pModelCom->Get_CurAnimation()->Get_AnimClipSection()->size() <= 0)
		return;

	if (m_pModelCom->Get_CurAnimation()->Is_OverTime_AnimClip("Dash", static_cast<_double>(m_fOverTime_AttackMove)))
	{
		if (true == m_pGKWStateMachine->Is_Arrive())
			return;

		_float fDistance = fabs(XMVectorGetX(XMVector3Length(m_pTargetPlayer->Get_OffsetPos() -
			m_pTransformCom->Get_Pos())));

		if (fDistance > 4.f)
		{
			m_pModelCom->Get_CurAnimation()->Set_StopAnimation(true);
			m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
			m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 4.f);

			m_pGkwEpController->Boss_RoadEffect(m_pTransformCom->Get_Pos());
		}
		else
		{
			m_pGKWStateMachine->Set_Arrive(true);
			m_pModelCom->Get_CurAnimation()->Set_StopAnimation(false);
			m_pGkwEpController->Boss_RoadEnd();
		}
	}
	else  if (m_pModelCom->Get_CurAnimation()->Is_BetweenTime_Clip("Dash"))
	{
		if (true == m_pGKWStateMachine->Is_Arrive())
			return;

		_float fDistance = fabs(XMVectorGetX(XMVector3Length(m_pTargetPlayer->Get_OffsetPos() -
			m_pTransformCom->Get_Pos())));


		if (fDistance > 4.f)
		{
			m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
			m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 4.f);
			m_pGkwEpController->Boss_RoadEffect(m_pTransformCom->Get_Pos());
		}
		else
		{
			m_pModelCom->Get_CurAnimation()->Set_StopAnimation(false);
			m_pGKWStateMachine->Set_Arrive(true);
			m_pGkwEpController->Boss_RoadEnd();
		}
	}
	else
	{
		if (m_pModelCom->Get_CurAnimation()->Is_BetweenTime_Clip("Dash_Thrust"))
		{
			if (true == m_pGKWStateMachine->Is_Arrive())
				return;

			_float fDistance = fabs(XMVectorGetX(XMVector3Length(m_pTargetPlayer->Get_OffsetPos() -
				m_pTransformCom->Get_Pos())));

			if (fDistance > 4.f)
			{
				m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 2.f);
				m_pGkwEpController->Boss_RoadEffect(m_pTransformCom->Get_Pos());
			}
			else
			{
				m_pGKWStateMachine->Set_Arrive(true);
				m_pGkwEpController->Boss_RoadEnd();
			}
		}
	}
}

void CGateKeeperWoman::Avoid_Move(const _double& _dTimeDelta)
{
	if (CMonsterState::STATE_DODGE != m_pGKWStateMachine->Get_State())
		return;

	if (m_pModelCom->Get_CurAnimation()->Is_BetweenTime_Clip("Dash_Avoid"))
	{
		_float fDistance = fabs(XMVectorGetX(XMVector3Length(m_pTargetPlayer->Get_OffsetPos() -
			m_pTransformCom->Get_Pos())));
		m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed * 0.5f);
	}
}

#pragma region USE_TOOL_METHOD
_bool CGateKeeperWoman::Extern_EditValue()
{
	_bool bActive = false;

	if (ImGui::TreeNodeEx(Get_Name()))
	{
		ImGui::PushItemWidth(200);
		Extern_PawnData();
		Extern_GobData();
		Extern_InitAnation();
		Extern_ModifySightDesc();
		bActive = ImGui::Button("Create");
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	return bActive;
}

_bool CGateKeeperWoman::Extern_HierakeyView(const string& _str)
{
	if (nullptr == m_pTransformCom)
		return false;

	_bool bActive = false;
	if (ImGui::TreeNodeEx(_str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(200);
		Extern_PawnData();
		Extern_GobData();
		Control_ColliderData();
		Extern_ShowCurAnimation_Data();
		Extern_ShowCurHierckeyNode(&m_vecBones);
		ImGui::DragFloat("AttackMove_Dash_Over", &m_fOverTime_AttackMove);
		_bool bClone = ImGui::Button("Clone");
		if (true == bClone)
		{
			BEGIN_SINGLETON(CGameInstance, pGameInstance);
			{
				CGateKeeperWoman::GATEKEEPERWOMANDESC tDesc;
				tDesc.eInitAnimType = INIT_IDLE;
				tDesc.tGobDesc = m_tGobDesc;
				tDesc.tMonsterDesc = m_tMonsterDesc;
				tDesc.tPawnStatus = m_tPawnStatus;
				pGameInstance->Clone_GameObject(g_iCurrentLevel, Get_Layer(), Get_Proto(), &tDesc);
			}
			END_SINGLETON(pGameInstance);
		}
		bActive = ImGui::Button("Delete");
		ImGui::PopItemWidth();
		ImGui::TreePop();
		return bActive;
	}
	return false;
}

HRESULT CGateKeeperWoman::Create_Self(CGameObject** _ppOut)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CGateKeeperWoman::GATEKEEPERWOMANDESC tDesc;
		tDesc.eInitAnimType = INIT_IDLE;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
			, Get_Proto(), &tDesc));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CGateKeeperWoman::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CGateKeeperWoman::Save_Data()
{
	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		GATEKEEPERWOMANDESC tDesc;
		tDesc.eInitAnimType = INIT_IDLE;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		pDataManager->Get_MonsterMgr()->Insert_GateKeepWoman(tDesc);
	}
	END_SINGLETON(pDataManager);
	return S_OK;
}

HRESULT CGateKeeperWoman::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
			, Get_Proto(), _pData));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

_bool CGateKeeperWoman::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

void CGateKeeperWoman::Reset_Data()
{
	__super::Reset_Data();
}
#pragma endregion USE_TOOL_METHOD

void CGateKeeperWoman::Extern_InitAnation()
{
}

void CGateKeeperWoman::Control_ColliderData()
{
	__super::Control_ColliderData();
}

CGateKeeperWoman* CGateKeeperWoman::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CGateKeeperWoman* pInstance = new CGateKeeperWoman(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("GateKeeperWoman::Create To Failed");
	}
	return pInstance;
}

CGameObject* CGateKeeperWoman::Clone(void* pArg)
{
	CGateKeeperWoman* pInstance = new CGateKeeperWoman(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGateKeeperWoman::Clone To Failed");
	}
	return pInstance;
}

void CGateKeeperWoman::Free()
{
	Safe_Release(m_pGKWStateMachine);
	Safe_Release(m_pGkwEpController);
	__super::Free();
}
