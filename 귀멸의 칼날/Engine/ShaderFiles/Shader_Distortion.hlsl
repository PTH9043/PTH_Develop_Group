#include "Shader_Defines.hpp"

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	float4 vWorldPos = vPosition;
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = vWorldPos;
	return Out;
}

//-------------------------- DISTORTION 1 -------------------------------------------

float3 g_vResolution = (float3)0;
float g_fTime = 0.8f;
texture2D g_Texture1;
texture2D g_Texture2;

static float4 out_shadertoy_out_color = { 0, 0, 0, 0 };
static float4 gl_FragCoord = float4(0, 0, 0, 0);

cbuffer DriverConstants : register(b1)
{
	float4 dx_ViewCoords : packoffset(c1);
	float3 dx_DepthFront : packoffset(c2);
	float2 dx_ViewScale : packoffset(c3);
	struct SamplerMetadata
	{
		int baseLevel;
		int internalFormatBits;
		int wrapModes;
		int padding;
		int4 intBorderColor;
	};
	SamplerMetadata samplerMetadata[2] : packoffset(c4);
};

float Wind(in float3 _p)
{
	float _d = (max(0.0, (0.27000001 - (max(0.0, (length(_p) - 0.69999999)) / 0.69999999))) / 0.27000001);
	float _x = max(0.2, (_p.x * 2.0));
	(_p.y *= (1.0 + (max(0.0, ((-_p.x) - 0.175)) * 1.5)));
	(_p -= ((_d * normalize(_p)) * 0.30000001));
	(_p += float3((g_fTime * 0.40000001), 0.0, 0.0));
	(_p = abs((frac(((_p + float3(1.5, 2.0, -1.5)) * 0.1)) - 0.5)));
	 for (int _i = { 0 }; (_i < 13); (_i++))
	{
		(_p = ((abs(_p) / dot(_p, _p)) - 0.69999999));
	}
	 return ((length(_p) * (1.0 + ((_d * 3.5) * _x))) + ((_d * 3.5) * _x));
}

#define ATOMIC_COUNTER_ARRAY_STRIDE 4

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

void PS_DISTORTION_IMAGE_1_MAIN(inout float4 _fragColor, in float2 fragCoord)
{
	float2 uv = fragCoord;
	float3 _dir = float3(uv, 1.f);
	_dir.x *= 1280.f / 720.f;
	float3 _from = float3(0.f, 0.f, -2.f + g_Texture1.Sample(DefaultSampler, uv * 0.5f + g_fTime).x * 0.025f);
	float _v3 = 0.f;
	float _l = { -0.1f };
	float _t = (g_fTime );
	for (int r = 10; r < 110; ++r)
	{
		float3 _p = _from + (r * _dir) * 0.025f;
		float _tx = g_Texture1.Sample(DefaultSampler, uv * 0.2f + float2(_t, 0.f)).x * 0.015f;
		if ((((length(_p) - 0.69999999) - _tx) > 0.0))
		{
			(_v3 += (min(50.0, Wind(_p)) * max(0.0, (1.0 - (r * 0.0049999999)))));
		}
		else
		{
			if ((_l < 0.0))
			{
				float sc55 = max(0.52999997, dot(normalize(_p), float3(-0.86386842, 0.43193421, -0.25916052)));
				(_l = ((((sc55 * sc55) * sc55) * sc55) * (0.5 + (g_Texture2.Sample(DefaultSampler, 
					(((uv * float2(2.0, 1.0)) * (1.0 + (_p.z * 0.5))) + float2((_tx + (_tx * 0.5)), 0.0))).x * 2.0))));
			}
		}
	}
	(_v3 /= 110.0);
	(_v3 *= 0.43000001);
	float3 _col = (float3((_v3 * 1.25), (_v3 * _v3), ((_v3 * _v3) * _v3)) + (_l * float3(0.55000001, 0.40000001, 0.30000001)));
	(_col *= (1.0 - (length(pow(abs(uv), float2(5.0, 5.0))) * 14.0)));
	(_fragColor = float4(_col, 1.0));
}

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_DISTORTION_1(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a == 0.f)
		discard;

	float4 OutValue;
	PS_DISTORTION_IMAGE_1_MAIN(OutValue, In.vTexUV);
	Out.vColor = OutValue;
	return Out;
}
//-------------------------- DISTORTION 2 -------------------------------------------

