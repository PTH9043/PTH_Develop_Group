#ifndef _LIGHTDIRECTION_HLSL_
#define _LIGHTDIRECTION_HLSL_

#include "LightShaderFunc.hlsli"

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

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_tLightParam.mScreenWorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_tLightParam.mScreenViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_tLightParam.mScreenProjMatrix);
    //Out.vPosition = float4(In.vPosition * 2.f, 1.f);
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
    float4 vAmbient : SV_TARGET0;
    float4 vShade : SV_TARGET1;
    float4 vSpecular : SV_TARGET2;
};

PS_OUT PS_Main(PS_In Input)
{
    PS_OUT Out = (PS_OUT) 0;

    // 0, 1, 3
    float4 vNormal = g_Texture0.Sample(g_Sampler_Normal, Input.vTexUV);
    float4 vWorldNormal = normalize(vector(vNormal.xyz * 2.f - 1.f, 0.f));
    float4 vDepth = g_Texture1.Sample(g_Sampler_Normal, Input.vTexUV);
    float4 vWorldPosition = g_Texture2.Sample(g_Sampler_Normal, Input.vTexUV);
    
    float4 vViewPosition = mul(vWorldPosition, g_ViewProjInfoArr[g_CamID].mViewMatrix);
    float4 vViewNormal = mul(vWorldNormal, g_ViewProjInfoArr[g_CamID].mViewMatrix);
    
    //if (vViewPosition.z <= 0.f)
    //    clip(-1);
    LIGHTCOLOR tLightColor = (LIGHTCOLOR) 0.f;

 
   
    tLightColor = LightingInWorld(vWorldPosition.xyz, vWorldNormal.xyz);
    
    
    //if (length(tLightColor.vDiffuse) != 0)
    //{
       

       
    //    float4 shadowViewPos = mul(vWorldPosition, g_ViewProjInfoArr[3].mViewMatrix);
    //    float4 shadowClipPos = mul(shadowViewPos, g_ViewProjInfoArr[3].mProjMatrix);
    //    float depth = shadowClipPos.z / shadowClipPos.w;

    //    // x [-1 ~ 1] -> u [0 ~ 1]
    //    // y [1 ~ -1] -> v [0 ~ 1]
    //    float2 uv = shadowClipPos.xy / shadowClipPos.w;
    //    uv.y = -uv.y;
    //    uv = uv * 0.5 + 0.5;

    //    if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
    //    {
    //        float shadowDepth = g_Texture3.Sample(g_Sampler_Normal, uv).x;
    //        if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
    //        {
    //            tLightColor.vDiffuse *= 0.5f;
    //            tLightColor.vSpecular = (float4) 0.f;
    //            tLightColor.vAmbient = (float4) 0.f;
    //        }
    //    }
    //}
    
   // if (length(tLightColor.vDiffuse) != 0)
   // {
       
   //     float width, height, numMips;
   //     g_Texture3.GetDimensions(0, width, height, numMips);
       
   //     float4 shadowViewPos = mul(vWorldPosition, g_ViewProjInfoArr[3].mViewMatrix);
   //     float4 shadowClipPos = mul(shadowViewPos, g_ViewProjInfoArr[3].mProjMatrix);
   //     float depth = shadowClipPos.z / shadowClipPos.w - 0.0001f;
        
   //     if (depth < 0.0f || depth > 1.0f)
   //     {
   //         continue;
   //     }
   //     // x [-1 ~ 1] -> u [0 ~ 1]
   //     // y [1 ~ -1] -> v [0 ~ 1]
   //     float2 uv = shadowClipPos.xy / shadowClipPos.w;
   //     uv.y = -uv.y;
   //     uv = uv * 0.5 + 0.5;
        
   //     if (uv.x < 0.0f || uv.x > 1.0f ||
			//uv.y < 0.0f || uv.y > 1.0f)
   //     {
   //         continue;
   //     }
        
   //     float dx = 1.0f / width;
   //     float percentLit = 0.0f;
   //     const float2 offsets[9] =
   //     {
   //         float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
			//float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
			//float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
   //     };
   //     [unroll]
   //     for (int j = 0; j < 9; ++j)
   //     {
   //         percentLit += g_Texture3.SampleCmpLevelZero(g_Sampler_Border, uv.xy + offsets[j], depth).r;
   //     }
   //     percentLit = percentLit / 9.0f;
        
   //     //if (percentLit < 0.33f)
   //     //    percentLit = 0.33f;
   //     //else if (percentLit < 0.66f)
   //     //    percentLit = 0.66f;
   //     //else
   //     //    percentLit = 0.99f;
        
     
   //     if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
   //     {
   //         tLightColor.vDiffuse *= percentLit;
   //         tLightColor.vSpecular = (float4) 0.f;
   //         tLightColor.vAmbient = (float4) 0.f;
   //     }
   // }
    
    if (length(vWorldPosition) == 0)
    {
        Out.vAmbient = float4(1.0, 1.0, 1.0, 1);
        Out.vShade = float4(1.0, 1.0, 1.0, 1);
        Out.vSpecular = float4(1.0, 1.0, 1.0, 1); //BlendDeffered에서 이 세가지 출력을 더하여 각 요소 1보다 크면 1로 고정하여 DiffuseDesc원본색 하도록
    }
    else
    {
        Out.vAmbient = tLightColor.vAmbient;
        Out.vShade = tLightColor.vDiffuse;
        Out.vSpecular = tLightColor.vSpecular;
    }
   
    
    return Out;
}

#endif // _LIGHTDIRECTION_HLSL_