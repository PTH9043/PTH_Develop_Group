#ifndef _UP_SAMPLE_HLSL_
#define _UP_SAMPLE_HLSL_

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

    // 입력 텍스처 좌표를 그대로 사용하여 샘플링합니다.
    float4 texColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV ); //In.vTexUV*0.5

    Out.vColor = texColor;
    
    return Out;
}

#endif // _UP_SAMPLE_HLSL_
