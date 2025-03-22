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


float		g_Alpha;
float       g_UVFlow;
float       g_UVFlowx;
float       g_UVFlowy;

float       g_HPPercent;
float       g_DamagePercent;

float       g_SkilPercent;

float		g_Angle;

Texture2D   g_HPTexture;
Texture2D   g_DamageTexture;

Texture2D   g_ColorTexture;

float		g_UltPercent;
Texture2D	g_UltTexture;

Texture2D   g_MapTexture;
float		g_MapX;
float		g_MapY;

float 		g_MiniGaugePercent;
float3 		g_MiniGameRGB;

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_Alpha(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	return Out;
}


PS_OUT PS_Smoke(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);// * g_Alpha;
	
	Out.vColor.a = 1.f - Out.vColor.r;
	
	if (Out.vColor.a < 0.01f)
		Out.vColor.a = 0.f;

		
	return Out;
}

PS_OUT PS_UIHP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.x < g_HPPercent)
		Out.vColor = g_HPTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	else if (g_HPPercent < In.vTexUV.x && In.vTexUV.x < g_DamagePercent)
		Out.vColor = g_DamageTexture.Sample(DefaultSampler, In.vTexUV); //* g_Alpha;
	else
		discard;

	


	return Out;
}

PS_OUT PS_UISkil(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	if (In.vTexUV.x < (1.f - g_SkilPercent))
		discard;


	return Out;
}

PS_OUT PS_UVFlow(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x -= g_UVFlow;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	return Out;
}

PS_OUT PS_Combo(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.r <= 1 && Out.vColor.g <= 1 && Out.vColor.b <= 1 && Out.vColor.a >= 0.3)
		Out.vColor = g_ColorTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a *= g_Alpha;

	//각도를 구해야한다 

	float base = (In.vTexUV.x - 0.5);
	float height = (In.vTexUV.y - 0.5);

	float radian = atan2(base, height);

	//(-180 ~ 180)

	if (radian > g_Angle)
		discard;

	if (Out.vColor.a == 0)
		discard;

	return Out;
}

PS_OUT PS_UIUlt(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.x > (1.f - g_UltPercent))
		Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	else
		discard;
	return Out;
}

PS_OUT PS_MiniMap(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	//g_MapX, g_MapY 중심UV좌표
	const float dist = 0.125f; // 중심UV로부터 거리(미니맵의 반지름)

	if (g_MapX + dist < In.vTexUV.x
		|| g_MapX - dist > In.vTexUV.x
		|| g_MapY + dist < In.vTexUV.y
		|| g_MapY - dist > In.vTexUV.y)
		discard; // 반지름을 넘어가는 것들은 자른다

	/*UV의 중심을 베이스의 중앙으로 맞추고 그 반지름(0.125)이상으로 넘어가는 것들을 자른다
	미니맵은 1000사이즈 베이스는 250사이즈 4분의 1비율
	UV좌표상 미니맵이 0~1이라면 베이스는 크기는 0.25,베이스의 반지름은 0.125가 된다 */
	

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}

PS_OUT PS_UVFlowY(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.y -= g_UVFlowy;
	
	In.vTexUV.x -= g_UVFlowx;
	
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	return Out;
}

PS_OUT PS_UVFlowUP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.y += g_UVFlowy;

	In.vTexUV.x += g_UVFlowx;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	return Out;
}

PS_OUT PS_MiniGauge(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.x < g_MiniGaugePercent)
		Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;

	else
		discard;
	
	return Out;
}

PS_OUT PS_RGB(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.r *= g_MiniGameRGB.x;
	Out.vColor.g *= g_MiniGameRGB.y;
	Out.vColor.b *= g_MiniGameRGB.z;
	Out.vColor.a *= g_Alpha;

	return Out;
}


struct PS_OUT_ALPHABLEND
{
	float4 vDiffuse : SV_TARGET0;
	float4 vGlow	: SV_TARGET1;
	float4 vBlur	: SV_TARGET2;
	float4 vMotionBlur : SV_TARGET3;
};

PS_OUT_ALPHABLEND PS_GLOW(PS_IN In)
{
	PS_OUT_ALPHABLEND Out = (PS_OUT_ALPHABLEND)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_Alpha;
	Out.vGlow = Out.vDiffuse;
	Out.vBlur = float4(0.1f, 0.f, 0.f, Out.vDiffuse.a);

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
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass UIPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha();
	}

	// 2 
	pass LogoSmoke
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Smoke();
	}

	
	//3
	pass HPPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UIHP();
	}


	//4
	pass SkilPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UISkil();
	}

	//5
	pass UVFlowPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UVFlow();
	}

	//6
	pass ComboPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Combo();
	}
	
	//7
	pass UltimatePass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UIUlt();
	}

	//8
	pass MiniMapPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MiniMap();
	}

	//9
	pass UVFlowyPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UVFlowY();
	}

	//10
	pass DepthPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha();
	}
	//11
	pass UVFlowUpPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UVFlowUP();
	}
	
	//12
	pass MiniGaugePass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MiniGauge();
	}

	//13
	pass RGBPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RGB();
	}
	
	//14
	pass UIGrow //Grow로 그리는 애들
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLOW();
	}



}


