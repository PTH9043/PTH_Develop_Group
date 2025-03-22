#ifndef _BLOOM_HLSL_
#define _BLOOM_HLSL_

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



//PS_OUT PS_Main(PS_In In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    vector vHDRColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vBlurColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vOriginalColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);
        
    
//    float4 vBloom = pow(pow(abs(vBlurColor), 2.2f) + pow(abs(vOriginalColor), 2.2f), 1.f / 2.2f);
  
//    float4 OutColor = vHDRColor;
    
//    OutColor = pow(abs(OutColor), 2.2f);
//    vBloom = pow(abs(vBloom), 2.2f);
//    OutColor += vBloom;
//    Out.vColor = pow(abs(OutColor), 1 / 2.2f);
    

//    return Out;
//}

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vHDRColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    vector vBlurColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
    vector vOriginalColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);

    // 블러 효과를 강조하기 위해 블러 텍스처의 기여도를 높입니다.
    float blurWeight = 2.0f; // 블러 기여도 가중치
    vector enhancedBlurColor = vBlurColor * blurWeight;

    float4 vBloom = pow(pow(abs(enhancedBlurColor), 2.2f) + pow(abs(vOriginalColor), 2.2f), 1.f / 2.2f);
  
    float4 OutColor = vHDRColor;
    
    OutColor = pow(abs(OutColor), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
    OutColor += vBloom;
    Out.vColor = pow(abs(OutColor), 1 / 2.2f);
    
    return Out;
}

//PS_OUT PS_Main(PS_In In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    vector vHDRColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vBlurColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vOriginalColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);

//    // 감마 값을 조정합니다.
//    float gamma = 2.2f;
//    float blurGamma = 2.4f; // 블러 효과를 강조하기 위해 블러 텍스처에 다른 감마 값을 사용합니다.

//    float4 vBloom = pow(pow(abs(vBlurColor), blurGamma) + pow(abs(vOriginalColor), gamma), 1.f / gamma);
  
//    float4 OutColor = vHDRColor;
    
//    OutColor = pow(abs(OutColor), gamma);
//    vBloom = pow(abs(vBloom), gamma);
//    OutColor += vBloom;
//    Out.vColor = pow(abs(OutColor), 1 / gamma);
    
//    return Out;
//}

//PS_OUT PS_Main(PS_In In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    vector vHDRColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vBlurColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
//    vector vOriginalColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);

//    // 블러 텍스처와 원본 텍스처의 기여도를 조정하여 블러 효과를 강조합니다.
//    float originalWeight = 0.5f; // 원본 텍스처 가중치
//    float blurWeight = 1.5f; // 블러 텍스처 가중치

//    float4 vBloom = pow(pow(abs(vBlurColor * blurWeight), 2.2f) + pow(abs(vOriginalColor * originalWeight), 2.2f), 1.f / 2.2f);
  
//    float4 OutColor = vHDRColor;
    
//    OutColor = pow(abs(OutColor), 2.2f);
//    vBloom = pow(abs(vBloom), 2.2f);
//    OutColor += vBloom;
//    Out.vColor = pow(abs(OutColor), 1 / 2.2f);
    
//    return Out;
//}

#endif // _BLOOM_HLSL_