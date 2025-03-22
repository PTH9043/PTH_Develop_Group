#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CGiantWhiteDevil.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "CHalberd.h"
#include "CMonsterEPController.h"

CGiantWhiteDevil::CGiantWhiteDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CGiantWhiteDevil::CGiantWhiteDevil(const CGiantWhiteDevil& _rhs) 
	: CMonster(_rhs)
{
}

HRESULT CGiantWhiteDevil::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGiantWhiteDevil::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		GWHITEDEVILDESC tDesc = *static_cast<GWHITEDEVILDESC*>(pArg);
		m_eInitIdleType = tDesc.eInitAnimType;
		m_tGobDesc = tDesc.tGobDesc;
		m_tMonsterDesc = tDesc.tMonsterDesc;
		m_tPawnStatus = tDesc.tPawnStatus;

		m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
		m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
		m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
		m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_GIANTWHITEDEVIL;
		m_tCharInfo.UpdateInfo(m_tPawnStatus);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Others()))
		return E_FAIL;

	m_fCurSpeed = 4.f;
	m_fRotationStandardValue = XMConvertToRadians(90.f) / 2.f;
	m_fLookToRotateValue = m_fRotationStandardValue;

	m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_IDLE, this);

	if (nullptr != m_pCollCollider)
		m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));

	m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
	m_pCollCollider->Set_Scale(_float3(20.f, 0.f, 0.f));
	return S_OK;
}

void CGiantWhiteDevil::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Animation(_dTimeDelta);

	if (false == g_bGameRun)
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
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
		m_pGWhiteDevilStateMachine->Tick(_dTimeDelta, this);
		Calculate_OffsetPosition();
		Add_AroundPossibleAttackPlayer();
	}
	END_SINGLETON(pGameInstance);
}

void CGiantWhiteDevil::Late_Tick(const _double& _dTimeDelta)
{
	if (true == g_bGameRun)
	{
		Restrict_AnimationToMoveOffset(m_pGWhiteDevilStateMachine);
		m_pGWhiteDevilStateMachine->Late_Tick(_dTimeDelta, this);
	}
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CGiantWhiteDevil::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGiantWhiteDevil::Damaged(CPawn* _pDamageOwner)
{
	if (CGWhiteDevilStateMachine::STATE_DIE == m_pGWhiteDevilStateMachine->Get_State() ||
		CGWhiteDevilStateMachine::STATE_DODGE == m_pGWhiteDevilStateMachine->Get_State())
		return E_FAIL;

	if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
		if (nullptr == pWeapon)
			return E_FAIL;;

		m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());
		m_bChangeSuppleCount = true;

		if (m_tCharInfo.Is_CheckDie())
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
			m_bDead = true;
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DIE, this);
		}
		else
		{
			if (true == m_tHurtTimer.bPass)
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

				m_tHurtTimer.Reset_Timer();
				if (CGWhiteDevilStateMachine::STATE_HIT == m_pGWhiteDevilStateMachine->Get_State() || 
					CGWhiteDevilStateMachine::STATE_ATTACK == m_pGWhiteDevilStateMachine->Get_State())
					return E_FAIL;
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_HIT, this);
			}
		}
	}
	else  if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
	{
	}
	return S_OK;
}

HRESULT CGiantWhiteDevil::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_ANIMMODEL, g_iCurrentLevel, PROTO_COMP_MODEL_GIANTWHITEDEVIL,(CComponent**)&m_pModelCom)))
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

HRESULT CGiantWhiteDevil::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGiantWhiteDevil::Ready_Others()
{
	if (FAILED(__super::Ready_Others()))
		return E_FAIL;

	m_pGWhiteDevilStateMachine = CGWhiteDevilStateMachine::Create(m_pDevice, m_pContext, this);
	if (nullptr == m_pGWhiteDevilStateMachine)
		return E_FAIL;

	_float3 v3PivotPos = _float3(0.f, 1.f, 0.21f);
	_float3 v3PivotScale = _float3(0.11f, 0.11f, 0.71f);
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		CHalberd::HALBERDDESC tDesc;
		tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_GIANTWHITEDEVIL_WHITELARGEHALBERD);
		tDesc.tEquipDesc.pOwnerObject = this;
		tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach";
		tDesc.tWeaponDesc.v3PivotColliderPos = v3PivotPos;
		tDesc.tWeaponDesc.v3PivotColliderSize = v3PivotScale;
		CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
			PROTO_GOB_HALBERD, &tDesc);
		m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon->ReturnSettingToValue(this);
		Safe_AddRef(pWeapon);
	}
	END_SINGLETON(pGameInstance);

	return S_OK;
}

