
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float3		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

texture2D	g_Texture;
texture2D	g_NormalTexture;
texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	//minfilter = linear;
	//magfilter = linear;
	//mipfilter = linear;
};

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


/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

struct PS_OUT_LIGHT
{
	float4 vShade : SV_TARGET0;
};


PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(vector(g_vLightDir, 0.f)) * -1.f, vNormal));
	Out.vShade.a = 1.f;

	return Out;
}


PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* 디퓨즈 타겟에 그려져있던 색과 셰이드 타겟에 그려져있느 ㄴ색을 혼합하여 백버퍼에 그린다. */
	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vDiffuseDesc * vShadeDesc;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Debug
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState	BS_Default
{
	BlendEnable[0] = false;
	//SrcBlend = src_alpha;
	//DestBlend = inv_src_alpha;
};



technique11			DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
}
