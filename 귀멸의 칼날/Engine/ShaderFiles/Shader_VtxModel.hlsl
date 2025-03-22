#include "Shader_Defines.hpp"

matrix		g_SocketMatrix;
vector		g_vColor = vector(1.f, 1.f, 1.f, 1.f);
float2		g_vSliceUV_START;
float2		g_vSliceUV_END = float2(0.f, 0.f);
float2		g_vSliceUV_DELTA;
bool		g_bCutBlack = false;

texture2D	g_FilterTexture;
float		g_fPlayerViewZ;
float4		g_vEdgeGlowColor = float4(0.5f, 0.f, 0.f, 1.f);
bool		g_ObjectMotionBlur = false;
bool		g_bFilter = true;

// ASAKUSA
texture2D	g_EmissiveTexture;
texture2D	g_SubDiffuseTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV0 : TEXCOORD0;
	float3 vTangent : TANGENT;
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

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV0;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (true == g_ObjectMotionBlur)
	{
		vector		vPrevPosition = float4(In.vPosition, 1.f);

		vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float4 NORMAL = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
		float3 dir = vPosition.xyz - vPrevPosition.xyz;
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

	if (true == g_ObjectMotionBlur)
	{
		vector		vPrevPosition =  float4(In.vPosition, 1.f);

		vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_SocketMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float3 dir = vPosition.xyz - vPrevPosition.xyz;
		float3 NORMAL = mul(Out.vNormal, g_ViewMatrix);
		float a = dot(normalize(dir), normalize(NORMAL));
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

//---------------------------------------------------------- PS ----------------------------------------------------------//

float fogEnd = 1000.f;
float fogStart = 0.5f;
float4	fogColor = float4(0.15f, 0.2f, 0.35f, 1.f);

float fDepth = 0.02f;

// fog
vector Fog_Effect(float zDepth, int type, vector vDiffuse)
{
	float fogFactor = 0.f;
	// Linear
	if (0 == type)
		fogFactor = saturate(max(fogEnd - zDepth, 0.f) / (fogEnd - fogStart));
	else if (1 == type)
		fogFactor = 1.f / exp(zDepth * fDepth);
	else if (2 == type)
		fogFactor = 1.f / exp(pow(zDepth * fDepth, 0.5f));

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
	float4 vEdgeGlow : SV_TARGET3;
	float4 vMotionBlur : SV_TARGET4;
	float4 vMapDepth : SV_TARGET5;
};

// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
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

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;

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
	Out.vMapDepth = Out.vDepth;

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

PS_OUT PS_MAPLIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse.a = saturate(pow(Out.vDiffuse.r - 0.2f, 2));
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);

	return Out;
}


PS_OUT PS_FOG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;

	Out.vDiffuse = Fog_Effect(In.vProjPos.w, 0, vDiffuseDesc);
	Out.vDiffuse.a = vDiffuseDesc.a;

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_EDGE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vEdgeGlow = g_vEdgeGlowColor;
	Out.vMapDepth = Out.vDepth;

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

float fEmissiveIntensity = 0.1f;
PS_OUT PS_ASAKUSA(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);
	vector vSubDiffuse = g_SubDiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse + float4(vSubDiffuse.rgb * vEmissive.r * fEmissiveIntensity, 0.f);// +float4(vEmissive.rrr * fEmissiveIntensity, 0.f);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;

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

//============================================================================================================

struct PS_OUT_DIFFUSE
{
	float4 vDiffuse : SV_TARGET0;
};

PS_OUT_DIFFUSE PS_SHADOW(PS_IN In)
{
	PS_OUT_DIFFUSE			Out = (PS_OUT_DIFFUSE)0;

	Out.vDiffuse.r = In.vProjPos.w / g_LightFar/*In.vProjPos.w*/;
	Out.vDiffuse.g = In.vProjPos.z / In.vProjPos.w;
	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT_DIFFUSE PS_SKY(PS_IN In)
{
	PS_OUT_DIFFUSE			Out = (PS_OUT_DIFFUSE)0;

	vector vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse = Fog_Effect(fogEnd - 200.f, 0, vDiffuseDesc);

	if (0.f == Out.vDiffuse.a)
		discard;

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

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexUV = In.vTexUV0;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	if (true == g_ObjectMotionBlur)
	{
		vector		vPrevPosition = float4(In.vPosition, 1.f);

		vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
		vPrevPosition = mul(vPrevPosition, g_PrevViewMatrix);
		vPrevPosition = mul(vPrevPosition, g_ProjMatrix);

		float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
		float3 NORMAL = mul(Out.vNormal, g_ViewMatrix);
		float a = dot(normalize(dir), normalize(NORMAL));
		if (a < 0.f)
			Out.vPosition = vPrevPosition;

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
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vVelocity : TEXCOORD3;
};


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
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;

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

PS_OUT PS_FOG_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;


	Out.vDiffuse = Fog_Effect(In.vProjPos.w, 0, vDiffuseDesc);
	Out.vDiffuse.a = vDiffuseDesc.a;

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

PS_OUT PS_EDGE_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;


	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vEdgeGlow = g_vEdgeGlowColor;
	Out.vMapDepth = Out.vDepth;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}


PS_OUT PS_ASAKUSA_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);
	vector vSubDiffuse = g_SubDiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	Out.vDiffuse = vDiffuse + vSubDiffuse + float4(vEmissive.xxx * 0.1f, 0.f);

	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_CamFar, In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f);
	Out.vMapDepth = Out.vDepth;

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

