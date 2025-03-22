#include "AnimMeshEffect.h"
#include "GameInstance.h"

CAnimMeshEffect::CAnimMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CEffect(_pDevice, _pContext)
{
}

CAnimMeshEffect::CAnimMeshEffect(const CEffect& _rhs)
	:CEffect(_rhs)
{
}

HRESULT CAnimMeshEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAnimMeshEffect::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_eEffectType = EFFECT_ANIMMESH;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		memcpy(&m_tEffectDesc, (EFFECTDESC*)_pArg, sizeof(EFFECTDESC));
		m_pTransformCom->Set_Scale(m_tEffectDesc.vScl);
		m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));
	}

	return S_OK;
}

void CAnimMeshEffect::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pVIBufferCom_Model)
		return;

	if (m_pParent == nullptr)
		return;

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || m_bDisable)
		return;

   	m_pVIBufferCom_Model->Update_Animation(_dTimeDelta);
}

void CAnimMeshEffect::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (!m_pParent->Is_Active())
		return;

	if (CEffectObject::PROJECT_CLIENT == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!m_bEnable || m_bDisable)
			return;
	}

	m_pRendererCom->Add_RenderGroup((CRenderer::RENDERID)(m_tEffectDesc.iRenderID + 3), this);
}

HRESULT CAnimMeshEffect::Render()
{
	if (nullptr == m_pVIBufferCom_Model)
		return E_FAIL;

	if (!m_pParent->Is_Active())
		return S_OK;

	if (!m_bEnable || m_bDisable)
		return S_OK;

 	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshContainers = m_pVIBufferCom_Model->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pVIBufferCom_Model->Effect_Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
			return E_FAIL;

		m_pVIBufferCom_Model->Render(i, m_pShaderCom, m_tEffectDesc.iPassIndex);
	}

	return S_OK;
}

HRESULT CAnimMeshEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(TEXT("Com_Renderer"), 0, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), 0, TEXT("Prototype_Component_Shader_VTXANIMMODEL"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimMeshEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrix;
	WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CAnimMeshEffect::Set_Texture(CTexture* pTexture)
{
	//m_pTextureCom = (CTexture*)pTexture->Clone(nullptr);
	m_iTextureIndex = 0;

	return S_OK;
}

HRESULT CAnimMeshEffect::Set_Model(CModel* pModel, _uint iIndex)
{
	if (nullptr != m_pVIBufferCom_Model)
		Safe_Release(m_pVIBufferCom_Model);

	m_pVIBufferCom_Model = pModel;
	if (nullptr == m_pVIBufferCom_Model)
		return E_FAIL;

	// 툴에서 선택한 리소스의 Index
	m_iMeshIndex = iIndex;

	return S_OK;
}

CAnimMeshEffect* CAnimMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimMeshEffect* pInstance = new CAnimMeshEffect(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Create Failed : CAnimMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect* CAnimMeshEffect::Clone(void* _pArg)
{
	CAnimMeshEffect* pInstance = new CAnimMeshEffect(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Cloned Failed : CAnimMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimMeshEffect::Free()
{
	__super::Free();
}
