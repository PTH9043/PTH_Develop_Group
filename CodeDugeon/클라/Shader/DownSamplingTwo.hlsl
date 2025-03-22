#ifndef _DOWN_SAMPLE_HLSL_
#define _DOWN_SAMPLE_HLSL_

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

const float2 inverseTextureSize = float2(1.0 / 1280.0, 1.0 / 1080.0); // 입력 텍스처의 해상도에 대한 역수

PS_OUT PS_Main(PS_In In)
{
    
    PS_OUT Out = (PS_OUT) 0;
    float2 texelSize = float2(1.0f / 320.f, 1.0f / 270.f);
    float2 baseUV = In.vTexUV;

    float4 colorSum = float4(0, 0, 0, 0);
    int sampleCount = 0;

    if (baseUV.x <= 0.01f || baseUV.x >= 0.99)
    {
        discard;
    }
    
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 offset = texelSize * float2(x, y);
            float2 sampleCoord;
            
            sampleCoord = baseUV + offset;

            
                float4 color = g_Texture0.Sample(g_Sampler_Normal, sampleCoord);
                colorSum += color;
                sampleCount++;
            
        }
    }

 
    if (sampleCount > 0)
    {
        colorSum /= sampleCount;
    }

    Out.vColor = colorSum; // 4x4 블록의 평균값

    return Out;
    
    
    
    
//=============================================================================================================    
  
    //float2 texelSize = float2(1.0f / 320.f, 1.0f / 270.f);
    //float2 baseUV = In.vTexUV;

    //float4 colorSum = float4(0, 0, 0, 0);
    //int sampleCount = 0;

    //for (int x = -1; x < 1; ++x)
    //{
    //    for (int y = -1; y < 1; ++y)
    //    {
    //        float2 offset = texelSize * float2(x, y);
    //        float2 sampleCoord = baseUV + offset;

          
    //        if (sampleCoord.x > 0.0f  && sampleCoord.x < 1.f && sampleCoord.y > 0.0f && sampleCoord.y < 1.f )
    //        {
    //            float4 color = g_Texture0.Sample(g_Sampler_Normal, sampleCoord);
    //            colorSum += color;
    //            sampleCount++;
    //        }
    //    }
    //}


    //if (sampleCount > 0)
    //{
    //    colorSum /= sampleCount;
    //}

    //Out.vColor = colorSum; 

    //return Out;
   
//=============================================================================================================
   
    
    //float2 baseUV = In.vTexUV; // * 16.0f;

    //float4 colorSum = float4(0, 0, 0, 0);
    //for (int y = -2; y < 2; ++y)
    //{
    //    for (int x = -2; x < 2; ++x)
    //    {
    //        float2 offsetUV = baseUV + float2(x, y) / float2(1280.0f / 4.f, 1080.0f / 4.f);
    //        if (offsetUV.x > 0.0f && offsetUV.x < 1.0f && offsetUV.y > 0.0f && offsetUV.y < 1.0f)
    //        {
    //            float4 color = g_Texture0.Sample(g_Sampler_Normal, offsetUV);
    //            colorSum += color;
                
    //        }
           
    //    }
    //}

    //Out.vColor = colorSum / 16.0f; // 6x6 블록의 평균값
    //return Out;
}

#endif // _DOWN_SAMPLE_HLSL_
