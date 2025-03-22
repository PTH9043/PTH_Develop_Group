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

cbuffer LoadingTimerBuffer : register(b13)
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

    float4 texColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    float duration = _durationTime ;
    float alpha = 1.0f;

    if (_ifPressed)
    {
        float timeSection = _durationTime% 3.0f;
        float dotInterval = 1.0f / 3.0f;

        if (In.vTexUV.x <= dotInterval)
        {
            // 첫 번째 점
            alpha = (timeSection < 1.0f) ? 1.0f : 0.0f;
        }
        else if (In.vTexUV.x <= 2.0f * dotInterval)
        {
            // 두 번째 점
            alpha = (timeSection >= 1.0f && timeSection < 2.0f) ? 1.0f : 0.0f;
        }
        else
        {
            // 세 번째 점
            alpha = (timeSection >= 2.0f) ? 1.0f : 0.0f;
        }

        texColor.a *= alpha;
    }

    Out.vColor = texColor;
    return Out;
}

#endif // _RECT_H_DEFAULTUI_HLSL_LSL_