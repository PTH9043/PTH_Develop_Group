#include "Shader_Defines.hpp"

texture2D	g_NoiseTexture;
float4		g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float2		g_vUVDir = float2(0.f, 0.f);
float2		g_vSliceUV_START;
float2		g_vSliceUV_END = float2(0.f, 0.f);
float2		g_vSliceUV_DELTA;
float		g_fAlpha = 1.f;
bool		g_bCutBlack = false;
bool		g_bNoise = false;

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


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	// 현재 픽셀의 결과를 출력하지 말아달라는 뜻이다.
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV);


	if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;

	Out.vColor *= g_vColor;
	Out.vColor.a *= 0.5f;


	if (g_bNoise)
	{
		//Out.vColor = vNoise;

		/*if (Out.vColor.r < 0.1f)
			discard;*/

			//Out.vColor = vNoise;


		if (Out.vColor.a > vNoise.a)
			discard;
	}

	return Out;
}

PS_OUT PS_UVFLOW(PS_IN In)

{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y));
	vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV);


	if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;


	Out.vColor *= g_vColor;
	Out.vColor.a *= g_fAlpha;

	if (g_bNoise)
	{
		//Out.vColor = vNoise;

		/*if (Out.vColor.r < 0.1f)
			discard;*/

			//Out.vColor = vNoise;


		if (Out.vColor.a > vNoise.a)
			discard;
	}

	return Out;
}

struct VS_OUT_SOFT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};


VS_OUT_SOFT VS_MAIN_SOFT(VS_IN In)
{
	VS_OUT_SOFT		Out = (VS_OUT_SOFT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = vPosition;

	return Out;
}


struct PS_IN_SOFT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFT(PS_IN_SOFT In)
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

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;

	vColor.a = vColor.a * fDistance;

	vColor *= g_vColor;

	Out.vColor = vColor;

	

	Out.vColor.a *= g_fAlpha;

	return Out;
}

technique11			DefaultTechnique
{
	// 0
	pass EffectDefault
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass EffectImagePass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	// 2
	pass UVFlowPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_UVFLOW();
	}

	// 3
	pass SoftEffect
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SOFT();
		PixelShader = compile ps_5_0 PS_MAIN_SOFT();
	}
	
	// 4
	pass NonDepth
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SOFT();
		PixelShader = compile ps_5_0 PS_MAIN_SOFT();
	}
}