float Saturate(in float f)
{
	return clamp(f, 0.f, 1.f);
}

void PS_DISTORTION_IMAGE_2_MAIN(inout float4 _fragColor, in float2 fragCoord)
{
	float time = g_fTime;
	float2 uv = fragCoord;
	//constant slow rotation
	float cost = cos(-time * 0.2);
	float sint = sin(-time * 0.2);
	float2x2 trm = float2x2(cost, sint, -sint, cost);
	float2 _p = uv;
	_p = mul(_p, transpose(trm));
	float _d = length(_p);
	float _cosr = cos(1.6 * sint * (1.6 * time));
	float _sinr = sin(1.6 * cost * (1.6 * time));
	float2x2 _rm = float2x2(_cosr, _sinr, -_sinr, _cosr);
	_p = lerp(_p, mul(_p, transpose(_rm)), _d);
	float _angle = (((atan2(_p.y, _p.x) / 6.2800002) * 0.5) + 0.5);
	_angle += (sin((((-time) * 5.0) + (frac(((_d * _d) * _d)) * 10.0))) * 0.0040000002);
	(_angle *= 6.0);
	(_angle = frac(_angle));
	float _arms = abs(((_angle * 2.0) - 1.0));
	(_arms = pow(_arms, (((10.0 * _d) * _d) + 5.0)));
	float _bulk= (1.0 - Saturate(_d));
	float sc = _bulk;
	float _core = ((((((((sc * sc) * sc) * sc) * sc) * sc) * sc) * sc )* sc);
	float4 _color = lerp(float4(2.0, 0.5, 0.1, 1.0), float4(0.80000001, 0.60000002, 1.0, 1.0), (_d * 2.0));
	(_fragColor = ((((_bulk * _arms) * _color) + _core) +
		((_bulk * 0.25) * lerp(_color, float4(1.0, 1.0, 1.0, 1.0), 0.5))));
	(_fragColor = (2.0 * _fragColor));
}

// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_DISTORTION_2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a == 0.f)
		discard;

	float4 OutValue;
	PS_DISTORTION_IMAGE_2_MAIN(OutValue, In.vTexUV);
	Out.vColor = vColor + ( OutValue);
	return Out;
}

// ======================== 3 ==============================

static float _gk1s[25] = { 0.003765, 0.015019, 0.023792, 0.015019, 0.003765, 0.015019, 0.059912, 
0.094907001, 0.059912, 0.015019, 0.023792, 0.094907001, 0.150342, 0.094907001, 0.023792, 0.015019, 
0.059912, 0.094907001, 0.059912, 0.015019, 0.003765, 0.015019, 0.023792, 0.015019, 0.003765 };


void PS_DISTORTION_IMAGE_3_MAIN(inout float4 _fragColor, in float2 fragCoord)
{
	float2 _uv = fragCoord;
	float2 _texel = (1.f / float2(1270.f, 1080.f));
	float4 totalColor = (float4)0.f;
	for (int i = 0; i < 5; ++i)
	{
		float f = g_fTime;
		for (int j = 0; j < 5; ++j)
		{
			float t = (float)j - 2.f;
			float4 Color = g_Texture2.Sample(DefaultSampler, _uv + float2(_texel.x * f, _texel.y * t));
			totalColor += Color * _gk1s[i * 5 + j];
		}
	}
	_fragColor = totalColor;
}

// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_DISTORTION_3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a == 0.f)
		discard;

	float4 OutValue;
	PS_DISTORTION_IMAGE_3_MAIN(OutValue, In.vTexUV);
	Out.vColor =  vColor * (OutValue);
	return Out;
}

// ======================== 4 ==============================

