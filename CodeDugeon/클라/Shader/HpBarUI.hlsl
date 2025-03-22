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
    float2 vTexUV1 : TEXCOORD1;
    float2 vTexUV2 : TEXCOORD2;
    float2 vTexUV3 : TEXCOORD3;
};

cbuffer HPBuffer : register(b13)
{
    float _MaxHp;
    float _CurHp;
    float _durationTime;
    float _padding;
};

static const float2 fDistortion1 =float2(0.1f, 0.2f);
static const float2 fDistortion2 =float2(0.1f, 0.3f);
static const float2 fDistortion3 = float2(0.1f, 0.1f);

static const float fDistortionScale = 0.8;
static const float fDistortionBias = 0.5;
static const float3 fScrollSpeeds = float3(1.3f, 2.1f, 2.3f);
static const float3 fScales = float3(1.0f, 5.0f, 1.0f);

VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vTexUV = In.vTexUV;
   
    

    Out.vTexUV1 = (In.vTexUV * fScales.x);
    Out.vTexUV1.x= Out.vTexUV1.x + (fGrobalDeltaTime*0.05 * fScrollSpeeds.x);

    Out.vTexUV2 = (In.vTexUV * fScales.y);
    Out.vTexUV2.x = Out.vTexUV2.x + (fGrobalDeltaTime * 0.05 * fScrollSpeeds.y);

   Out.vTexUV3 = (In.vTexUV * fScales.z);
    Out.vTexUV3.x = Out.vTexUV3.x + (fGrobalDeltaTime * 0.05 * fScrollSpeeds.z);

    return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float2 vTexUV : TEXCOORD0;
    float2 vTexUV1 : TEXCOORD1;
    float2 vTexUV2 : TEXCOORD2;
    float2 vTexUV3 : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    //float2 movedUV = In.vTexUV + float2(fGrobalDeltaTime * 0.15, fGrobalDeltaTime * 0.05);

    //float4 HpColor = g_Texture0.Sample(g_Sampler_Normal, movedUV);
    //float4 Noise = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);

    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;
    
    noise1 = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV1);
    noise2 = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV2);
    noise3 = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV3);
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;
    
    noise1.xy = noise1.xy * fDistortion1.xy;
    noise2.xy = noise2.xy * fDistortion2.xy;
    noise3.xy = noise3.xy * fDistortion3.xy;
    finalNoise = noise1 + noise2 + noise3;
    
    perturb = ((1.0f - In.vTexUV.y) * fDistortionScale) + fDistortionBias;
    
    noiseCoords.xy = (finalNoise.xy * perturb) + In.vTexUV.xy;


   
  
    float4 HpColor = g_Texture0.Sample(g_Sampler_Normal, noiseCoords.xy);

 
    float HpRatio = _CurHp / _MaxHp;
    
    float2 center = float2(0.5, 0.5);
    float distanceFromCenter = distance(In.vTexUV, center);

    float fadeStart = 0.48; // Start of fade range
    float fadeEnd = 0.7; // End of fade range
    float alpha = 1.0;

    if (distanceFromCenter > fadeStart)
    {
        // Compute alpha based on distance between fadeStart and fadeEnd
        alpha = saturate((fadeEnd - distanceFromCenter) / (fadeEnd - fadeStart));
    }

    float4 OrangeColor = float4(1.0, 0.5, 0.0, 1.0);
    if (In.vTexUV.x <= HpRatio)
    {
        Out.vColor = HpColor;

    }
    else
    {
        Out.vColor = float4(0.15, 0.15, 0.15, HpColor.a);
    }
    Out.vColor.a *= alpha;
    return Out;
}

#endif // _DEFAULTUI_HLSL_