//==================================================================================================

/*
struct PS_OUT_NONLIGHT
{
	float4 vColor : SV_TARGET0;
	float4 vBrightness : SV_TARGET1;
};


PS_OUT_NONLIGHT PS_NONLIGHT_BLOOM(PS_IN In)
{
	PS_OUT_NONLIGHT	Out = (PS_OUT_NONLIGHT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	float4 BrightColor = 0.f;
	float brightness = dot(Out.vColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 0.9)
		BrightColor = float4(Out.vColor.rgb, 0.f);

	Out.vBrightness = BrightColor;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}
*/

//======================================NONALPHABLEND==========================================

float4 vGlowColor = float4(1.f, 1.f, 1.f, 1.f);

PS_OUT_DIFFUSE PS_COLOR_GLOW(PS_IN In)
{
	PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vGlowColor;
	Out.vDiffuse.a = vDiffuse.a;

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_OUT_DIFFUSE PS_DIFFUSE_GLOW(PS_IN In)
{
	PS_OUT_DIFFUSE	Out = (PS_OUT_DIFFUSE)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}


//====================================ALPHABLEND=====================================

struct PS_OUT_ALPHABLEND
{
	float4 vDiffuse : SV_TARGET0;
	float4 vGlow	: SV_TARGET1;
	float4 vBlur	: SV_TARGET2;
	float4 vMotionBlur : SV_TARGET3;
};

float g_fGlowValue = 0.2f;

PS_OUT_ALPHABLEND PS_ALPHABLEND(PS_IN In)
{
	PS_OUT_ALPHABLEND		Out = (PS_OUT_ALPHABLEND)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vGlow = Out.vDiffuse;
	Out.vBlur = float4(g_fGlowValue, 0.f, 0.f, Out.vDiffuse.a);

	if (true == g_ObjectMotionBlur)
	{
		Out.vMotionBlur = In.vVelocity;
		Out.vMotionBlur.z = 1.f;
		Out.vMotionBlur.w = In.vVelocity.z / In.vVelocity.w;
	}

	return Out;
}




technique11	DefaultTechnique
{
	// 0 : Renderer_NonAlphaBlend
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1 : Renderer_NonAlphaBlend
	pass Socket
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 2 : Renderer_NonAlphaBlend
	pass Normal
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	// 3 : Renderer_NonAlphaBlend
	pass Fog
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FOG();
	}

	// 4 : Renderer_NonAlphaBlend
	pass FogNormal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FOG_NORMAL();
	}

	// 5 : Renderer_Shadow
	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}

	// 6 : Renderer_Priority
	pass Sky
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SKY();
	}

	// 7 : Renderer_Glow
	pass DiffuseGlow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE_GLOW();
	}

	// 8 : Renderer_Glow
	pass ColorGlow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR_GLOW();
	}

	// 9 : Renderer_AlphaBlend
	pass Maplight
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAPLIGHT();
	}

	// 10 : Renderer_AlphaBlend
	pass AlphaBlend
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
	}

	// 11 : MAPOBJ
	pass Default_MAPOBJ
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPOBJ();
	}
	

	// 12 : Renderer_NonAlphaBlend
	pass Model_EdgeGlow_Diffuse
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EDGE();
	}

	// 13 : Renderer_NonAlphaBlend
	pass Model_EdgeGlow_Normal
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EDGE_NORMAL();
	}

	// 14 : Renderer_NonAlphaBlend
	pass Asakusa_Normal
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ASAKUSA_NORMAL();
	}

	// 15 : Renderer_NonAlphaBlend
	pass Asakusa
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ASAKUSA();
	}
}

