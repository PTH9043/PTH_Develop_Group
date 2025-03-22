#include "Shader_Defines.hpp"

texture2D	g_Texture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_ALPHATEST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

// -------------------------------------------------------------------------------NONALPHABLEND

texture2D	g_ShadeTexture;
texture2D	g_SpecularTexture;
texture2D	g_ShadowTexture;
texture2D	g_AmbientTexture;
texture2D	g_FogNoiseTexture;

float	fogEnd = 1000.f;
float	fogStart = 0.2f;
float4	fogColor = float4(0.15f, 0.2f, 0.35f, 1.f);
int		fogType = 2;
float	fogExp = 0.005f;
float	fTime = 0.f;
bool	bFog = false;

// 안개 쉐이더
void Fog_Effect(in float2 vUV, in float4 vDiffuse, out float4 vOut)
{
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);
	//vector	vFogDesc1 = g_FogNoiseTexture.Sample(DefaultSampler, vUV + float2(fTime * 0.03f, 0.f));
	//vector	vFogDesc2 = g_FogNoiseTexture.Sample(DefaultSampler, vUV + float2(fTime * 0.06f, 0.f));
	//vector  vFogFinal = vFogDesc1 + vFogDesc2;
	float	zDepth = vDepthDesc.r * g_CamFar;

	float fogFactor = 0.f;
	// Linear
	if (0 == fogType)
		fogFactor = saturate(max(fogEnd - zDepth, 0.f) / (fogEnd - fogStart));
	else if (1 == fogType)
		fogFactor = 1.f / exp(zDepth * fogExp);
	else if (2 == fogType)
		fogFactor = 1.f / exp(pow(zDepth * fogExp, 0.5f));

	vOut = lerp(fogColor, vDiffuse, fogFactor);//fogFactor * vDiffuse + (1.f - fogFactor) * fogColor * vFogFinal.r;
}

struct PS_OUT_BLEND
{
	float4 vColor : SV_TARGET0;
	//float4 vBrightness : SV_TARGET1;
};

bool	bShadow = false;

PS_OUT_BLEND PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT_BLEND			Out = (PS_OUT_BLEND)0;

	/* 디퓨즈 타겟에 그려져있던 색과 셰이드 타겟에 그려져있느 ㄴ색을 혼합하여 백버퍼에 그린다. */
	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadowDesc = g_ShadowTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vAmbientDesc = g_AmbientTexture.Sample(DefaultSampler, In.vTexUV);

	vector vColor = vDiffuseDesc * (vShadeDesc + vAmbientDesc + vSpecularDesc);

	if (bFog)
	{
		Fog_Effect(In.vTexUV, vColor, Out.vColor);
		vector vNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
		if (1.f == vNormal.a)
			Out.vColor = vColor;
	}
	else
		Out.vColor = vColor;

	Out.vColor *= vShadowDesc;

	Out.vColor.a = vDiffuseDesc.a;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

//--------------------------------------------------------------------------------------- BLOOM TEST


Texture2D g_BrightnessTexture;

static const float Weight[13] =
{
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2 dim;
	g_BrightnessTexture.GetDimensions(dim.x, dim.y);

	float du = 1.f / dim.x;
	float dv = 1.f / dim.y;

	vector vColor = 0;
	float2	uv = 0;
	float fTotal = 0;
	for (int i = -6; i < 6; ++i)
	{
		for (int j = -6; j < 6; j++)
		{
			uv = In.vTexUV + float2(i * du * 10.f, j * dv * 10.f);
			vColor += Weight[6 + i] * Weight[6 + j] * g_BrightnessTexture.Sample(ClampSampler, uv);
			fTotal += Weight[6 + i] * Weight[6 + j];
		}
	}
	vColor /= fTotal;

	vector vHDRColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vBloomColor = vColor;
	vector vBloomOriTex = g_BrightnessTexture.Sample(DefaultSampler, In.vTexUV);

	// 감마 공간 텍스쳐 -> 선형 공간 : pow(abs(vBloomColor), 2.2f)
	// 선형 공간 텍스쳐 -> 감마 공간 : pow(abs(vBloomColor), 1 / 2.2f)
	//	vector vBloom = pow(pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f), 1.f / 2.2f);
	//vector vBloom = pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f);
	//Out.vColor = pow(abs(pow(abs(vHDRColor), 2.2f) + vBloom), 1 / 2.2f);

	Out.vColor = vBloomColor * 2.f + vBloomOriTex + vHDRColor;

	return Out;
}

//====================================================================== GLOW

Texture2D g_GlowTexture;
Texture2D g_BlurDegreeTexture;

PS_OUT PS_GLOW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);			// 원본
	vector	vGlow = g_GlowTexture.Sample(DefaultSampler, In.vTexUV);				// 밝기 추출 블러
	vector	vBlurDegree = g_BlurDegreeTexture.Sample(DefaultSampler, In.vTexUV);	

	//Out.vColor = vColor + sqrt(vGlow) * sqrt(vBlurDegree.x);
	Out.vColor = vColor + vGlow * vBlurDegree.x;

	return Out;
}

//=============================================================================================

texture2D g_DistortionTexture;

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);			// 원본
	vector	vDistortion = g_DistortionTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor + vDistortion;

	return Out;
}


//==================================================================================================== HDR

float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);
float g_fMiddleGrey = 1.f;
float g_fLumWhiteSqr = 9.f;
float g_fLScalePlusValue = 1.f;

#define BLUR_ITERATIONS 3
#define BLUR_SIZE .03
#define BLUR_SUBDIVISIONS 32

float3 ToneMapping(float3 vHDRColor)
{
	float fLScale = dot(vHDRColor, LUM_FACTOR);
	fLScale *= g_fMiddleGrey;
	fLScale = (fLScale + fLScale * fLScale / g_fLumWhiteSqr) / (g_fLScalePlusValue + fLScale);
	return vHDRColor * fLScale;
}

texture2D g_HDRTexture;

PS_OUT PS_MAIN_HDR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);
	if (vColor.a == 0)
		discard;

	float3 vHDR = g_HDRTexture.Sample(PointSampler, In.vTexUV).xyz;

	vColor.xyz = ToneMapping(vColor.xyz + vHDR);

	Out.vColor = float4(vColor.xyz, 1.f);

	return Out;
}

texture2D g_MotionBlur;
texture2D g_JudgeDraw;

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = g_JudgeDraw.Sample(DefaultSampler, In.vTexUV);
	if (vColor.r == 0)
		discard;

	float4 vMotion = g_MotionBlur.Sample(PointSampler, In.vTexUV);

	Out.vColor = vMotion;

	return Out;
}


technique11			DefaultTechnique
{
	// 0 : Render MRT
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	// 1 : Render Blend
	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	// 2 : Render Glow
	pass Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLOW();
	}

	// 3
	pass CopyAlphaTest
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHATEST();
	}

	// 테스트
	// 4
	pass BlendBloom
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
	}

	// 5 : Render Distortion
	pass Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	// 6 : Render HDR
	pass HDR
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HDR();
	}

	// 7 : Render HDR
	pass MOTIONBLUR
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
	}
}
