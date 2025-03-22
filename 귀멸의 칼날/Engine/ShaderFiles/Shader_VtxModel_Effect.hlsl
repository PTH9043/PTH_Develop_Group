#include "Shader_Defines.hpp"

vector		g_vColor = vector(1.f, 1.f, 1.f, 1.f);
float2		g_vUVDir = float2(0.f, 0.f);
float2		g_vSliceUV_START;
float2		g_vSliceUV_END = float2(0.f, 0.f);
float2		g_vSliceUV_DELTA;
float2		g_vUV_Mul = float2(1.f, 1.f);
int			g_iSamplerMode = 0;
float		g_fAlpha = 1.f;
float		g_fTime;
float		g_fDeltaX;
float		g_fRatio;
bool		g_bCutBlack = false;
bool		g_bNoise = false;
bool		g_bMask = false;
bool		g_bGlow = false;
bool		g_bUVFlow = false;
float		g_fGlowValue = 0.f;
bool		g_ObjectMotionBlur = false;
float3		g_vScrollSpeedsX, g_vScrollSpeeds, g_vNoiseScale;
float2		g_vDirection1, g_vDirection2, g_vDirection3;
float		g_fDistortionScale, g_fDistortionBias;
texture2D	g_NoiseTexture;
texture2D	g_MaskTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (g_bUVFlow)
		Out.vTexUV = float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y);
	else
		Out.vTexUV = In.vTexUV;

	Out.vTexUV *= g_vUV_Mul;

	if (true == g_ObjectMotionBlur)
	{
		vector	vPrevPosition = mul(vector(In.vPosition, 1.f), g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float4 NORMAL = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
		float3 dir = vPosition.xyz - vPrevPosition.xyz;
		float a = dot(normalize(dir), normalize(NORMAL.xyz));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;
		else
			Out.vPosition = vPosition;
		float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
		velocity.y *= -1.f;
		Out.vVelocity.xy = velocity;
		Out.vVelocity.zw = Out.vPosition.zw;
	}

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vGlow	: SV_TARGET1;
	float4 vBlur	: SV_TARGET2;
	float4 vMotionBlur : SV_TARGET3;
};

struct RESULTSAMPLER
{
	vector vColor : SV_TARGET0;
	vector vNoise : SV_TARGET1;
};

RESULTSAMPLER InitSampler(float2 uv, int iMode, int iBasic)
{
	RESULTSAMPLER Out = (RESULTSAMPLER)0;

	if (0 == iMode)
	{
		if (0 == iBasic)
		{
			Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
			Out.vNoise = g_NoiseTexture.Sample(DefaultSampler, uv);
		}
		else
		{
			Out.vColor = g_DiffuseTexture.Sample(ClampSampler, uv);
			Out.vNoise = g_NoiseTexture.Sample(ClampSampler, uv);
		}
	}
	else if (1 == iMode)
	{
		Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
		Out.vNoise = g_NoiseTexture.Sample(DefaultSampler, uv);
	}
	else if (2 == iMode)
	{
		Out.vColor = g_DiffuseTexture.Sample(ClampSampler, uv);
		Out.vNoise = g_NoiseTexture.Sample(ClampSampler, uv);
	}
	else if (3 == iMode)
	{
		Out.vColor = g_DiffuseTexture.Sample(UWVCSampler, uv);
		Out.vNoise = g_NoiseTexture.Sample(UWVCSampler, uv);
	}
	else if (4 == iMode)
	{
		Out.vColor = g_DiffuseTexture.Sample(UCVWSampler, uv);
		Out.vNoise = g_NoiseTexture.Sample(UCVWSampler, uv);
	}

	return Out;
}

// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 0);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	vColor *= g_vColor;

	vColor.a *= g_fAlpha;

	Out.vDiffuse = vColor;

	if (g_bNoise)
		Out.vDiffuse *= vNoise;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

struct VS_OUT_NORMAL
{
	float4 vPosition : SV_POSITION;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};


VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	vector	vPrevPosition = mul(vector(In.vPosition, 1.f), g_PrevWorldMatrix);
	vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
	vPrevPosition = mul(vPrevPosition, g_ProjMatrix);


	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (true == g_ObjectMotionBlur)
	{
		float4 NORMAL = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
		float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
		float a = dot(normalize(dir), normalize(NORMAL.xyz));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;

		float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
		velocity.y *= -1.f;
		Out.vVelocity.xy = velocity;
		Out.vVelocity.zw = Out.vPosition.zw;
	}

	return Out;
}

struct PS_IN_NORMAL
{
	float4 vPosition : SV_POSITION;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};


PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bNoise)
	{
		if (0.58f <= vNoise.r)
		{
			vector TempColor = vector(1.f, 0.5f, 0.f, 1.f);
			vNoise *= TempColor;
			vColor.rgb += vNoise.r;
		}
	}

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

PS_OUT EFFECT_DASHTRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bNoise)
	{
		if (0.01f < vColor.r)
		{
			vColor.rgb = vNoise.rgb;
		}
	}

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

PS_OUT EFFECT_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bNoise)
	{
		if (0.58f <= vNoise.r)
		{
			vector TempColor = vector(1.f, 0.5f, 0.f, 1.f);
			vNoise *= TempColor;
			vColor.rgb += vNoise.r;
		}
	}

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}
	return Out;
}

PS_OUT GUARD(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse *= g_vColor;

	if (g_bNoise)
	{
		if (0.58f <= vNoise.r)
		{
			vector TempColor = vector(1.f, 0.5f, 0.f, 1.f);
			vNoise *= TempColor;
			Out.vDiffuse.rgb += vNoise.r;
		}
	}

	if (g_bCutBlack)
		Out.vDiffuse.a = Out.vDiffuse.r;

	vector vColor2 = vector(0.f, 0.f, 1.f, 0.5f);

	if (0.f == Out.vDiffuse.r)
		Out.vDiffuse *= vColor2;

	//Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

// Aura

struct VS_OUT_AURA
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;

	float2 vNoiseUV1 : TEXCOORD4;
	float2 vNoiseUV2 : TEXCOORD5;
	float2 vNoiseUV3 : TEXCOORD6;
};

VS_OUT_AURA VS_MAIN_AURA(VS_IN In)
{
	VS_OUT_AURA		Out = (VS_OUT_AURA)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	vector	vPrevPosition = mul(vector(In.vPosition, 1.f), g_PrevWorldMatrix);
	vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
	vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	//Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (g_bUVFlow)
		Out.vTexUV = float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y);
	else
		Out.vTexUV = In.vTexUV;

	Out.vTexUV *= g_vUV_Mul;

	// 노이즈
	{
		float3 scrollSpeedsX = g_vScrollSpeedsX;
		float3 scrollSpeeds = g_vScrollSpeeds;
		float3 scales = g_vNoiseScale;
		Out.vNoiseUV1 = (In.vTexUV * scales.x);
		Out.vNoiseUV1.x = Out.vNoiseUV1.x - (g_fTime * scrollSpeedsX.x);
		Out.vNoiseUV1.y = Out.vNoiseUV1.y - (g_fTime * scrollSpeeds.x);

		Out.vNoiseUV2 = (In.vTexUV * scales.y);
		Out.vNoiseUV2.x = Out.vNoiseUV2.x - (g_fTime * scrollSpeedsX.y);
		Out.vNoiseUV2.y = Out.vNoiseUV2.y - (g_fTime * scrollSpeeds.y);

		Out.vNoiseUV3 = (In.vTexUV * scales.z);
		Out.vNoiseUV3.x = Out.vNoiseUV3.x - (g_fTime * scrollSpeedsX.z);
		Out.vNoiseUV3.y = Out.vNoiseUV3.y - (g_fTime * scrollSpeeds.z);
	}

	// 모션블러
	if (true == g_ObjectMotionBlur)
	{
		float4 NORMAL = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
		float3 dir = vPosition.xyz - vPrevPosition.xyz;
		float a = dot(normalize(dir), normalize(NORMAL.xyz));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;
		else
			Out.vPosition = vPosition;
		float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
		velocity.y *= -1.f;
		Out.vVelocity.xy = velocity;
		Out.vVelocity.zw = Out.vPosition.zw;
	}

	return Out;
}

struct PS_IN_AURA
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;

	float2 vNoiseUV1 : TEXCOORD4;
	float2 vNoiseUV2 : TEXCOORD5;
	float2 vNoiseUV3 : TEXCOORD6;
};

PS_OUT PS_MAIN_AURA(PS_IN_AURA In)
{
	PS_OUT			Out = (PS_OUT)0;

	/*if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}*/

	//Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y));

	float2 distortion1 = g_vDirection1;
	float2 distortion2 = g_vDirection2;
	float2 distortion3 = g_vDirection3;

	vector noise1 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV1);
	vector noise2 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV2);
	vector noise3 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV3);

	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	vector finalNoise = noise1 + noise2 + noise3;

	float perturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	float2 noiseCoords = (finalNoise.xy * perturb) + In.vTexUV;

	Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, noiseCoords.xy);

	Out.vDiffuse *= g_vColor;

	if (g_bCutBlack)
	{
		vector maskColor = g_MaskTexture.Sample(DefaultSampler, In.vTexUV);
		Out.vDiffuse.a = Out.vDiffuse.r;
	}

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

