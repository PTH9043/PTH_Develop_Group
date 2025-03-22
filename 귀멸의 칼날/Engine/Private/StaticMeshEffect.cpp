#include "StaticMeshEffect.h"
#include "GameInstance.h"

CStaticMeshEffect::CStaticMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CEffect(_pDevice, _pContext)
{
	m_PrevWorldMatrix = CMethod::XMMatrixIdentity_P();
}

CStaticMeshEffect::CStaticMeshEffect(const CStaticMeshEffect& _rhs)
	: CEffect(_rhs)
{
	m_PrevWorldMatrix = CMethod::XMMatrixIdentity_P();
}

HRESULT CStaticMeshEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CStaticMeshEffect::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_eEffectType = EFFECT_STATICMESH;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		m_tEffectDesc = *static_cast<EFFECTDESC*>(_pArg);
		m_pTransformCom->Set_Scale(m_tEffectDesc.vScl);
		m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));
	}

	return S_OK;
}

void CStaticMeshEffect::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (m_pParent == nullptr)
		return;

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return;
	}

	m_fAuraTime += (_float)_dTimeDelta;
}

void CStaticMeshEffect::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	__super::Late_Tick(_dTimeDelta);

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return;
	}




	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	m_pRendererCom->Add_RenderGroup((CRenderer::RENDERID)(m_tEffectDesc.iRenderID + 3), this);
}

HRESULT CStaticMeshEffect::Render()
{
	if (nullptr == m_pVIBufferCom_Model)
		return E_FAIL;

	if (!m_pParent->Is_Active())
		return S_OK;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return S_OK;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return S_OK;
	}



	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshContainers = m_pVIBufferCom_Model->Get_NumMeshContainers();
	//iNumMeshContainers -= m_tEffectDesc.iTestMrlCount;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pVIBufferCom_Model->Effect_Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
		{
			if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
				return E_FAIL;
		}

		//m_pVIBufferCom_Model->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		if (m_tEffectDesc.bNoise)
		{
			if (FAILED(m_pTextureCom[TEXTURE_NOISE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_NoiseTexture", m_iNoiseIndex)))
				return E_FAIL;
		}

		if (m_tEffectDesc.bEnableMask)
		{
			if (0 > m_tEffectDesc.iMaskIndex || m_pTextureCom[TEXTURE_MASK]->Get_TextureSize() < (size_t)m_tEffectDesc.iMaskIndex)
				m_tEffectDesc.iMaskIndex = 0;

			if (FAILED(m_pTextureCom[TEXTURE_MASK]->SetUp_ConstantTable_Index(m_pShaderCom, "g_MaskTexture", m_tEffectDesc.iMaskIndex)))
				return E_FAIL;
		}

		m_pVIBufferCom_Model->Render_NonAnim(i, m_pShaderCom, m_tEffectDesc.iPassIndex);
	}

	return S_OK;
}

HRESULT CStaticMeshEffect::Set_Texture(CTexture* pTexture)
{
	//m_pTextureCom = (CTexture*)pTexture->Clone(nullptr);
	m_iTextureIndex = 0;

	return S_OK;
}

HRESULT CStaticMeshEffect::Set_Model(CModel* pModel, _uint iIndex)
{
	if (nullptr != m_pVIBufferCom_Model)
		Safe_Release(m_pVIBufferCom_Model);

	//m_pVIBufferCom_Model = pModel;
	m_pVIBufferCom_Model = pModel;
	if (nullptr == m_pVIBufferCom_Model)
		return E_FAIL;

	// 툴에서 선택한 리소스의 Index
	m_iMeshIndex = iIndex;

	return S_OK;
}

HRESULT CStaticMeshEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(TEXT("Com_Renderer"), 0, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), 0, TEXT("Prototype_Component_Shader_VTXMODEL_EFFECT"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticMeshEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrix;
	_float camFar = pGameInstance->Get_CamFar();
	WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tEffectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTime", &m_fAuraTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bCutBlack", &m_tEffectDesc.bCutBlack, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
		return E_FAIL;

	// UV
	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_START", &m_vUV_Start, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_END", &m_vUV_End, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_DELTA", &m_vUV_Delta, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bUVFlow", &m_tEffectDesc.bUVFlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vUVDir", &m_vUV_Dir, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vUV_Mul", &m_tEffectDesc.vUV_Mul, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSamplerMode", &m_tEffectDesc.iSamplerMode, sizeof(_int))))
		return E_FAIL;

	if (m_tEffectDesc.iPassIndex == 13)
	{
		if (FAILED(m_pShaderCom->Set_SRV("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TARGET_DEPTH))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_SRV("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TARGET_MAPOBJDEPTH))))
			return E_FAIL;
	}

	// Noise
	if (FAILED(m_pShaderCom->Set_RawValue("g_bNoise", &m_tEffectDesc.bNoise, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vScrollSpeedsX", &m_tEffectDesc.vScrollSpeedsX, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vScrollSpeeds", &m_tEffectDesc.vScrollSpeeds, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vNoiseScale", &m_tEffectDesc.vNoiseScale, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDirection1", &m_tEffectDesc.vDistortion1, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDirection2", &m_tEffectDesc.vDistortion2, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDirection3", &m_tEffectDesc.vDistortion3, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortionScale", &m_tEffectDesc.fDistortionScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortionBias", &m_tEffectDesc.fDistortionBias, sizeof(_float))))
		return E_FAIL;

	// Mask
	if (FAILED(m_pShaderCom->Set_RawValue("g_bMask", &m_tEffectDesc.bEnableMask, sizeof(_bool))))
		return E_FAIL;

	// Glow
	if (FAILED(m_pShaderCom->Set_RawValue("g_bGlow", &m_tEffectDesc.bGlow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fGlowValue", &m_tEffectDesc.fGlowValue, sizeof(_float))))
		return E_FAIL;

	// Dissolve
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	m_PrevWorldMatrix = WorldMatrix;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CStaticMeshEffect* CStaticMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CStaticMeshEffect* pInstance = new CStaticMeshEffect(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Create Failed : CStaticMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect* CStaticMeshEffect::Clone(void* _pArg)
{
	CStaticMeshEffect* pInstance = new CStaticMeshEffect(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Cloned Failed : CStaticMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticMeshEffect::Free()
{
	__super::Free();
}
