#pragma once
namespace Engine {

#pragma region VIEWPROJ

	typedef struct tagViewProjMatrix {

		_float4x4 mViewMatrix{ _float4x4::Identity };
		_float4x4 mProjectionMatrix{ _float4x4::Identity };
		_float4x4 mPrevViewMatrix{ _float4x4::Identity };
		_float		   fCamFar{ 0.f };
		_float3	  vCamPosition{};
	}VIEWPROJMATRIX;

	typedef struct tagViewProjParam {
		ARRAY<VIEWPROJMATRIX, MAX_CAM_SIZE>	arrViewProjParams{};
		_float																			 fGrobalDeltaTime{ 0 };
		_bool																			isObjectMotionBlur{ false };
		_bool																			isViewProjBoolPadding1{ false };
		_bool																			isViewProjBoolPadding2{ false };
		_bool																			isViewProjBoolPadding3{ false };
		// Padding Value
		_float2																		vViewProjPaddings{ 0.f, 0.f };
	}VIEWPROJPARAM;

#pragma endregion VIEWPROJ

#pragma region GROBADATA

	/*
	@ Date: 2024-04-10, Writer: 박태현
	@ Explain
	-  머터리얼을 담아놓은 Param
	*/
	struct MATERIALGROBALINFO
	{
		// Model MaterialInfo
		MODELMATERIALINFO		stModelMaterialInfoGroup[MAX_MATERIALPARAM_CNT];
	};

	struct GLOBALPARAM
	{
		//MATERIALGROBALINFO		stMaterialGrobalInfo;
	};

#pragma endregion GLOBALDATA

#pragma region TRANSFORM 

	typedef struct tagTransformMatrix 
	{
		_float4x4 mWorldMatrix{ _float4x4::Identity };
		_float4x4 mPrevWorldMatrix{ _float4x4::Identity };
		_uint		iCamIndex{ 0 };
		_int			iMotionBlurOn{ 0 };
		_int2		iPadding;
	}TRANSFORMPARAM;

#pragma endregion TRANSFORM

#pragma region MATERIAL 



#pragma endregion MATERIAL 

#pragma region LIGHT 

	struct LIGHTCOLOR
	{
		LIGHTCOLOR() = default;
		LIGHTCOLOR(const _float4& _vDiffuse, const _float4& _vAmbient, const _float4& _vSpecular) :
			vDiffuse(_vDiffuse), vAmbient(_vAmbient), vSpecular(_vSpecular)
		{}

		_float4      vDiffuse{ 0.f, 0.f, 0.f, 0.f };
		_float4      vAmbient{ 0.f, 0.f, 0.f, 0.f };
		_float4      vSpecular{ 0.f, 0.f, 0.f, 0.f };
	};

	struct RIMLIGHT
	{
		RIMLIGHT() = default;
		RIMLIGHT(const _float _fRimLightPower, const _float _fRimMin, const _float _fRimMax)
			: fRimLightPower(_fRimLightPower), fRimMin(_fRimMin), fRimMax(_fRimMax)
		{}

		// RimLight
		_float			fRimLightPower = 10.f;
		_float			fRimMin = 0.f;
		_float			fRimMax = 0.3f;
	};

	struct LIGHTPOWER
	{
		LIGHTPOWER() = default;
		LIGHTPOWER(const _float _ffLightPower, const _float _fSpecularPower)
			: fLightPower(_ffLightPower), fSpecularPower(_fSpecularPower)
		{}

		_float         fLightPower{ 0 };
		_float         fSpecularPower{ 0 };
	};


	typedef struct tagLightParam
	{
		_float3					vCamPos = _float3(0.f, 0.f, 0.f);
		_float						 fCamFar{ 0.f };

		_float4x4				mScreenWorldMatrix{ _float4x4::Identity };
		_float4x4				mScreenViewmatrix{ _float4x4::Identity };
		_float4x4				mScreenProjMatrix{ _float4x4::Identity };
		_float4x4				mViewMatrix{ _float4x4::Identity };
		_float4x4				mProjMatrix{ _float4x4::Identity };
		_float4x4				mViewMatrixInv{ _float4x4::Identity };
		_float4x4				mProjMatrixInv{ _float4x4::Identity };
	}LIGHTPARAM;

