
#include "Shader_Defines.hpp"

texture2D g_InputTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_Position;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_Target0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_InputTexture.Sample(DefaultSampler, In.vTexUV);
	//Out.vColor = vector(0.f, 1.f, 0.f, 1.f);

	//if (Out.vColor.a == 0.f)
	//	discard;

	return Out;
}

technique11			DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}