void CGiantWhiteDevil::Calculate_OffsetPosition()
{
/*	if (false == m_pModelCom->Is_ChangeAnimation())
	{
		_matrix matWorld = 
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		_float3 v3Pos;
		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();
		matWorld.r[3] = XMLoadFloat3(&m_v3OffsetPosition);
		Coll_Tick(matWorld);
	}
	else*/  if (CGWhiteDevilStateMachine::STATE_ATTACK == m_pGWhiteDevilStateMachine->Get_State() 
	|| CGWhiteDevilStateMachine::STATE_DODGE == m_pGWhiteDevilStateMachine->Get_State())
	{
		_matrix matWorld = 
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();

		_float3 v3Pos;
		XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
		m_v3OffsetPosition.y = v3Pos.y;

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

void CGiantWhiteDevil::Bind_Dodge_Position()
{
	if (false == m_bChangeDodgeMatrix)
		return;

	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_bChangeDodgeMatrix = false;
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CGiantWhiteDevil::Add_AroundPossibleAttackPlayer()
{
	if (CGiantDevilStateMachine::STATE_ATTACK == m_pGWhiteDevilStateMachine->Get_State())
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
		if (nullptr != pWeapon)
		{
			pWeapon->Add_CheckColliderList(m_pTargetPlayer);
		}
	}
}

void CGiantWhiteDevil::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (nullptr == m_pTargetPlayer)
		return;

	if (CGWhiteDevilStateMachine::STATE_IDLE != m_pGWhiteDevilStateMachine->Get_State())
		return;

	CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

	if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
		return;

	if (false == m_bFindPlayer)
	{
		if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
		{
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
			m_bFindPlayer = true;
		}
		else
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
	}
	else
	{
		m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
	}
}

void CGiantWhiteDevil::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstnace)
{
	if (CGWhiteDevilStateMachine::STATE_FIND != m_pGWhiteDevilStateMachine->Get_State())
		return;
}

void CGiantWhiteDevil::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CGWhiteDevilStateMachine::STATE_RETARGET != m_pGWhiteDevilStateMachine->Get_State())
		return;

	if (m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(3.f))
	{
		m_fCurSpeed = 4.f;
		m_fRotationStandardValue = XMConvertToRadians(90.f) / 2.f;
		m_fLookToRotateValue = m_fRotationStandardValue;

		_float fDistance = PlayerToDistance();

		CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
		if (nullptr != pWeapon)
		{
			_float fPlayerToDot = SelfToDot_DirToPlayerLookConvert();

			if (fPlayerToDot <= 30.f && fPlayerToDot >= -30.f)
			{
				if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
					|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
				{
					m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
				}
			}
		}

		if (fDistance <= 3)
		{
			m_iAttackIndex = ATK_5;
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
		}
		else if (fDistance <= 5)
		{
			_float fPlayerToDot = SelfToDot_DirToPlayerLookConvert();

			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
			else if (CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
			else
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 80;

				if (iRan <= 19)
					m_iAttackPattern = PAT_1;
				else if (iRan <= 39)
					m_iAttackPattern = PAT_2;
				else if (iRan <= 59)
					m_iAttackPattern = PAT_3;
				else if (iRan <= 80)
					m_iAttackPattern = PAT_4;


				if (PAT_2 == m_iAttackPattern || PAT_3 == m_iAttackPattern)
				{
					Calculate_DirectionToLookPlayer();
					m_pGWhiteDevilStateMachine->Set_PatternMoveTimer(CUSTIMER(4.f));
				}
				else if (PAT_4 == m_iAttackPattern)
				{
					m_eDirection = DIR_BACK;
				}

				iRan = _pGameInstance->Get_RandomCount() % 80;
				if (iRan <= 19)
					m_iAttackIndex = ATK_1;
				else if (iRan <= 39)
					m_iAttackIndex = ATK_2;
				else if (iRan <= 59)
					m_iAttackIndex = ATK_3;
				else if (iRan <= 80)
					m_iAttackIndex = ATK_4;

				_float fDot = PlayerToDot_DirToSelfLookConvert();
				if (fDot <= 10.f && fDot >= -10.f)
				{
					m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
				}
				else if (fDot <=135.f && fDot <= -135.f)
				{
					m_iAttackIndex = ATK_6;
					m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
				}
				else
				{
					m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
				}
			}
		}
		else
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 80;

			if (iRan <= 19)
				m_iAttackPattern = PAT_1;
			else if (iRan <= 39)
				m_iAttackPattern = PAT_2;
			else if (iRan <= 59)
				m_iAttackPattern = PAT_3;
			else if (iRan <= 80)
				m_iAttackPattern = PAT_4;


			if (PAT_2 == m_iAttackPattern || PAT_3 == m_iAttackPattern)
			{
				Calculate_DirectionToLookPlayer();
				m_pGWhiteDevilStateMachine->Set_PatternMoveTimer(CUSTIMER(4.f));
			}
			else if (PAT_4 == m_iAttackPattern)
			{
				m_eDirection = DIR_BACK;
			}

			iRan = _pGameInstance->Get_RandomCount() % 80;
			if (iRan <= 19)
				m_iAttackIndex = ATK_1;
			else if (iRan <= 39)
				m_iAttackIndex = ATK_2;
			else if (iRan <= 59)
				m_iAttackIndex = ATK_3;
			else if (iRan <= 80)
				m_iAttackIndex = ATK_4;

			if (fDistance >= 10.f)
			{
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);
			}
			else
			{
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
			}
		}
	}
}