PS_OUT TEST6(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}



	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse *= g_vColor;

	if (g_bNoise)
	{
		if (0.58f <= vNoise.r)
		{
			vector TempColor = vector(1.f, 0.5f, 0.f, 1.f);
			vNoise *= TempColor;
			Out.vDiffuse.rgb += vNoise.r;
		}
	}

	vector maskColor = g_MaskTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse.a = maskColor.r;


	if (g_bCutBlack)
		Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

PS_OUT EFFECT_WATERFALL(PS_IN_AURA In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 0);
	Out.vDiffuse = SamplerOut.vColor;

	if (g_bNoise)
	{
		float2 distortion1 = float2(0.1f, 0.2f);
		float2 distortion2 = float2(0.1f, 0.3f);
		float2 distortion3 = float2(0.1f, 0.1f);

		float distortionScale = 0.8f;
		float distortionBias = 0.5f;

		vector noise1 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV1);
		vector noise2 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV2);
		vector noise3 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV3);

		noise1 = (noise1 - 0.5f) * 2.0f;
		noise2 = (noise2 - 0.5f) * 2.0f;
		noise3 = (noise3 - 0.5f) * 2.0f;

		noise1.xy = noise1.xy * distortion1.xy;
		noise2.xy = noise2.xy * distortion2.xy;
		noise3.xy = noise3.xy * distortion3.xy;

		vector finalNoise = noise1 + noise2 + noise3;

		float perturb = ((1.0f - In.vTexUV.y) * distortionScale) + distortionBias;

		float2 noiseCoords = (finalNoise.xy * perturb) + In.vTexUV;

		Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, noiseCoords.xy);
	}

	if (g_bMask)
	{
		vector vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexUV);
		Out.vDiffuse.a = vMask.r;
	}

	if (g_bCutBlack)
		Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDiffuse.a *= fDistance;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}



//====================================================================================================

float2 center = float2(0.5, 0.5);

int OCTAVES = 6;

float rand(float2 coord) {
	return frac(sin(dot(coord, float2(12.9898, 78.233))) * 43758.5453123);
}

float noise(float2 coord) {
	float2 i = floor(coord);
	float2 f = frac(coord);

	// 4 corners of a rectangle surrounding our point
	float a = rand(i);
	float b = rand(i + float2(1.0, 0.0));
	float c = rand(i + float2(0.0, 1.0));
	float d = rand(i + float2(1.0, 1.0));

	float2 cubic = f * f * (3.0 - 2.0 * f);

	return lerp(a, b, cubic.x) + (c - a) * cubic.y * (1.0 - cubic.x) + (d - b) * cubic.x * cubic.y;
}

float fbm(float2 coord) {
	float value = 0.0;
	float scale = 0.5;

	for (int i = 0; i < OCTAVES; i++) {
		float n = noise(coord);
		if (g_bNoise)
			n = g_DiffuseTexture.Sample(DefaultSampler, coord);
		value += n * scale;
		coord *= 2.0;
		scale *= 0.5;
	}
	return value;
}

float egg_shape(float2 coord, float radius) {
	float2 diff = abs(coord - center);

	if (coord.y < center.y) {
		diff.y /= 2.0;
	}
	else {
		diff.y *= 2.0;
	}

	float dist = sqrt(diff.x * diff.x + diff.y * diff.y) / radius;
	float value = clamp(1.0 - dist, 0.0, 1.0);
	return clamp(value * value, 0.0, 1.0);
}

float4 fragment(float2 UV) {
	float2 scaled_coord = UV * 6.0;

	float warp = UV.y;
	float dist_from_center = abs(UV.x - 0.5) * 4.0;
	if (UV.x > 0.5) {
		warp = 1.0 - warp;
	}

	float4 color = 0.f;
	float2 warp_vec = float2(warp, 0.0);
	float motion_fbm = fbm(scaled_coord + float2(g_fTime * 0.4, g_fTime * 1.3)); // used for distorting the smoke_fbm texture
	float smoke_fbm = fbm(scaled_coord + float2(0.0, g_fTime * 1.0) + motion_fbm + warp_vec * dist_from_center);

	float egg_s = egg_shape(UV, 0.5);

	float thres = 0.1;
	smoke_fbm *= egg_s;
	smoke_fbm = clamp(smoke_fbm - thres, 0.0, 1.0) / (1.0 - thres);
	if (smoke_fbm < 0.1) {
		smoke_fbm *= smoke_fbm / 0.1;
	}
	smoke_fbm /= egg_s;
	smoke_fbm = sqrt(smoke_fbm);
	smoke_fbm = clamp(smoke_fbm, 0.0, 1.0);

	color = float4(smoke_fbm.xxx, 1.f);

	//float4 color = float4(vec3(egg_s), 1.0);
	return color;
}

