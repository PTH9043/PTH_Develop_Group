#include "WindBall.h"
#include "Character.h"
#include "GameInstance.h"

CWindBall::CWindBall(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	:CProjectile(_pDevice, _pContext)
{
}

CWindBall::CWindBall(const CWindBall& _rhs)
	: CProjectile(_rhs)
{
}

HRESULT CWindBall::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWindBall::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 이펙트를 설정한다.
	m_pMainEffect = pGameInstance->Set_EffectTarget(TEXT("Aka_WindBall2"), m_pTransformCom);
	//Safe_AddRef(m_pMainEffect);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CWindBall::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CWindBall::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CWindBall::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWindBall::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWindBall::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	// 콜라이더 세팅 
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 5.f, 5.f, 5.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_SPHERE, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWindBall::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 나 자신을 삭제
	pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_PROJECTILE, this);

	if (nullptr != m_pMainEffect)
	{
		// 이펙트 객체를 삭제
		//pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECT, m_pMainEffect);
		//Safe_Release(m_pMainEffect);
		static_cast<CEffectObject*>(m_pMainEffect)->Set_Play(false);
		static_cast<CEffectObject*>(m_pMainEffect)->EffectObjectReset();
		m_pMainEffect->Set_Active(false);

		// 삭제 될때의 이펙트 생성
		pGameInstance->SetEffectStateCall(TEXT("Aka_WindBallHit"), m_pTransformCom->Get_WorldMatrix());
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CWindBall::Move(const _double& _dTimeDelta)
{
	__super::Move(_dTimeDelta);
}

void CWindBall::Running_Timer(const _double& _dTimeDelta)
{
	__super::Running_Timer(_dTimeDelta);
}

void CWindBall::Check_Collision()
{
	// 없는 객체가 있다면 return한다.
	if (nullptr == m_pColliderCom || nullptr == m_tProjectileDesc.pTargetCollider ||
		nullptr == m_tProjectileDesc.pTarget || nullptr == m_tProjectileDesc.pOwner)
		return;

	// 콜라이더 충돌을 체크한다.
	if (true == m_pColliderCom->Is_Collision(m_tProjectileDesc.pTargetCollider))
	{
		if (false == static_cast<CCharacter*>(m_tProjectileDesc.pOwner)->Check_Enemy_State(static_cast<CCharacter*>(m_tProjectileDesc.pTarget)))
			return;

		static_cast<CCharacter*>(m_tProjectileDesc.pTarget)->Hit_ByProjectile(0.f, 0.f, m_tProjectileDesc.fDamage, 0.1f, static_cast<CCharacter*>(m_tProjectileDesc.pOwner));
		// 삭제한다.
		Delete_Self();
	}
}

CWindBall* CWindBall::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CWindBall* pInstance = new CWindBall(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CWindBall::Clone(void* pArg)
{
	CWindBall* pInstance = new CWindBall(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Clone To Failed");
#endif
	}
	return pInstance;
}

void CWindBall::Free(void)
{
	__super::Free();
}
