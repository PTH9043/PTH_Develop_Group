#include "Oni.h"
#include "GameInstance.h"
#include "Controller.h"
#include "Animator.h"
#include "Battle_Manager.h"
#include "Camera_Free.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "Player.h"

#include "MainView.h"
#include "ImGuiClass_Manager.h"

COni::COni(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

COni::COni(const COni& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT COni::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eCharType = CHAR_TYPE::ONI;
	m_bWrapSampler = true;
	return S_OK;
}

HRESULT COni::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_fMoveSpeed = 30.f;
	m_fRotateSpeed = XMConvertToRadians(90.f) * 3.f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTargetObject = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0));
	RELEASE_INSTANCE(CGameInstance);

	m_iOniState = ONISTATE_IDLE;
	m_bActive = false;

	return S_OK;
}

void COni::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Tick(_dTimeDelta);
	}

	Give_State(_dTimeDelta);
	Animation_Control_State(_dTimeDelta);
	Behavior_State(_dTimeDelta);

	__super::Tick(_dTimeDelta);
}

void COni::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pController)
		m_pController->Late_Tick(_dTimeDelta);

	// 오니 충돌함수 
	Collision_Oni(_dTimeDelta);
}

HRESULT COni::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pHitColliderCom->Render_Debug();
#endif // _DEBUG

	return S_OK;
}

