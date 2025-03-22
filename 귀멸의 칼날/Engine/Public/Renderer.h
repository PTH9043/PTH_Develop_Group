#pragma once
#include "Component.h"

#define RENDERTEST 0

BEGIN(Engine)
class CPostProcessing;
class CTarget_Manager;
class CLight_Manager;

class CVIBuffer_Rect;
class CShader;
class CCollider;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	typedef struct tagDebugRender
	{
		// Shadow
		_float	fShadowDepth = 1.f;
		_float4	vShadowColor = _float4(0.7f, 0.7f, 0.7f, 1.f);

		// HDR
		_float	fMiddleGrey = 10.f;
		_float	fLumWhiteSqr = 80.f;
		_float	fLScalePlusValue = 1.f;

		// Blur
		_float	fBlur = 0.5f;

		// Shake
		_float	fShakeAmount = 0.1f;
		_float	fShakeDepth = 100.f;

		// FadeOut
		_uint	iFadeType = 0; // 0 : FadeIn, 1 : FadeOut, 2: FadeInOut
		_float	fFadeSpeed = 0.1f;
		_float	fFadeAmount = 0.f;
		_float4 vFadeColor = _float4(0.f, 0.f, 0.f, 1.f);

		// Fog
		_float	fogEnd = 1000.f;
		_float	fogStart = 1.f;
		_float4 fogColor = _float4(0.15f, 0.2f, 0.35f, 1.f);
		_int	fogType = 2;
		_float	fogExp = 0.001f;
		_float	fTime = 0.f;

		// Distortion
		_float2 v2UvFlowValue = _float2(0.2f, 2.0f);
		float	fMultipleTimeValue = 0.0125f;
		float	fDistortionDivineValue = 8.0f;

		// MotionBlur
		int iNumBlurValue = 50;
		int iDivineVelocityValue = 50;

		// Outline
		_float	minSeparation = 0.f;
		_float	maxSeparation = 0.4f;
		_float	minDistance = 0.f;
		_float	maxDistance = 10.f;
		_int	outlineSize = 1;
		_float3	colorModifier = _float3(0.f, 0.f, 0.f);

		// RimLight
		_float fRimLightPower = 3.f;
		_float fRimMin = 0.f;
		_float fRimMax = 1.f;

		// Com Render
		_bool	bDebugCom = false;
		_bool	bDebug[3] = { false };

		// ShaderEffect
		_bool	bHDR = true;
		_bool	bEdge = false;
		_bool	bMotionBlur = true;
		_bool	bShake = false;
		_bool	bOutline = true;
		_bool	bShadow = true;
		_bool	bDebugMRT = false;
		_bool	bFog = true;
		_bool	bFadeInOut = false;
		_bool	bShade = true;
		_bool	bRimlight = true;
		_bool	bEffect = true;
	}DEBUGRENDER;
public:
	enum RENDERID {
		RENDER_PRIORITY,RENDER_SHADOW, RENDER_NONALPHABLEND,
		RENDER_ALPHABLEND, RENDER_DISTORTION, RENDER_NONLIGHT, RENDER_UI, RENDER_END
	};

	enum RENDERDEBUG {
		DEBUG_MAP, DEBUG_PLAYER, DEBUG_ETC, DEBUG_END
	};

protected:

	explicit CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderer() = default;
public:
	void Clear();
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	HRESULT Add_RenderGroup(const RENDERID & _eRenderGroup, class CGameObject* pGameObject);
#ifdef _DEBUG
	// Collider
	HRESULT Add_Components(const RENDERDEBUG & _eDebugGroup, CComponent * _pComponents);
#endif 
	// Get DebugREnder
	DEBUGRENDER* Get_DebugRender() { return &m_tDebugRender; }

	void	Start_FadeInOut(_float fFadeSpeed, _bool bFadeWhite, _uint iFadeType);

public:
	HRESULT Draw();
private:
	// Priority
	HRESULT Render_Priority();
	// Shadow
	HRESULT	Render_ShadowLight();
	// NonAlphaBlend
	HRESULT Render_NonAlphaBlend();
	// Render Lights
	HRESULT Render_Lights();
	// Render Blend
	HRESULT Render_Blend();
	// Non Light
	HRESULT Render_NonLight();
	// AlphaBlend
	HRESULT Render_AlphaBlend();
	// Distortion
	HRESULT Render_Distortion();
	// Render Motion Blur
	HRESULT Render_MotionBlur();
	// Render Shake
	HRESULT Render_Shake();
	// HDR
	HRESULT Render_HDR();
	// UI 
	HRESULT Render_UI();

private:
	void	SetUp_FadeInOut();

	// BackBuffer
	HRESULT Render_Target(const _tchar * _pRenderTargetTag);

//#ifdef _DEBUG
	// Debug
	HRESULT Render_Debug();
//#endif

private:
	typedef list<class CGameObject*>			RENDERGROUPS;
	RENDERGROUPS								m_RenderGroup[RENDER_END];
#ifdef _DEBUG
	typedef list<CComponent*>					COMPONENTS;
	COMPONENTS									m_Components[DEBUG_END];
#endif
private:
	CPostProcessing* m_pPostProcessing = nullptr;
	CTarget_Manager* m_pTarget_Manager = nullptr;
	CLight_Manager* m_pLight_Manager = nullptr;

private:
	class CRenderHandler* m_pRenderHandler = nullptr;

private:
	_float4x4		m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	DEBUGRENDER		m_tDebugRender;

public:
	static CRenderer* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END