PS_OUT PS_SMOKENOISE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = fragment(In.vTexUV);

	if (g_bCutBlack)
		Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}
	return Out;
}

//============================================================================================================

PS_OUT PS_NOISECUSTOM(PS_IN_AURA In)
{
	PS_OUT			Out = (PS_OUT)0;

	/*if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}*/

	//Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y));

	float2 distortion1 = g_vDirection1;
	float2 distortion2 = g_vDirection2;
	float2 distortion3 = g_vDirection3;

	vector noise1 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV1);
	vector noise2 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV2);
	vector noise3 = g_NoiseTexture.Sample(DefaultSampler, In.vNoiseUV3);

	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	vector finalNoise = noise1 + noise2 + noise3;

	float perturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	float2 noiseCoords = (finalNoise.xy * perturb) + In.vTexUV;

	Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, noiseCoords.xy);

	vector maskColor = g_MaskTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a = maskColor.r;
	if (g_bCutBlack)
		Out.vDiffuse.a = Out.vDiffuse.r;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}


PS_OUT PS_TEST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bNoise)
	{
		vector vResult = vColor * vNoise * 2.f;

		vColor = saturate(vResult);
	}

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}


PS_OUT PS_TEST2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bNoise)
	{
		vector vResult = vColor * vNoise * 2.f;

		vColor = saturate(vResult);
	}

	if (g_bCutBlack)
	{
		if (0.1f > vColor.r)
			discard;
	}

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}

//=============================================================================================================


// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_ALPHATEST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	float2	vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 0);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor *= g_vColor;

	vColor.a *= g_fAlpha;

	Out.vDiffuse = vColor;

	if (g_bNoise)
		Out.vDiffuse *= vNoise;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_SMOKE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	/*float fTemp = smoothstep(0.f, 1.f, length(vNoise.rgb));

	if (fTemp < g_fRatio + 0.5f)
		Out.vDiffuse = vNoise;*/


	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}


	if (g_fRatio > vNoise.r)
		discard;

	return Out;
}

PS_OUT PS_REVERSAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	RESULTSAMPLER SamplerOut = InitSampler(In.vTexUV, g_iSamplerMode, 1);
	vector vColor = SamplerOut.vColor;
	vector vNoise = SamplerOut.vNoise;

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	Out.vDiffuse = vColor;

	Out.vDiffuse *= g_vColor;

	Out.vDiffuse.a *= g_fAlpha;

	/*float fTemp = smoothstep(0.f, 1.f, length(vNoise.rgb));

	if (fTemp < g_fRatio + 0.5f)
		Out.vDiffuse = vNoise;*/


	if (g_bGlow)
	{
		Out.vGlow = Out.vDiffuse;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);
	}

	Out.vDiffuse = float4((1.f - Out.vDiffuse.r), (1.f - Out.vDiffuse.g), (1.f - Out.vDiffuse.b), Out.vDiffuse.a);


	return Out;
}


technique11	DefaultTechnique
{
	// 0
	pass EffectDefault
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass EffectStaticMeshPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	// 2
	pass DashTrail
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 EFFECT_DASHTRAIL();
	}

	// 3
	pass DashTrail_NonDepth
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 EFFECT_DASHTRAIL();
	}

	// 4
	pass EffectNormal
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 EFFECT_NORMAL();
	}

	// 5
	pass Guard
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 GUARD();
	}

	// 6
	pass SmokeNoise
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKENOISE();
	}

	// 7
	pass WaterFall
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_AURA();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 EFFECT_WATERFALL();
	}

	// 8
	pass Noise
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_AURA();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AURA();
	}

	// 9
	pass NoiseNonDepth
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_AURA();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AURA();
	}

	// 10
	pass EffectAlphaTest
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	// 11 디퓨즈, 노이즈 곱하기
	pass EffectTest
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TEST();
	}

	// 12
	pass EffectTest2
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TEST2();
	}

	// 13 : Target_Depth 이용
	pass EffectPass3Depth
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	// 14
	pass DefualtNonDepth
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 15
	pass Smoke
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKE();
	}

	// 16
	pass COLOR_REVERSAL
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_REVERSAL();
	}
}

