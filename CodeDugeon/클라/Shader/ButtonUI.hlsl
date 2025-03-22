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

cbuffer ButtonTimerBuffer : register(b13)
{
    float _ifPressed;
    float _durationTime;
    float2 _padding;
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

    float4 texColor;
    if (_ifPressed)
    {
        texColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else
    {
        texColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
    }

    float borderSize = 0.01;

// Define the rectangular border conditions
    bool isBorder = (In.vTexUV.x < borderSize || In.vTexUV.x > (1.0 - borderSize) ||
                 In.vTexUV.y < borderSize || In.vTexUV.y > (1.0 - borderSize));

    if (_ifPressed && isBorder)
    {
        Out.vColor = float4(float3(1, 0, 0), 1.0); // Red color for the border
    }
    else
    {
        Out.vColor = texColor;
    }

   // if (Out.vColor.a < 0.1)
   //     discard;

    return Out;

}

#endif // _RECT_H_DEFAULTUI_HLSL_LSL_