void CGiantWhiteDevil::DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDodge)
	{
		Bind_Dodge_Position();
		m_bEndDodge = false;
	}

	if (CGWhiteDevilStateMachine::STATE_DODGE != m_pGWhiteDevilStateMachine->Get_State())
		return;

		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
			m_bChangeDodgeMatrix = true;
			m_bEndDodge = true;
		}
		else
		{
			Calculate_OffsetMatrix();
		}
}

void CGiantWhiteDevil::MOVE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CGWhiteDevilStateMachine::STATE_MOVE != m_pGWhiteDevilStateMachine->Get_State())
		return;

	_float fDistacne = PlayerToDistance();

	if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
		|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
	{
		_float fPlayerToDot = SelfToDot_DirToPlayerLookConvert();

		if (fPlayerToDot <= 30.f && fPlayerToDot >= -30.f)
		{
			CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
			if (nullptr != pWeapon)
			{
				if (CWeapon::WEAPON_GUN == pWeapon->Get_EquipType())
					m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
			}

			if (fDistacne <= 4.f)
			{
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
			}
		}
	}

	if (fDistacne >= 10.f)
		m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);

	if (!strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Idle_N_Loop"))
	{
		m_fLookToRotateValue = 0.f;
		m_fRotationStandardValue = 0.f;
		m_fCurSpeed = 0.f;
	}
	else if(!strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
	{
		m_fRotationStandardValue = XMConvertToRadians(90.f) / 2.f;
		m_fLookToRotateValue = m_fRotationStandardValue;
		m_fCurSpeed = 4.f;
	}
	else
	{
		m_fCurSpeed = 4.f;
	}

	_vector vLook = m_pTransformCom->Get_Look();
	_vector vRight = m_pTransformCom->Get_Right();
	if (PAT_1 == m_iAttackPattern)
	{
		if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta))
		{
			m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(true);
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
		}
		else
		{
			if (fDistacne <= 5.f)
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);

			m_pGWhiteDevilStateMachine->Set_PlayerFindIdleState(true);
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Idle_N_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
		}
	}
	else if (PAT_2 == m_iAttackPattern)
	{
		// 플레이어가 앞에 있는지 확인
		_float fDot = PlayerToDot_DirToSelfLookConvert();
		if (fDot <= 10 && fDot >= -10)
		{		// 특정 시간이 지나면
			if (false == m_pGWhiteDevilStateMachine->Get_PatternMoveTimer()->Is_Over(_dTimeDelta))
			{
				// 거리가 6보다 가까울 때 뒤로 빠진다.
				if (fDistacne <= 6.f)
				{
					Calculate_DirectionToLookPlayer();

					if (CCharacter::DIR_FOR == m_eDirection)
						m_eDirection = DIR_BACK;
					else if (CCharacter::DIR_FORLEFT == m_eDirection)
						m_eDirection = DIR_BACKRIGHT;
					else if (CCharacter::DIR_FORRIGHT == m_eDirection)
						m_eDirection = DIR_BACKLEFT;

					if (CCharacter::DIR_BACK == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(-vLook);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}
					else if (CCharacter::DIR_BACKLEFT == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(-vLook - vRight);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}
					else if (CCharacter::DIR_BACKRIGHT == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(-vLook + vRight);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}

					if (m_eDirection != m_ePrevDirection)
						m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);

					m_ePrevDirection = m_eDirection;
				}
				else
				{
					m_pGWhiteDevilStateMachine->Set_PlayerFindIdleState(true);
					// 만약 6보다 가깝지 않다면 가만히 있는다.
					if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Idle_N_Loop"))
						m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
				}
			}
			else
			{
				m_pGWhiteDevilStateMachine->Get_PatternMoveTimer()->Reset_Timer();
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);
			}
		}
		else
		{
			m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(true);
			// 만약 6보다 가깝지 않다면 가만히 있는다.
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
			else
				RotateLookToPlayer(_dTimeDelta);
		}
	}
	else if (PAT_3 == m_iAttackPattern)
	{			// 특정 시간이 지나면
		_float fDot = PlayerToDot_DirToSelfLookConvert();
		if (fDot <= 10 && fDot >= -10)
		{
			if (false == m_pGWhiteDevilStateMachine->Get_PatternMoveTimer()->Is_Over(_dTimeDelta))
			{
				if (fDistacne <= 6.f)
				{
					m_pGWhiteDevilStateMachine->Set_PlayerFindIdleState(true);

					if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Idle_N_Loop"))
						m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
				}
				else
				{
					Calculate_DirectionToLookPlayer();

					if (CCharacter::DIR_FOR == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(vLook);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}
					else if (CCharacter::DIR_FORLEFT == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(vLook - vRight);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}
					else if (CCharacter::DIR_FORRIGHT == m_eDirection)
					{
						_vector vDir = XMVector3Normalize(vLook + vRight);
						m_pTransformCom->Translate_Dir(vDir, _dTimeDelta, m_fCurSpeed / 2.f);
					}

					if (m_eDirection != m_ePrevDirection)
						m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
				}
			}
			else
			{
				m_pGWhiteDevilStateMachine->Get_PatternMoveTimer()->Reset_Timer();
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);
			}
		}
		else
		{
			m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(true);
			// 만약 6보다 가깝지 않다면 가만히 있는다.
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
			else
				RotateLookToPlayer(_dTimeDelta);
		}
	}
	else if (PAT_4 == m_iAttackPattern)
	{
		if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta))
		{
			m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(true);
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);
		}
		else
		{
			m_pGWhiteDevilStateMachine->Set_PlayerFindIdleState(true);
			if (!strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_MOVE, this);

			if (fDistacne <= 5.f)
			{
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
			}
			else if(fDistacne >= 10.f)
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);

			m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed / 2.f);
		}

	}

	m_pGWhiteDevilStateMachine->Set_PlayerFindIdleState(false);
	m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(false);
	m_eDirection = DIR_END;
}

