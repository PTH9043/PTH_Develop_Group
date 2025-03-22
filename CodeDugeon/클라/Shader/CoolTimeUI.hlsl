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
    float _defaultCoolTime;
    float _leftCoolTime;
};

VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION0;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

float4 ComputeCooldownMask(float2 uv, float duration)
{
    float angle = 2.0 * 3.14159265358979323846 * (1.0 - duration);
    float2 center = float2(0.5, 0.5);
    float2 dir = normalize(uv - center);
    float currentAngle = atan2(dir.y, dir.x);
    currentAngle = currentAngle < 0 ? currentAngle + 2.0 * 3.14159265358979323846 : currentAngle;

    return currentAngle <= angle ? float4(1, 1, 1, 1) : float4(0.25, 0.25, 0.25, 1);
}

PS_OUT PS_Main(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 texColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    //float durationFraction = saturate(_leftCoolTime / _defaultCoolTime);
    float durationFraction = _leftCoolTime / _defaultCoolTime;
   // float durationFraction = frac((_defaultCoolTime - fGrobalDeltaTime) / _defaultCoolTime);
    
    float4 cooldownMask = ComputeCooldownMask(In.vTexUV, durationFraction);

  
    Out.vColor = texColor * cooldownMask;

    return Out;
}


#endif // _RECT_H_DEFAULTUI_HLSL_LSL_