	typedef struct tagLightInfo
	{
		tagLightInfo() = default;
		tagLightInfo(const LIGHTTYPE& _eLightInfo,const LIGHTACTIVE& _eLightActive, const _float4& _vDiffuse, const _float4& _vAmbient, const _float4& _vSpecular,
			const _float3& _vDirection, const _float3& _vPosition, const _float& _fRange, const _float& _fAngle = 0.f,
			const _float _fLightPower = 1.f, const _float _fSpecularPowValue = 1.f
		,const _float _fFallOff=0.f,const _float _fPhi=0.f,const _float _fTheta=0.f
			,const _float3 _vAttenuation=_float3(0,0,0))
			: eLightType(_eLightInfo), eLightActive(_eLightActive), vDiffuse(_vDiffuse), vAmbient(_vAmbient), vSpecular(_vSpecular)
			, vDirection(_vDirection), vPosition(_vPosition),
			fRange(_fRange), fAngle(_fAngle), fLightPower(_fLightPower), fSpecularPowValue( _fSpecularPowValue)
			,fFallOff(_fFallOff), fPhi(_fPhi), fTheta(_fTheta), vAttenuation(_vAttenuation)
		{}
		_float4					vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
		_float4					vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
		_float4					vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
		_float4					vDirection = _float4(0.f, 0.f, 0.f, 0.f);
		_float4					vPosition = _float4(0.f, 0.f, 0.f, 0.f);
		// 4 =====================
		LIGHTTYPE			eLightType = LIGHTTYPE::LIGHT_END;
		LIGHTACTIVE    eLightActive = LIGHTACTIVE::ISACTIVE;
		_float						fRange = 0.f;
		_float						fAngle{ 0.f };
		_float						fLightPower{ 1.f };
		// 4 =====================
		_float						fSpecularPowValue{ 1.f };
		//2024-4-8 서정원 용빠 조명을 위한 추가
		_float						fFallOff=5.0f;
		_float						fPhi= 0.f;//Phi 가 더 커야함
		_float						fTheta=0.f;
		_float3					vAttenuation=_float3(1.0f,0.001f,0.0001f);
		

	}LIGHTINFO;

	typedef struct tagLightControlParam
	{
		LIGHTINFO								tLightInfo;
	}LIGHTCONTROLPARAM;

	typedef struct tagLightsControlParam {
		ARRAY<LIGHTINFO, MAX_LIGHTS> tLightInfos;
		_uint    nLights;
	}LIGHTPARAMS;
#pragma endregion LIGHT

#pragma region MODELDATAPARAM

	struct MODELDATAPARAM {

		_uint					iMaterialIndex;
		_float3				PaddingValue;
	};

	

#pragma endregion MODELDATAPARAM

#pragma region ANIMATIONPARAM

	typedef struct tagAnimationParam
	{
		_int		iOutLineExist = 0;
		_int		iNomralExist = 1;
		_int		iObjectDissolve = 0;
		_float	fDissolveTimer = 0;
	}ANIMATIONPARAM;

	typedef struct tagDrawRim
	{
		_int		_drawRim = false;
		_float3     RimRGB = _float3(1, 0, 0);
	}RIMDRAW;

#pragma endregion ANIMATIONPARAM

#pragma region BONEPARAM

	struct SOCKETMATRIXPARAM
	{
		_float4x4 SocketMatrix;
	};

	typedef struct tagBoneMatrixParam
	{
		ARRAY<_float4x4, MAX_BONE_SIZE>		arrBoneMatrix;
	}BONEMATRIXPARAM;

	struct MODELPIVOTMATRIX {
		_float4x4	PivotMatrix;
	};

#pragma endregion BONEPARAM

#pragma region PARTICLE

	struct GLOBALPARTICLEINFO
	{
		GLOBALPARTICLEINFO() : iMaxCount{ 0 }, iAddCount{ 0 },
			 fDeltaTime{ 0 }, fAccTime{ 0 },
			fMinLifeTime{ 0 }, fMaxLifeTime{ 0 }, fMinSpeed{ 0 },
			fMaxSpeed{ 0 }, fStartScaleParticle{ 0 }, fEndScaleParticle{ 0 }, fParticleThickness{ 0 }, fParticleKind{ 0 },
			fParticleDirection{ _float3(0,0,0) }, fAnimSizeX{ 0 }, fAnimSizeY{ 0 }, fParticlePosition{ _float3(0,0,0) }, fNextAnimTime{ 0 }	, fPadding{ _float3(0,0,0) }
		{}

		_uint		iMaxCount;
		_uint		iAddCount;
		_float		fDeltaTime;
		_float		fAccTime;
		// ==============
		
		_float		fMinLifeTime;
		_float		fMaxLifeTime;
		_float		fMinSpeed;
		_float		 fMaxSpeed;
		// ===============
		
		_float		fStartScaleParticle;
		_float		fEndScaleParticle;
		_float		fParticleThickness;
		_uint		fParticleKind;

		_float3     fParticleDirection;
		_float     fAnimSizeX;
		//================
		
		
		_float      fAnimSizeY;
		_float3	fParticlePosition;
		
		_float   fNextAnimTime;
		_float3 fPadding;
		
		
		
	};

