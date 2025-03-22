#include "Renderer.h"
#include "GameObject.h"
#include "Level_Manager.h"

#include "GameInstance.h"
#include "Graphic_Device.h"
#include "PostProcessing.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Collider.h"
#include "PipeLine.h"
#include "RenderHandler.h"
#include "Texture.h"

#include "UI_Base.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext),
	m_pTarget_Manager(CTarget_Manager::Get_Instance()),
	m_pLight_Manager(CLight_Manager::Get_Instance()),
	m_pPostProcessing(CPostProcessing::Get_Instance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPostProcessing);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
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

	_uint iLightSizeX = 8000;
	_uint iLightSizeY = 6000;
	if (FAILED(m_pTarget_Manager->Create_LightDSV(iLightSizeX, iLightSizeY)))
		return E_FAIL;

	// Add Target manager
	{
		CRenderTarget::RENTDESC tDesc;
		// Deffered
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DIFFUSE_DEFERRED, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_NORMAL_DEFERRED, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DEPTH, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_EDGEGLOW, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_MOTIONBLUR_DEFERRED, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_MAPOBJDEPTH, tDesc)))
				return E_FAIL;
		}

		// Light
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_AMBIENT, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SHADE_DEFERRED, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SPECULAR_LIGHT, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SHADOW, tDesc)))
				return E_FAIL;
		}
		// Shadow
		{

			tDesc = CRenderTarget::RENTDESC((_float)iLightSizeX, (_float)iLightSizeY,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SHADOW_DEPTH, tDesc)))
				return E_FAIL;
		}
		// NonAlphaBlend
		{
			tDesc = CRenderTarget::RENTDESC((_float)ViewPort.Width, (_float)ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_NONALPHABLEND, tDesc)))
				return E_FAIL;
		}
		// AlphaBlend
		{
			tDesc = CRenderTarget::RENTDESC((_float)ViewPort.Width, (_float)ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_ALPHABLEND, tDesc)))
				return E_FAIL;
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_GLOW, tDesc)))
				return E_FAIL;
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_MOTIONBLUR_ALPHABLEND, tDesc)))
				return E_FAIL;
			tDesc = CRenderTarget::RENTDESC((_float)ViewPort.Width, (_float)ViewPort.Height,
				DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_BLUR_DEGREE, tDesc)))
				return E_FAIL;
		}
		// Distortion
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.5f, 0.5f, 1.f, 1.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DISTORTION_DIFFUSE, tDesc)))
				return E_FAIL;
		}
		// Temp
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_TEMP, tDesc)))
				return E_FAIL;
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_TEMP2, tDesc)))
				return E_FAIL;
		}
		// NonAlpha MotionBlur
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DIFFUSE_MOTIONBLUR_NONALPHA, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_JUDGEDRAW_MOTIONBLUR_NONALPHA, tDesc)))
				return E_FAIL;
		}
		// Alpha MotionBlur
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_DIFFUSE_MOTIONBLUR_ALPHA, tDesc)))
				return E_FAIL;

			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_JUDGEDRAW_MOTIONBLUR_ALPHA, tDesc)))
				return E_FAIL;
		}
		// Shake
		{
			tDesc = CRenderTarget::RENTDESC(ViewPort.Width, ViewPort.Height,
				DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
			if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TARGET_SHAKE_DIFFUSE, tDesc)))
				return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_DIFFUSE_DEFERRED)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_NORMAL_DEFERRED)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_DEPTH)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_EDGEGLOW)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_MOTIONBLUR_DEFERRED)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DEFERRED, TARGET_MAPOBJDEPTH)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_AMBIENT)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_SHADE_DEFERRED)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_SPECULAR_LIGHT)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_LIGHTACC, TARGET_SHADOW)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_SHADOW, TARGET_SHADOW_DEPTH)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_DISTORTION, TARGET_DISTORTION_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_NONALPHABLEND, TARGET_NONALPHABLEND)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_SHAKE, TARGET_SHAKE_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_ALPHABLEND, TARGET_ALPHABLEND)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_ALPHABLEND, TARGET_GLOW)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_ALPHABLEND, TARGET_BLUR_DEGREE)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_ALPHABLEND, TARGET_MOTIONBLUR_ALPHABLEND)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_MOTIONBLUR_NONALPHA, TARGET_DIFFUSE_MOTIONBLUR_NONALPHA)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_MOTIONBLUR_NONALPHA, TARGET_JUDGEDRAW_MOTIONBLUR_NONALPHA)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_MOTIONBLUR_ALPHA, TARGET_DIFFUSE_MOTIONBLUR_ALPHA)))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_MOTIONBLUR_ALPHA, TARGET_JUDGEDRAW_MOTIONBLUR_ALPHA)))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_TEMP, TARGET_TEMP)))
			return E_FAIL;		
		if (FAILED(m_pTarget_Manager->Add_MRT(MRT_TEMP2, TARGET_TEMP2)))
			return E_FAIL;

	}
	m_WorldMatrix._11 = ViewPort.Width;
	m_WorldMatrix._22 = ViewPort.Height;
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f)));

	{
		m_pRenderHandler = CRenderHandler::Create(m_pDevice, m_pContext);
		if (Is_Nullptr(m_pRenderHandler))
			return E_FAIL;
	}

