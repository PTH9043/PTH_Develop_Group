
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D g_UVTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	//minfilter = linear;
	//magfilter = linear;0
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
	float4 vAfterImg : SV_TARGET0;
};

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vUV = g_UVTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vAfterImg = vUV;

	if (vUV.a == 0)
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

	/*DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;*/
};

BlendState	BS_Default
{
	BlendEnable[0] = false;
	//SrcBlend = src_alpha;
	//DestBlend = inv_src_alpha;
};

BlendState	BS_Add
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;
};

technique11			DefaultTechnique
{
	pass AlphaBlend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
	}
}