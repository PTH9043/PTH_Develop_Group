#pragma once

#include "UComponent.h"

BEGIN(Engine)
class UShader;
class UComponent;
class UPipeLine;
class USceneManager;
class UVIBufferRect;
class UVIBufferCube;
class URenderTargetManager;
class URenderHandler;
class UDefferedCamera;
class USmallDefferedCamera;
class UShadowCamera;
class UShaderConstantBuffer;
class UGraphicDevice;
class UGlobalConstantBuffer;
class UComputeManager;
class UPawn;
class URenderTargetGroup;


class URenderer final : public UComponent{
	using PAWNLIST = LIST<SHPTR<UPawn>>;
	using DRAWINGRENDERCONTAINER = UNORMAP<_wstring, PAWNLIST>;
	using SHADERODERS = UNORMAP<_wstring, SHPTR<UShader>>;
public:
#ifdef _USE_DEBUGGING
	typedef struct tagRenderDebugging
	{
		_bool		isShowRenderTarget{ false };
		_bool		isShowComponents{ false };
	}RENDERDEBUGGING;
#endif

	URenderer(CSHPTRREF <UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand,
		CSHPTRREF<UGraphicDevice> _spGraphicDevice, 	CSHPTRREF<UPipeLine> _spPipeLine,
		CSHPTRREF<USceneManager> _spSceneManager, CSHPTRREF<URenderTargetManager> _spRenderTargetManager,
		CSHPTRREF<UComputeManager> _spComputeManager);
	NO_COPY(URenderer)
	DESTRUCTOR(URenderer)
public:
#ifdef _USE_DEBUGGING
	RENDERDEBUGGING& GetRenderDebugging() { return m_stRenderDebugging; }
#endif
public:
	CLONE_THIS(URenderer)
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vectDatas) override;

	HRESULT AddRenderGroup(const RENDERID _eID, CSHPTRREF<UShader> _spShader, CSHPTRREF<UPawn> _spPawn);
#ifdef _USE_DEBUGGING
	HRESULT AddDebugRenderGroup(const DEBUGRENDERID _eID,
		CSHPTRREF<UShader> _spShader, CSHPTRREF<UPawn> _spPawn);
#endif
	void Tick(const _double& _dTimeDelta);
	HRESULT Render();
	void ClearRenderingData();
	void ClearAllData();
	// Render Other Camera
	void RenderOtherCamera();
	void BindDefferedTransform(SHPTR<UShader> _spShader);
	void TurnOnFog() { m_bTurnShader.m_bTurnFog = true; }
	void TurnOffFog() { m_bTurnShader.m_bTurnFog = false; }
	void SetGameStartEffect() { m_bTurnShader.m_bGameStart = true; }
	void TurnOnDieEffect() { m_bTurnShader.m_bTurnDie = true; }
	void TurnOffDieEffect() { m_bTurnShader.m_bTurnDie = false; }
	void TurnOnAbilityEffect() { if (!m_bTurnShader.m_bTurnHit && !m_bTurnShader.m_bTurnDie)m_bTurnShader.m_bTurnAbility = true; }
	_bool GetIfAbilityIsOn() { return m_bTurnShader.m_bTurnAbility; }
	_float GetAbilityTime() { return m_bTurnShader.m_fAbilityTime; }
	void TurnOffAbilityEffect() { m_bTurnShader.m_bTurnAbility = false; }
	void TurnOnHitEffect() { if (!m_bTurnShader.m_bTurnDie)m_bTurnShader.m_bTurnHit = true; }
	void TurnOffHitEffect() { m_bTurnShader.m_bTurnHit = false; }
private:
	void RenderRTs();
	// Priority
	void RenderPriority();
	//Pos+Normal for Outline
	void RenderPosNormal();
	// Shadow
	void	RenderShadowDepth();
	// NonAlpha
	void RenderNonAlphaBlend();
	// Render Lights
	void RenderLights();
	// Render Blend
	void RenderBlend();
	// Non Light
	void RenderNonLight();
	// Alpha Blend
	void RenderAlphaBlend();
	// Distortion
	void RenderDistortion();
	// MotionBlur
	void RenderMotionBlur();
	// Render 3D UI
	void Render3DUI();
	// UI 
	void Render2DUIPriority();
	void Render2DUIMiddle();
	void Render2DUILast();
	// HDR
	void RenderHDR();
	void RenderHDRTWO();
	void RenderGrayScale();
	// Bloom, Blur
	void RenderHorizontalBlur();
	void RenderVerticalBlur();
	void RenderBloom();
	void DownSample();
	void DownSample2();
	void UpSample();
	void UpSampleGray();
	// Render End 
	void RenderEnd();
#ifdef _USE_DEBUGGING
	void RenderDebug();
#endif
	SHPTR<UShader> FindShader(const _wstring& _wstrProto);
	// Draw Just Object
	void RenderObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList);
	void RenderShadowObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList);
	void RenderOutlineObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList, _bool _pass);
	

	SHPTR<UShader> FrameReadyDrawFinalRenderTarget(const _wstring& _wstrShaderName, const RTGROUPID _eGroupID);
	SHPTR<UShader> FrameReadyDrawLast(const _wstring& _wstrShaderName);

	void BindGrobalBuffer();
private:
	GLOBALPARAM																				m_stGlobalParam;
	SHPTR< UGlobalConstantBuffer>											m_spGlobalBuffer;
	_float																								m_fGrobalDeltaTime;
	// Deffered Transform Param		
	TRANSFORMPARAM																	 m_stFinalRenderTransformParam;
	TRANSFORMPARAM																	 m_stSmallRenderTransformParam;
	SHPTR<UShaderConstantBuffer>											m_spTransformConstantBuffer;
	SHPTR<UDefferedCamera>														m_spDefferedCamera;
	SHPTR<USmallDefferedCamera>														m_spSmallDefferedCamera;
	SHPTR<UShadowCamera>														m_spShadowCamera;
	// Actives 																							 
	ARRAY<DRAWINGRENDERCONTAINER, RI_END>					m_arrActiveDrawRenderList;
	//Shader Container 																		   
	SHADERODERS																			m_ShaderObjects;
	// Manager																						  
	SHPTR<UGraphicDevice>															m_spGraphicDevice;
	SHPTR<UPipeLine>																		m_spPipeLine;
	SHPTR<USceneManager>															m_spSceneManager;
	SHPTR<URenderTargetManager>											m_spRenderTargetManager;
	SHPTR<UComputeManager>													m_spComputeManager;
	SHPTR<UCommand>																	m_spCastingCommand;
	SHPTR<UVIBufferRect>																m_spVIBufferPlane;
	_short																								m_sNonAlphaBlendIndex;

	DRAWSHADERBUFFER																	m_bTurnShader;
	SHPTR<UShaderConstantBuffer>											m_spTurnShaderConstantBuffer;
	_int TurnDie = 0;
	_int TurnHit = 0;
	_int TurnAbility = 0;

#ifdef _USE_DEBUGGING																	
	 ARRAY<DRAWINGRENDERCONTAINER, DEBUG_RI_END>	 m_arrDrawDebugRenderList;
	RENDERDEBUGGING																	m_stRenderDebugging;
#endif
};

END