//#ifdef _DEBUG

	// 1번째 열(기본 조명연산)
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_DIFFUSE_DEFERRED, _float2(0.f, 0.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_NORMAL_DEFERRED, _float2(0.f, 100.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_DEPTH, _float2(0.f, 200.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_SHADE_DEFERRED, _float2(0.f, 300.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_MOTIONBLUR_DEFERRED, _float2(0.f, 400.f), _float2(100.f, 100.f))))
		return E_FAIL;

	// 2번째 열(그림자)
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_SHADOW_DEPTH, _float2(100.f, 0.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_SHADOW, _float2(100.f, 100.f), _float2(100.f, 100.f))))
		return E_FAIL;

	// 3번째 열(AlphaBlend)
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_ALPHABLEND, _float2(200.f, 0.f), _float2(100.f, 100.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_GLOW, _float2(200.f, 100.f), _float2(100.f, 100.f))))
		return E_FAIL;	
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_BLUR_DEGREE, _float2(200.f, 200.f), _float2(100.f, 100.f))))
		return E_FAIL;

	// 4번째 열(Distortion)
	if (FAILED(m_pTarget_Manager->Ready_Debug_Desc(TARGET_DISTORTION_DIFFUSE, _float2(300.f, 0.f), _float2(100.f, 100.f))))
		return E_FAIL;
//#endif

	return S_OK;
}