HRESULT COni::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT COni::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	// 오니 피격, 오니끼리 충돌 콜라이더 
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 5.f, 5.f, 5.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 5.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_SPHERE, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	// 플레이어가 가져가서 쓰는 히트 콜라이더 
	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 2.5f, 4.f, 1.5f };
	ColliderDesc.vTranslation = _float3{ 0.f, 5.f, 0.f };
	if (FAILED(__super::Add_Component(COM_COLLIDER_HIT, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
		return E_FAIL;

	// Recognize / Battle 상태 정하는 콜라이더 
	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 35.f, 35.f, 35.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_LIMIT, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pAttackColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void COni::Give_State(const _double& _dTimeDelta)
{
	if (ONISTATE_IDLE == m_iOniState || ONISTATE_RECOGNIZE == m_iOniState)
	{
		// 플레이어가 배틀상태 콜라이더에 닿고, 애니메이터타입이 등장이 아닐때 -> 배틀 
		CCollider* pPlayerCollider = static_cast<CCollider*>(m_pTargetObject->Get_Component(COM_COLLIDER_HIT));
		if (m_pAttackColliderCom->Is_Collision(pPlayerCollider) && "JUMP" != m_pAnimator->Get_AnimationType())
		{
			m_iOniState = ONISTATE_BATTLE;
		}
		else if (m_bActive)
		{
			m_iOniState = ONISTATE_RECOGNIZE;
		}
		else
			m_iOniState = ONISTATE_IDLE;
	}

	// 각각의 상태 안에서 실제 행동할 상태를 지정해줄 함수 
	switch (m_iOniState % ONISTATE_JUDGE)
	{
	case ONISTATE_IDLE:
		Idle_Oni(_dTimeDelta);
		break;
	case ONISTATE_RECOGNIZE:
		Recognize_Oni(_dTimeDelta);
		break;
	case ONISTATE_BATTLE:
		Battle_Oni(_dTimeDelta);
		break;
	default:
		break;
	}
}

void COni::Animation_Control_State(const _double& _dTimeDelta)
{
	switch (m_iOniState % ONISTATE_JUDGE)
	{
	case ONISTATE_IDLE:
		Idle_State(_dTimeDelta);
		break;
	case ONISTATE_RECOGNIZE:
		Recognize_State(_dTimeDelta);
		break;
	case ONISTATE_BATTLE:
		Battle_State(_dTimeDelta);
		break;
	case ONISTATE_DEAD:
	{
	}
	break;
	default:
		break;
	}
}

void COni::Behavior_State(const _double& _dTimeDelta)
{
	m_vEdgeGlowColor = _float4(1.f, 0.f, 0.f, m_fEdge);

	VOIDDATAS data;
	// 죽음 상태일때, 지정한 동작에서 삭제되도록
	{
		if ("DOWN" == m_pAnimator->Get_AnimationType())
		{
			Down_State(_dTimeDelta, data);

			if (true == m_tBattleData.Is_Die())
			{
				if (nullptr != m_pTargetObject)
					m_pTargetObject->Set_TargetTransform(nullptr);

				if (m_tDeleteTimer.Is_Over(_dTimeDelta))
				{
					Delete_Self();
					m_tBattleData.bBattleStop = true;
				}
			}
		}
		else if ("AIR_HIT" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Air_Hit_State(_dTimeDelta, data);
		}
		else if ("BOUND" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Bound_State(_dTimeDelta, data);
		}
		else if ("SPIN" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Spin_State(_dTimeDelta, data);
		}
		else if ("BLOW" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Air_Down_State(_dTimeDelta, data);
		}
		else if ("HIT" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Hit_State(_dTimeDelta, data);
		}
		else if ("AIR_DOWN" == m_pAnimator->Get_AnimationType() && true == m_pAnimator->Is_EventPlay())
		{
			Air_Down_State(_dTimeDelta, data);
		}
	}
}

void COni::Check_AttackCollision(CCharacter* _pOther)
{
	// 다운상태일떄 콜라이더 체크에 넣지 않는다.
	if ("DOWN" == _pOther->Get_Animator()->Get_AnimationType())
	{
		if ("DASH" == m_strAnimType || "AIR_DASH" == m_strAnimType)
			m_pAnimator->Input_Trigger("SUCCESS_DASH");

		return;
	}

	//if (true == m_bMSkillCheck)
	//	return;

	CCollider* pOtherHitCollider = static_cast<CCollider*>(_pOther->Get_Component(COM_COLLIDER_HIT));
	if (Is_Nullptr(pOtherHitCollider))
		return;
	{
		// 내가 때릴수 있고, 상대방이 맞을 수 있는지 확인        
		if (true == m_pAnimator->Is_AttackColliderActive() && false == ((COni*)_pOther)->m_bIsHit)
		{
			// 공격 성공 시 true
			if (m_pAnimAttackCollider->Is_Collision(pOtherHitCollider))
			{
				Success_AnimAttack(_pOther);

				m_bIsAttackSuccess = true;
			}
		}
		// 공격 콜라이더가 꺼져야 상대가 다시 맞을 수 있는 상태로 변경
		else if (false == m_pAnimator->Is_AttackColliderActive() && m_bIsAttackSuccess)
		{
			((COni*)_pOther)->m_bIsHit = false;
			m_bIsAttackSuccess = false;
		}
	}
}

void COni::Coll_Tick(_matrix _WorldMatrix)
{
	__super::Coll_Tick(_WorldMatrix);

	if (nullptr != m_pHitColliderCom)
		m_pHitColliderCom->Tick(_WorldMatrix);
}

#ifdef _DEBUG
void COni::Coll_Render(void)
{
	__super::Coll_Render();
}
#endif

void COni::Restrict_Nav(const _double& _dTimeDelta)
{
	// 밀치기 도중이면 적용하지 않는다.
	if (true == m_bIsHitPush)
		return;

	// Root본 애니메이션의 Root값을 받아서 이동합니다. ( 네비게이션에 막히면 이동하지 않습니다. )
	if (m_pAnimator->Is_Play())
	{
		if (true == m_pAnimator->Is_RootBone())
		{
			_matrix vRootBoneMatrix = m_pAnimator->Get_RootBoneMatrix();

			_vector vRootBonePos = CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS);

			// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
			//if (true == m_pNavigationCom->Is_Move(vRootBonePos))
			m_pTransformCom->Move_Pos_NotNavigateY(vRootBonePos);
		}
		// 애니메이션에서 넣어준 이동값이 있다면 이동한다.
		if (true == m_pAnimator->Is_BoneTransform() && true == m_pAnimator->Is_EventPlay())
		{
			_vector BoneTransformPos = m_pAnimator->Get_BoneTransformPos();

			_bool bIsIn = false;

			// 지상 공격이나 지상에서의 스텝 도중에는 네비게이션의 Y를 태운다.
			if ("ATTACK" == m_strAnimType || "STEP" == m_strAnimType)
				m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
			else if ("ATTACK_DOWN" == m_strAnimType)
			{
				_float fNaviHeight = m_pNavigationCom->Compute_Height(m_pTransformCom->Get_Pos());
				_vector vPos = m_pTransformCom->Get_Pos() + BoneTransformPos;

				if (XMVectorGetY(vPos) < fNaviHeight)
					vPos = XMVectorSetY(vPos, fNaviHeight);

				m_pTransformCom->Move_Pos_NotNavigateY(vPos);
			}
			// 그 외의 경우에는 네비게이션 Y를 태우지 않는다.
			else
				m_pTransformCom->Move_Pos_NotNavigateY(m_pTransformCom->Get_Pos() + BoneTransformPos);

			m_bNaviCheck = bIsIn;
		}
	}
	//// Root본 애니메이션의 Root값을 받아서 이동합니다. ( 네비게이션에 막히면 이동하지 않습니다. )
	//if (m_pAnimator->Is_Play())
	//{
	//	if (true == m_pAnimator->Is_RootBone())
	//	{
	//		_matrix vRootBoneMatrix = m_pAnimator->Get_RootBoneMatrix();

	//		_vector vRootBonePos = CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS);

	//		// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
	//		if (true == m_pNavigationCom->Is_Move(vRootBonePos))
	//			m_pTransformCom->Move_PosNotNavigation(vRootBonePos);
	//	}
	//	// 애니메이션에서 넣어준 이동값이 있다면 이동한다.
	//	if (true == m_pAnimator->Is_BoneTransform())
	//	{
	//		_vector BoneTransformPos = m_pAnimator->Get_BoneTransformPos();

	//		_bool bIsIn = false;

	//		// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
	//		bIsIn = m_pNavigationCom->Is_Move(m_pTransformCom->Get_Pos() + BoneTransformPos);


	//		if (true == bIsIn)
	//		{
	//			// Y값이 포함되어있거나 AIR_ATTACK , AIR_STEP 같은 경우의 애니메이션은 네비게이션 Y값을 태우지 않는다.
	//			if (0 != XMVectorGetY(BoneTransformPos) || "AIR_ATTACK" == m_strAnimType || "AIR_STEP" == m_strAnimType ||
	//				"AIR_DOWN" == m_strAnimType || "AIR_HIT" == m_strAnimType || "BOUND" == m_strAnimType || "JUMP" == m_strAnimType)
	//				m_pTransformCom->Move_PosNotNavigation(m_pTransformCom->Get_Pos() + BoneTransformPos);
	//			// 그 외의 경우에는 네비게이션 Y를 태운다.
	//			else
	//				m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
	//		}

	//	}
	//}
}

void COni::Collision_Oni(const _double& _dTimeDelta)
{
}

HRESULT COni::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);

	_uint iLayerNum = pBattleManager->Get_CurrentOniLayer();
	switch (iLayerNum)
	{
	case 1:
		pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_ONI1, this);
		break;
	case 2:
		pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_ONI2, this);
		break;
	case 3:
		pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_ONI3, this);
		break;
	default:
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CBattle_Manager);
	return S_OK;
}