#define h21(p) ( fract(sin(dot(p,vec2(12.9898,78.233)))*43758.5453) ) //hash21
#define rot(a)   mat2(cos(a + vec4(0,11,33,0)))                             //rotate 2d
#define p(t, a, b, c, d) ( a + b*cos( 6.28318*(c*t+d) ) )                   //iq's palette

float noise(in float2 _f)
{
	float2 v = floor(_f);
	float2 _u = (_f * _f) * (3.f - (2.f * _f));
	return lerp(lerp(frac((sin(dot((v + float2(0.0, 0.0)), float2(12.9898, 78.233002))) * 43758.547)),
		frac((sin(dot((v + float2(1.0, 0.0)), float2(12.9898, 78.233002))) * 43758.547)), _u.x),
		lerp(frac((sin(dot((v + float2(0.0, 1.0)), float2(12.9898, 78.233002))) * 43758.547)),
		frac((sin(dot((v + float2(1.0, 1.0)), float2(12.9898, 78.233002))) * 43758.547)), _u.x), _u.y);
}

float3 HexToSqr(in float2 _s)
{
	float3 _r = (float3)0.f;
	if (_s.y > 0.f - abs(_s.x) *0.57776999)
	{
		if (_s.x > 0)
		{
			_r = float3(_s.x, (_s.y + ((_s.x / 1.73) *0.86000001) * 2.f), 3.f);
		}
		else
		{
			_r = float3(_s.x, (_s.y + ((_s.x / 1.73) *0.86000001) * 2.f), 2.f);
		}
	}
	else
	{
		_r = float3(_s.x, (_s.y + ((_s.x / 1.73) *0.86000001) * 2.f), 1.f);
	}
	return _r;
}

float2 To_Sqr(in float2 _lc)
{
	return float2((_lc.x + _lc.y) * 0.5f, (_lc.x - _lc.y) * 0.28902);
}

float mod_emu(float x, float y)
{
	return x - y * floor(x / y);
}

void Voxel_float4(in float2 _uv, in float2 _id, in float2 _lc, in float2 _sh, inout float4 _value)
{
	float _T = g_fTime;
	float _sm = { 10.f };
	float2 _sp = { 20.f ,20.f };


	float2 _ic = float2(_uv.x + (_uv.y * 1.73f), (_uv.x - (_uv.y * 1.73f)));
	_uv += _sh * float2(-1.f, 0.28902f);
	float2 _ii = floor(_ic);
	float _th = lerp(lerp(1.0, (noise((_ii * 0.5)) * 0.30000001),
		smoothstep(4.0, 1.0, abs(((_ii.x + 15.0) - (noise(float2((_ii.y * 0.1), 0.0)) * 15.0))))),
		smoothstep(2.0, 4.0, length(((_ii + _sp) - float2(-0.5, (_T + 0.5))))), smoothstep(2.0, -1.0, (_ii.y - _T)));

	float sc_1 = noise(float2(((frac((sin(dot(_ii, float2(12.9898, 78.233002))) * 43758.547)) * _ii) + (g_fTime * 0.5))));
	float _s= ((((((((sc_1 * sc_1) * sc_1) * sc_1) * sc_1) * sc_1) * sc_1) * sc_1) * 0.75);
	float sc_2 = noise((mul((_ii * 0.2), transpose(float2x2(0.54030228, 0.84385395, -0.84857029, 0.54030228)))
		- (g_fTime * 0.02)));
	float _hg = ((((((sc_2 * sc_2) * sc_2) * sc_2) - 0.5) * 2.0) + _s);
	(_hg = (((_hg + 1.0) * _hg) - 1.0));

	float _sz = (1.1 + ((_s * 1.5) * _th));
	float3 _vx= HexToSqr((_lc - float2(_sh.x, ((_sh.y - (((_hg * 2.0) - 1.0) / _sz)) * 0.28902))));
	(_vx.xy *= _sz);

	float4 _V= float4((float3(0.89999998, 0.89999998, 0.89999998) + 
		(float3(0.69999999, 0.69999999, 0.69999999) * 
		cos((6.2831802 * ((((float3(0.25999999, 0.25999999, 0.25999999) * _ii.y) * 0.050000001)
			+ ((_hg * 0.30000001) * _th)) + float3(0.0, 0.1, 0.2)))))), 1.0);

	float _f = lerp(0.30000001, (0.89999998 - (_vx.z * 0.15000001)),
		smoothstep((0.44999999 + _sm), (0.44999999 - _sm), max(abs((_vx.x - 0.5)), abs((_vx.y - 0.5)))));

	(_f = lerp(_f, (1.0 - length((_vx.xy - float2(0.72000003, 0.72000003)))),
		smoothstep((0.40000001 + _sm), (0.40000001 - _sm), length((_vx.xy - 0.5)))));
	(_f = lerp(_f, 0.40000001, smoothstep((0.039999999 + _sm), (-_sm),
		abs((length((_vx.xy - 0.5)) - 0.40000001)))));
	(_f -= ((_f * smoothstep(5.0, 3.0, length(((_ic + _sp) - float2(-0.5, (_T + 0.5)))))) * 0.5));
	(_f += ((_hg + 1.0) * 0.07));
	(_value = lerp(_value, (_V * _f), smoothstep((1.0 + _sm), (1.0 - _sm), max(_vx.x, _vx.y))));
}

