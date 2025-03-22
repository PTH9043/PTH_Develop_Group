#include "Shader_Defines.hpp"


texture2D	g_LightDepthTexture;

matrix		g_ProjMatrixInv;
matrix		g_ViewMatrixInv;

float3		g_vCamPosition;
float3		g_vLightPosition;

float3		g_vLightDir;
float3		g_vLightPos;
float		g_fRange;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);


struct tagSSAO_In
{
	float2	vUV;
	float	fDepth;
	float	fViewZ;
	float3	vNormal;
};

struct tagSSAO_Out
{
	float4 vAmbient;
};

float fShadowDepth = 1.f;
bool bShade = true;
bool bRimlight = true;

//================================================================================================


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

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT_LIGHT
{
	float4 vAmbient : SV_TARGET0;
	float4 vShade : SV_TARGET1;
	float4 vSpecular : SV_TARGET2;
	float4 vShadow : SV_TARGET3;
};


void Directional_Shade(in float4 vNormalDesc, out float4 vShade)
{
	vector	vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	float4	vLightDir = normalize(vector(g_vLightDir, 0.f));
	float	fLightDensity = dot(vLightDir * -1.f, normalize(vNormal));

	//fLightDensity = pow((fLightDensity * 0.5f) + 0.5f, 2); // 하프 램버트
	// 캐릭터한테만 카툰 쉐이딩
	if (1.f == vNormalDesc.a)
	{
		fLightDensity = fLightDensity * 0.5f + 0.5f;
		fLightDensity = floor(fLightDensity * 2.f) * (1 / 2.f);
	}
	vShade = g_vLightDiffuse * fLightDensity/* + g_vLightAmbient * g_vMtrlAmbient*/;
	vShade.a = 1.f;
}

void Directional_Specular(in float4 vNormal, in float4 vWorldPos, out float4 vSpecular)
{
	float4	vLightDir = normalize(vector(g_vLightDir, 0.f));

	// vReflect : 빛과 노멀의 반사벡터
	// vCamLook : 카메라와 물체의 방향벡터
	vector	vReflect = normalize(reflect(vLightDir, vNormal));
	vector	vCamLook = normalize(vector(vWorldPos.xyz - g_vCamPosition, 0.f));

	vector	vHalfVector = normalize(vCamLook + vLightDir);
	float fPoweredHDotN = pow(saturate(dot(vHalfVector, vNormal)), 10.f);
	float fSpecularSmooth = smoothstep(0.05f, 0.1f, fPoweredHDotN);

	vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecularSmooth; //pow(saturate(dot(vCamLook * -1.f, vReflect)), 10.f);
	vSpecular.a = 0.f;
}


float4 vShadowColor = float4(0.5f, 0.5f, 0.5f, 1.f);
void Directional_Shadow(in float4 vWorldPos, out float4 vShadow)
{
	vector vLightProjPos = mul(vWorldPos, g_LightViewMatrix);
	vLightProjPos = mul(vLightProjPos, g_LightProjMatrix);
	// currentDepth : 화면에 나오는 픽셀의 Light 위치의 View 깊이
	float currentDepth = vLightProjPos.w;
	float2 uv = vLightProjPos.xy / vLightProjPos.w;
	uv.x = uv.x * 0.5f + 0.5f;
	uv.y = uv.y * -0.5f + 0.5f;
	vShadow = vector(1.f, 1.f, 1.f, 1.f);
	if (0.f <= uv.x && uv.x <= 1.f && 0.f < uv.y && uv.y < 1.f)
	{
		vector vLightDepth = g_LightDepthTexture.Sample(ClampSampler, uv);
		float lightDepth = vLightDepth.r * g_LightFar;
		if (currentDepth - fShadowDepth > lightDepth)
		{
			vShadow = vShadowColor;
		}
	}
}


