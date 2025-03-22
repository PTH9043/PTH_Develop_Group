#include "Shader_Defines.hpp"


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_CombineMatrix, g_PivotMatrix, g_TargetWorldMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;

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
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWS, matWSV, matWSVP;

	matrix			SocketMatrix = g_CombineMatrix;

	//SocketMatrix = mul(g_BoneOffsetMatrix, g_CombineMatrix);
	SocketMatrix = mul(SocketMatrix, g_PivotMatrix);
	SocketMatrix = mul(SocketMatrix, g_TargetWorldMatrix);

	matWS = mul(g_WorldMatrix, SocketMatrix);
	matWSV = mul(matWS, g_ViewMatrix);
	matWSVP = mul(matWSV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWSVP);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), matWS));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWS);
	Out.vProjPos = Out.vPosition;

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


	return Out;
}
/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/
struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
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

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = normalize(vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f));
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	if (Out.vDiffuse.a < 0.1f)
		discard;

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

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

// 0보다 작은 값은 0, 1보다 큰 값은 1로 지정
// saturate

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

	pass Socket
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
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
}
