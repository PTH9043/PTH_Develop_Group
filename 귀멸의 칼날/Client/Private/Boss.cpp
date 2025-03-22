#include "Boss.h"
#include "GameInstance.h"
#include "Battle_Manager.h"

#include "AnimationClip.h"
#include "BonesCollider.h"

CBoss::CBoss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CBoss::CBoss(const CBoss& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CBoss::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eCharType = CHAR_TYPE::TEONI;
	return S_OK;
}

HRESULT CBoss::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_fMoveSpeed = 30.f;
	m_fRotateSpeed = XMConvertToRadians(90.f) * 3.f;

	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
	pBattleManager->Start_Battle(this, CBattle_Manager::CHAR_AI);
	RELEASE_INSTANCE(CBattle_Manager);

	return S_OK;
}

void CBoss::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pTargetObject)
	{
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
			m_pTargetObject = pBattleManager->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
			RELEASE_INSTANCE(CBattle_Manager);
		}
		else if (LEVEL_STORY == g_iCurrentLevel)
		{
			CGameInstance* pGaemInstance = GET_INSTANCE(CGameInstance);
			m_pTargetObject = static_cast<CCharacter*>(pGaemInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	if (nullptr == m_pTargetObject)
		return;


	// 애니메이터 
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Tick(_dTimeDelta);
	}

	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	// 상태 주기 
	Move_HitCheck(_dTimeDelta, pGameInstace); // IDLE -> 포효 
	Move_StepCheck(_dTimeDelta, pGameInstace); // IDEL -> 공격상태  or 스텝 
	Give_State(_dTimeDelta, pGameInstace); // IDEL -> 앞스텝 or 공격 / 공격상태 -> 공격 
	// 트리거 
	Animation_Control_State(_dTimeDelta, pGameInstace);
	// 애니메이션에 대한 이동
	Behavior_State(_dTimeDelta, pGameInstace);
	// 상태 초기화 
	Init_State(_dTimeDelta, pGameInstace);

	RELEASE_INSTANCE(CGameInstance);

	__super::Tick(_dTimeDelta);
}

void CBoss::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pTargetObject)
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CBoss::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	//  히트콜라이더 
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 25.f, 25.f, 25.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 15.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_HIT, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	// 공격범위 콜라이더 
	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 5.f, 5.f, 5.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_LIMIT, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pAttackColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CBoss::Coll_Tick(_matrix _WorldMatrix)
{
	__super::Coll_Tick(_WorldMatrix);
}

#ifdef _DEBUG
void CBoss::Coll_Render(void)
{
	__super::Coll_Render();
}
#endif

void CBoss::Check_AttackCollision(CCharacter* _pOther)
{
	// 다운상태일떄 콜라이더 체크에 넣지 않는다.
	if ("DOWN" == _pOther->Get_Animator()->Get_AnimationType())
	{
		if ("DASH" == m_strAnimType || "AIR_DASH" == m_strAnimType)
			m_pAnimator->Input_Trigger("SUCCESS_DASH");

		return;
	}

	CCollider* pOtherHitCollider = static_cast<CCollider*>(_pOther->Get_Component(COM_COLLIDER_HIT));
	if (Is_Nullptr(pOtherHitCollider))
		return;
	{

		CCollider* pRockCollider = (CCollider*)CGameInstance::Get_Instance()->Get_Component(g_iCurrentLevel, LAYER_PROJECTILE, COM_COLLIDER_SPHERE);
		if (nullptr != pRockCollider)
		{
			if (pRockCollider->Is_Collision(pOtherHitCollider))
			{
				_pOther->Hit_ByProjectile(10.f, 0.f, 5.f, 0.1f, this);
				return;
			}
		}


		// 내가 때릴수 있고, 상대방이 맞을 수 있는지 확인        
		if (true == m_pAnimator->Is_AttackColliderActive() && false == _pOther->Is_Hit())
		{
			// 공격 성공 시 true
			if (m_pAnimAttackCollider->Is_Collision(pOtherHitCollider))
			{
				Success_AnimAttack(_pOther);
			}

		}
		// 공격 콜라이더가 꺼져야 상대가 다시 맞을 수 있는 상태로 변경
		else if (false == m_pAnimator->Is_AttackColliderActive())
		{
			_pOther->Set_Hit(false);
		}
	}
}

void CBoss::Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter)
{
	__super::Hit_ByProjectile(_fPushForce, _fHeight, _fDamage, _fDelayTime, _pEnemyCharcter);
}

void CBoss::Restrict_Nav(const _double& _dTimeDelta)
{
	// Root본 애니메이션의 Root값을 받아서 이동합니다. ( 네비게이션에 막히면 이동하지 않습니다. )
	if (m_pAnimator->Is_Play())
	{
		if (true == m_pAnimator->Is_RootBone())
		{
			_matrix vRootBoneMatrix = m_pAnimator->Get_RootBoneMatrix();

			_vector vRootBonePos = CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS);

			// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
			if (true == m_pNavigationCom->Is_Move(vRootBonePos))
				m_pTransformCom->Move_PosNotNavigation(vRootBonePos);
		}
		// 애니메이션에서 넣어준 이동값이 있다면 이동한다.
		if (true == m_pAnimator->Is_BoneTransform())
		{
			_vector BoneTransformPos = m_pAnimator->Get_BoneTransformPos();

			_bool bIsIn = false;

			// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
			bIsIn = m_pNavigationCom->Is_Move(m_pTransformCom->Get_Pos() + BoneTransformPos);


			if (true == bIsIn)
			{
				// Y값이 포함되어있거나 AIR_ATTACK , AIR_STEP 같은 경우의 애니메이션은 네비게이션 Y값을 태우지 않는다.
				if (0 != XMVectorGetY(BoneTransformPos) || "AIR_ATTACK" == m_strAnimType || "AIR_STEP" == m_strAnimType ||
					"AIR_DOWN" == m_strAnimType || "AIR_HIT" == m_strAnimType || "BOUND" == m_strAnimType || "JUMP" == m_strAnimType)
					m_pTransformCom->Move_PosNotNavigation(m_pTransformCom->Get_Pos() + BoneTransformPos);
				// 그 외의 경우에는 네비게이션 Y를 태운다.
				else
					m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
			}

			m_bNaviCheck = bIsIn;
		}
	}
}


