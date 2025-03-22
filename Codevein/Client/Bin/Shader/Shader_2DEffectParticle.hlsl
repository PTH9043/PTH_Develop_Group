#include "Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
texture2D g_DepthTexture;

float			g_OwnerFadeOut = 0.f;


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;
	float4 vColor : TEXCOORD5;
	float4 vBlendColor : TEXCOORD6;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vColor : TEXCOORD1;
	float4 vBlendColor : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vColor = In.vColor;
	Out.vBlendColor = In.vBlendColor;
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vColor : TEXCOORD1;
	float4 vBlendColor : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vTexColor= g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (vTexColor.r <= 0.1f && vTexColor.g <= 0.1f && vTexColor.b <= 0.1f)
		discard;

	if (vTexColor.a <= 0.1f)
		discard;

	if (vTexColor.r >= 0.95f && vTexColor.g >= 0.95f && vTexColor.b >= 0.95f)
		vTexColor.rgb = 0.f;

	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);
	float			fViewZ = vDepthDesc.x * 500.0f;

	float		fDistance = normalize(saturate(fViewZ - In.vProjPos.w));

	vector vCurColor = (vTexColor + In.vColor);
	vCurColor *= (1 - In.vBlendColor);
	Out.vColor = vCurColor;
	if(fDistance != 0.f)
		Out.vColor.a *=  fDistance;

	Out.vColor.a *= (1 - g_OwnerFadeOut);
	return Out;
}



technique11			DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
