#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CAncientHannibal.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "COneHandSword.h"
#include "CParticle.h"
#include "CAncEPController.h"
#include "CMonsterEPController.h"
#include "CPotal.h"

CAncientHannibal::CAncientHannibal(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CAncientHannibal::CAncientHannibal(const CAncientHannibal& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CAncientHannibal::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CAncientHannibal::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		ANCHANNIBALDESC tDesc = *static_cast<ANCHANNIBALDESC*>(pArg);
		m_eInitIdleType = tDesc.eInitAnimType;
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

	m_fCurSpeed = 6.f;
	m_fRotationStandardValue = XMConvertToRadians(90.f) / 2.f;
	m_fRotValue = m_fRotationStandardValue;

	m_pAncHannibalStateMachine->Change_State(CAncHanibalStateMachine::STATE_IDLE, this);

	if (nullptr != m_pCollCollider)
		m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));

	m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);

	m_tMoveTimer = CUSTIMER(3.f);
	m_tSnakeTimer = CUSTIMER(15.f);

	Load_BonesCollider(m_pModelCom, &m_vecBones);

	m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
	m_pCollCollider->Set_Scale(_float3(20.f, 0.f, 0.f));

	CAncEPController::ANCEPDESC tDesc;
	tDesc.pInstance = this;
	m_pAncEpController = CAncEPController::Create(m_pDevice, m_pContext, &tDesc);

	m_tCharInfo.fMaxHp = m_tCharInfo.fCurHp = 1.f;
	return S_OK;
}

void CAncientHannibal::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Animation(_dTimeDelta);

	if (false == g_bGameRun)
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		m_pAncEpController->Update(_dTimeDelta, m_pAncHannibalStateMachine, this);
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
		m_pAncHannibalStateMachine->Tick(_dTimeDelta, this);
		Calculate_OffsetPosition();
		// Attack 
		Add_AroundPossibleAttackPlayer();
		m_pAncEpController->Update(_dTimeDelta, m_pAncHannibalStateMachine, this);
		Judge_AttackStrongCount();
	}
	END_SINGLETON(pGameInstance);
}

void CAncientHannibal::Late_Tick(const _double& _dTimeDelta)
{
	if (true == g_bGameRun)
	{
		m_pAncHannibalStateMachine->Late_Tick(_dTimeDelta, this);
		Restrict_AnimationToMoveOffset(m_pAncHannibalStateMachine);
		m_pAncEpController->Late_Update(_dTimeDelta, m_pAncHannibalStateMachine, this);
	}
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CAncientHannibal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAncientHannibal::Damaged(CPawn* _pDamageOwner)
{
	if (CAncHanibalStateMachine::STATE_DODGE == m_pAncHannibalStateMachine->Get_State())
		return E_FAIL;

	if (CAncHanibalStateMachine::STATE_DIE == m_pAncHannibalStateMachine->Get_State())
		return E_FAIL;

	if (CMonsterState::STATE_HIT == m_pAncHannibalStateMachine->Get_State())
		return S_OK;

	if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
		if (nullptr == pWeapon)
			return E_FAIL;;

		m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());
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
			m_pPotal->Set_Active(true);
			CTransform* pTransform = static_cast<CTransform*>(m_pPotal->Get_Component(COM_TRANSFORM));
			if (nullptr != pTransform)
				pTransform->Set_Pos(XMLoadFloat3(&m_v3OffsetPosition) + XMVectorSet(0.f, 2.f, 0.f, 0.f));

			m_pAncHannibalStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_DIE, this);
		}
		else
		{
			if (true == m_tHurtTimer.bPass)
			{
				if (CMonsterState::STATE_ATTACK == m_pAncHannibalStateMachine->Get_State())
					return S_OK;

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
				m_pAncHannibalStateMachine->Change_State(CGWhiteDevilStateMachine::STATE_HIT, this);
			}
		}
	}
	return S_OK;
}

_bool CAncientHannibal::Is_Collision(CCollider* _pCollider)
{
	for (auto& iter : m_vecBones)
	{
		if (true == iter.Is_Collision(_pCollider))
			return true;
	}

	return false;
}

HRESULT CAncientHannibal::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_ANIMMODEL, g_iCurrentLevel, PROTO_COMP_MODEL_ANCIENTHANNIBAL, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Reset_TransformData()))
		return E_FAIL;

	{
		// CollCollider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(1.5f, 2.5f, 1.5f);
		tCollDesc.vTranslation = _float3(0.f, 2.5f, 0.f);
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// First Check
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(1.5f, 2.5f, 1.5f);
		tCollDesc.vTranslation = _float3(0.f, 2.5f, 0.f);
		if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
			(CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(5.f, 2.5f, 2.5f);
		tCollDesc.vTranslation = _float3(0.f, 2.5f, 0.f);
		if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pRangeCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(20.f, 2.5f, 10.f);
		tCollDesc.vTranslation = _float3(0.f, 2.5f, 0.f);
		if (FAILED(__super::Add_Component(COM_ATTACKDISCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pAttackNowRangeCollider, &tCollDesc)))
			return E_FAIL;
	}

	if (FAILED(Find_OffsetNode()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAncientHannibal::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAncientHannibal::Ready_Others()
{
	if (FAILED(__super::Ready_Others()))
		return E_FAIL;

	m_pAncHannibalStateMachine = CAncHanibalStateMachine::Create(m_pDevice, m_pContext, this);
	if (nullptr == m_pAncHannibalStateMachine)
		return E_FAIL;

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		m_pPotal = static_cast<CPotal*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
			LAYER_POTAL, PROTO_GOB_POTAL));
		Safe_AddRef(m_pPotal);
		m_pPotal->Set_Active(false);
	}
	END_SINGLETON(pGameInstance);

	return S_OK;
}

