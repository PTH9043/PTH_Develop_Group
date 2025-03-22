
#include "Shader_Defines.hpp"

vector		g_vColor = vector(0.f, 1.f, 0.f, 1.f);

struct VS_IN
{
	float3 vPosition : POSITION;
	float4 vColor : COLOR0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vColor : COLOR0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vColor = In.vColor;

	return Out;
}


/* w나누기 연산.  */
/* 뷰포트로 변환. */
/* 래스터라이즈를 수행 .*/

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vColor: COLOR0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;	
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0; 

	Out.vColor = In.vColor * g_vColor;

	return Out;
}



technique11			DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
