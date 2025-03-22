#ifndef _RECT_HLSL_
#define _RECT_HLSL_

#include "ShaderGrobalFunc.hlsli"

cbuffer RECTCOLOR : register(b4)
{
    float4 g_RectColor;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION0;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN In) //이름 VS_Main PS_Main 유지.
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = Compute_FinalMatrix(In.vPosition);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_Main(PS_In Input)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_RectColor;
    return Out;
}

#endif // _RECT_HLSL_