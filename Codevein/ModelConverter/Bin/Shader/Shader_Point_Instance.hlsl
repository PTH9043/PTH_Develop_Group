#include "Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
float3		g_vCamPosition;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : PSIZE;

	float4 vRight : TEXCOORD0;
	float4 vUp : TEXCOORD1;
	float4 vLook : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	
	Out.vPosition = mul(vPosition, g_WorldMatrix);		
	Out.vPSize = In.vPSize;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN( point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT			Out[4];

	float3			vLook = g_vCamPosition - In[0].vPosition.xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3			vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	matrix			matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(1.f, 0.f);

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(0.f, 0.f);


	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(0.f, 1.f);



	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(1.f, 1.f);


	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
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

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.gb = 0.f;
	Out.vColor.r = 1.f;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}



technique11			DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
