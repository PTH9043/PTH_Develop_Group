#ifndef _DEBUG2DTARGET_HLSL_
#define _DEBUG2DTARGET_HLSL_

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

VS_OUT VS_Main(VS_IN In)
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

PS_OUT PS_Main(PS_In Input)
{
    PS_OUT Out = (PS_OUT) 0;

    if (length(g_TransformPadding)!=0)
    Out.vColor = g_Texture0.Sample(g_Sampler_Normal, Input.vTexUV).r; //ShadowMap 휜색으로 만들기 위해
    else
    Out.vColor = g_Texture0.Sample(g_Sampler_Normal, Input.vTexUV);
    if (IsColorUp(Out.vColor, 0.95f))
    {
        discard;
    }

    return Out;
}

#endif // _DEBUG2DTARGET_HLSL_