void COni::Collision_Player(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* PlayerCollider = (m_pTargetObject->Get_BoneCollider())[1]->Get_Collider();
	if (m_pColliderCom->Is_Collision(PlayerCollider))
	{
		_vector vDir = m_pTransformCom->Get_Pos() - m_pTargetObject->Get_Transform()->Get_Pos();
		_float fLength = XMVectorGetX(XMVector3Length(vDir));

		_float fDistnace = fabs(m_pColliderCom->Get_Scale().x + PlayerCollider->Get_Scale().x - fLength);
		m_pTransformCom->Translate_Dir(XMVector3Normalize(vDir), 1.f, fDistnace);
	}

	RELEASE_INSTANCE(CGameInstance);
}

//_bool COni::Hit_SuperArmor(void)
//{
//	// 슈퍼아머가 적용되는 경우에 맞아도 경직을 받지 않고 데미지만 받는다.
//	if ("SKILL_1" == m_strAnimType || "SKILL_2" == m_strAnimType || "ULTIMATE" == m_strAnimType || "INIT" == m_strAnimType || "AWAKE" == m_strAnimType)
//		return true;
//
//	return false;
//}

void COni::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
	if (true == m_bMSkillCheck)
		return;

	__super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

	CImGuiClass_Manager* pInstance = GET_INSTANCE(CImGuiClass_Manager);
	{
		static_cast<CMainView*>(pInstance->Get_ImGuiClass(IMGCLASS_MAIN))->Add_Explain("Oni Damaged");
	}
	RELEASE_INSTANCE(CImGuiClass_Manager);

	Minus_OniHp();
}


void COni::Idle_Oni(const _double& _dTimeDelta)
{
}

void COni::Recognize_Oni(const _double& _dTimeDelta)
{
}

void COni::Battle_Oni(const _double& _dTimeDelta)
{
}

CGameObject* COni::Clone(void* pArg)
{
	return nullptr;
}

void COni::Free(void)
{
	Safe_Release(m_pHitColliderCom);

	__super::Free();
}
