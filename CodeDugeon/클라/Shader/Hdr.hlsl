#ifndef _HDR_HLSL_
#define _HDR_HLSL_

#include "ShaderGrobalFunc.hlsli"


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

    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};



PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseDesc = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);

    float4 HighColor = float4(0, 0, 0, 0);
    float brightness = dot(vDiffuseDesc.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 0.99)
        HighColor = float4(vDiffuseDesc.rgb, 1.f);
  
    Out.vColor = HighColor;
 

    return Out;
}

#endif // _HDR_HLSL_