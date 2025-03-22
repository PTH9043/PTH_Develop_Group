#ifndef _DEFAULTUI_HLSL_
#define _DEFAULTUI_HLSL_

#include "ShaderGrobalFunc.hlsli"


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
    VS_OUT Out = (VS_OUT) 0;

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

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
   
    
    return Out;
}

#endif // _RECT_H_DEFAULTUI_HLSL_LSL_