HRESULT CAncientHannibal::Find_OffsetNode()
{
	m_pNodeArr[NODE_OFFSET] = m_pModelCom->Get_HierarchyNodes("CHARA_OFFSET");
	m_pNodeArr[NODE_CHEST] = m_pModelCom->Get_HierarchyNodes("joint_Hip");
	for (_uint i = 0; i < NODE_END; ++i)
	{
		Safe_AddRef(m_pNodeArr[i]);
		if (nullptr == m_pNodeArr[i])
			return E_FAIL;
	}

	return S_OK;
}

void CAncientHannibal::Calculate_OffsetPosition()
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
	else*/  if (CAncHanibalStateMachine::STATE_ATTACK == m_pAncHannibalStateMachine->Get_State() || 
		CAncHanibalStateMachine::STATE_DODGE == m_pAncHannibalStateMachine->Get_State() 
		|| CAncHanibalStateMachine::STATE_HIT == m_pAncHannibalStateMachine->Get_State())
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
	else if (m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
	{
		_matrix matWorld =
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();
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

void CAncientHannibal::Bind_Dodge_Position()
{
	if (false == m_bChangeDodgeMatrix)
		return;

	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_bChangeDodgeMatrix = false;
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CAncientHannibal::Add_AroundPossibleAttackPlayer()
{
	if (CAncHanibalStateMachine::STATE_ATTACK == m_pAncHannibalStateMachine->Get_State())
	{
		if (nullptr != m_pCollCollider)
		{
			CCollider* pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));

			pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
			if (nullptr == pCollider)
				return;

			if (true == Is_Collision(pCollider))
				m_pTargetPlayer->Damaged(this);

			if(true == m_pAncEpController->Is_Collision(pCollider))
				m_pTargetPlayer->Damaged(this);
		}
	}
}

void CAncientHannibal::Judge_AttackStrongCount()
{
	if (ATK_FIRSTORM == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_STRONG;
	else if(ATK_GILD_N == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_NORMAL;
	else if (ATK_GILD_SP == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_STRONG;
	else if(ATK_HALFTURN == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_WEEK;
	else if(ATK_HOOK == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_NORMAL;
	else if (ATK_JUMP == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_NORMAL;
	else if (ATK_POINTSHOOT == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_NORMAL;
	else if (ATK_SLASH == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_WEEK;
	else if (ATK_STAMP == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_NORMAL;
	else if (ATK_TAIL == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_WEEK;
	else if (ATK_WEAPONCOMBO1 == m_iAttackIndex)
		m_eAtkStrong = ATK_STR::AS_STRONG;
}

void CAncientHannibal::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (nullptr == m_pTargetPlayer)
		return;

	if (CMonsterState::STATE_IDLE != m_pAncHannibalStateMachine->Get_State())
		return;

	if (CMonsterState::STATE_IDLE != m_pAncHannibalStateMachine->Get_State())
		return;

	CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

	if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
		return;

	if (false == m_bFindPlayer)
	{
		if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
		{
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_FIND, this);
			m_bFindPlayer = true;
		}
	}
	else
	{
		m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_FIND, this);
	}
}

void CAncientHannibal::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstnace)
{
	if (CMonsterState::STATE_FIND != m_pAncHannibalStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
	}
}

void CAncientHannibal::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CMonsterState::STATE_RETARGET != m_pAncHannibalStateMachine->Get_State())
		return;

	Next_Retarget(_dTimeDelta, _pGameInstance);
}

void CAncientHannibal::DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDodge)
	{
		m_bEndDodge = false;
		Bind_Dodge_Position();
	}

	if (CMonsterState::STATE_DODGE != m_pAncHannibalStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDodgeMatrix = true;
		m_bEndDodge = true;

		Next_Dogde(_dTimeDelta, _pGameInstance);
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}

void CAncientHannibal::MOVE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CMonsterState::STATE_MOVE != m_pAncHannibalStateMachine->Get_State())
		return;

	NextMove(_dTimeDelta, _pGameInstance);
}

void CAncientHannibal::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CMonsterState::STATE_RUN != m_pAncHannibalStateMachine->Get_State())
		return;
	NextRun(_dTimeDelta, _pGameInstance);
}

