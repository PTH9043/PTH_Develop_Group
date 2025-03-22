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

float _fTexWidth = 1280.f;
float _fTexHeight = 720.f;

float2 _ScreenSunPos = float2(0.5, 0.1);
int _iNumSamples = 10;
float _fUVNumSamples = 4.f;
float _Exposure = 1.f;
float _Density = 1.f;
float _illuminationDecay = 1.f;
float _Decay = 0.6f;
float2 _SunDirVel = float2(0, -3);

void VolumetricLight(in float2 uv)
{
	//fC 는 오리지날씬 텍스쳐 색상값
	float4 fC = g_DiffuseTexture.Sample(DefaultSampler, uv);// tex2D(Samp_Texture_Orig, uv);
	float4 PsColor = fC;

	//샘플링 갯수
	//갯수가 많아질수록 부드러워집니다만,
	//그만큼 부하는 커지겠지요.
	int iCount = 20;

	//오리지날씬과 동일한 uv 값을 받아와서
	float2 texCoord = uv;

	//uv 를 이동시킬 방향성을 구해두고,
	float2 deltaTexCoord = texCoord - _ScreenSunPos;

	//광선의 밀도치
	//발광체가 만들어 내는 빔의 간격에 영향을 끼칩니다.
	deltaTexCoord.x *= 1.f / (float)iCount * _Density;
	deltaTexCoord.y *= 1.f / (float)iCount * _Density;

	//Samp_Texture_Light
	float4 color = vector(0.3f, 0.3f, 0.3f, 0.3f);//tex2D(Samp_Texture_Light, texCoord);

	//광선의 감소치 
	float illuminationDecay = _illuminationDecay;
	float4 sample = float4(1, 1, 1, 1);

	for (int i = 0; i < iCount; i++)
	{
		texCoord -= deltaTexCoord;

		//fTexWidth, fTexHeight 는 스크린 크기 (1280, 720 같은....) 
		//sun 방향성 값으로,
		//광선이 뻗어나가는 방향을 정할수 있습니다
		texCoord.x += 1.f / _fTexWidth * _SunDirVel.x;
		texCoord.y += 1.f / _fTexHeight * _SunDirVel.y;

		//
		vector vLight = vector(0.3f, 0.3f, 0.3f, 0.3f);

		//광선이 멀어질수록 약해지듯이
		//감소치를 곱해줍니다.
		vLight *= illuminationDecay;

		//컬러값을 누적시킵니다.
		color += vLight;

		//감소처리
		illuminationDecay *= _Decay;
	}

	//오리지날 픽셀컬러에 더해 줍니다.
	//좀더 강한 빛을 원한다면,
	//_Exposure 값을 크게 주면 됩니다.
	PsColor = fC + (color * _Exposure);

	return PsColor;
}



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}



technique11			DefaultTechnique
{
	pass Light
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}