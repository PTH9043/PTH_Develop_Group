#include "Shader_Defines.hpp"

struct VS_IN
{
	float3 vPosition : POSITION;	
	float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;	
	float3 vTexUV : TEXCOORD0;	
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
	float3 vTexUV : TEXCOORD0;	
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;	
};


/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0; 

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}



technique11			DefaultTechnique
{
	pass Sky
	{
		SetRasterizerState(RS_Sky);
		SetDepthStencilState(DSS_NonDepth, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}