void CAncientHannibal::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndAttack)
	{
		if (ATK_TAIL == m_iPrevAttackIndex || ATK_HALFTURN == m_iPrevAttackIndex)
		{
			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() - m_pTransformCom->Get_Look());
			m_iPrevAttackIndex = 100;
		}

		Bind_Attack_Postion();
		m_bEndAttack = false;
	}

	if (CMonsterState::STATE_ATTACK != m_pAncHannibalStateMachine->Get_State())
		return;

	if (ATK_JUMP == m_iAttackIndex || ATK_SLASH == m_iAttackIndex || ATK_STAMP == m_iAttackIndex
		|| ATK_HOOK == m_iAttackIndex || ATK_ONEHAND == m_iAttackIndex || ATK_FIRSTORM == m_iAttackIndex
		|| ATK_GILD_N == m_iAttackIndex || ATK_GILD_SP == m_iAttackIndex || ATK_POINTSHOOT == m_iAttackIndex
		 || ATK_WEAPONCOMBO1 == m_iAttackIndex)
	{

		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_pAncEpController->End_FireWallState();
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);

			if (ATK_JUMP == m_iAttackIndex)
			{
				m_pAncEpController->Jump_State(XMLoadFloat3(&m_v3OffsetPosition));
			}
		}
		else
		{
			if (ATK_FIRSTORM == m_iAttackIndex)
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f) &&
					false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					CTransform* pTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
					if(nullptr != pTransform)
						m_pAncEpController->Attack_FireWallStatre(pTransform->Get_Pos(), m_pTransformCom->Get_Pos());

					m_pModelCom->Get_CurAnimation()->Set_FastValue(1.f);
				}
				else
				{
					m_pModelCom->Get_CurAnimation()->Set_FastValue(3.f);
				}
			}
			else if (ATK_POINTSHOOT == m_iAttackIndex)
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f) &&
					false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
				{
					CTransform* pTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
					if (nullptr != pTransform)
						m_pAncEpController->Attack_Fire_WallState_Just5(pTransform->Get_Pos(), m_pTransformCom->Get_Pos());

					m_pModelCom->Get_CurAnimation()->Set_FastValue(1.f);
				}
				else
				{
					m_pModelCom->Get_CurAnimation()->Set_FastValue(3.f);
				}
			}
			else if (ATK_GILD_N == m_iAttackIndex || ATK_GILD_SP == m_iAttackIndex)
			{
				if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f))
				{
					m_pAncEpController->Jump_State(XMLoadFloat3(&m_v3OffsetPosition));
				}
			}
			Calculate_OffsetMatrix();
		}
	}
	else if (ATK_TAIL == m_iAttackIndex)
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			m_iPrevAttackIndex = ATK_TAIL;
			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
	else if (ATK_HALFTURN == m_iAttackIndex)
	{
		if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
		{
			_float fPlayerToDistance = PlayerToDistance();

			if (fPlayerToDistance <= 6.f)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 5;
				if (0 == iRan)
					m_iAttackIndex = ATK_SLASH;
				else if (1 == iRan)
					m_iAttackIndex = ATK_ONEHAND;
				else if (2 == iRan)
					m_iAttackIndex = ATK_HOOK;
				else if (3 == iRan)
					m_iAttackIndex = ATK_STAMP;
				else
					m_iAttackIndex = ATK_WEAPONCOMBO1;
			}
			else if(fPlayerToDistance <= 10.f)
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 3;
				if (0 == iRan)
					m_iAttackIndex = ATK_JUMP;
				else if (1 == iRan)
					m_iAttackIndex = ATK_GILD_N;
				else if (2== iRan)
					m_iAttackIndex = ATK_GILD_SP;

			}
			else
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 2;
				if (0 == iRan)
					m_iAttackIndex = ATK_FIRSTORM;
				else if (1 == iRan)
					m_iAttackIndex = ATK_POINTSHOOT;
			}
			m_iPrevAttackIndex = ATK_HALFTURN;

			m_pModelCom->Get_CurAnimation()->Set_AnimSupIndex(CAnimation::SUPINDEX_INIT);
			m_pModelCom->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);

			m_bChangeAttackMatrix = true;
			m_bEndAttack = true;

			m_pAncEpController->Jump_State(XMLoadFloat3(&m_v3OffsetPosition));
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
}

void CAncientHannibal::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDamagedPosition)
	{
		Bind_Damaged_Position();
		m_bEndDamagedPosition = false;
	}

	if (CMonsterState::STATE_HIT != m_pAncHannibalStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDamagedPosition = true;
		m_bEndDamagedPosition = true;
		m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}

void CAncientHannibal::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{

	if (CMonsterState::STATE_DIE != m_pAncHannibalStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDiePosition = true;
		m_bEndDiePosition = true;

		BEGIN_SINGLETON(CGameInstance, pGameInstnace)
		{
			pGameInstnace->Add_DeleteObject(g_iCurrentLevel, LAYER_MONSTER, this);
		}
		END_SINGLETON(pGameInstnace);
	}
}

#pragma region NEXT_STATE

