#ifndef _GUARD_HLSL_
#define _GUARD_HLSL_

#include "ShaderGrobalFunc.hlsli"


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION0;
    float2 vTexCoord : TEXCOORD0;
    float2 vTexCoords1 : TEXCOORD1;
    float2 vTexCoords2 : TEXCOORD2;
    float2 vTexCoords3 : TEXCOORD3;
};


static const float2 fDistortion1 = float2(1, 0.1);
static const float2 fDistortion2 = float2(5, 0.3);
static const float2 fDistortion3 = float2(5, 5);
static const float fDistortionScale = 0.8;
static const float fDistortionBias = 0.5;
static const float3 fScrollSpeeds = float3(0.15, 0.15, 0.15);
static const float3 fScales = float3(1, 1, 3);

//static const float2 fDistortion1 = float2(1, 1);
//static const float2 fDistortion2 = float2(1, 0.5);
//static const float2 fDistortion3 = float2(0.6,0.1);
//static const float fDistortionScale=0.8;
//static const float fDistortionBias=0.5;
//static const float3 fScrollSpeeds = float3(0.15, 0.15, 0.15);
//static const float3 fScales = float3(1, 2, 3);


VS_OUT VS_Main(VS_IN In) //이름 VS_Main PS_Main 유지.
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    
    Out.vTexCoord = In.vTexCoord;
    

    // 첫 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 첫 번째 노이즈 텍스처에 대한 텍스처 좌표를 계산합니다.
    Out.vTexCoords1 = (In.vTexCoord * fScales.x);
    Out.vTexCoords1.y = Out.vTexCoords1.y + (fGrobalDeltaTime * fScrollSpeeds.x);

    // 두 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 두 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
    Out.vTexCoords2 = (In.vTexCoord * fScales.y);
    Out.vTexCoords2.y = Out.vTexCoords2.y + (fGrobalDeltaTime * fScrollSpeeds.y);

    // 세 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 세 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
    Out.vTexCoords3 = (In.vTexCoord * fScales.z);
    Out.vTexCoords3.y = Out.vTexCoords3.y + (fGrobalDeltaTime * fScrollSpeeds.z);


    return Out;
}



struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
    float4 vNormal : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vPosition : SV_Target4;
    float4 vGlow : SV_Target5;
};

float Rand(float2 co)//0.5~1.0
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

PS_OUT PS_Main(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;

    // 3 개의 서로 다른 텍스처 좌표를 사용하여 동일한 노이즈 텍스처를 샘플링하여 3 개의 다른 노이즈 스케일을 얻습니다.
    noise1 = g_Texture1.Sample(g_Sampler_Normal, In.vTexCoords1);
    noise2 = g_Texture1.Sample(g_Sampler_Normal, In.vTexCoords2);
    noise3 = g_Texture1.Sample(g_Sampler_Normal, In.vTexCoords3);

    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;

   
    noise1.xy = noise1.xy * fDistortion1.xy;
    noise2.xy = noise2.xy * fDistortion2.xy;
    noise3.xy = noise3.xy * fDistortion3.xy;

    finalNoise = noise1 + noise2 + noise3;

   
    perturb = ((1.0f - In.vTexCoord.y) * fDistortionScale) + fDistortionBias;

  
    noiseCoords.xy = (finalNoise.xy * perturb) + In.vTexCoord.xy;


    fireColor = g_Texture0.Sample(g_Sampler_Clamp, noiseCoords.xy);

  
    alphaColor = g_Texture2.Sample(g_Sampler_Clamp, noiseCoords.xy);

    fireColor.a = alphaColor;
       
    float3 normalizedColor = fireColor.rgb / max(max(fireColor.r, fireColor.g), fireColor.b);
    normalizedColor *= 3.0f;
    normalizedColor = saturate(normalizedColor);
    float4 AmpColor = float4(normalizedColor * max(max(fireColor.r, fireColor.g), fireColor.b), fireColor.a);

   
    float4 HighColor = float4(fireColor.r * Rand(noise1.xy), fireColor.g * Rand(noise2.xy), fireColor.b * Rand(noise3.xy), fireColor.a);
    float brightness = dot(HighColor.rgb, float3(0.2126, 0.7152, 0.0722));
    
      //  if (fireColor.a < 0.15)
      //  fireColor = float4(0, 0.5, 0.5, 0.35);
   
    
    Out.vGlow = float4(fireColor.xyz, 0.5f);
    Out.vColor = fireColor;
   

  
 
 
    
    return Out;
   
}

#endif // _GUARD_HLSL_