HRESULT CRenderer::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(const RENDERID& _eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[_eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);
	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Add_Components(const RENDERDEBUG& _eDebugGroup, CComponent* _pComponents)
{
	if (nullptr == _pComponents)
		return E_FAIL;

	m_Components[_eDebugGroup].push_back(_pComponents);
	Safe_AddRef(_pComponents);
	return S_OK;
}

#endif

// Fade종류 (0 : FadeIn, 1 : FadeOut, 2: FadeOutIn)
void CRenderer::Start_FadeInOut(_float fFadeSpeed, _bool bFadeWhite, _uint iFadeType)
{
	m_tDebugRender.bFadeInOut = true;
	m_tDebugRender.bShake = true;
	m_tDebugRender.iFadeType = iFadeType;
	
	if (bFadeWhite)
		m_tDebugRender.vFadeColor = _float4(1.f, 1.f, 1.f, 1.f);
	else
		m_tDebugRender.vFadeColor = _float4(0.f, 0.f, 0.f, 1.f);

	// FadeOut부터일때
	m_tDebugRender.fFadeAmount = 0.f;
	m_tDebugRender.fFadeSpeed = fabs(fFadeSpeed);

	// FadeIn일때
	if (0 == iFadeType)
	{
		m_tDebugRender.fFadeAmount = 1.f;
		m_tDebugRender.fFadeSpeed = -1.f *fabs(fFadeSpeed);
	}
}

HRESULT CRenderer::Draw()
{

	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_ShadowLight()))
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

	if (FAILED(Render_MotionBlur()))
		return E_FAIL;

	if (FAILED(Render_Shake()))
		return E_FAIL;

	if (FAILED(Render_HDR()))
		return E_FAIL;

	if (FAILED(Render_Distortion()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	for (_uint i = 0; i < RENDERID::RENDER_END; i++)
	{
		for (auto& pObject : m_RenderGroup[i])
			Safe_Release(pObject);
		m_RenderGroup[i].clear();
	}

//#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
//#endif
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	m_RenderGroup[RENDER_PRIORITY].sort([](CGameObject* Obj1, CGameObject* Obj2)
		{
			return Obj1->Get_DepthValue() > Obj2->Get_DepthValue();
		});


	return m_pRenderHandler->Rendering_GameObject(&m_RenderGroup[RENDER_PRIORITY]);
}

HRESULT CRenderer::Render_ShadowLight()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (m_tDebugRender.bShadow)
	{
		if (FAILED(m_pTarget_Manager->Begin_LightDepth_MRT(m_pContext, MRT_SHADOW)))
			return E_FAIL;

		for (auto& pGameObject : m_RenderGroup[RENDER_SHADOW])
		{
			m_tDebugRender.bShadow = false;
			if (FAILED(pGameObject->RenderLight()))
			{
				MSG_BOX("Failed To RenderLight : GameObject");
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
		m_RenderGroup[RENDER_SHADOW].clear();

		if (FAILED(m_pTarget_Manager->End_LightDepth_MRT(m_pContext, MRT_SHADOW)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	return m_pRenderHandler->Rendering_GameObject(MRT_DEFERRED, &m_RenderGroup[RENDER_NONALPHABLEND]);
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	DEBUGRENDER& tDesc = m_tDebugRender;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	{
		_float4x4 lightViewMatrix = pPipeLine->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		_float4x4 lightProjMatrix = pPipeLine->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

		_float4x4 ViewMatrixInv = pPipeLine->Get_TransformFloat4x4Inv_TP(CPipeLine::D3DTS_VIEW);
		_float4x4 ProjMatrixInv = pPipeLine->Get_TransformFloat4x4Inv_TP(CPipeLine::D3DTS_PROJ);
		_float4x4 ViewMatrix = pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);

		_float3 v3CamPos = pPipeLine->Get_CamPositionF3();
		_float3 v3LightPos = pPipeLine->Get_LightPositionF3();
		_float fCamFar = pPipeLine->Get_CamFar();
		_float fLightFar = pPipeLine->Get_LightFar();


		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"),  RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , 
			RH_VALUE(m_ProjMatrix, "g_ProjMatrix"), RH_VALUE(v3CamPos, "g_vCamPosition"), 
			RH_VALUE(ViewMatrixInv, "g_ViewMatrixInv"), RH_VALUE(ProjMatrixInv, "g_ProjMatrixInv"), RH_VALUE(fCamFar, "g_CamFar"),
			RH_VALUE(v3LightPos, "g_vLightPosition"), RH_VALUE(fLightFar, "g_LightFar"), 
			RH_VALUE(lightViewMatrix, "g_LightViewMatrix"), RH_VALUE(lightProjMatrix, "g_LightProjMatrix"),
			RH_VALUE(tDesc.fShadowDepth, "fShadowDepth"), RH_VALUE(tDesc.vShadowColor, "vShadowColor"),
			RH_VALUE(tDesc.fRimLightPower, "fRimLightPower"), RH_VALUE(tDesc.fRimMax, "fRimMax"), RH_VALUE(tDesc.fRimMin, "fRimMin"),
			RH_VALUE(tDesc.bShade, "bShade"),  RH_VALUE(tDesc.bRimlight, "bRimlight")
		};

		// Texture
		RH_TEX_VEC Texes{ RH_TEX(TARGET_NORMAL_DEFERRED, "g_NormalTexture"), RH_TEX(TARGET_DEPTH, "g_DepthTexture"), RH_TEX(TARGET_SHADOW_DEPTH, "g_LightDepthTexture") };

		// Ambient, Shade, Specular, Shadow 생성
		if (FAILED(m_pRenderHandler->Render_Lights(MRT_LIGHTACC, SHADER_LIGHT, Values, Texes)))
		{
			RELEASE_INSTANCE(CPipeLine);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pPostProcessing->DownScale_Blur(m_pTarget_Manager->Get_SRV(TARGET_SHADOW), m_pPostProcessing->Get_UAV(PH_BLUR))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_ShadowTexture", m_pPostProcessing->Get_SRV(PH_BLUR))))
		return E_FAIL;

	DEBUGRENDER& tDesc = m_tDebugRender;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	_float fCamFar = pPipeLine->Get_CamFar();
	{
		RH_VALUE_VEC Values = { RH_VALUE(m_WorldMatrix, "g_WorldMatrix"),  RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix"),
			RH_VALUE(fCamFar, "g_CamFar"), RH_VALUE(tDesc.fTime, "fTime"),
			RH_VALUE(tDesc.bFog, "bFog"), RH_VALUE(tDesc.fogColor, "fogColor"), RH_VALUE(tDesc.fogStart, "fogStart"),
			RH_VALUE(tDesc.fogEnd, "fogEnd"), RH_VALUE(tDesc.fogExp, "fogExp"), RH_VALUE(tDesc.fogType, "fogType") };
		RH_TEX_VEC Texes = { RH_TEX(TARGET_DIFFUSE_DEFERRED, "g_DiffuseTexture"), RH_TEX(TARGET_SHADE_DEFERRED, "g_ShadeTexture") , RH_TEX(TARGET_DEPTH, "g_DepthTexture"),
		RH_TEX(TARGET_SPECULAR_LIGHT, "g_SpecularTexture"), RH_TEX(TARGET_AMBIENT, "g_AmbientTexture"), RH_TEX(TARGET_NORMAL_DEFERRED, "g_NormalTexture") };

		if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_NONALPHABLEND, SHADER_DEFERRED, 1, Values, Texes)))
		{
			MSG_BOX("Failed To Render : MRT_NONALPHABLEND");
			RELEASE_INSTANCE(CPipeLine);
			return E_FAIL;
		}
	}

	// 외각선
	{
		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"),  RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix"),
							RH_VALUE(fCamFar, "g_CamFar"), RH_VALUE(tDesc.minSeparation, "minSeparation"), RH_VALUE(tDesc.maxSeparation, "maxSeparation"),
							RH_VALUE(tDesc.minDistance, "minDistance"), RH_VALUE(tDesc.maxDistance, "maxDistance"), 
							RH_VALUE(tDesc.outlineSize, "outlineSize"), RH_VALUE(tDesc.colorModifier, "colorModifier"), RH_VALUE(tDesc.bOutline, "bOutline")};
		RH_TEX_VEC Texes{ RH_TEX(TARGET_NONALPHABLEND, "g_DiffuseTexture"), RH_TEX(TARGET_NORMAL_DEFERRED, "g_NormalTexture"),
						RH_TEX(TARGET_DEPTH, "g_DepthTexture"), RH_TEX(TARGET_EDGEGLOW, "g_ColorTexture") };
		if (FAILED(m_pRenderHandler->Rendering(SHADER_EDGEGLOW, 2, Values, Texes)))
		{
			MSG_BOX("Failed To Render : EDGE");
			RELEASE_INSTANCE(CPipeLine);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CPipeLine);


	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	if (FAILED(m_pRenderHandler->Rendering_GameObject(&m_RenderGroup[RENDER_NONLIGHT])))
	{
		MSG_BOX("Failed To Render : NonLight");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	// Glow 그룹에 있는 객체 깊이값 정렬
	m_RenderGroup[RENDER_ALPHABLEND].sort([](CGameObject* Obj1, CGameObject* Obj2)
		{
			return Obj1->Get_DepthValue() > Obj2->Get_DepthValue();
 		});

  	if (FAILED(m_pRenderHandler->Rendering_GameObject(MRT_ALPHABLEND, &m_RenderGroup[RENDER_ALPHABLEND])))
		return E_FAIL;

	if (FAILED(m_pPostProcessing->DownScale_Blur(m_pTarget_Manager->Get_SRV(TARGET_GLOW), m_pPostProcessing->Get_UAV(PH_BLUR), m_tDebugRender.fBlur)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_GlowTexture", m_pPostProcessing->Get_SRV(PH_BLUR))))
		return E_FAIL;

	if (FAILED(m_pPostProcessing->DownScale_Blur(m_pTarget_Manager->Get_SRV(TARGET_BLUR_DEGREE), m_pPostProcessing->Get_UAV(PH_BLUR2), m_tDebugRender.fBlur)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_BlurDegreeTexture", m_pPostProcessing->Get_SRV(PH_BLUR2))))
		return E_FAIL;

	RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"),  RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
	RH_TEX_VEC Texes{ RH_TEX(TARGET_ALPHABLEND, "g_DiffuseTexture")/*, RH_TEX(TARGET_BLUR_DEGREE, "g_BlurDegreeTexture")*/ };
	if (FAILED(m_pRenderHandler->Rendering(SHADER_DEFERRED, 2, Values, Texes)))
		return E_FAIL;


	{	
		if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_DiffuseTexture", m_pPostProcessing->Get_SRV(PH_BLUR))))
			return E_FAIL;
		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
		RH_TEX_VEC Texes{ };
		if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_TEMP2, SHADER_DEFERRED, 3, Values, Texes)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	CLevel_Manager* pLevel_Manager = GET_INSTANCE(CLevel_Manager);
	{
		if (FAILED(m_pRenderHandler->Rendering_GameObject(MRT_DISTORTION, &m_RenderGroup[RENDER_DISTORTION])))
		{
			RELEASE_INSTANCE(CPipeLine);
			RELEASE_INSTANCE(CLevel_Manager);
			return E_FAIL;
		}
		
		// 백버퍼를 temp 타겟에 복사
		{
			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_DiffuseTexture", CGraphic_Device::Get_Instance()->Get_BackBufferSRV())))
				return E_FAIL;
			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
			RH_TEX_VEC Texes{ };
			if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_TEMP, SHADER_DEFERRED, 3, Values, Texes)))
				return E_FAIL;
		}

		_float3 v3CamPos = pPipeLine->Get_CamPositionF3();
		_float fDeltaSeconds = pLevel_Manager->Get_DeltaTime() * 0.01f;

		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix"),
			RH_VALUE(fDeltaSeconds, "g_fTime"), RH_VALUE(m_tDebugRender.v2UvFlowValue, "g_UvFlowValue"), 
			RH_VALUE(m_tDebugRender.fMultipleTimeValue, "g_fMultipleTimeValue"), RH_VALUE(m_tDebugRender.fDistortionDivineValue, "g_DistortionDivineValue")};
		// Texture 
		RH_TEX_VEC Texes{ RH_TEX(TARGET_DISTORTION_DIFFUSE, "g_DDTexture"), RH_TEX(TARGET_TEMP, "g_DiffuseTexture"), };

		/*SETUP_TEXTURE_SRV_NAME(m_pTarget_Manager->Get_Shader(SHADER_DISTORTION),
			m_pTarget_Manager->Get_Distortion(), "g_Texture1", L"Distortion3");
		SETUP_TEXTURE_SRV_NAME(m_pTarget_Manager->Get_Shader(SHADER_DISTORTION),
			m_pTarget_Manager->Get_Distortion(), "g_Texture2", L"T_e_Plc_P1007_Atk_Noise003");*/

		// Begin
		if (FAILED(m_pRenderHandler->Rendering(SHADER_DISTORTION, 5, Values, Texes)))
		{
			RELEASE_INSTANCE(CPipeLine);
			RELEASE_INSTANCE(CLevel_Manager);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CPipeLine);
	RELEASE_INSTANCE(CLevel_Manager);
	return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (false == m_tDebugRender.bMotionBlur)
		return S_OK;

	float g_PassAlpha = 0.04f;
	float g_VelocityAlpha = 0.f;
	{
		// NonAlpha
		{
			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_MOTIONBLUR)->Set_SRV("g_DiffuseTexture",
				CGraphic_Device::Get_Instance()->Get_BackBufferSRV())))
				return E_FAIL;
			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") ,
								RH_VALUE(m_ProjMatrix, "g_ProjMatrix"), RH_VALUE(m_tDebugRender.iNumBlurValue, "g_iNumBlurValue"),
								RH_VALUE(m_tDebugRender.iDivineVelocityValue, "g_iDivineVelocityValue"), 
								RH_VALUE(g_PassAlpha, "g_PassAlpha"), RH_VALUE(g_VelocityAlpha, "g_VelocityAlpha") };
			RH_TEX_VEC Texes{ RH_TEX(TARGET_MOTIONBLUR_DEFERRED, "g_VelocityTexture") };
			if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_MOTIONBLUR_NONALPHA, SHADER_MOTIONBLUR, 0, Values, Texes)))
				return E_FAIL;
		}

		{

			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") ,RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
			RH_TEX_VEC Texes{ RH_TEX(TARGET_DIFFUSE_MOTIONBLUR_NONALPHA, "g_MotionBlur"), RH_TEX(TARGET_JUDGEDRAW_MOTIONBLUR_NONALPHA, "g_JudgeDraw") };
			if (FAILED(m_pRenderHandler->Rendering(SHADER_DEFERRED, 7, Values, Texes)))
				return E_FAIL;
		}
	}

	{
		// Alpha
		{

			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_MOTIONBLUR)->Set_SRV("g_DiffuseTexture",
				CGraphic_Device::Get_Instance()->Get_BackBufferSRV())))
				return E_FAIL;

			g_PassAlpha = 0.2f;
			g_VelocityAlpha = 0.f;

			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") ,RH_VALUE(m_ProjMatrix, "g_ProjMatrix"), 
								RH_VALUE(m_tDebugRender.iNumBlurValue, "g_iNumBlurValue"), RH_VALUE(m_tDebugRender.iDivineVelocityValue, "g_iDivineVelocityValue"),
								RH_VALUE(g_PassAlpha, "g_PassAlpha"), RH_VALUE(g_VelocityAlpha, "g_VelocityAlpha") };
			RH_TEX_VEC Texes{ RH_TEX(TARGET_MOTIONBLUR_ALPHABLEND, "g_VelocityTexture") };
			if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_MOTIONBLUR_ALPHA, SHADER_MOTIONBLUR, 0, Values, Texes)))
				return E_FAIL;
		}

		{

			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
			RH_TEX_VEC Texes{ RH_TEX(TARGET_DIFFUSE_MOTIONBLUR_ALPHA, "g_MotionBlur"), RH_TEX(TARGET_JUDGEDRAW_MOTIONBLUR_ALPHA, "g_JudgeDraw") };
			if (FAILED(m_pRenderHandler->Rendering(SHADER_DEFERRED, 7, Values, Texes)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_Shake()
{
	SetUp_FadeInOut();

	if (false == m_tDebugRender.bShake)
		return S_OK;

	{
		if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_DiffuseTexture", CGraphic_Device::Get_Instance()->Get_BackBufferSRV())))
			return E_FAIL;
		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
		RH_TEX_VEC Texes{ };
		if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_SHAKE, SHADER_DEFERRED, 3, Values, Texes)))
			return E_FAIL;
	}
	{
		_float CamFar = CPipeLine::Get_Instance()->Get_CamFar();
		RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix"), RH_VALUE(m_ProjMatrix, "g_ProjMatrix"),
							RH_VALUE(m_tDebugRender.fShakeAmount, "fShakeAmount"), RH_VALUE(m_tDebugRender.fShakeDepth, "fShakeDepth"),
							RH_VALUE(CamFar, "g_CamFar"), RH_VALUE(m_tDebugRender.vFadeColor, "vFadeColor"), RH_VALUE(m_tDebugRender.fFadeAmount, "fFadeAmount")};
		RH_TEX_VEC Texes{ RH_TEX(TARGET_SHAKE_DIFFUSE, "g_DiffuseTexture")/*, RH_TEX(TARGET_DEPTH, "g_DepthTexture")*/ };
		if (FAILED(m_pRenderHandler->Rendering(SHADER_MOTIONBLUR, 1, Values, Texes)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	if (m_tDebugRender.bHDR)
	{
		// HDR Texture
		{
			_float fAverLum = 1.1f;
			if (FAILED(m_pPostProcessing->HDR(CGraphic_Device::Get_Instance()->Get_BackBufferSRV(), fAverLum)))
				return E_FAIL;
			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_HDRTexture", m_pPostProcessing->Get_SRV(PH_TEMP))))
				return E_FAIL;
		}

		// 백버퍼를 temp 타겟에 복사
		{
			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_DiffuseTexture", CGraphic_Device::Get_Instance()->Get_BackBufferSRV())))
				return E_FAIL;
			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
			RH_TEX_VEC Texes{ };
			if (FAILED(m_pRenderHandler->Rendering_Texture(MRT_TEMP, SHADER_DEFERRED, 3, Values, Texes)))
				return E_FAIL;
		}

		// Render HDR
		{
			if (FAILED(m_pTarget_Manager->Get_Shader(SHADER_DEFERRED)->Set_SRV("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TARGET_TEMP))))
				return E_FAIL;
			RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"), RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix"),
			RH_VALUE(m_tDebugRender.fMiddleGrey, "g_fMiddleGrey") , RH_VALUE(m_tDebugRender.fLumWhiteSqr, "g_fLumWhiteSqr"), RH_VALUE(m_tDebugRender.fLScalePlusValue, "g_fLScalePlusValue") };
			RH_TEX_VEC Texes{};
			if (FAILED(m_pRenderHandler->Rendering(SHADER_DEFERRED, 6, Values, Texes)))
				return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CRenderer::Render_UI()
{
	if (m_tDebugRender.bFadeInOut)
	{
		for (auto& iter : m_RenderGroup[RENDER_UI])
			Safe_Release(iter);

		m_RenderGroup[RENDER_UI].clear();
	}
	else
	{
		m_RenderGroup[RENDER_UI].sort([](CGameObject* Obj1, CGameObject* Obj2)
			{
				return Obj1->Get_DepthValue() > Obj2->Get_DepthValue();
			});

		if (FAILED(m_pRenderHandler->Rendering_GameObject(&m_RenderGroup[RENDER_UI])))
			return E_FAIL;
	}

	return S_OK;
}

void CRenderer::SetUp_FadeInOut()
{
	if (m_tDebugRender.bFadeInOut)   
	{
		m_tDebugRender.fFadeAmount += m_tDebugRender.fFadeSpeed;

		m_tDebugRender.fShakeAmount += m_tDebugRender.fFadeSpeed * 0.2f;

		// 0 : FadeIn
		if (0 == m_tDebugRender.iFadeType)
		{
			if (m_tDebugRender.fFadeAmount < 0.f)
			{
				m_tDebugRender.bFadeInOut = false;
				m_tDebugRender.bShake = false;
				m_tDebugRender.fShakeAmount = 0.1f;
				m_tDebugRender.fFadeAmount = 0.f;
			}
		}
		// 1 : FadeOut
		else if (1 == m_tDebugRender.iFadeType)
		{
			if (m_tDebugRender.fFadeAmount > 1.f)
			{
				//m_tDebugRender.bFadeInOut = false;
				//m_tDebugRender.bShake = false;
				//m_tDebugRender.fShakeAmount = 0.1f;
				m_tDebugRender.fFadeAmount = 1.f;
			}
		}
		// 2 : FadeInOut
		else if (2 == m_tDebugRender.iFadeType)
		{
			if (m_tDebugRender.fFadeAmount > 1.f)
			{
				m_tDebugRender.fShakeAmount = 0.1f;
				m_tDebugRender.fFadeAmount = 1.f;
				m_tDebugRender.fFadeSpeed *= -1.f;
			}
			if (m_tDebugRender.fFadeAmount < 0.f)
			{
				m_tDebugRender.bFadeInOut = false;
				m_tDebugRender.bShake = false;
				m_tDebugRender.fShakeAmount = 0.1f;
				m_tDebugRender.fFadeAmount = 0.f;
			}
		}
	}

}

HRESULT CRenderer::Render_Target(const _tchar* _pRenderTargetTag)
{
	// Value
	RH_VALUE_VEC Values{ RH_VALUE(m_WorldMatrix, "g_WorldMatrix"),  RH_VALUE(m_ViewMatrix, "g_ViewMatrix") , RH_VALUE(m_ProjMatrix, "g_ProjMatrix") };
	// Texture
	RH_TEX_VEC Texes{ RH_TEX(_pRenderTargetTag, "g_Texture") };

	if (FAILED(m_pRenderHandler->Rendering(SHADER_DEFERRED, 0, Values, Texes)))
		return E_FAIL;

	return S_OK;
}


//#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
#ifdef _DEBUG
	for (_uint i = 0; i < (_uint)DEBUG_END; i++)
	{
		for (auto& iter : m_Components[i])
		{
			if (m_tDebugRender.bDebug[i] && nullptr != iter)
				iter->Render_Debug();

			Safe_Release(iter);
		}
		m_Components[i].clear();
	}
#endif

	if (m_tDebugRender.bDebugMRT)
	{
		m_pTarget_Manager->Render_Debug(MRT_DEFERRED);
		m_pTarget_Manager->Render_Debug(MRT_LIGHTACC);
		m_pTarget_Manager->Render_Debug(MRT_SHADOW);
		m_pTarget_Manager->Render_Debug(MRT_ALPHABLEND);
		m_pTarget_Manager->Render_Debug(MRT_DISTORTION);
	}

	return S_OK;
}

//#endif

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRenderer* pInstance = new CRenderer(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Renderer::Create To Failed");
	}
	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	++m_dwRefCnt;
	return this;
}

void CRenderer::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPostProcessing);
	Safe_Release(m_pRenderHandler);

#ifdef _DEBUG
	for (_uint i = 0; i < DEBUG_END; i++)
	{
		for (auto& iter : m_Components[i])
			Safe_Release(iter);
		m_Components[i].clear();
	}

#endif
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderGroup[i])
			Safe_Release(iter);

		m_RenderGroup[i].clear();
	}

	__super::Free();
}