void CGiantWhiteDevil::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CGWhiteDevilStateMachine::STATE_RUN != m_pGWhiteDevilStateMachine->Get_State())
		return;

	_float fDistance = PlayerToDistance();

	if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
		|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
	{
		if (fDistance <= 4.f)
		{
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DODGE, this);
		}
	}

	if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta))
	{
		m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(true);
		if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);
	}
	else
	{
		m_fCurSpeed = 4.f;
		if (fDistance <= 4.f)
		{
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
		}
		else
		{
			m_pGWhiteDevilStateMachine->Set_PlayerNotForward_MoveStation(false);
			if (!strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "AS_TGiantWhite_THalberd_Walk_B_Loop"))
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RUN, this);
			m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
		}
	}
}

void CGiantWhiteDevil::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndAttack)
	{
		Bind_Attack_Postion();
		m_bEndAttack = false;

		if (ATK_6 == m_iAttackIndex)
		{
			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Look());
		}
	}

	if (CGWhiteDevilStateMachine::STATE_ATTACK != m_pGWhiteDevilStateMachine->Get_State())
		return;

	if (ATK_1 == m_iAttackIndex || ATK_2 == m_iAttackIndex || ATK_3 == m_iAttackIndex 
		|| ATK_4 == m_iAttackIndex || ATK_5 == m_iAttackIndex)
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
	else if (ATK_6 == m_iAttackIndex)
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}

	Add_AroundPossibleAttackPlayer();
}

