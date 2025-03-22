#include "Projectile.h"
#include "GameInstance.h"

CProjectile::CProjectile(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	:CPawn(_pDevice, _pContext)
{
}

CProjectile::CProjectile(const CProjectile& _rhs)
	: CPawn(_rhs)
{
}

HRESULT CProjectile::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::NativeConstruct_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	// 투사체 구조체를 채운다.
	PROJECTILEDESC* pProjectileDesc = static_cast<PROJECTILEDESC*>(pArg);

	// 주인객체를 채운다.
	if (nullptr != pProjectileDesc->pOwner)
	{
		m_tProjectileDesc.pOwner = pProjectileDesc->pOwner;
		Safe_AddRef(m_tProjectileDesc.pOwner);
	}

	// 타겟객체를 채운다.
	if (nullptr != pProjectileDesc->pTarget)
	{
		m_tProjectileDesc.pTarget = pProjectileDesc->pTarget;
		Safe_AddRef(m_tProjectileDesc.pTarget);
	}

	// 네비게이션 컴포넌트를 채운다.
	if (nullptr != pProjectileDesc->pNavigation)
	{
		m_tProjectileDesc.pNavigation = pProjectileDesc->pNavigation;
		Safe_AddRef(m_tProjectileDesc.pNavigation);
	}

	// 타겟의 콜라이더를 채운다.
	if (nullptr != pProjectileDesc->pTargetCollider)
	{
		m_tProjectileDesc.pTargetCollider = pProjectileDesc->pTargetCollider;
		Safe_AddRef(m_tProjectileDesc.pTargetCollider);
	}

	m_tProjectileDesc.fDamage = pProjectileDesc->fDamage;
	m_tProjectileDesc.fLifeTime = pProjectileDesc->fLifeTime;
	m_tProjectileDesc.fSpeed = pProjectileDesc->fSpeed;
	m_tProjectileDesc.vCreatePos = pProjectileDesc->vCreatePos;
	m_tProjectileDesc.vMoveDirection = pProjectileDesc->vMoveDirection;

	// 위치와 바라볼 각도를 설정한다.
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tProjectileDesc.vCreatePos));	
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMLoadFloat3(&m_tProjectileDesc.vMoveDirection));
	}		

	return S_OK;
}

void CProjectile::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	// 이동한다
	Move(_dTimeDelta);

	if(nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	Check_Collision();

	// 시간이 다되면 삭제한다.
	Running_Timer(_dTimeDelta);
}

HRESULT CProjectile::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 나 자신을 삭제
	pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_PROJECTILE, this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CProjectile::Move(const _double& _dTimeDelta)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Move_Forward(_dTimeDelta, m_tProjectileDesc.fSpeed);
}

void CProjectile::Running_Timer(const _double& _dTimeDelta)
{	
	// 라이프 타임을 설정하지 않은 객체 ( 계속 살아있게한다 조건이 되면 삭제 )
	if (m_tProjectileDesc.fLifeTime == 0.f)
		return;

	// 라이프 타임이 다되면 삭제한다.
	if (m_tProjectileDesc.fLifeTime <= m_dTimeAcc)
		Delete_Self();
	else
		m_dTimeAcc += _dTimeDelta;
}

void CProjectile::Check_Collision()
{
}

CProjectile* CProjectile::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CProjectile* pInstance = new CProjectile(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CProjectile::Clone(void* pArg)
{
	CProjectile* pInstance = new CProjectile(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Clone To Failed");
#endif
	}
	return pInstance;
}

void CProjectile::Free(void)
{
	Safe_Release(m_tProjectileDesc.pOwner);
	Safe_Release(m_tProjectileDesc.pTarget);
	Safe_Release(m_tProjectileDesc.pNavigation);
	Safe_Release(m_tProjectileDesc.pTargetCollider);
	Safe_Release(m_pColliderCom);
	//Safe_Release(m_pMainEffect);

	__super::Free();
}