void Draw(in float2 _uv, inout float4 _value)
{
	float _T = g_fTime;
	float _sm = { 10.f };
	float2 _sp = { 20.f ,20.f };

	_sp = float2(1.f - noise(float2(_T * 0.1f, 0.f)) * 15.f, 0.f);
	float2 _st = float2(_uv.x + _uv.y * 1.73f, _uv.x - (_uv.y * 1.73f));
	float2 _sc = To_Sqr(_st + _sp) - _uv + float2(0.f, (2.f + noise(float2(g_fTime * 5.f, 0.f)) * 0.2f));
	float2 _vc = _uv;
	if (length(_vc + _sc) < 3.f)
	{
		_vc += _sc;
		float s = length(_vc * 0.34999999);
		_vc += _vc * (((s * s) * s) * s) - _sc;
		_sm = _sm + 0.07f;
		_vc += To_Sqr(float2(0.f, g_fTime));
		float2 _id = floor(float2(_vc.x + (_vc.y * 1.73f), _vc.x - (_vc.y * 1.73f)));
		float _n = mod_emu((_id.x + _id.y + 1.f), 2.0f);
		_st = float2((1.f - _n) * 0.5f - _vc.x, (_vc.y * 1.73) - (_n * 0.5));
		_id = floor(_st) * float2(1.f, 2.f) + float2(_n * 0.5f, _n);
		float2 _lc = (frac(_st) - float2(0.5, 0.5));
		_lc.y *= 0.57804;
		float2 _shArr[7] = { 0.0, -2.0, 0.5, -1.0, -0.5, -1.0, 0.0, 0.0, 0.5, 1.0, -0.5, 1.0, 0.0, 2.0 };
		for (int i = 0; i < 7; ++i)
		{
			Voxel_float4(_vc, _id, _lc, _shArr[i], _value);
		}
	}
	_uv += _sc;
	if (length(_uv) < 3.0f)
	{
		(_value = lerp(_value, float4(0.0, 0.050000001, 0.15000001, 1.0),
			(noise(mul((float2((-_uv.x), ((_uv.y + (_uv.x / 1.73)) * 0.86000001)) * 1.5),
				transpose(float2x2(cos((((-_T) * 0.15000001) + float4(0.0, 11.0, 33.0, 0.0))))))) * 0.40000001)));

		(_value += ((smoothstep(3.0, -10.0, _uv.y) * length(_uv)) * 0.1));
	}
	(_value = lerp(_value, float4(0.0, 0.0, 0.0, 0.0), (smoothstep((0.02 + _sm), (0.02 - _sm),
		abs((length(_uv) - 3.0))) * 0.25)));
}

