#include "Shader_Defines.hpp"


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D g_NormalTexture;

bool		g_bNavigationDebugColor = false;

float4		g_Color = (float4)0;
float4		g_BlendColor = (float4)0;
float2		g_UVControlPos = (float2)0;
float2		g_UVEndControlPos = (float2)0;
bool		g_UVRestrict = false;
bool		g_bUVControl = false;

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

/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/

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
	float4 vWorldPos : SV_TARGET3;
};

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = normalize(vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f));
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	return Out;
}

PS_OUT PS_MAIN_EFFMODEL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 v2UV = In.vTexUV;
	float2		g_UVControlPos = (float2)0;
	float2		g_UVEndControlPos = (float2)0;

	if (false == g_bUVControl)
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + g_Color;
		Out.vDiffuse *= (1 - g_BlendColor);
		Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
		Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
		Out.vWorldPos = In.vPosition / 100000000.f;
	}
	else
	{
		if (false == g_UVRestrict)
		{
			if (v2UV.x < g_UVControlPos.x && v2UV.y < g_UVControlPos.y)
			{
				Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + g_Color;
				Out.vDiffuse *= (1 - g_BlendColor);
				Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
				Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
				Out.vWorldPos = In.vPosition / 100000000.f;
			}
			else
				discard;
		}
		else
		{
			if (v2UV.x > g_UVControlPos.x && v2UV.x < g_UVEndControlPos.x)
			{
				if (v2UV.y > g_UVControlPos.y && v2UV.y < g_UVEndControlPos.y) {

					Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + g_Color + g_BlendColor;
					Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
					Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
					Out.vWorldPos = In.vPosition / 100000000.f;
				}
				else
					discard;
			}
			else
				discard;
		}
	}

	return Out;
}

VS_OUT VS_MAIN_SKY(VS_IN In)
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

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN_SKY(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
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
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = vector(In.vPosition, 1.f);
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
};

PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3 vNormal = normalize(vNormalDesc * 2.f - 1.f).xyz;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}


technique11	DefaultTechnique
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

	pass Sky
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SKY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKY();
	}

	pass Wire
	{
		SetRasterizerState(RS_WireFrame);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DEBUG
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	pass EffectModel
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFMODEL();
	}
}