void CAncientHannibal::Next_Retarget(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(10.f))
	{
		m_iAttackPattern = 100000;
		_float fDistance = PlayerToDistance();
		_float fDot = PlayerToDot_DirToSelfLookConvert();

		CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
		if (nullptr != pWeapon)
		{			
			if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
			{
				// 공격 상태 일때
				if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
					|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
				{
					_float fPlayerToDot = SelfToDot_DirToPlayerLookConvert();

					if (fPlayerToDot <= 30.f && fPlayerToDot >= -30.f)
					{
						m_iAttackPattern = PAT_JUSTMOVEBACK;
					}
				}
			}
		}

		// 거리가 20보다 클 때
		if (fDistance > 20.f)
		{
			// HalfTurn Attack
			m_iAttackIndex = ATK_FIRSTORM;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else if (fDistance > 12.f)
		{
			// HalfTurn Attack
			m_iAttackIndex = ATK_POINTSHOOT;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else

		// 플레이어가 앞이나 뒤에 있는 경우
		if ((fDot <= 10.f && fDot >= -10.f) || (fDot >= 170.f || fDot <= -170.f))
		{
			// 거리가 7보다 작을 때
			if (fDistance <= 6.f)
			{
				// 플레이어가 공격 상태일 때
				if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
					|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
				{
					CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
					// 무기 있을 때
					if (nullptr != pWeapon)
					{
						// 무기가 총일 때
						if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
						{
							// 현재 플레이어를 보고 있을 때
							if (fDot <= 10.f && fDot >= -10.f)
							{
								// HalfTurn Attack
								m_iAttackIndex = ATK_HALFTURN;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							// 뒤에 있을 때
							else
							{
								// 꼬리 공격
								m_iAttackIndex = ATK_TAIL;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
						}
						else
						{
							// 랜덤
							_uint iRan = _pGameInstance->Get_RandomCount() % 3;
							if (0 == iRan || 1 == iRan)
							{
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
							}
							// 3분의 1 확률
							else if (2 == iRan)
							{
								// 현재 플레이어를 보고 있을 때
								if (fDot <= 10.f && fDot >= -10.f)
								{
									// HalfTurn Attack
									m_iAttackIndex = ATK_HALFTURN;
									m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
								}
								// 뒤에 있을 때
								else
								{
									// 꼬리 공격
									m_iAttackIndex = ATK_TAIL;
									m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
								}
							}
						}
					}
					// 무기 없을 때
					else
					{
						// 랜덤
						_uint iRan = _pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan || 1 == iRan)
						{
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						// 3분의 1 확률
						else if (2 == iRan)
						{
							// 현재 플레이어를 보고 있을 때
							if (fDot <= 10.f && fDot >= -10.f)
							{
								// HalfTurn Attack
								m_iAttackIndex = ATK_HALFTURN;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
							// 뒤에 있을 때
							else
							{
								// 꼬리 공격
								m_iAttackIndex = ATK_TAIL;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
							}
						}
					}
				}
				// 공격 상태가 아닐 때
				else
				{
					// 플레이어가 앞에 있을 때
					if (fDot <= 10.f && fDot >= -10.f)
					{
						///  6분의 1 확률로 각 행동들을 결정
						_uint iRan = _pGameInstance->Get_RandomCount() % 14;

						// 3분의 1
						if (0 == iRan || 1 == iRan || 2 == iRan)
						{
							// 랜덤 카운트 다시 발생
							iRan = _pGameInstance->Get_RandomCount() % 10;

							// 절반의 확률로 JumpAttack
							if (0 <= iRan && iRan <= 4)
								m_iAttackIndex = ATK_JUMP;
							// 절반의 확률로 Glid Normal
							else
							{
								iRan = _pGameInstance->Get_RandomCount() % 4;
								// 또 3분의 1은 특별 GILD 공격
								if (iRan == 3)
									m_iAttackIndex = ATK_GILD_SP;
								// 아니라면 그냥 노멀한 공격
								else
									m_iAttackIndex = ATK_GILD_N;
							}
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						// 3, 4일때
						else if (3 == iRan || 4 == iRan)
						{
							// Hook 공격
							m_iAttackIndex = ATK_HOOK;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						// 5, 6
						else if (5 == iRan || 6 == iRan)
						{
							// HalfTurn 후 공격
							m_iAttackIndex = ATK_HALFTURN;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						// 7, 8
						else if (7 == iRan || 8 == iRan)
						{
							// 뒤로 물린 후 백 공격
							m_iAttackPattern = PAT_BACKANDSHOOT;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						// 9, 10
						else if (9 == iRan || 10 == iRan)
						{
							// 뒤로 물린 후 스네이크
							m_iAttackPattern = PAT_DOGDEANDSNAKE;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						// 11, 12
						else if (11 == iRan || 12 == iRan)
						{
							// 뒤로 물린 후 스네이크
							m_iAttackPattern = PAT_DOGDESHOOT;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
						// 마지막 확률
						else if (13 == iRan)
						{
							// 연속 공격
							m_iAttackIndex = ATK_WEAPONCOMBO1;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
					}
					// 플레이어가 뒤에 있을 때
					else
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 3;
						if (0 == iRan)
						{
							// 0 꼬리 공격
							m_iAttackIndex = ATK_TAIL;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						else if (1 == iRan)
						{
							// 1 하프 턴 공격
							m_iAttackIndex = ATK_HALFTURN;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						else if (2 == iRan)
						{
							// 뒤로 물러 선 뒤 점프 공격 아니면 콤보 공격
							iRan = _pGameInstance->Get_RandomCount() % 2;
							if (0 == iRan)
								m_iAttackPattern = PAT_DOGDEANDJUMPATTACK;
							else if (1 == iRan)
								m_iAttackPattern = PAT_DOGDEANDWEAPONCOMBO;

							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
					}
				}
			}
			// 거리가 7보다 클 때
			else
			{
				// 앞에 있을 경우
				if ((fDot <= 10.f && fDot >= -10.f))
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
					{
						// 
						iRan = _pGameInstance->Get_RandomCount() % 5;

						if (0 == iRan || 1 == iRan)
							m_iAttackIndex = ATK_JUMP;
						else if (2 == iRan || 3 == iRan)
							m_iAttackIndex = ATK_GILD_N;
						else 
							m_iAttackIndex = ATK_GILD_SP;

						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if (1 == iRan)
					{
						// 4분의 1 확률로 앞, 뒤, 좌우 결정
						iRan = _pGameInstance->Get_RandomCount() % 8;
						if (0 == iRan || 4 == iRan)
							m_iAttackPattern = PAT_MOVEBACK;
						else if (1 == iRan || 5 == iRan)
							m_iAttackPattern = PAT_MOVELEFT;
						else if (2 == iRan || 6 == iRan)
							m_iAttackPattern = PAT_MOVERIGHT;
						else if (3 == iRan || 7 == iRan)
							m_iAttackPattern = PAT_MOVEFORWARD;

						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else
					{
						// 달림
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
					}
				}
				// 플레이어가 뒤에 있을 경우
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
					{
						// 하프 턴
						m_iAttackIndex = ATK_HALFTURN;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
					}
					else if(1 == iRan)
					{					
						// 4분의 1 확률로 앞, 뒤, 좌우 결정
						iRan = _pGameInstance->Get_RandomCount() % 8;
						if (0 == iRan || 4 == iRan)
							m_iAttackPattern = PAT_MOVEBACK;
						else if (1 == iRan || 5 == iRan)
							m_iAttackPattern = PAT_MOVELEFT;
						else if (2 == iRan || 6 == iRan)
							m_iAttackPattern = PAT_MOVERIGHT;
						else if (3 == iRan || 7 == iRan)
							m_iAttackPattern = PAT_MOVEFORWARD;

						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else
					{
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
					}
				}
			 }
		}
		// 플레이어가 앞이나 뒤에 있지 않을 때
		else
		{
			// 공격 상태 일때
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
				|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				// 거리가 7보다 작을 때
				if (fDistance <= 6.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() %4;
					if (0 == iRan)
					{
						// 만약 플레이어가 뒤쪽에 있을 경우
						if (fDot >= 120 || fDot <= -120)
						{
							// 0 꼬리 공격
							m_iAttackIndex = ATK_TAIL;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						// 아니라면
						else
						{
							// 그냥  뒤로 빠진다.
							m_iAttackPattern = PAT_BACKAVOIDMOVE1;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
						}
					}
					else if (1 == iRan)
					{
						// 1  두 번 물러 난 후, 스네이크
						m_iAttackPattern = PAT_DOUBLEDOGDEAFTERSNAKE;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else if (2 == iRan)
					{
						// 2 한 번 물러 난다.
						m_iAttackPattern = PAT_NORMALDOGDE;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
					else
					{
						// 2 한 번 물러 난다.
						m_iAttackPattern = PAT_DOGDESHOOT;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 2;
					if (0 == iRan)
					{
						// 4분의 1 확률로 앞, 뒤, 좌우 결정
						iRan = _pGameInstance->Get_RandomCount() % 8;
						if (0 == iRan || 4 == iRan)
							m_iAttackPattern = PAT_MOVEBACK;
						else if (1 == iRan || 5 == iRan)
							m_iAttackPattern = PAT_MOVELEFT;
						else if (2 == iRan || 6 == iRan)
							m_iAttackPattern = PAT_MOVERIGHT;
						else if (3 == iRan || 7 == iRan)
							m_iAttackPattern = PAT_MOVEFORWARD;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else
					{
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
					}
				}
			}
			// 공격 상태가 아닐 때
			else
			{
				if (fDistance <= 6.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 3;
					if (0 == iRan)
					{
						// 4분의 1 확률로 앞, 뒤, 좌우 결정
						iRan = rand() % 8;
						if (0 == iRan || 4 == iRan)
							m_iAttackPattern = PAT_MOVEBACK;
						else if (1 == iRan || 5 == iRan)
							m_iAttackPattern = PAT_MOVELEFT;
						else if (2 == iRan || 6 == iRan)
							m_iAttackPattern = PAT_MOVERIGHT;
						else if (3 == iRan || 7 == iRan)
							m_iAttackPattern = PAT_MOVEFORWARD;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (1 == iRan)
					{
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
					}
					else
					{
						if (fDot >= 120.f || fDot <= -120)
						{
							m_iAttackIndex = ATK_TAIL;
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						}
						else
						{
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
						}
					}
				}
				else
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 2;
					if (0 == iRan)
					{
						// 4분의 1 확률로 앞, 뒤, 좌우 결정
						iRan = rand() % 8;
						if (0 == iRan || 4 == iRan)
							m_iAttackPattern = PAT_MOVEBACK;
						else if (1 == iRan || 5 == iRan)
							m_iAttackPattern = PAT_MOVELEFT;
						else if (2 == iRan || 6 == iRan)
							m_iAttackPattern = PAT_MOVERIGHT;
						else if (3 == iRan || 7 == iRan)
							m_iAttackPattern = PAT_MOVEFORWARD;

						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					else if (1 == iRan)
					{
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
					}
				}
			}
		}


		if (100000 == m_iAttackPattern)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 2;
			if (0 == iRan)
			{
				// 4분의 1 확률로 앞, 뒤, 좌우 결정
				iRan = _pGameInstance->Get_RandomCount() % 8;
				if (0 == iRan || 4 == iRan)
					m_iAttackPattern = PAT_MOVEBACK;
				else if (1 == iRan || 5 == iRan)
					m_iAttackPattern = PAT_MOVELEFT;
				else if (2 == iRan || 6 == iRan)
					m_iAttackPattern = PAT_MOVERIGHT;
				else if (3 == iRan || 7 == iRan)
					m_iAttackPattern = PAT_MOVEFORWARD;

				m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
			}
			else if (1 == iRan)
			{
				m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
			}
		}
	}
}

void CAncientHannibal::Next_Dogde(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	_float fDot = PlayerToDot_DirToSelfLookConvert();
	if (fDot <= 15.f && fDot >= -15.f)
	{
		if (PAT_NORMALDOGDE == m_iAttackPattern)
		{
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
		else if (PAT_DOUBLEDOGDEAFTERSNAKE == m_iAttackPattern)
		{
			m_pAncHannibalStateMachine->Set_DoubleDogde(true);
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
		}
		else if (PAT_DOGDEANDSNAKE == m_iAttackPattern)
		{
			m_iAttackIndex = ATK_FIRSTORM;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else if (PAT_DOGDEANDJUMPATTACK == m_iAttackPattern)
		{
			m_iAttackIndex = ATK_JUMP;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else if (PAT_DOGDESHOOT == m_iAttackPattern)
		{
			m_iAttackIndex = ATK_POINTSHOOT;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else if (PAT_DOGDEANDWEAPONCOMBO == m_iAttackPattern)
		{
			m_iAttackIndex = ATK_WEAPONCOMBO1;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else
		{
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
		}
	}
	else
	{
		m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RETARGET, this);
	}
}

void CAncientHannibal::NextMove(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (PAT_MOVEFORWARD != m_iAttackPattern && PAT_MOVERIGHT != m_iAttackPattern&&
		PAT_MOVELEFT != m_iAttackPattern&& PAT_MOVEBACK != m_iAttackPattern&&
		PAT_JUSTMOVEBACK != m_iAttackPattern)
	{
		_uint iRan = _pGameInstance->Get_RandomCount() % 4;
		if (0 == iRan)
			m_iAttackIndex = PAT_MOVEBACK;
		else if (1 == iRan)
			m_iAttackIndex = PAT_MOVELEFT;
		else if (2 == iRan )
			m_iAttackIndex = PAT_MOVERIGHT;
		else if (3 == iRan)
			m_iAttackIndex = PAT_MOVEFORWARD;
	}
	if (100000 == m_iAttackPattern)
	{
		_uint iRan = _pGameInstance->Get_RandomCount() % 2;
		if (0 == iRan)
		{
			// 4분의 1 확률로 앞, 뒤, 좌우 결정
			iRan = _pGameInstance->Get_RandomCount() % 8;
			if (0 == iRan || 4 == iRan)
				m_iAttackPattern = PAT_MOVEBACK;
			else if (1 == iRan || 5 == iRan)
				m_iAttackPattern = PAT_MOVELEFT;
			else if (2 == iRan || 6 == iRan)
				m_iAttackPattern = PAT_MOVERIGHT;
			else if (3 == iRan || 7 == iRan)
				m_iAttackPattern = PAT_MOVEFORWARD;

			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
		}
		else if (1 == iRan)
		{
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
		}
	}

	_float fDistance = PlayerToDistance();
	_float fDot = PlayerToDot_DirToSelfLookConvert();

	CWeapon* pWeapon = static_cast<CWeapon*>(m_pTargetPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
	if (nullptr != pWeapon)
	{
		if (CWeapon::WEAPON_GUN == pWeapon->Get_WeaponType())
		{
			// 공격 상태 일때
			if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
				|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
			{
				_float fPlayerToDot = SelfToDot_DirToPlayerLookConvert();

				if (fPlayerToDot <= 30.f && fPlayerToDot >= -30.f)
				{
					m_iAttackPattern = PAT_JUSTMOVEBACK;
				}
			}
		}
	}		
	// 플레이어가 공격 상황일경우
	if (CPlayerStateMachine::STATE_ATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
		|| CPlayerStateMachine::STATE_SPATTACK == m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
	{	
		// 거리가 20보다 클 때
		if (fDistance > 20.f)
		{
			// HalfTurn Attack
			m_iAttackIndex = ATK_FIRSTORM;
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else
		// 거리가 가까울 때
		if (fDistance <= 7.f)
		{			// 공격 상태 일때
				// 플레이어가 앞에 있을 때
			if (fDot <= 12.f && fDot >= -12.f)
			{
				///  6분의 1 확률로 각 행동들을 결정
				_uint iRan = _pGameInstance->Get_RandomCount() % 14;

				// 3분의 1
				if (0 == iRan || 1 == iRan || 2 == iRan)
				{
					// 랜덤 카운트 다시 발생
					iRan = _pGameInstance->Get_RandomCount() % 10;

					// 절반의 확률로 JumpAttack
					if (0 <= iRan && iRan <= 4)
						m_iAttackIndex = ATK_JUMP;
					// 절반의 확률로 Glid Normal
					else
					{
						iRan = _pGameInstance->Get_RandomCount() % 4;
						// 또 3분의 1은 특별 GILD 공격
						if (iRan == 3)
							m_iAttackIndex = ATK_GILD_SP;
						// 아니라면 그냥 노멀한 공격
						else
							m_iAttackIndex = ATK_GILD_N;
					}
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				// 3, 4일때
				else if (3 == iRan || 4 == iRan)
				{
					// Hook 공격
					m_iAttackIndex = ATK_HOOK;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				// 5, 6
				else if (5 == iRan || 6 == iRan)
				{
					// HalfTurn 후 공격
					m_iAttackIndex = ATK_HALFTURN;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				// 7, 8
				else if (7 == iRan || 8 == iRan)
				{
					// 뒤로 물린 후 백 공격
					m_iAttackPattern = PAT_BACKANDSHOOT;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
				// 9, 10
				else if (9 == iRan || 10 == iRan)
				{
					// 뒤로 물린 후 스네이크
					m_iAttackPattern = PAT_DOGDEANDSNAKE;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
				// 11, 12
				else if (11 == iRan || 12 == iRan)
				{
					// 뒤로 물린 후 스네이크
					m_iAttackPattern = PAT_DOGDESHOOT;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
				// 마지막 확률
				else if (13 == iRan)
				{
					// 연속 공격
					m_iAttackIndex = ATK_WEAPONCOMBO1;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
			// 플레이어가 뒤에 있을 때
			else
			{
				_uint iRan = _pGameInstance->Get_RandomCount() % 3;
				if (0 == iRan)
				{
					// 0 꼬리 공격
					m_iAttackIndex = ATK_TAIL;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				else if (1 == iRan)
				{
					// 1 하프 턴 공격
					m_iAttackIndex = ATK_HALFTURN;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				else if (2 == iRan)
				{
					// 뒤로 물러 선 뒤 점프 공격 아니면 콤보 공격
					iRan = _pGameInstance->Get_RandomCount() % 2;
					if (0 == iRan)
						m_iAttackPattern = PAT_DOGDEANDJUMPATTACK;
					else if (1 == iRan)
						m_iAttackPattern = PAT_DOGDEANDWEAPONCOMBO;

					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_DODGE, this);
				}
			}
		}
	}
	if (PAT_JUSTMOVEBACK == m_iAttackPattern)
	{
		if (CPlayerStateMachine::STATE_ATTACK != m_pTargetPlayer->Get_PlayerStateMachine()->Get_State()
			&& CPlayerStateMachine::STATE_SPATTACK != m_pTargetPlayer->Get_PlayerStateMachine()->Get_State())
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 8;
			if (0 == iRan || 1 == iRan)
				m_iAttackIndex = PAT_MOVEBACK;
			else if (2 == iRan || 3 == iRan)
				m_iAttackIndex = PAT_MOVELEFT;
			else if (4 == iRan || 5 == iRan)
				m_iAttackIndex = PAT_MOVERIGHT;
			else if (6 == iRan || 7 == iRan)
				m_iAttackIndex = PAT_MOVEFORWARD;
		}
		else
		{
			m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed);
		}
	}
	else
	{
		// 플레이어를 바라보고 있을 때
		if (RotateLookToPlayer(_dTimeDelta, 5.f))
		{
			m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
			m_pAncHannibalStateMachine->Set_PlayerFindIdleState(false);
			m_pAncHannibalStateMachine->Set_PlayerNotForward_MoveStation(false);

			// 앞으로 가야하는 패턴
			if (PAT_MOVEFORWARD == m_iAttackPattern)
			{
				// 현재 애니메이션이 아닐 경우 체인지
				if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_Walk_F_Loop"))
				{
					m_eDirection = DIR_FOR;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
				}
				// 거리가 가까우면 공격
				if (fDistance <= 5.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 6;
					if (0 == iRan)
						m_iAttackIndex = ATK_SLASH;
					else if (1 == iRan)
						m_iAttackIndex = ATK_ONEHAND;
					else if (2 == iRan)
						m_iAttackIndex = ATK_HOOK;
					else if (3 == iRan)
						m_iAttackIndex = ATK_HALFTURN;
					else if (4 == iRan)
						m_iAttackIndex = ATK_WEAPONCOMBO1;
					else if (5 == iRan)
						m_iAttackIndex = ATK_STAMP;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				else
				{
					// 아니라면 계속 움직여라
					m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
				}
			}
			else if (PAT_MOVELEFT == m_iAttackPattern || PAT_MOVERIGHT == m_iAttackPattern)
			{
				if (fDistance >= 5.f)
				{
					if (true == m_tMoveTimer.Is_Over(_dTimeDelta))
					{
						_uint iRan = _pGameInstance->Get_RandomCount() % 6;
						if (0 == iRan)
							m_iAttackIndex = ATK_SLASH;
						else if (1 == iRan)
							m_iAttackIndex = ATK_ONEHAND;
						else if (2 == iRan)
							m_iAttackIndex = ATK_HOOK;
						else if (3 == iRan)
							m_iAttackIndex = ATK_HALFTURN;
						else if (4 == iRan)
							m_iAttackIndex = ATK_WEAPONCOMBO1;
						else if (5 == iRan)
							m_iAttackIndex = ATK_STAMP;

						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
						m_tMoveTimer.fStandardTime = (_float)(_pGameInstance->Get_RandomCount() % 5 + 3);
						m_tMoveTimer.Reset_Timer();
					}
					else
					{
						if (PAT_MOVELEFT == m_iAttackPattern)
						{
							if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_Walk_L_Loop"))
							{
								m_eDirection = DIR_LEFT;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
							}
							m_pTransformCom->Move_Left(_dTimeDelta, m_fCurSpeed / 2.f);
						}
						else if (PAT_MOVERIGHT == m_iAttackPattern)
						{
							if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_Walk_R_Loop"))
							{
								m_eDirection = DIR_RIGHT;
								m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
							}
							m_pTransformCom->Move_Right(_dTimeDelta, m_fCurSpeed / 2.f);
						}

						if (fDistance >= 8.f)
						{
							m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_RUN, this);
						}
					}
				}
				else
				{
					if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_Walk_F_Loop"))
					{
						m_eDirection = DIR_FOR;
						m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
					}
					m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed);
				}
			}
			else if (PAT_MOVEBACK == m_iAttackPattern)
			{
				if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_Walk_F_Loop"))
				{
					m_eDirection = DIR_FOR;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
				}
				if (fDistance >= 8.f)
				{
					_uint iRan = _pGameInstance->Get_RandomCount() % 4;
					if (0 == iRan)
						m_iAttackIndex = ATK_JUMP;
					else if (1 == iRan)
						m_iAttackIndex = ATK_GILD_N;
					else if (2 == iRan)
						m_iAttackIndex = ATK_POINTSHOOT;
					else if (3 == iRan)
						m_iAttackIndex = ATK_GILD_SP;

					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed);
			}
			else if(PAT_BACKANDSHOOT == m_iAttackPattern)
			{
				if (fDistance >= 8.f)
				{
					m_iAttackIndex = ATK_POINTSHOOT;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
				m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed);
			}
			m_eDirection = DIR_END;
		}
		// 아닌 상황일 때
		else
		{
			// 현재 플레이어가 가깝고 뒤에 있을 경우 
			if (fDot >= 150.f || fDot <= -150.f)
			{
				if (fDistance <= 6.f)
				{
					m_iAttackIndex = ATK_TAIL;
					m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
				}
			}
			if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_IdleTurn_L0")
				&& strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_IdleTurn_R0"))
			{
				m_pAncHannibalStateMachine->Set_PlayerNotForward_MoveStation(true);
				m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);
			}
		}
	}
}

void CAncientHannibal::NextRun(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	_float fDistance = PlayerToDistance();
	_float fDot = PlayerToDot_DirToSelfLookConvert();

	// 거리가 20보다 클 때
	if (fDistance > 20.f)
	{
		// HalfTurn Attack
		m_iAttackIndex = ATK_FIRSTORM;
		m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
	}
	else
	if (true == RotateLookToPlayer(_dTimeDelta, 5))
	{
		if (fDistance <= 5.f)
		{
			_uint iRan = _pGameInstance->Get_RandomCount() % 5;
			if (0 == iRan)
				m_iAttackIndex = ATK_SLASH;
			else if (1 == iRan)
				m_iAttackIndex = ATK_ONEHAND;
			else if (2 == iRan)
				m_iAttackIndex = ATK_HOOK;
			else if (3 == iRan)
				m_iAttackIndex = ATK_HALFTURN;
			else if (4 == iRan)
				m_iAttackIndex = ATK_WEAPONCOMBO1;

			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_ATTACK, this);
		}
		else
		{
			m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
			m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 2.5f);

			m_pAncHannibalStateMachine->Set_PlayerFindIdleState(false);
			m_pAncHannibalStateMachine->Set_PlayerNotForward_MoveStation(false);
		}
	}
	else
	{
		if (strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_IdleTurn_L0")
			&& strcmp(m_pModelCom->Get_CurAnimation()->Get_Name().c_str(), "TADrago_TCmn_IdleTurn_R0"))
		{
			m_pAncHannibalStateMachine->Set_PlayerNotForward_MoveStation(true);
			m_pAncHannibalStateMachine->Change_State(CMonsterState::STATE_MOVE, this);

		}
	}
}

#pragma endregion NEXTSTATE

#pragma region USE_TOOL_METHOD
_bool CAncientHannibal::Extern_EditValue()
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

_bool CAncientHannibal::Extern_HierakeyView(const string& _str)
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
		_bool bClone = ImGui::Button("Clone");
		if (true == bClone)
		{
			BEGIN_SINGLETON(CGameInstance, pGameInstance);
			{
				CAncientHannibal::ANCHANNIBALDESC tDesc;
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

HRESULT CAncientHannibal::Create_Self(CGameObject** _ppOut)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CAncientHannibal::ANCHANNIBALDESC tDesc;
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

HRESULT CAncientHannibal::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CAncientHannibal::Save_Data()
{
	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		CAncientHannibal::ANCHANNIBALDESC tDesc;
		tDesc.eInitAnimType = m_eInitIdleType;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		pDataManager->Get_MonsterMgr()->Insert_AncHannibal(tDesc);
	}
	END_SINGLETON(pDataManager);
	return S_OK;
}

HRESULT CAncientHannibal::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
			, Get_Proto(), _pData));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

_bool CAncientHannibal::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

void CAncientHannibal::Reset_Data()
{
	__super::Reset_Data();
}

void CAncientHannibal::Extern_InitAnation()
{
}

void CAncientHannibal::Control_ColliderData()
{
	__super::Control_ColliderData();
}

#pragma endregion USE_TOOL_METHOD
CAncientHannibal* CAncientHannibal::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CAncientHannibal* pInstance = new CAncientHannibal(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAncientHannibal::Create To Failed");
	}
	return pInstance;
}

CGameObject* CAncientHannibal::Clone(void* pArg)
{
	CAncientHannibal* pInstance = new CAncientHannibal(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAncientHannibal::Clone To Failed");
	}
	return pInstance;
}

void CAncientHannibal::Free()
{
	Safe_Release(m_pAncHannibalStateMachine);
	Safe_Release(m_pAncEpController);
	Safe_Release(m_pPotal);
	__super::Free();
}
