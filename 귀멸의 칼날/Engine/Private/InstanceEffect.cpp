#include "InstanceEffect.h"
#include "GameInstance.h"

CInstanceEffect::CInstanceEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CEffect(_pDevice, _pContext)
{
	m_PrevWorldMatrix = CMethod::XMMatrixIdentity_P();
}

CInstanceEffect::CInstanceEffect(const CEffect& _rhs)
	: CEffect(_rhs)
{
	m_PrevWorldMatrix = CMethod::XMMatrixIdentity_P();
}

HRESULT CInstanceEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInstanceEffect::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_eEffectType = EFFECT_INSTANCE;

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

void CInstanceEffect::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (m_pParent == nullptr)
		return;

	if (nullptr == m_pTransformCom || nullptr == m_pVIBufferCom_Instance)
		return;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return;
	}

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return;

	if (static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
		m_pVIBufferCom_Instance->Tick((_float)_dTimeDelta);
}

void CInstanceEffect::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return;
	}

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return;

	m_pRendererCom->Add_RenderGroup((CRenderer::RENDERID)(m_tEffectDesc.iRenderID + 3), this);
	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CInstanceEffect::Render()
{
	if (nullptr == m_pVIBufferCom_Instance)
		return E_FAIL;

	if (CEffectObject::PROJECT_TOOL != static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
			return S_OK;
	}

	if (!m_pParent->Is_Active())
		return S_OK;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(m_tEffectDesc.iPassIndex);

	m_pVIBufferCom_Instance->Render();

	return S_OK;
}

HRESULT CInstanceEffect::Set_Texture(CTexture* pTexture)
{
	//m_pTextureCom = (CTexture*)pTexture->Clone(nullptr);
	m_iTextureIndex = 0;

	return S_OK;
}

INSTANCEDESC CInstanceEffect::Get_InstanceEffectDesc()
{
	if (nullptr == m_pVIBufferCom_Instance)
		return INSTANCEDESC();

	return m_pVIBufferCom_Instance->Get_InstanceDesc();
}

void CInstanceEffect::Set_InstanceEffectDesc(INSTANCEDESC tInstanceDesc)
{
	if (nullptr == m_pVIBufferCom_Instance)
		return;

	m_pVIBufferCom_Instance->Set_InstanceDesc(tInstanceDesc);
}

HRESULT CInstanceEffect::ChangeInstanceCount(_uint iCount)
{
	if (nullptr == m_pVIBufferCom_Instance)
		return E_FAIL;

	if (0 < iCount)
	{
		if (FAILED(m_pVIBufferCom_Instance->ChangeBufferCount(iCount)))
		{
			MSG_BOX("인스턴싱 카운트 재할당 실패");
			return E_FAIL;
		}
	}

	return S_OK;
}

void CInstanceEffect::InstanceBufferReset()
{
	m_pVIBufferCom_Instance->Reset();
}

HRESULT CInstanceEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(TEXT("Com_Renderer"), 0, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), 0, TEXT("Prototype_Component_Shader_VTXRECTINSTANCE"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), 0, TEXT("Prototype_Component_VIBuffer_Effect_Instance"), (CComponent**)&m_pVIBufferCom_Instance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceEffect::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionF3(), sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vScale", &m_tEffectDesc.vScaleDelta, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tEffectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	_float fGlow = m_tEffectDesc.fGlowValue;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fGlowValue", &fGlow, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bCutBlack", &m_tEffectDesc.bCutBlack, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bNoise", &m_tEffectDesc.bNoise, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bGlow", &m_tEffectDesc.bGlow, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_START", &m_vUV_Start, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_END", &m_vUV_End, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vSliceUV_DELTA", &m_vUV_Delta, sizeof(_float2))))
		return E_FAIL;

	_float camFar = pGameInstance->Get_CamFar();
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vUVDir", &m_vUV_Dir, sizeof(_float2))))
		return E_FAIL;

	// teoni
	if (8 == pGameInstance->Get_CurLevel())
	{
		if (FAILED(m_pShaderCom->Set_SRV("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TARGET_MAPOBJDEPTH))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_SRV("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TARGET_DEPTH))))
			return E_FAIL;
	}
	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	if (m_tEffectDesc.bNoise)
	{
		if (FAILED(m_pTextureCom[TEXTURE_NOISE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_NoiseTexture", m_iNoiseIndex)))
			return E_FAIL;
	}

	m_PrevWorldMatrix = WorldMatrix;

	INSTANCEDESC tDesc = m_pVIBufferCom_Instance->Get_InstanceDesc();
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBillboard", &tDesc.bBillboard, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color1", &tDesc.vColor1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color2", &tDesc.vColor2, sizeof(_float4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CInstanceEffect* CInstanceEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInstanceEffect* pInstance = new CInstanceEffect(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Create Failed : CInstanceEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect* CInstanceEffect::Clone(void* _pArg)
{
	CInstanceEffect* pInstance = new CInstanceEffect(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Cloned Failed : CInstanceEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstanceEffect::Free()
{
	__super::Free();
}
