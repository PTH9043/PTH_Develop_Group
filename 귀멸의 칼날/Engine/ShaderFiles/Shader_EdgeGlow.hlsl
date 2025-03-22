#include "Shader_Defines.hpp"

texture2D	g_ColorTexture;

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

struct PS_OUT
{
	float4 vColor : SV_TARGET0;	
};

float  iTime = 0.f;
float4 vGlowColor = float4(1.f, 1.f, 1.f, 1.f);

float lookup(float2 p, float dx, float dy)
{
	float2 dim;
	g_DiffuseTexture.GetDimensions(dim.x, dim.y);

	float d = sin(iTime * 5.0) * 0.5 + 1.5; // kernel offset
	float2 uv = p.xy + float2(dx * d / dim.x, dy * d / dim.y);
	vector c = g_DiffuseTexture.Sample(ClampSampler, uv);

	// return as luma
	return (0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b);
}

PS_OUT PS_EDGEGLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float d = sin(iTime * 5.0) * 0.5 + 1.5; // kernel offset
	float2 p = In.vTexUV;

	// simple sobel edge detection
	float gx = 0.0;
	gx += -1.0 * lookup(p, -1.0, -1.0);
	gx += -2.0 * lookup(p, -1.0, 0.0);
	gx += -1.0 * lookup(p, -1.0, 1.0);
	gx += 1.0 * lookup(p, 1.0, -1.0);
	gx += 2.0 * lookup(p, 1.0, 0.0);
	gx += 1.0 * lookup(p, 1.0, 1.0);

	float gy = 0.0;
	gy += -1.0 * lookup(p, -1.0, -1.0);
	gy += -2.0 * lookup(p, 0.0, -1.0);
	gy += -1.0 * lookup(p, 1.0, -1.0);
	gy += 1.0 * lookup(p, -1.0, 1.0);
	gy += 2.0 * lookup(p, 0.0, 1.0);
	gy += 1.0 * lookup(p, 1.0, 1.0);

	// hack: use g^2 to conceal noise in the video
	float g = gx * gx + gy * gy;
	float g2 = g * (sin(iTime) / 2.0 + 0.5);

	vector col = g_DiffuseTexture.Sample(ClampSampler, p);
	vector vEdgeColor = g_ColorTexture.Sample(DefaultSampler, p);

	if (vEdgeColor.a == 1.f)
		col += vEdgeColor * vector(g, g, g, 0.0);

	Out.vColor = col;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

//=========================================================================================

float3x3 Kx = { -1, 0, 1,
				-2, 0, 2,
				-1, 0, 1 };

float3x3 Ky = { 1,  2,  1,
				0,  0,  0,
			   -1, -2, -1 };


float3x3 K = { -2, -1,  0,
			   -1,  0,  1,
			    0,  1,  2 };

struct PS_OUT2
{
	float4 vColor1 : SV_TARGET0;
	float4 vColor2 : SV_TARGET1;
};

PS_OUT2 PS_OUTLINE(PS_IN In)
{
	PS_OUT2 Out = (PS_OUT2)0;

	float2 dim;
	g_DiffuseTexture.GetDimensions(dim.x, dim.y);

	float Lx = 0;
	float Ly = 0;
	float res = 0;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x / dim.x, y / dim.y);
			float3 tex = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV + offset).rgb;
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			Lx += luminance * Kx[y + 1][x + 1];
			Ly += luminance * Ky[y + 1][x + 1];
			res += luminance * K[y + 1][x + 1];
		}
	}

	float L = sqrt((Lx * Lx) + (Ly * Ly));
	res += 0.5f;

	vector vNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	if (vNormal.a == 1.f)
	{
		Out.vColor1 = float4(L.xxx, 1);
		Out.vColor2 = float4(res.xxx, 1);
	}

	return Out;
}


//===========================================================================

float EdgeGlow(in float2 uv)
{
	// simple sobel edge detection
	float gx = 0.0;
	gx += -1.0 * lookup(uv, -1.0, -1.0);
	gx += -2.0 * lookup(uv, -1.0, 0.0);
	gx += -1.0 * lookup(uv, -1.0, 1.0);
	gx += 1.0 * lookup(uv, 1.0, -1.0);
	gx += 2.0 * lookup(uv, 1.0, 0.0);
	gx += 1.0 * lookup(uv, 1.0, 1.0);

	float gy = 0.0;
	gy += -1.0 * lookup(uv, -1.0, -1.0);
	gy += -2.0 * lookup(uv, 0.0, -1.0);
	gy += -1.0 * lookup(uv, 1.0, -1.0);
	gy += 1.0 * lookup(uv, -1.0, 1.0);
	gy += 2.0 * lookup(uv, 0.0, 1.0);
	gy += 1.0 * lookup(uv, 1.0, 1.0);

	// hack: use g^2 to conceal noise in the video
	float g = gx * gx + gy * gy;

	return g;
}

float	minSeparation = 1.0;
float	maxSeparation = 3.0;
float	minDistance = 0.5;
float	maxDistance = 2.0;
int		outlineSize = 1;
float3	colorModifier = float3(0.324, 0.063, 0.099);
bool	bOutline = false;

PS_OUT PS_OUTLINE1(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// Diffuse(NonAlphaBlend) 텍스쳐
	float2 texSize;
	g_DiffuseTexture.GetDimensions(texSize.x, texSize.y);
	vector	vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	// 깊이 텍스쳐
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float fViewZ = vDepthDesc.x * g_CamFar;
	float depth = saturate(1.0f - ((g_CamFar - fViewZ)/ g_CamFar));
	float separation = lerp(maxSeparation, minSeparation, depth);

	float mx = 0.0;

	for (int i = -outlineSize; i <= outlineSize; ++i) 
	{
		for (int j = -outlineSize; j <= outlineSize; ++j) 
		{
			float2 uv = In.vTexUV + ((float2(i, j) * separation) / texSize);

			float4 vTmpDepth = g_DepthTexture.Sample(ClampSampler, uv);
			float fTmpViewZ = vTmpDepth.x * g_CamFar;

			mx = max(mx, abs(fViewZ - fTmpViewZ));
		}
	}

	float diff = smoothstep(minDistance, maxDistance, mx);
	float3 lineColor = vDiffuseDesc.rgb * colorModifier;

	Out.vColor = vDiffuseDesc;
	if (vNormalDesc.a == 1.f && bOutline)
	{
		vector vEdgeColor = g_ColorTexture.Sample(DefaultSampler, In.vTexUV);
		if (1.f == vEdgeColor.a)
		{
			float g = EdgeGlow(In.vTexUV);
			vEdgeColor.rgb *= g;

			Out.vColor.rgb += vEdgeColor.rgb;
		}
		else
		{
			Out.vColor.rgb = lerp(vDiffuseDesc, float4(lineColor, 1.f), diff);
		}
	}

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

technique11			DefaultTechnique
{
	pass EdgeGlow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EDGEGLOW();
	}

	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE();
	}	
	
	// 2
	pass Outline1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE1();
	}

}
