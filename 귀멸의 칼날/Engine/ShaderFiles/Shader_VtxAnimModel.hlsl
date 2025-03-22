#include "Shader_Defines.hpp"

float4 g_vEdgeGlowColor = float4(0.f, 0.f, 0.f, 0.f);

Texture2D			g_BoneTexture;
Texture2D			g_PrevBoneTexture;
Texture2D			g_OutLineTexture;
Texture2D			g_DissolveTexture;
bool				g_OutLineExist = false;
bool				g_NormalExist = false;
bool				g_ObjectMotionBlur = false;

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
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float2 vOutLineUV : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
	float4 vVelocity : TEXCOORD4;
};

float4x4 LoadBoneMatrix(float iIndex)
{
	uint2 baseUV = 0;
	baseUV.x = ((uint)iIndex * 4) % 512;
	baseUV.y = ((uint)iIndex * 4) / 512;

	vector Row[4];
	for (uint i = 0; i < 4; i++)
	{
		uint3 uv = uint3(baseUV.x + i, baseUV.y, 0);
		Row[i] = g_BoneTexture.Load(uv);
	}
	float4x4 mat = transpose(float4x4(Row[0], Row[1], Row[2], Row[3]));

	return mat;
}

float4x4 LoadPrevBoneMatrix(float iIndex)
{
	uint2 baseUV = 0;
	baseUV.x = ((uint)iIndex * 4) % 512;
	baseUV.y = ((uint)iIndex * 4) / 512;

	vector Row[4];
	for (uint i = 0; i < 4; i++)
	{
		uint3 uv = uint3(baseUV.x + i, baseUV.y, 0);
		Row[i] = g_PrevBoneTexture.Load(uv);
	}
	float4x4 mat = transpose(float4x4(Row[0], Row[1], Row[2], Row[3]));

	return mat;
}

vector Return_OutLine_Texture_Clamp(float2 UV)
{
	float4 vSampleOutLine = float4(1.f, 1.f, 1.f, 1.f);
	if (g_OutLineExist)
	{
		vSampleOutLine = g_OutLineTexture.Sample(ClampSampler, UV);
		vSampleOutLine.a = 1.f;
	}
	return vSampleOutLine;
}

vector Return_OutLine_Texture_Wrap(float2 UV)
{
	float4 vSampleOutLine = float4(1.f, 1.f, 1.f, 1.f);
	if (g_OutLineExist)
	{
		vSampleOutLine = g_OutLineTexture.Sample(DefaultSampler, UV);
		vSampleOutLine.a = 1.f;
	}
	return vSampleOutLine;
}

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	/*matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;*/

	matrix BoneMatrix = LoadBoneMatrix(In.vBlendIndex.x) * In.vBlendWeight.x +
		LoadBoneMatrix(In.vBlendIndex.y) * In.vBlendWeight.y +
		LoadBoneMatrix(In.vBlendIndex.z) * In.vBlendWeight.z +
		LoadBoneMatrix(In.vBlendIndex.w) * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

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

	if(true == g_ObjectMotionBlur)
	{
		matrix PrevBoneMatrix = LoadPrevBoneMatrix(In.vBlendIndex.x) * In.vBlendWeight.x +
			LoadPrevBoneMatrix(In.vBlendIndex.y) * In.vBlendWeight.y +
			LoadPrevBoneMatrix(In.vBlendIndex.z) * In.vBlendWeight.z +
			LoadPrevBoneMatrix(In.vBlendIndex.w) * fWeightW;

		vector		vPrevPosition = mul(float4(In.vPosition, 1.f), PrevBoneMatrix);

		vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
		float4 NORMAL = mul(Out.vNormal, g_ViewMatrix);
		float a = dot(normalize(dir), normalize(NORMAL.xyz));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;
		else
			Out.vPosition = vPosition;
		float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
		velocity.y *= -1.f;
		Out.vVelocity.xy = velocity;
		Out.vVelocity.zw = Out.vPosition.zw;
	}

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
	float4 vVelocity : TEXCOORD4;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vEdgeGlow : SV_TARGET3;
	float4 vMotionBlur : SV_TARGET4;
};


