#ifndef _VERTICALBLUR_HLSL_
#define _VERTICALBLUR_HLSL_

#include "ShaderGrobalFunc.hlsli"




//struct VS_IN
//{
//    float3 vPosition : POSITION;
//    float2 vTexUV : TEXCOORD0;
//};

//struct VS_OUT
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexUV : TEXCOORD0;
//    float2 texCoord1 : TEXCOORD1;
//    float2 texCoord2 : TEXCOORD2;
//    float2 texCoord3 : TEXCOORD3;
//    float2 texCoord4 : TEXCOORD4;
//    float2 texCoord5 : TEXCOORD5;
//    float2 texCoord6 : TEXCOORD6;
//    float2 texCoord7 : TEXCOORD7;
//    float2 texCoord8 : TEXCOORD8;
//    float2 texCoord9 : TEXCOORD9;
  
//};

//const float TexHeight = 540;

//VS_OUT VS_Main(VS_IN In)
//{
//    VS_OUT Out = (VS_OUT) 0;

//    Out.vPosition = Compute_FinalMatrix(In.vPosition);
//    Out.vTexUV = In.vTexUV;

//    float texelSize = 1.f / TexHeight;
//    Out.texCoord1 = In.vTexUV + float2(0.0f, texelSize * -4.0f);
//    Out.texCoord2 = In.vTexUV + float2(0.0f, texelSize * -3.0f);
//    Out.texCoord3 = In.vTexUV + float2(0.0f, texelSize * -2.0f);
//    Out.texCoord4 = In.vTexUV + float2(0.0f, texelSize * -1.0f);
//    Out.texCoord5 = In.vTexUV + float2(0.0f, texelSize * 0.0f);
//    Out.texCoord6 = In.vTexUV + float2(0.0f, texelSize * 1.0f);
//    Out.texCoord7 = In.vTexUV + float2(0.0f, texelSize * 2.0f);
//    Out.texCoord8 = In.vTexUV + float2(0.0f, texelSize * 3.0f);
//    Out.texCoord9 = In.vTexUV + float2(0.0f, texelSize * 4.0f);
    
    
    
//    return Out;
//}

//struct PS_In
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexUV : TEXCOORD0;
//    float2 texCoord1 : TEXCOORD1;
//    float2 texCoord2 : TEXCOORD2;
//    float2 texCoord3 : TEXCOORD3;
//    float2 texCoord4 : TEXCOORD4;
//    float2 texCoord5 : TEXCOORD5;
//    float2 texCoord6 : TEXCOORD6;
//    float2 texCoord7 : TEXCOORD7;
//    float2 texCoord8 : TEXCOORD8;
//    float2 texCoord9 : TEXCOORD9;
//};

//struct PS_OUT
//{
//    float4 vColor : SV_Target0;
//};

//PS_OUT PS_Main(PS_In In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    float weight0, weight1, weight2, weight3, weight4;
//    float normalization;
   
    
//    weight0 = 1.0f;
//    weight1 = 0.9f;
//    weight2 = 0.55f;
//    weight3 = 0.18f;
//    weight4 = 0.1f;

    
//    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

    
//    weight0 = weight0 / normalization;
//    weight1 = weight1 / normalization;
//    weight2 = weight2 / normalization;
//    weight3 = weight3 / normalization;
//    weight4 = weight4 / normalization;
   
    
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord1) * weight4;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord2) * weight3;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord3) * weight2;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord4) * weight1;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord5) * weight0;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord6) * weight1;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord7) * weight2;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord8) * weight3;
//    Out.vColor += g_Texture0.Sample(g_Sampler_Normal, In.texCoord9) * weight4;
   
//    Out.vColor.a = 1;
//    return Out;
//}


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float2 texCoord4 : TEXCOORD4;
    float2 texCoord5 : TEXCOORD5;
    float2 texCoord6 : TEXCOORD6;
    float2 texCoord7 : TEXCOORD7;
    float2 texCoord8 : TEXCOORD8;
    float2 texCoord9 : TEXCOORD9;
    float2 texCoord10 : TEXCOORD10;
    float2 texCoord11 : TEXCOORD11;
};

