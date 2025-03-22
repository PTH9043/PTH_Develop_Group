
#include "Shader_Defines.hpp"

Texture2D			g_BoneTexture;
Texture2D			g_OutLineTexture;
bool					g_OutLineExist = false;
//Texture2D			g_BoneTextures[100];

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV0 : TEXCOORD0;
	float2 vTexUV1 : TEXCOORD1;
	float2 vTexUV2 : TEXCOORD2;
	float2 vTexUV3 : TEXCOORD3;
	float2 vTexUV4 : TEXCOORD4;
	float3 vTangent : TANGENT;
	uint4  vBlendIndex : BLENDINDEX; /* 현재 정점에 영향ㅇ르 주는 뼈의 인덱스(메시커넽이너에 영햐을 주는 뼈들 중), 최대 4개. */
	float4 vBlendWeight : BLENDWEIGHT; /* 위에서 사용되고 있는 인덱스번째 뼈의 영향비율(최대 1) . */

	float4 vRight : TEXCOORD5;
	float4 vUp : TEXCOORD6;
	float4 vLook : TEXCOORD7;
	float4 vTranslation : TEXCOORD8;
	unsigned int iGroupID : TEXCOORD9;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float2 vOutLineUV : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

vector Return_OutLine_Texture(float2 UV)
{
	float4 vSampleOutLine = float4(1.f, 1.f, 1.f, 1.f);
	if (g_OutLineExist)
	{
		vSampleOutLine = g_OutLineTexture.Sample(DefaultSampler, UV);
	}
	return vSampleOutLine;
}

float4x4 LoadBoneMatrix(float iIndex, unsigned int iGroupID)
{
	uint2 baseUV = 0;
	baseUV.x = ((uint)iIndex * 4) % 512;
	baseUV.y = ((uint)iIndex * 4) / 512;

	baseUV.y += iGroupID * 8;

	vector Row[4];
	for (uint i = 0; i < 4; i++)
	{
		uint3 uv = uint3(baseUV.x + i, baseUV.y, 0);
		Row[i] = g_BoneTexture.Load(uv);
	}
	float4x4 mat = transpose(float4x4(Row[0], Row[1], Row[2], Row[3]));

	return mat;
}

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	/*matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;*/

	matrix BoneMatrix = LoadBoneMatrix(In.vBlendIndex.x, In.iGroupID) * In.vBlendWeight.x +
		LoadBoneMatrix(In.vBlendIndex.y, In.iGroupID) * In.vBlendWeight.y +
		LoadBoneMatrix(In.vBlendIndex.z, In.iGroupID) * In.vBlendWeight.z +
		LoadBoneMatrix(In.vBlendIndex.w, In.iGroupID) * fWeightW;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	vPosition = mul(vPosition, TransformMatrix);
	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV0;
	Out.vOutLineUV = In.vTexUV1;
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
	float2 vOutLineUV : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
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

	Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV) *
		Return_OutLine_Texture(In.vOutLineUV);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}


struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW	Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth.r = In.vProjPos.w / g_LightFar;
	Out.vShadowDepth.a = 1.f;

	return Out;
}


///// Pass : 1 ////////////////

struct VS_OUT_NORMAL
{
	float4 vPosition : SV_POSITION;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float2 vOutLineUV : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;

};


VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	//matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
	//	g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
	//	g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
	//	g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;


	matrix BoneMatrix = LoadBoneMatrix(In.vBlendIndex.x, In.iGroupID) * In.vBlendWeight.x +
		LoadBoneMatrix(In.vBlendIndex.y, In.iGroupID) * In.vBlendWeight.y +
		LoadBoneMatrix(In.vBlendIndex.z, In.iGroupID) * In.vBlendWeight.z +
		LoadBoneMatrix(In.vBlendIndex.w, In.iGroupID) * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector		vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = vPosition;

	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexUV = In.vTexUV0;
	Out.vOutLineUV = In.vTexUV1;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;


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
	float2 vOutLineUV : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV) *
		Return_OutLine_Texture(In.vOutLineUV);
	vector vNormalDesc = g_NormalTexture.Sample(ClampSampler, In.vTexUV);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 500.0f, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

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

	pass Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	// 2
	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
}