bool	g_bDissolve = false;
float	dissolveAmount = 0.3f;
float4	edgeColor = float4(0.5f, 0.f, 0.f, 0.f);
float	noiseTiling = 2.f;
float	edgeThickness = 0.1f;


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (false == g_bWrapSampler)
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV) *
			Return_OutLine_Texture_Wrap(In.vOutLineUV);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) *
			Return_OutLine_Texture_Wrap(In.vOutLineUV);
	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vEdgeGlow = g_vEdgeGlowColor;

	if (g_bDissolve)
	{
		float4 dissolveNoise = g_DissolveTexture.Sample(DefaultSampler, In.vTexUV * noiseTiling);
		float remappedDissolve = dissolveAmount * (1.01f + edgeThickness) - edgeThickness;
		float4 step1 = step(remappedDissolve, dissolveNoise);
		float4 step2 = step(remappedDissolve + edgeThickness, dissolveNoise);
		float4 edge = step1 - step2;
		edge.a = Out.vDiffuse.a;
		float4 edgeColorArea = edge * edgeColor;
		float4 combinedColor = lerp(Out.vDiffuse, edgeColorArea, edge.r);

		Out.vDiffuse.rgb = combinedColor.rgb;
		Out.vDiffuse.a = step1.r;
		Out.vNormal.a = 0.f;
	}

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}
	
	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

//======================================================================================================

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW	Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth.r = In.vProjPos.w / g_LightFar;
	Out.vShadowDepth.g = In.vProjPos.z / In.vProjPos.w;
	Out.vShadowDepth.a = 1.f;

	return Out;
}

//======================================================================================================

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
	float4 vVelocity : TEXCOORD4;
};


VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	/*matrix		BoneMatrix = g_Bones.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bones.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bones.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bones.BoneMatrices[In.vBlendIndex.w] * fWeightW;*/
	matrix BoneMatrix = LoadBoneMatrix(In.vBlendIndex.x) * In.vBlendWeight.x +
		LoadBoneMatrix(In.vBlendIndex.y) * In.vBlendWeight.y +
		LoadBoneMatrix(In.vBlendIndex.z) * In.vBlendWeight.z +
		LoadBoneMatrix(In.vBlendIndex.w) * fWeightW;

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

	if (true == g_ObjectMotionBlur)
	{
		matrix PrevBoneMatrix = LoadPrevBoneMatrix(In.vBlendIndex.x) * In.vBlendWeight.x +
			LoadPrevBoneMatrix(In.vBlendIndex.y) * In.vBlendWeight.y +
			LoadPrevBoneMatrix(In.vBlendIndex.z) * In.vBlendWeight.z +
			LoadPrevBoneMatrix(In.vBlendIndex.w) * fWeightW;

		vector		vPrevPosition = mul(float4(In.vPosition, 1.f), PrevBoneMatrix);

		vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
		float4 NORMAL = mul(Out.vNormal, g_ViewMatrix);
		float a = dot(normalize(dir), normalize(NORMAL.xyz));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;
		else
			Out.vPosition = vPosition;
		float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
		velocity.y *= -1.f;
		Out.vVelocity.xy = velocity;
		Out.vVelocity.zw = Out.vPosition.zw;
	}
	return Out;
}

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
	float4 vVelocity : TEXCOORD4;
};

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (false == g_bWrapSampler)
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV) *
			Return_OutLine_Texture_Wrap(In.vOutLineUV);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) *
			Return_OutLine_Texture_Wrap(In.vOutLineUV);
	}

	vector vNormalDesc = g_NormalTexture.Sample(ClampSampler, In.vTexUV);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

//======================================================================================================

struct PS_OUT_GLOW
{
	float4 vDiffuse : SV_TARGET0;
	float4 vGlow	: SV_TARGET1;
};

float4 g_vGlowColor = float4(1.f, 1.f, 1.f, 1.f);


PS_OUT_GLOW PS_GLOW(PS_IN In)
{
	PS_OUT_GLOW		Out = (PS_OUT_GLOW)0;

	Out.vDiffuse = g_vGlowColor;
	Out.vGlow = g_vGlowColor;

	return Out;
}



technique11	DefaultTechnique
{
	// 0 : Renderer_NonBlendAlpha
	pass Clamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1 : Renderer_NonBlendAlpha
	pass Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	// 2 : Renderer_Shadow
	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}

	// 3 : Renderer_AlphaBlend
	pass Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLOW();
	}

}
