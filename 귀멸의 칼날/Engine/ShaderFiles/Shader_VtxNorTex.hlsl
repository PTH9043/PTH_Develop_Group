#include "Shader_Defines.hpp"

texture2D	g_SourDiffuseTexture;
texture2D	g_DestDiffuseTexture;
texture2D	g_FilterTexture;
texture2D	g_BrushTexture;
float3		g_vBrushPos = float3(50.f, 0.f, 15.f);
float		g_fBrushRange = 10.f;


struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;	
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;

};


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0; 

	vector	vSourDiffuse = g_SourDiffuseTexture.Sample(DefaultSampler, In.vTexUV * 20.f);	
	vector	vDestDiffuse = g_DestDiffuseTexture.Sample(DefaultSampler, In.vTexUV * 20.f);	
	vector	vFilter = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vBrush = vector(0.f, 0.f, 0.f, 0.f);

	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2	vTexUV;

		vTexUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vTexUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);		

		vBrush = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}

	Out.vDiffuse = vSourDiffuse * vFilter + vDestDiffuse * (1.f - vFilter) + vBrush;
	Out.vDiffuse.a = 1.f;

	/* -1 ~ 1 => 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	
	return Out;
}

PS_OUT PS_WIREFRAME(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	Out.vDiffuse.a = 1.f;

	/* -1 ~ 1 => 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	return Out;
}

technique11			DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass WirePass
	{
		SetRasterizerState(RS_WireFrame);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WIREFRAME();
	}
}