void CBoss::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
	/*__super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

	m_tBattleData.Decrease_HP(5.f);
	if (true == m_bIsHit)
		m_iHitCount += 1;*/

	// 맞았다는것을 체크해준다.
	m_iHitCount += 1;

	m_bIsHit = true;

	// 때린 대상의 애니메이터를 얻어온다.
	CAnimator* pEnemyAnimator = _pEnemyCharcter->Get_Animator();
	CTransform* pEnemyTransform = _pEnemyCharcter->Get_Transform();

	// Collider의 Dir값이 갖고있는 정보를 가져온다. ( x = 데미지 , y = 멈추는 시간, z = 아직 안씀( 추후에 추가 예정 ) )
	_vector vHitInformation = pEnemyAnimator->Get_ColliderDir();
	if(false == m_bGuard)
		m_tBattleData.Decrease_HP(XMVectorGetX(vHitInformation));
	else 
		m_tBattleData.Decrease_HP(XMVectorGetX(vHitInformation) / 3);

	// 인풋이 성공했는지 확인하는 불 변수
	_bool bIsInput = false;

	// 콜라이더의 타입
	_string strColliderType = "";
	_string strSuccessTrigger = "";

	if (nullptr != pEnemyAnimator->Get_CurrentAnimationClip())
		strColliderType = pEnemyAnimator->Get_CurrentAnimationClip()->Get_ColliderType();

	// 공격한 객체의 히트 카운트를 늘린다.
	_pEnemyCharcter->Get_BattleData().iHitCount += 1;
	// 콤보 타이머를 0으로 초기화한다.
	_pEnemyCharcter->Get_BattleData().tComboTimer.Reset_Timer();

	// 히트 이펙트를 생성한다.
	Hit_Effect(strColliderType, _pEnemyCharcter);

	Check_Hit_Type(strColliderType, _pEnemyCharcter);

	// 공격 타입이 뭔지 체크한다.
	_pEnemyCharcter->Check_Attack_Type(strColliderType);

	// 배틀매니저로 양쪽 캐릭터의 시간을 멈춘다.
	_pBattleManager->Set_DelayTime(0.1f);

}

void CBoss::Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	BONECOLLIDERS EnemyBoneCollider = _pEnemyCharcter->Get_BoneCollider();
	if (EnemyBoneCollider.size() < 2)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// 만약 1번 공격 콜라이더와 나의 Hit콜라이더가 안부딧히면 2번 공격 콜라이더 사용
	CBonesCollider* pAttackCollider = EnemyBoneCollider[1];
	CCollider* pMyHitCollider = (CCollider*)Get_Component(COM_COLLIDER_HIT);
	if (nullptr != pMyHitCollider)
	{
		if (false == pMyHitCollider->Is_Collision(pAttackCollider->Get_Collider()) && EnemyBoneCollider.size() >= 3)
		{
			pAttackCollider = EnemyBoneCollider[2];
		}
	}

	_float3 vAttackDir = pAttackCollider->Get_Direction();

	_float fSworldAngleXY = CMethod::Compute_Radian_XY_Plane(XMVectorSet(0.f, 1.f, 0.f, 0.f), vAttackDir);

	_matrix PlayerMatrix = m_pTransformCom->Get_WorldMatrix();
	PlayerMatrix.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);

	_matrix ScaleMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	_matrix RotationMatrix = XMMatrixRotationZ(fSworldAngleXY);

	_matrix WorldMatrix = ScaleMatrix * RotationMatrix * PlayerMatrix;

	if (("HIT_FRONT" == strColliderType) || ("HIT_FRONT_AIR" == strColliderType) || ("HIT_BACK" == strColliderType) ||
		("HIT_BACK_AIR" == strColliderType) || ("HIT_RIGHT" == strColliderType) || ("HIT_RIGHT_AIR" == strColliderType)
		|| ("HIT_LEFT" == strColliderType) || ("HIT_LEFT_AIR" == strColliderType))
	{
		_uint randomDmg = rand() % 3;
		if (0 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_1", WorldMatrix);
		else if (1 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_2", WorldMatrix);
		else if (2 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_3", WorldMatrix);

		if (true == m_bGuard && 2 < m_iHitCount)
		{
			pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
			m_iHitCount = 0;
		}
	}
	else if (("HIT_FRONT_S" == strColliderType) || ("HIT_BACK_S" == strColliderType) || ("HIT_S" == strColliderType)
		|| ("HIT_UPPER" == strColliderType) || ("STRIKE" == strColliderType) || ("HIT_DOWN" == strColliderType)
		|| ("BLOW" == strColliderType))
	{
		pGameInstance->SetEffectStateCall(L"SlashDmg2", WorldMatrix);
		pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
	}

	RELEASE_INSTANCE(CGameInstance);
}

CGameObject* CBoss::Clone(void* pArg)
{
	return nullptr;
}

void CBoss::Free(void)
{
	__super::Free();
}
