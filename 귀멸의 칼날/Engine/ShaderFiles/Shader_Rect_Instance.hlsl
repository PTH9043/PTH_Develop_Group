#include "Shader_Defines.hpp"

float4				g_vColor;
float2				g_vSliceUV_START;
float2				g_vSliceUV_END = float2(0.f, 0.f);
float2				g_vSliceUV_DELTA;
float2				g_vUVDir;
float3				g_vScale;
float				g_fAlpha = 1.f;
float				g_fGlowValue = 0.1f;
bool				g_bCutBlack = false;
bool				g_bGlow = false;
bool				g_bUVFlow = false;
bool				g_bBillboard = false;

float4				g_Color1;
float4				g_Color2;
float				g_Alpha = 0.1f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;

};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT		Out;

	vector		vPosition;
	if (g_bBillboard)
	{
		vector tmpPostion = mul(vector(In.vTranslation.xyz, 1.f), g_WorldMatrix);
		float4x4	InstanceMatrix = float4x4(In.vRight, In.vUp, In.vLook, tmpPostion);

		float4x4	matVP;
		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		vPosition = mul(float4(In.vPosition, 1.f), InstanceMatrix);
		vPosition = mul(vPosition, matVP);
	}
	else
	{
		float4x4	InstanceMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
		float4x4	matWV, matWVP;

		matWV = mul(g_WorldMatrix, g_ViewMatrix);
		matWVP = mul(matWV, g_ProjMatrix);

		vPosition = mul(float4(In.vPosition, 1.f), InstanceMatrix);
		vPosition = mul(vPosition, matWVP);
	}

	float2 uv = In.vTexUV;
	if (g_bUVFlow)
		uv = float2(In.vTexUV.x + g_vUVDir.x, In.vTexUV.y + g_vUVDir.y);
	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		uv.x = g_vSliceUV_START.x + (uv.x * g_vSliceUV_DELTA.x);
		uv.y = g_vSliceUV_START.y + (uv.y * g_vSliceUV_DELTA.y);
	}

	Out.vPosition = vPosition;
	Out.vTexUV = uv;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vGlow : SV_TARGET1;
	float4 vBlur : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_BLACKCUT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.r < 0.1f)
		discard;
	else
	{
		float a = Out.vColor.a;
		Out.vColor = (Out.vColor.r * g_Color1) + ((1.f - Out.vColor.r) * g_Color2);
	}

	return Out;
}

PS_OUT PS_BLEND(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2	vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	float	fViewZ = vDepthDesc.x * g_CamFar;
	float	fDistance = saturate(fViewZ - In.vProjPos.w);


	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;

	Out.vColor *= g_vColor;

	// 알파 서서히 사라지게
	Out.vColor.a *= g_fAlpha;

	// 소프트 이펙트
	Out.vColor.a *= fDistance;

	if (g_bGlow)
	{
		Out.vGlow = Out.vColor;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vColor.a);
	}

	return Out;
}


PS_OUT PS_BLEND_COLOR(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2	vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	float	fViewZ = vDepthDesc.x * g_CamFar;
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = (vColor.r * g_Color1) + ((1.f - vColor.r) * g_Color2);
	if (g_bCutBlack)
		Out.vColor.a = vColor.r;

	Out.vColor.a *= g_fAlpha;
	Out.vColor.a *= fDistance;

	if (g_bGlow)
	{
		Out.vGlow = Out.vColor;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vColor.a);
	}

	return Out;
}


PS_OUT PS_ALPHATEST(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2	vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	float	fViewZ = vDepthDesc.x * g_CamFar;
	float	fDistance = saturate(fViewZ - In.vProjPos.w);


	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_bCutBlack)
	{
		if (0.01f > Out.vColor.r)
			discard;
	}

	Out.vColor *= g_vColor;

	// 알파 서서히 사라지게
	Out.vColor.a *= g_fAlpha;

	// 소프트 이펙트
	Out.vColor.a *= fDistance;

	if (g_bGlow)
	{
		Out.vGlow = Out.vColor;
		Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vColor.a);
	}

	return Out;
}


technique11 DefaultTechnique
{
	pass Rect//0
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass BlackCutRect//1
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACKCUT();
	}
	pass Blend//2
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND();
	}
	pass Color//3
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Blend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND_COLOR();
	}
	pass AlphaTest//4
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHATEST();
	}
}
