
sampler DefaultSampler = sampler_state
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
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

RasterizerState RS_SkyModel
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = true;
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
//D3D 11_DEPTH_STENCILOP_DESC FrontFace;
//D3D11_DEPTH_STENCILOP_DESC BackFace;

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Sky
{
	DepthEnable = false;
	DepthWriteMask = zero;
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
	SrcBlend[0] = src_alpha;
	DestBlend[0] = inv_src_alpha;
	BlendOp[0] = Add;
};