float fRimLightPower = 1.f;
float fRimMin = 0.f;
float fRimMax = 0.3f;
float RimLight(in float4 vWorldPos, in float4 vNormal)
{
	float4	vCamLook = normalize(vector(vWorldPos.xyz - g_vCamPosition, 0.f));

	float4	vLightDir = normalize(vector(g_vLightDir, 0.f));
	float	fLightDensity = dot(vCamLook * -1.f, normalize(vNormal));

	float	rimLightIntensity = max(0.f, 1.f - fLightDensity);
	rimLightIntensity = pow(rimLightIntensity, fRimLightPower);
	rimLightIntensity = smoothstep(fRimMin, fRimMax, rimLightIntensity);
	if (0.f == fRimLightPower)
		rimLightIntensity = 0.f;

	return rimLightIntensity;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	// 노말값 (vNormalDesc.a가 1이면 플레이어 0이면 지형)
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));

	// 깊이 텍스쳐
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * g_CamFar;

	// 월드 좌표 구하기
	vector vWorldPos = vector(In.vTexUV.x * 2.f - 1.f, In.vTexUV.y * -2.f + 1.f, vDepthDesc.y, 1.f);
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	// Calculate Shade 
	if (bShade)
		Directional_Shade(vNormalDesc, Out.vShade);
	//Out.vShade += RimLight(vWorldPos, vNormal);

	// Calculate Specular
	//Directional_Specular(vNormal, vWorldPos, Out.vSpecular);
	if (bRimlight)
		Out.vSpecular = RimLight(vWorldPos, vNormal);

	// Calculate Shadow
	Directional_Shadow(vWorldPos, Out.vShadow);

	// Calculate Ambient
	if (bShade)
	{
		Out.vAmbient = g_vLightAmbient * g_vMtrlAmbient;
		Out.vAmbient.a = 1.f;
	}
	else
		Out.vAmbient = float4(1.f, 1.f, 1.f, 1.f);

	return Out;
}


void Point_Shade(in float4 vNormalDesc, in float4 vWorldPos, out float4 vShade)
{
	vector	vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	vector	vLightDir = vector(vWorldPos.xyz - g_vLightPos, 0.f);
	float	fDistance = length(vLightDir);
	
	float	fLightDensity = dot(normalize(vLightDir) * -1.f, vNormal);
	float	fAtt = saturate((g_fRange - fDistance) / g_fRange);
	if (1.f == vNormalDesc.a)
	{
		fLightDensity = fLightDensity * 0.5f + 0.5f;
		fLightDensity = floor(fLightDensity * 2.f) * (1 / 2.f);
	}

	vShade = g_vLightDiffuse * fLightDensity * fAtt;
	vShade.a = 1.f;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * g_CamFar;

	// 월드 좌표 구하기
	vector vViewPortPos = vector(In.vTexUV.x * 2.f - 1.f, In.vTexUV.y * -2.f + 1.f, vDepthDesc.y, 1.f);
	vector vProjPos = vViewPortPos * fViewZ;
	vector vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector vWorldPos = mul(vViewPos, g_ViewMatrixInv);

	vector	vLightDir = vector(vWorldPos.xyz - g_vLightPos, 0.f);
	float	fDistance = length(vLightDir);

	float	fAtt = saturate((g_fRange - fDistance) / g_fRange);


	// Calculate Shade
	Point_Shade(vNormalDesc, vWorldPos, Out.vShade);
	

	// Calculate Specular
	//vector	vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	//vector	vCamLook = normalize(vector(vWorldPos.xyz - g_vCamPosition, 0.f));
	//Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vCamLook * -1.f, vReflect)), 30.f) * fAtt;
	//Out.vSpecular.a = 0.f;

	// Calculate Ambient
	Out.vAmbient = g_vLightAmbient * g_vMtrlAmbient * fAtt;
	Out.vAmbient.a = 1.f;

	return Out;
}


// For. Point Light
BlendState	BS_Add
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendEnable[2] = true;
	BlendEnable[3] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;
};

technique11			DefaultTechnique
{
	// 0
	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	// 1
	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}
}