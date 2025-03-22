#include "CRenderer.h"
#include "CGameObject.h"
#include "CLevel_Manager.h"
#include "CTarget_Manager.h"
#include "CLight_Manager.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"
#include "CCollider.h"
#include "CPipeLine.h"

CRenderer::CRenderer(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CComponent(_pDevice, _pContext), m_pTarget_Manager(CTarget_Manager::Get_Instance()), 
	m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}

void CRenderer::Plus_PadeOutValue(const _float& _fPadeOut)
{
	this->m_fPadeOutValue += _fPadeOut;

	if (m_fPadeOutValue > 1.f)
		m_fPadeOutValue = 1.f;
	else if (m_fPadeOutValue < 0.f)
		m_fPadeOutValue = 0.f;
}

void CRenderer::Clear()
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderGroup[i])
			Safe_Release(iter);

		m_RenderGroup[i].clear();
	}
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	D3D11_VIEWPORT			ViewPort;

	_uint		iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewPort);

	// Add Target manager
	{
		CRenderTarget::RENTDESC tDesc;
		// Deffered
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DIFFUSE_DEFFERD, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_NORMAL_DEFFERD, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f));
			/* For.Target_Depth */
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DEPTH_LIGHT, tDesc)))
				return E_FAIL;
		}
		// Light
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SHADE_DEFFERD, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			/* For.Target_Depth */
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SPECULAR_LIGHT, tDesc)))
				return E_FAIL;
		}
		// Alpha Blend
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DIFFUSE_ALPHABLEND, tDesc)))
				return E_FAIL;
		}
		 

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFFED, TARGET_DIFFUSE_DEFFERD)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFFED, TARGET_NORMAL_DEFFERD)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFFED, TARGET_DEPTH_LIGHT)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_SHADE_DEFFERD)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_SPECULAR_LIGHT)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_ALPHABLEND, TARGET_DIFFUSE_ALPHABLEND)))
			return E_FAIL;
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewPort.Width;
	m_WorldMatrix._22 = ViewPort.Height;
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f)));

	// VIBuffer, Shader Create
	{
		m_pVIBufferRect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
		if (nullptr == m_pVIBufferRect)
			return E_FAIL;

		m_pDefferdShader = CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\ShaderFiles\\Shader_Deffered.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement);
		if (nullptr == m_pDefferdShader)
			return E_FAIL;

		m_pAlphaBlendShader = CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\ShaderFiles\\Shader_AlphaBlend.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement);
		if (nullptr == m_pAlphaBlendShader)
			return E_FAIL;
	}

#ifdef _DEBUG

	//if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_BLEND_ALPHABLEND, _float2(0.f, 0.f), _float2(200.f, 200.f))))
	//	return E_FAIL;

#endif


	BEGIN_SINGLETON(CLevel_Manager, pLevelManager)
	{
		pLevelManager->Add_Renderer(this);
	}
	END_SINGLETON(pLevelManager);
	return S_OK;
}

HRESULT CRenderer::NativeConstruct_Clone(void * pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(const RENDERID& _eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[_eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Add_Components(CComponent* _pComponents)
{
	if (nullptr == _pComponents)
		return E_FAIL;

	m_Components.push_back(_pComponents);
	Safe_AddRef(_pComponents);
	return S_OK;
}
#endif
HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();
	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, MRT_DEFFED)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, MRT_DEFFED)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	// 장치에 셰이드 타겟을 바인딩 했어
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, MRT_LIGHTACC)))
		return E_FAIL;

	if(FAILED(m_pDefferdShader->Set_SRV("g_NormalTexture", m_pTarget_Manager->Get_SRV(TARGET_NORMAL_DEFFERD))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_SRV("g_DepthTexture", m_pTarget_Manager->Get_SRV(TARGET_DEPTH_LIGHT))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	BEGIN_SINGLETON(CPipeLine, pPipeLine);
	{
		_float4x4			ViewMatrixInv, ProjMatrixInv;

		XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
		XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));

		if (FAILED(m_pDefferdShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pDefferdShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pDefferdShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPositionF3(), sizeof(_float3))))
			return E_FAIL;
	}
	END_SINGLETON(pPipeLine);

	if (FAILED(m_pLight_Manager->Render_Lights(m_pDefferdShader, m_pVIBufferRect)))
		return E_FAIL;

	// 셰이드 타겟에 값을 기록해 주기 위해 사각형 버퍼를 그린다. 빛의 개수 만큼
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, MRT_LIGHTACC)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pDefferdShader->Set_SRV("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TARGET_DIFFUSE_DEFFERD))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_SRV("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TARGET_SHADE_DEFFERD))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_SRV("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TARGET_SPECULAR_LIGHT))))
		return E_FAIL;

	if (FAILED(m_pDefferdShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pDefferdShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pDefferdShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pDefferdShader->Set_RawValue("g_PadeOut", &m_fPadeOutValue, sizeof(_float))))
		return E_FAIL;

	m_pDefferdShader->Begin(3);

	m_pVIBufferRect->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONLIGHT].clear();
	
	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& iter : m_Components)
	{
		if (nullptr != iter)
			iter->Render();

		Safe_Release(iter);
	}
	m_Components.clear();

//	m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"));
//	m_pTarget_Manager->Render_Debug(MRT_ALPHABLEND);
	return S_OK;
}

#endif
CRenderer * CRenderer::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
{
	CRenderer* pInstance = new CRenderer(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Renderer::Create To Failed");
	}
	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();
	return this;
}

void CRenderer::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pDefferdShader);
	Safe_Release(m_pAlphaBlendShader);
	Safe_Release(m_pVIBufferRect);

#ifdef _DEBUG
	for (auto& iter : m_Components)
		Safe_Release(iter);

	m_Components.clear();
#endif
	for(_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderGroup[i])
			Safe_Release(iter);

		m_RenderGroup[i].clear();
	}

	__super::Free();
}