void PS_DISTORTION_IMAGE_4_MAIN(inout float4 _fragColor, in float2 fragCoord)
{
	float _T = g_fTime;
	float _sm = { 10.f };
	float2 _sp = { 20.f ,20.f };

	float2 _rz = float2(1280.f, 720.f);
	float2 _uv = ((fragCoord + fragCoord) - _rz) / -_rz.y;
	_uv += -0.5f;
	_uv *= 1.f + sin(g_fTime * 0.3f) * 0.25f;
	_sm = 3.f / _rz.y;
	_T = (g_fTime * 4.f) - ((sin(g_fTime * 0.5f) + 1.f) * 5.f);
	Draw(_uv * 5.f, _fragColor);
	_uv *= 5.f;
	_fragColor = pow(_fragColor, float4(0.71428573, 0.71428573, 0.71428573, 0.71428573));
}

PS_OUT PS_DISTORTION_4(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a == 0.f)
		discard;

	vector vDistortion = g_Texture2.Sample(DefaultSampler, In.vTexUV);

	float4 OutValue = (float4)0.f;
	PS_DISTORTION_IMAGE_4_MAIN(OutValue, In.vTexUV);
	Out.vColor = vColor + (vDistortion *  OutValue);
	return Out;
}

float2 g_UvFlowValue = float2(0.2f, 2.0f);
float	g_fMultipleTimeValue = 0.0125f;
float	g_DistortionDivineValue = 8.0f;
float4	g_vColor1 = float4(0.2f, 0.8f, 0.f, 0.3f);
texture2D g_DDTexture;

void PS_DISTORTION_IMAGE_5_MAIN(inout float4 _fragColor, in float2 fragCoord)
{
	float2 uv = fragCoord;
	float time = g_fTime;
	float4 vDistortion = g_Texture2.Sample(DefaultSampler, +float2(uv.x * g_UvFlowValue.x + g_fTime *
		g_fMultipleTimeValue, uv.y * g_UvFlowValue.y)) / g_DistortionDivineValue;
	float2 where = uv.xy - vDistortion.xy;
	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, where);
	
	_fragColor = float4(vColor +  vDistortion);
	_fragColor.a = _fragColor.r;
}

PS_OUT PS_DISTORTION_5(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDD = g_DDTexture.Sample(DefaultSampler, In.vTexUV);
	if (vDD.a == 0.f)
		discard;

	float4 OutValue;
	PS_DISTORTION_IMAGE_5_MAIN(OutValue, In.vTexUV);
	OutValue = lerp(OutValue, vDD, 0.5f);
	Out.vColor = OutValue;
	Out.vColor.a = Out.vColor.r * 2.5f;
	return Out;
}

//----------------------------------------------------------------------------------------

float2	vDistortionColor = float2(1.f, 1.f);

PS_OUT PS_COLOR_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDistortion = g_DDTexture.Sample(DefaultSampler, In.vTexUV);

	float2 distort_amount = vDistortionColor * vDistortion.xy;

	distort_amount = 0.5f - distort_amount;
	if (distort_amount.x > 0.5) { distort_amount.x -= 1.0; }// wrap around
	if (distort_amount.y > 0.5) { distort_amount.y -= 1.0; }// wrap around
	distort_amount /= g_DistortionDivineValue;

	Out.vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV + distort_amount);

	if (0.f == vDiffuse.a)
		discard;

	return Out;
}

//-----------------------------------------------------------------------------------------

technique11			DefaultTechnique
{
	//0
	pass DISTORTION_1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_1();
	}

	//1
	pass DISTORTION_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_2();
	}

	pass DISTORTION_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_3();
	}

	pass DISTORTION_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_4();
	}

	pass DISTORTION_5
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_5();
	}

	//5
	pass ColorDistortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0.f);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR_DISTORTION();
	}
}


//float2 Trans = In.vTexUV + 0.001f;
//
//vector vDistor = g_DistortionTexture.Sample(DefaultSampler, Trans);
//float fWeight = vDistor.r * 0.5f;
//
//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vDistor.xy * 0.05f);
//
//if (Out.vColor.a == 0.f)
//discard;
//
//if (Out.vColor.r <= 0.1f && Out.vColor.g <= 0.1f && Out.vColor.b <= 0.1f)
//discard;