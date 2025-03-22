#include "Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

float			g_FillMount = 0.f;

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

/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/

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

PS_OUT PS_BAR_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	In.vTexUV.x -= g_FillMount;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.a * 0.5f;
	// 현재 픽셀의 결과를 출력하지 말아달라는 뜻이다.
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MPBAR_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = Out.vColor + vector(0.f, 0.f, 0.75f, 1.f);

	Out.vColor.r = 0;
	Out.vColor.a = Out.vColor.a * 0.5f;
	// 현재 픽셀의 결과를 출력하지 말아달라는 뜻이다.
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_CHECKPOINTIMG_NONACTIVE_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	if (Out.vColor.r >= 0.9f && Out.vColor.g >= 0.9f && Out.vColor.b >= 0.9f)
		Out.vColor.rgb = 1.f;
	else
		Out.vColor.rgb *= 0.4f;

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.a = 1.f;
	return Out;
}

PS_OUT PS_CHECKPOINTINTERFACE_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a * 0.4f;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}


technique11			DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Bar
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BAR_MAIN();
	}

	pass MpBar
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MPBAR_MAIN();
	}

	pass CheckPointImg
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CHECKPOINTIMG_NONACTIVE_MAIN();
	}

	pass CheckPointInterface
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_CHECKPOINTINTERFACE_MAIN();
	}
}
