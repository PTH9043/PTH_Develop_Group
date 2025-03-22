#include "Shader_Defines.hpp"

float3		g_vCamPosition;
float		g_fPlayerViewZ;
float3		g_vPlayerPos;
matrix		g_SocketMatrix;
bool		g_bFilter = true;

texture2D		g_FilterTexture;
bool				g_ObjectMotionBlur = false;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV0 : TEXCOORD0;
	float3 vTangent : TANGENT;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), TransformMatrix * g_WorldMatrix));
	Out.vTexUV = In.vTexUV0;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), TransformMatrix * g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (true == g_ObjectMotionBlur)
	{
		vector		vPrevPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

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




VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWS, matWSV, matWSVP;

	matWS = mul(g_WorldMatrix, g_SocketMatrix);
	matWSV = mul(matWS, g_ViewMatrix);
	matWSVP = mul(matWSV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWSVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV0;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

//---------------------------------------------------------- PS ----------------------------------------------------------//

// 안개쉐이더
vector Fog_Effect(float zDepth, vector vDiffuse, int fogType)
{
	float fogEnd = g_CamFar;
	float fogStart = 5.f;
	float fogFactor = 0.f;
	vector fogColor = vector(0.3f, 0.3f, 0.3f, 1.0f);

	// Linear
	if (0 == fogType)
		fogFactor = saturate(max(fogEnd - zDepth, 0.f) / (fogEnd - fogStart));
	else if (1 == fogType)
		fogFactor = 1.f / exp(zDepth * 0.5f);
	else if (2 == fogType)
		fogFactor = 1.f / exp(pow(zDepth * 0.2f, 5.f));

	vDiffuse = fogFactor * vDiffuse + (1.f - fogFactor) * fogColor;
	return vDiffuse;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vMotionBlur : SV_TARGET3;
};


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
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


PS_OUT PS_MAIN_MAPOBJ(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	// 픽셀의 viewZ보다 플레이어의 viewZ가 크다면 그리지 않는다
	if (g_bFilter)
	{
		if (In.vProjPos.w < g_fPlayerViewZ)
		{
			float2 uv = In.vProjPos.xy / In.vProjPos.w;
			uv.x = uv.x * 0.5f + 0.5f;
			uv.y = uv.y * -0.5f + 0.5f;
			float4 vFilter = g_FilterTexture.Sample(DefaultSampler, uv);
			Out.vDiffuse.a *= vFilter.x;
		}
	}
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW			Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth.r = In.vProjPos.w / g_LightFar/*In.vProjPos.w*/;
	Out.vShadowDepth.a = 1.f;

	return Out;
}


//===========================================================================================

struct VS_OUT_NORMAL
{
	float4 vPosition : SV_POSITION;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};


VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix			matWV, matWVP;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), TransformMatrix * g_WorldMatrix));
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), TransformMatrix * g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexUV = In.vTexUV0;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (true == g_ObjectMotionBlur)
	{
		vector		vPrevPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

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

struct PS_IN_NORMAL
{
	float4 vPosition : SV_POSITION;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (g_bFilter)
	{
		if (In.vProjPos.w < g_fPlayerViewZ)
		{
			float2 uv = In.vProjPos.xy / In.vProjPos.w;
			uv.x = uv.x * 0.5f + 0.5f;
			uv.y = uv.y * -0.5f + 0.5f;
			float4 vFilter = g_FilterTexture.Sample(DefaultSampler, uv);
			Out.vDiffuse.a *= vFilter.x;
		}
	}

	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
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



technique11	DefaultTechnique
{
	// 0
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
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

	// 3 맵오브젝트
	pass MapObj
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPOBJ();
	}
}