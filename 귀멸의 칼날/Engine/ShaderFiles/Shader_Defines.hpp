

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_PrevWorldMatrix, g_PrevViewMatrix;
matrix		g_LightViewMatrix, g_LightProjMatrix;


float		g_CamFar = (float)0, g_LightFar = (float)0;
int			g_iUVIndex = (int)0;
bool		g_bWrapSampler = false;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;


sampler DefaultSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ClampSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

sampler UWVCSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = clamp;
};

sampler UCVWSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/

RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_None
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_WireFrame
{
	FillMode = WireFrame;
	CullMode = Back;
	FrontCounterClockwise = false;
};
//BOOL DepthEnable;
//D3D11_DEPTH_WRITE_MASK DepthWriteMask;
//	 DepthFunc;
//BOOL StencilEnable;
//UINT8 StencilReadMask;
//UINT8 StencilWriteMask;
//D3D11_DEPTH_STENCILOP_DESC FrontFace;
//D3D11_DEPTH_STENCILOP_DESC BackFace;

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_NonDepth
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DSS_Debug
{
	DepthEnable = false;
	DepthWriteMask = zero;

	/*DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;*/
};

//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;

BlendState	BS_Default
{
	BlendEnable[0] = false;
//SrcBlend = src_alpha;
//DestBlend = inv_src_alpha;
};


BlendState	BS_AlphaBlend
{
	BlendEnable[0] = true;
//BlendEnable[1] = true;
SrcBlend[0] = src_alpha;
DestBlend[0] = inv_src_alpha;
BlendOp[0] = Add;
//BlendOpAlpha[0] = Add;
};


//AlphaToCoverageEnable = FALSE;
//IndependentBlendEnable = FALSE;
//BlendEnable[0] = TRUE;
//SrcBlend[0] = D3D11_BLEND_SRC_ALPHA;
//DestBlend[0] = D3D11_BLEND_INV_SRC_ALPHA;
//BlendOp[0] = D3D11_BLEND_OP_ADD;
//SrcBlendAlpha[0] = D3D11_BLEND_ONE;
//DestBlendAlpha[0] = D3D11_BLEND_INV_SRC_ALPHA;
//BlendOpAlpha[0] = D3D11_BLEND_OP_ADD;
//RenderTargetWriteMask[0] = D3D11_COLOR_WRITE_ENABLE_ALL;


BlendState	BS_Blend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendEnable[2] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = Add;
	SrcBlendAlpha = src_alpha;
	DestBlendAlpha = inv_src_alpha;
	BlendOpAlpha = Add;
};


