#include "CEffectParticleGroup.h"
#include "CGameInstance.h"
#include "C2DEffectParticle.h"
#include "CParticle.h"
#include "CGameInstance.h"

CEffectParticleGroup::CEffectParticleGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
	: CGameObject(_pDevice, _pContext)
{
	m_tEffectParticleGroup = *static_cast<EFFECTPARTICLEGROUP*>(_pArg);
}

CEffectParticleGroup::CEffectParticleGroup(const CEffectParticleGroup& _rhs)
	: CGameObject(_rhs), m_tEffectParticleGroup(_rhs.m_tEffectParticleGroup)
{
}

void CEffectParticleGroup::Set_Pos(_vector vPos)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Pos(vPos);

}

void CEffectParticleGroup::Set_Look(_fvector vLook)
{
	{
		if (nullptr != m_p2DEffectParticle)
		{
			CTransform* pTransform = static_cast<CTransform*>(m_p2DEffectParticle->Get_Component(COM_TRANSFORM));
			pTransform->LookAt(pTransform->Get_Pos() + vLook);
		}
	}
	{
		if (nullptr != m_pParticle)
		{
			CTransform* pTransform = static_cast<CTransform*>(m_pParticle->Get_Component(COM_TRANSFORM));
			pTransform->LookAt(pTransform->Get_Pos() + vLook);
		}
	}
}

void CEffectParticleGroup::Turn_Off_LifeLoop()
{
	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Turn_Off_LifeLoop();
	if (nullptr != m_pParticle)
		m_pParticle->Turn_Off_LifeLoop();
}

void CEffectParticleGroup::Turn_On_LifeLoop()
{
	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Turn_On_LifeLoop();
	if (nullptr != m_pParticle)
		m_pParticle->Turn_On_LifeLoop();
}

void CEffectParticleGroup::Turn_OffLoop(const _float& _fLifeTime)
{
	if (nullptr != m_p2DEffectParticle)
	{
		m_p2DEffectParticle->Set_Loop(false);
		m_p2DEffectParticle->Set_LifeTime(_fLifeTime);
	}
	if (nullptr != m_pParticle)
	{
		m_pParticle->Set_Loop(false);
		m_pParticle->Set_LifeTime(_fLifeTime);
	}
}

void CEffectParticleGroup::Turn_OnLoop()
{
	if (nullptr != m_p2DEffectParticle)
	{
		m_p2DEffectParticle->Set_Loop(true);
	}
	if (nullptr != m_pParticle)
	{
		m_pParticle->Set_Loop(true);
	}
}

void CEffectParticleGroup::Not_Reset(const _bool& _bIsNotReset)
{
	if (nullptr != m_p2DEffectParticle)
	{
		m_p2DEffectParticle->Set_NotReset(_bIsNotReset);
	}
	if (nullptr != m_pParticle)
	{
		m_pParticle->Set_NotReset(_bIsNotReset);
	}
}

HRESULT CEffectParticleGroup::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEffectParticleGroup::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(Ready_Comp()))
		return E_FAIL;

	if(lstrcmp(m_tEffectParticleGroup.str2DEffectPath.c_str(), L" "))
		m_p2DEffectParticle = C2DEffectParticle::Create_LoadFile(m_pDevice, m_pContext, m_tEffectParticleGroup.str2DEffectPath);
	if (lstrcmp(m_tEffectParticleGroup.strPatriclePath.c_str(), L" "))
		m_pParticle = CParticle::Create_LoadFile(m_pDevice, m_pContext, m_tEffectParticleGroup.strPatriclePath);


	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Set_OwnerTransform(m_pTransformCom);

	if (nullptr != m_pParticle)
		m_pParticle->Set_OwnerTransform(m_pTransformCom);


	if (nullptr != pArg)
	{
		EFFECTPARTICLEGROUPDESC* pEffectParticleGroupDesc = static_cast<EFFECTPARTICLEGROUPDESC*>(pArg);
		m_pOwnerTransform = pEffectParticleGroupDesc->pTransform;
		Safe_AddRef(m_pOwnerTransform);

		m_v3SupplementPos = pEffectParticleGroupDesc->v3SupplementPos;
	}
	return S_OK;
}

void CEffectParticleGroup::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pOwnerTransform)
	{
		m_pTransformCom->Set_Pos(m_pOwnerTransform->Get_Pos());
		m_pTransformCom->Set_Scale(m_pOwnerTransform->Get_Scale());
	}

	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Tick(_dTimeDelta);

	if(nullptr != m_pParticle)
		m_pParticle->Tick(_dTimeDelta);
}

void CEffectParticleGroup::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Late_Tick(_dTimeDelta);

	if (nullptr != m_pParticle)
		m_pParticle->Late_Tick(_dTimeDelta);
}

HRESULT CEffectParticleGroup::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffectParticleGroup::Active(const bool& _bTrue)
{
	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Set_Active(_bTrue);

	if (nullptr != m_pParticle)
		m_pParticle->Set_Active(_bTrue);

	if (nullptr != m_pOwnerTransform)
		m_pTransformCom->Set_Pos(m_pOwnerTransform->Get_Pos() + XMLoadFloat3(&m_v3SupplementPos));

}

void CEffectParticleGroup::Reset()
{
	if (nullptr != m_p2DEffectParticle)
		m_p2DEffectParticle->Reset();

	if (nullptr != m_pParticle)
		m_pParticle->Reset();
}

HRESULT CEffectParticleGroup::Ready_Comp()
{
	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC,
		PROTO_COMP_SHADER_PARTICLE, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC,
		PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CEffectParticleGroup* CEffectParticleGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* _pArg)
{
	CEffectParticleGroup* pInstance = new CEffectParticleGroup(pDevice, pContext, _pArg);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectParticleGroup::Create To Failed");
	}
	return pInstance;
}

CGameObject* CEffectParticleGroup::Clone(void* pArg)
{
	CEffectParticleGroup* pInstance = new CEffectParticleGroup(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectParticleGroup::Clone To Failed");
	}
	return pInstance;
}

void CEffectParticleGroup::Free()
{
	Safe_Release(m_p2DEffectParticle);
	Safe_Release(m_pParticle);
	Safe_Release(m_pOwnerTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	__super::Free();
}