void CGiantWhiteDevil::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDamagedPosition)
	{
		Bind_Damaged_Position();
		m_bEndDamagedPosition = false;
	}

	if (CGWhiteDevilStateMachine::STATE_HIT != m_pGWhiteDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDamagedPosition = true;
		m_bEndDamagedPosition = true;
		m_pGWhiteDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RETARGET, this);
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}

void CGiantWhiteDevil::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDiePosition)
	{
		m_bEndDiePosition = false;
		Bind_Die_Position();
	}

	if (CGWhiteDevilStateMachine::STATE_DIE != m_pGWhiteDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DIE, this);
		m_bChangeDiePosition = true;
		m_bEndDiePosition = true;
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}
#pragma region USE_TOOL_METHOD
_bool CGiantWhiteDevil::Extern_EditValue()
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

_bool CGiantWhiteDevil::Extern_HierakeyView(const string& _str)
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
		_bool bClone = ImGui::Button("Clone");
		if (true == bClone)
		{
			BEGIN_SINGLETON(CGameInstance, pGameInstance);
			{
				CGiantWhiteDevil::GWHITEDEVILDESC tDesc;
				tDesc.eInitAnimType = m_eInitIdleType;
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

HRESULT CGiantWhiteDevil::Create_Self(CGameObject** _ppOut)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CGiantWhiteDevil::GWHITEDEVILDESC tDesc;
		tDesc.eInitAnimType = m_eInitIdleType;
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

HRESULT CGiantWhiteDevil::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CGiantWhiteDevil::Save_Data()
{
	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		CGiantWhiteDevil::GWHITEDEVILDESC tDesc;
		tDesc.eInitAnimType = m_eInitIdleType;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		pDataManager->Get_MonsterMgr()->Insert_GWhiteDevilData(tDesc);
	}
	END_SINGLETON(pDataManager);
	return S_OK;
}

HRESULT CGiantWhiteDevil::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
			, Get_Proto(), _pData));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

_bool CGiantWhiteDevil::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

void CGiantWhiteDevil::Reset_Data()
{
	__super::Reset_Data();
}

void CGiantWhiteDevil::Extern_InitAnation()
{
	const char* IdleType[INIT_ANIMTYPE::INIT_END] = { "Idle", "Walk" };
	ImGui::Combo("IdleType", (_int*)&m_eInitIdleType, IdleType, INIT_ANIMTYPE::INIT_END);
}

#pragma endregion USE_TOOL_METHOD

CGiantWhiteDevil* CGiantWhiteDevil::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CGiantWhiteDevil* pInstance = new CGiantWhiteDevil(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGiantWhiteDevil::Create To Failed");
	}
	return pInstance;
}

CGameObject* CGiantWhiteDevil::Clone(void* pArg)
{
	CGiantWhiteDevil* pInstance = new CGiantWhiteDevil(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGiantWhiteDevil::Clone To Failed");
	}
	return pInstance;
}

void CGiantWhiteDevil::Free()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_WEAPON, m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
	}
	END_SINGLETON(pGameInstance);
	Safe_Release(m_pGWhiteDevilStateMachine);
	__super::Free();
}


/*
	else if (ATK_4 == m_iAttackIndex)
	{
		if (1 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.0f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
				Bind_Supplement_Position();
			}
		}
		else if (2 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Set_AtkIndex(0);
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
				m_bEndAttack = true;
				m_bChangeAttackMatrix = true;
			}
			else if (false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.2f))
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	else if (ATK_5 == m_iAttackIndex)
	{
		if (1 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.0f))
			{
				Calculate_OffsetMatrix();
				m_bChangeSuppleAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
				Bind_Supplement_Position();
			}
		}
		else if (2 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Set_AtkIndex(0);
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
				m_bEndAttack = true;
			}
			else if (false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.2f))
			{
				Calculate_OffsetMatrix();
			}
		}
	}
	else if (ATK_7S == m_iAttackIndex)
	{
		if (0 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.6f))
			{
				m_bChangeSuppleAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_ATTACK, this);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
		else if (1 == m_pGWhiteDevilStateMachine->Get_AtkIndex())
		{
			if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
			{
				m_bChangeAttackMatrix = true;
				m_pGWhiteDevilStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_RETARGET, this);
				m_pGWhiteDevilStateMachine->Set_AtkIndex(0);
			}
			else
			{
				Calculate_OffsetMatrix();
			}
		}
	}
*/