const float TexHeight = 540;


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

   // Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vPosition = float4(In.vPosition*2, 1.f);
  
    Out.vTexUV = In.vTexUV;

    float texelSize = 1.f / TexHeight;

    // 블러 좌표의 범위가 텍스처 경계를 벗어나지 않도록 클램핑
    Out.texCoord1 = In.vTexUV + float2(0.f, texelSize * -5.0f);
    Out.texCoord2 = In.vTexUV + float2(0.f, texelSize * -4.0f);
    Out.texCoord3 = In.vTexUV + float2(0.f, texelSize * -3.0f);
    Out.texCoord4 = In.vTexUV + float2(0.f, texelSize * -2.0f);
    Out.texCoord5 = In.vTexUV + float2(0.f, texelSize * -1.0f);
    Out.texCoord6 = In.vTexUV + float2(0.f, texelSize * 0.0f);
    Out.texCoord7 = In.vTexUV + float2(0.f, texelSize * 1.0f);
    Out.texCoord8 = In.vTexUV + float2(0.f, texelSize * 2.0f);
    Out.texCoord9 = In.vTexUV + float2(0.f, texelSize * 3.0f);
    Out.texCoord10 = In.vTexUV + float2(0.f, texelSize * 4.0f);
    Out.texCoord11 = In.vTexUV + float2(0.f, texelSize * 5.0f);
    
    
    //Out.texCoord1 = In.vTexUV + float2(0.f, texelSize * -0.5f);
    //Out.texCoord2 = In.vTexUV + float2(0.f, texelSize * -0.4f);
    //Out.texCoord3 = In.vTexUV + float2(0.f, texelSize * -0.3f);
    //Out.texCoord4 = In.vTexUV + float2(0.f, texelSize * -0.2f);
    //Out.texCoord5 = In.vTexUV + float2(0.f, texelSize * -0.1f);
    //Out.texCoord6 = In.vTexUV + float2(0.f, texelSize * 0.0f);
    //Out.texCoord7 = In.vTexUV + float2(0.f, texelSize * 0.1f);
    //Out.texCoord8 = In.vTexUV + float2(0.f, texelSize * 0.2f);
    //Out.texCoord9 = In.vTexUV + float2(0.f, texelSize * 0.3f);
    //Out.texCoord10 = In.vTexUV + float2(0.f, texelSize * 0.4f);
    //Out.texCoord11 = In.vTexUV + float2(0.f, texelSize * 0.5f);

    return Out;
}




struct PS_In
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float2 texCoord4 : TEXCOORD4;
    float2 texCoord5 : TEXCOORD5;
    float2 texCoord6 : TEXCOORD6;
    float2 texCoord7 : TEXCOORD7;
    float2 texCoord8 : TEXCOORD8;
    float2 texCoord9 : TEXCOORD9;
    float2 texCoord10 : TEXCOORD10;
    float2 texCoord11 : TEXCOORD11;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    float weight0, weight1, weight2, weight3, weight4, weight5;
    float normalization;
    
    weight0 = 1.0f;
    weight1 = 0.9f;
    weight2 = 0.55f;
    weight3 = 0.18f;
    weight4 = 0.1f;
    weight5 = 0.05f;
  
    

    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4 + weight5));

    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;
    weight5 = weight5 / normalization;
    
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord1) * weight5;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord2) * weight4;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord3) * weight3;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord4) * weight2;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord5) * weight1;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord6) * weight0;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord7) * weight1;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord8) * weight2;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord9) * weight3;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord10) * weight4;
    Out.vColor += g_Texture0.Sample(g_Sampler_Clamp, In.texCoord11) * weight5;
   
    Out.vColor.a = 1;
    return Out;
}


#endif // _VERTICALBLUR_HLSL_