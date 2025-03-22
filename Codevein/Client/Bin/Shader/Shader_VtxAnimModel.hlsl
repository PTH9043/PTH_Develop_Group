#include "Shader_Defines.hpp"


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_AnimWorldMatrix;

/* 특정 메시컨테이너에 영향을 주는 뼈들의 행렬. */
struct tagBoneMatrices
{
	matrix		BoneMatrices[256];
};

tagBoneMatrices		g_Bones;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D g_DissorveTexture;
texture2D g_Dissorve_ShowTexture;

float g_fPlusTimeDelta = 0.f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
	uint4  vBlendIndex : BLENDINDEX; /* 현재 정점에 영향ㅇ르 주는 뼈의 인덱스(메시커넽이너에 영햐을 주는 뼈들 중), 최대 4개. */
	float4 vBlendWeight : BLENDWEIGHT; /* 위에서 사용되고 있는 인덱스번째 뼈의 영향비율(최대 1) . */
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

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vNormal = normalize(mul(vNormal, g_AnimWorldMatrix));
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_AnimWorldMatrix);
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
	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	vector		vNormal = normalize(mul(float4(In.vNormal, 0.f), BoneMatrix));
	vector		vTangent = normalize(mul(float4(In.vTangent, 0.f), BoneMatrix));

	Out.vPosition = vPosition;

	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
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
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 1.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/

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

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3 vNormal = normalize(vNormalDesc * 2.f - 1.f).xyz;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = normalize(vector(vNormal * 0.5f + 0.5f, 0.f));
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 1.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DESOURVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = normalize(vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f));
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 1.0f, 0.0f);
	Out.vWorldPos = In.vPosition / 100000000.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	vector vDisorveColor = g_DissorveTexture.Sample(DefaultSampler, In.vTexUV);
	vector vShow_DisorveColor = g_Dissorve_ShowTexture.Sample(DefaultSampler, In.vTexUV);
	float fColorLength = vDisorveColor.r + vDisorveColor.g + vDisorveColor.b;
	if (fColorLength <= g_fPlusTimeDelta)
	{
		if (fColorLength - 0.2f <= g_fPlusTimeDelta)
		{
			discard;
		}
		Out.vDiffuse += float4(1.f, -0.22f, -0.22f, 0.f);
	}

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

	pass Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}
	pass Desorve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DESOURVE();
	}
}
