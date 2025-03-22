#ifndef _MOTIONBLUR_
#define _MOTIONBLUR_

#include "ShaderGrobalFunc.hlsli"

const static int g_iNumBlurValue = 50;
const static int g_iDivineVelocityValue = 50;
const static float g_PassAlpha = 0.04f;
const static float g_VelocityAlpha = 0.f;

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

VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = Compute_FinalMatrix(In.vPosition);

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
    float4 vJudge : SV_TARGET1;
};

PS_OUT PS_Main(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
     
    int iNumBlurSampler = g_iNumBlurValue;
    float4 Velocity = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);

    int iCnt = 1;

    float4 OutColor = (float4) 0.f;
    float4 OUT = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    float judgeData = 0;
    Velocity.xy /= float(g_iDivineVelocityValue);
    for (int i = iCnt; i < iNumBlurSampler; ++i)
    {
        OutColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV + Velocity.xy * (float) i);
        if (Velocity.a < OutColor.a + g_PassAlpha)
        {
            iCnt++;
            OUT += OutColor;
            Out.vJudge.a = 1;
        }
    }
    
    Out.vColor = OUT;
    Out.vColor /= ((float) iCnt);
    return Out;
}

#endif