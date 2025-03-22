#include "ImageEffect.h"
#include "GameInstance.h"

CImageEffect::CImageEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CEffect(_pDevice, _pContext)
{
}

CImageEffect::CImageEffect(const CEffect& _rhs)
	: CEffect(_rhs)
{
}

HRESULT CImageEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CImageEffect::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_eEffectType = EFFECT_IMAGE;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		memcpy(&m_tEffectDesc, (EFFECTDESC*)_pArg, sizeof(EFFECTDESC));
		m_pTransformCom->Set_Scale(m_tEffectDesc.vScl);
		m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));
	}

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(1280, 720, 0.f, 1.f));
	//m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

	return S_OK;
}

void CImageEffect::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (m_pParent == nullptr)
		return;

	if (!m_pParent->Is_Active())
		return;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return;

	if (m_tEffectDesc.bOrthogonal)
	{
		_float3 vScale = _float3(1280.f, 720.f, 1.f);
		m_pTransformCom->Set_Scale(vScale);
	}
}

void CImageEffect::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (!m_pParent->Is_Active())
		return;

	if (CEffectObject::PROJECT_CLIENT == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		if (!m_bEnable || !m_bSlice || m_bDisable)
			return;
	}

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	m_pRendererCom->Add_RenderGroup((CRenderer::RENDERID)(m_tEffectDesc.iRenderID + 3), this);
}

HRESULT CImageEffect::Render()
{
	if (nullptr == m_pVIBufferCom_Rect)
		return E_FAIL;

	if (!m_pParent->Is_Active())
		return S_OK;

	if (!m_bEnable || !m_bSlice || m_bDisable)
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(m_tEffectDesc.iPassIndex);

	m_pVIBufferCom_Rect->Render();

	return S_OK;
}

HRESULT CImageEffect::Set_Texture(CTexture* pTexture)
{
	/*if (nullptr != m_pTextureCom)
	{
		Safe_Release(m_pTextureCom);
		Safe_Release(m_Components[TEXT("Com_Texture")]);
		m_Components.erase(TEXT("Com_Texture"));
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), 0, strFileName.c_str(), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);*/

	//m_pTextureCom = (CTexture*)pTexture->Clone(nullptr);
	m_iTextureIndex = 0;

	return S_OK;
}

HRESULT CImageEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(TEXT("Com_Renderer"), 0, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(TEXT("Com_Texture"), 0, TEXT("Prototype_Component_Texture_UI_Test"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), 0, TEXT("Prototype_Component_Shader_VTXTEX_EFFECT"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), 0, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom_Rect)))
		return E_FAIL;

	return S_OK;
}

HRESULT CImageEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrix, ViewMatrix;
	WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	_float camFar = pGameInstance->Get_CamFar();

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (m_tEffectDesc.bOrthogonal)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tEffectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vUVDir", &m_vUV_Dir, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_SRV("g_DepthTexture", pGameInstance->Get_RenderTarget_SRV(TARGET_MAPOBJDEPTH))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	if (m_tEffectDesc.bNoise)
	{
		if (FAILED(m_pTextureCom[TEXTURE_NOISE]->SetUp_ConstantTable_Index(m_pShaderCom, "g_NoiseTexture", m_iNoiseIndex)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CImageEffect* CImageEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CImageEffect* pInstance = new CImageEffect(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Create Failed : CImageEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect* CImageEffect::Clone(void* _pArg)
{
	CImageEffect* pInstance = new CImageEffect(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Cloned Failed : CImageEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImageEffect::Free()
{
	__super::Free();
}
