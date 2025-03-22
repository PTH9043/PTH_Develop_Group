#include "Shader_Defines.hpp"

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
};

cbuffer NoiseDistortion
{
	float g_fDistortionScale = 0.8f;
	float g_fDistortionBias = 0.5f;
	float2 g_vDistortion1 = float2(0.1f, 0.2f);
	float2 g_vDistortion2 = float2(0.1f, 0.3f);
	float2 g_vDistortion3 = float2(0.1f, 0.1f);
}

cbuffer Noise
{
	float3 g_vScale = float3(1.f, 2.f, 3.f);
	float3 g_vScrollSpeed = float3(1.f, 2.1f, 2.3f);
}


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	Out.vTexCoord1 = (In.vTexUV * g_vScale.x);
	Out.vTexCoord2 = (In.vTexUV * g_vScale.y);
	Out.vTexCoord3 = (In.vTexUV * g_vScale.y);

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

struct PS_OUT_ALPHABLEND
{
	float4 vDiffuse : SV_TARGET0;
	float4 vGlow	: SV_TARGET1;
	float4 vBlur	: SV_TARGET2;
	float4 vMotionBlur : SV_TARGET3;
};

texture2D g_NoiseTexture;

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT_ALPHABLEND PS_MAIN(PS_IN In)
{
	PS_OUT_ALPHABLEND	Out = (PS_OUT_ALPHABLEND)0;

	float4 vNoise1 = g_NoiseTexture.Sample(DefaultSampler, In.vTexCoord1);
	vNoise1 = (vNoise1 - 0.5f) * 2.f;
	vNoise1.xy *= g_vDistortion1.xy;

	float4 vFinalNoise = vNoise1;// + vNoise2 + vNoise3;
	float fPerturb = ((1.f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	float2 vNoiseCoord = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;
	vNoiseCoord.y *= 2.f;

	float4 vFireColor = g_DiffuseTexture.Sample(DefaultSampler, vNoiseCoord);

	Out.vDiffuse = vFireColor;
	Out.vGlow = Out.vDiffuse;
	Out.vBlur = float4(0.1f, 0.f, 0.f, Out.vDiffuse.a);


	/*if (Out.vColor.r <= 0.05f && Out.vColor.g <= 0.05f && Out.vColor.b <= 0.05)
		discard;*/

	//Out.vColor.a = Out.vColor.r;
	return Out;
}

//----------------------------------------------------------------------------------------

technique11			DefaultTechnique
{
	//0
	pass DefaultPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}


//float2 Trans = In.vTexUV + 0.001f;
//
//vector vDistor = g_DistortionTexture.Sample(DefaultSampler, Trans);
//float fWeight = vDistor.r * 0.5f;
//
//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vDistor.xy * 0.05f);
//
//if (Out.vColor.a == 0.f)
//discard;
//
//if (Out.vColor.r <= 0.1f && Out.vColor.g <= 0.1f && Out.vColor.b <= 0.1f)
//discard;