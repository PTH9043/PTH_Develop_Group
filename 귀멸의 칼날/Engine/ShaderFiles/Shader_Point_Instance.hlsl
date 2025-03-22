#include "Shader_Defines.hpp"

float3		g_vCamPosition;
float4		g_vColor;
float2		g_vSliceUV_START;
float2		g_vSliceUV_END = float2(0.f, 0.f);
float2		g_vSliceUV_DELTA;
float2		g_vUVDir;
float3		g_vScale;
float		g_fAlpha = 1.f;
bool		g_bCutBlack = false;
bool		g_bGlow = false;

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

	float4 vRight : TEXCOORD0;
	float4 vUp : TEXCOORD1;
	float4 vLook : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = In.vPSize;
	Out.vRight = In.vRight;
	Out.vUp = In.vUp;
	Out.vLook = In.vLook;
	Out.vTranslation = In.vTranslation;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;

	float4 vRight : TEXCOORD0;
	float4 vUp : TEXCOORD1;
	float4 vLook : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT			Out[4];

	float2 vSize = float2(g_vScale.x, g_vScale.y);

	float3			vLook = g_vCamPosition - In[0].vPosition.xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * vSize.x * 0.05f;
	float3			vUp = normalize(cross(vLook, vRight)) * vSize.y * 0.05f;

	/*float3			vLook = g_vCamPosition - In[0].vPosition.xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.05f;
	float3			vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.05f;*/

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

[maxvertexcount(6)]
void GS_MAIN_SPARK(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT			Out[4];

	float2 vSize = float2(g_vScale.x, g_vScale.y);

	float3			vUp = In[0].vUp.xyz * vSize.y * 1.5f;
	float3			vRight = In[0].vRight.xyz * vSize.x * 0.3f;

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

struct VS_OUT_SOFT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;

	float4 vRight : TEXCOORD0;
	float4 vUp : TEXCOORD1;
	float4 vLook : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;

	float4 vProjPos : TEXCOORD4;
};

VS_OUT_SOFT VS_MAIN_SOFT(VS_IN In)
{
	VS_OUT_SOFT		Out = (VS_OUT_SOFT)0;

	matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	Out.vProjPos = mul(vPosition, g_ViewMatrix);
	Out.vProjPos = mul(Out.vProjPos, g_ProjMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = In.vPSize;
	Out.vRight = In.vRight;
	Out.vUp = In.vUp;
	Out.vLook = In.vLook;
	Out.vTranslation = In.vTranslation;

	return Out;
}

struct GS_IN_SOFT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;

	float4 vRight : TEXCOORD0;
	float4 vUp : TEXCOORD1;
	float4 vLook : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;

	float4 vProjPos : TEXCOORD4;
};

struct GS_OUT_SOFT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_NOTLOOK(point GS_IN_SOFT In[1], inout TriangleStream<GS_OUT_SOFT> OutStream)
{
	GS_OUT_SOFT			Out[4];

	float2 vSize = float2(g_vScale.x, g_vScale.y);

	float3			vLook = float3(0.f, 0.f, 1.f);
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * vSize.x * 0.05f;
	float3			vUp = normalize(cross(vLook, vRight)) * vSize.y * 0.05f;

	/*float3			vLook = g_vCamPosition - In[0].vPosition.xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.05f;
	float3			vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.05f;*/

	matrix			matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(1.f, 0.f);
	Out[0].vProjPos = In[0].vProjPos;

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(0.f, 0.f);
	Out[1].vProjPos = In[0].vProjPos;

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(0.f, 1.f);
	Out[2].vProjPos = In[0].vProjPos;

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(1.f, 1.f);
	Out[3].vProjPos = In[0].vProjPos;

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
	float4 vGlow	: SV_TARGET1;
	float4 vBlur : SV_TARGET2;
};


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	/*if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;*/

	Out.vColor *= g_vColor;

	//Out.vColor += 10.f;

	Out.vColor.a *= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_bCutBlack)
		Out.vColor.a = Out.vColor.r;

	Out.vColor *= g_vColor;

	//Out.vColor.rgb += 0.5f;

	Out.vColor.a *= g_fAlpha;

	if (g_bGlow)
		Out.vGlow = Out.vColor;

	return Out;
}

struct PS_IN_SOFT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFT(PS_IN_SOFT In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0.f < g_vSliceUV_END.x && 0.f < g_vSliceUV_END.y)
	{
		In.vTexUV.x = g_vSliceUV_START.x + (In.vTexUV.x * g_vSliceUV_DELTA.x);
		In.vTexUV.y = g_vSliceUV_START.y + (In.vTexUV.y * g_vSliceUV_DELTA.y);
	}

	float2	vTexUV;

	// UV 좌표를 0 ~ 1 사이의 값으로 변환
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	// View 스페이스 상의 깊이 값으로 변환
	float	fViewZ = vDepthDesc.x * g_CamFar;

	// 현재 픽셀의 카메라로부터의 거리를 구함
	float	fDistance = saturate(fViewZ - In.vProjPos.w);

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_bCutBlack)
		vColor.a = vColor.r;

	vColor.a = vColor.a * fDistance;

	vColor *= g_vColor;

	Out.vColor.a *= g_fAlpha;

	Out.vColor = vColor;

	return Out;
}

technique11			DefaultTechnique
{
	// 0 : 빌보드
	pass DefaultPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1 :
	pass DefaultBlendPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	// 2 : 길죽하게
	pass SparkPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 3
	pass SparkBlendPass
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK();
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	// 4 : 그냥 출력
	pass NotLook
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOFT();
		GeometryShader = compile gs_5_0 GS_NOTLOOK();
		PixelShader = compile ps_5_0 PS_MAIN_SOFT();
	}

	// 5
	pass Temp1
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
