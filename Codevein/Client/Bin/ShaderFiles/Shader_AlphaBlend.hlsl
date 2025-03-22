
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;

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

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
		Out.vColor.rgb += 0.5f;

	if (Out.vColor.r > 0.9f && Out.vColor.g > 0.9f && Out.vColor.b < 0.9f)
		Out.vColor.rgb *= 0.5f;

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