	struct PARTICLE
	{
		PARTICLE() : vWorldPos{ 0, 0, 0 }, fCurTime{ 0 }, vWorldDir{ 0, 0, 0 }, fLifeTime{ 0 }, iAlive{ 0 }, vAnimUV{ 0,0 }, vPadding { 0 }
		{}
		_float3		vWorldPos;
		_float			fCurTime;
		// ===============
		_float3		vWorldDir;
		_float			fLifeTime;
		// ==============
		_uint			iAlive;
		_float2  vAnimUV;
		_float  vPadding;
		
	};

	struct PARTICLEPLUS 
	{
		PARTICLEPLUS():vWorldPos{ 0, 0, 0 }, fCurTime{ 0 }, vWorldDir{ 0, 0, 0 }, fLifeTime{ 0 }, iAlive{ 0 }, fScaleFactor{ 0,0,0 }
		,fStartTime{0},fAmp{0},fPeriod{0},PlusPadding{0}{}
		_float3		vWorldPos;
		_float			fCurTime;
		// ===============
		_float3		vWorldDir;
		_float			fLifeTime;
		// ==============
		_uint			iAlive;
		_float3 fScaleFactor;
		//===============
		_float fStartTime;
		_float fAmp;
		_float fPeriod;
		_float PlusPadding;
	};

	struct ComputeParticleType {
		ComputeParticleType() :fParticleType{ PARTICLE_TYPE_DEFAULT }, fParticleLifeTimeType{ PARTICLE_LIFETIME_TYPE_DEFAULT }, padding { 0, 0 } {}
		PARTICLE_TYPE fParticleType;
		PARTICLE_LIFETIME_TYPE fParticleLifeTimeType;
		_float2 padding;
	};

	typedef struct tagParticleParam
	{
		GLOBALPARTICLEINFO stGlobalParticleInfo{};
	}PARTICLEPARAM;


	struct COMPUTESHADERINFO
	{
		_uint iAddCount{ 0 };
		_int3 padding{};
	};

#pragma endregion PARTICLE
#pragma region BLOOD
	struct BLOODTIMER
	{
		_float m_fFullTime{};
		_float m_fLeftTime{};
	};

#pragma endregion BLOOD
#pragma region FIRE

	struct FIRENOISEBUFFER {
		_float fFrameTime;
		_float3 fScrollSpeeds;
		_float3 fScales;
		_float padding;
	};

	struct FIREDISTORTIONBUFFER {
		_float2 fDistortion1;
		_float2 fDistortion2;
		_float2 fDistortion3;
		_float fDistortionScale;
		_float fDistortionBias;
	};

#pragma endregion FIRE

#pragma region FOG

	struct FOGBUFFER {
		_float fFogStart;
		_float fFogEnd;
		_float fFogDensity;
		_uint fType;
		_float4 fFogColor;
	};

	

#pragma endregion FOG

#pragma region RENDERSHADER
	struct DRAWSHADERBUFFER {
		_uint m_bGameStart{};
		_uint m_bTurnFog{};
		_uint m_bTurnDie{};
		_uint m_bTurnAbility{};
		_uint m_bTurnHit{};
		_float m_fGameStartTime;
		_float m_fDieTime;
		_float m_fAbilityTime;
		_float m_fHitTime;
		_float3 _padding;
	};

#pragma endregion RENDERSHADER
#pragma region WATER

	struct WATERIFNODESC
	{
		_float2 vWaterNormalMapOffset{}; // 물 표면의 노말 맵 이동 오프셋
		_float fWaveAmplitude{ 0.f }; // 파고
		_float fWaveFrequency{ 0.f }; // 파동 빈도
		_float fWaveSpeed{ 0.f }; // 파동 속도

		_float Padding;
	};

	struct WATEROBJECTPARAM
	{
		WATERIFNODESC   waterStruct;
		_float4x4				   g_mTextureAnimation;
	};

#pragma endregion WATER

#pragma region DEBUGGING

	typedef struct tagDebuggingParam
	{
		_float4			vDebugging{ 0.f, 0.f, 0.f, 0.f };
	}DEBUGGINPARAM;

#pragma endregion DEBUGGING


#pragma region DEAULTOBJECT

	struct NORMALCUBEPARAM
	{
		_float4	g_vNormalCubeColor{ 1.f, 1.f, 1.f, 1.f };
		_int			g_iNormalCubeTextureBind{ 0 };
		_int3		g_iNormalCubePadding{ 0, 0, 0 };
	};

#pragma endregion  DEFAULTOBEJCT

#pragma region TERRAINTESS

	struct TERRAINTESSPARAM
	{
		_int2        iTiles;
		_float2     vHeightMapResolution;
		_float2     vMinMaxTessellationDistance;
		_float       fTesselationMaxLevel;
		_float       fTessPadding;

		TERRAINTESSPARAM() :
			iTiles{ 0, 0 }, vHeightMapResolution{ 0, 0 }, vMinMaxTessellationDistance{ 0, 0 },
			fTesselationMaxLevel{ 0 }, fTessPadding{ 0 }
		{}
	};

#pragma endregion TERRAINTESS
}