#ifndef _3DNORMALCUBE_HLSL_
#define _3DNORMALCUBE_HLSL_

#include "ShaderGrobalFunc.hlsli"

cbuffer NorCubeParam : register(b13)
{
    float4 g_vNormalCubeColor;
    int g_iNormalCubeTextureBind;
    int3 g_iNormalCubePadding;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vNormal = mul(float4(In.vNormal, 1.f), g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;
    return Out;
}


struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float4 vNormal : NORMAL;
    float3 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1; 
    float4 vDepth : SV_TARGET2; 
    float4 vPosition : SV_Target3; 
};

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_iNormalCubeTextureBind == 0)
    {
        Out.vDiffuse = g_vNormalCubeColor;
    }
    else
    {
        Out.vDiffuse = g_Texture8.Sample(g_Sampler_Clamp, In.vTexUV);
    }
   
    VIEWPROJINFO tMainViewProj = GetViewProjInfo();
    
    Out.vNormal = normalize(In.vNormal);
    Out.vDepth = float4(In.vProjPos.w / tMainViewProj.fCamFar, In.vProjPos.z / In.vProjPos.w, 1.f, In.vPosition.w);
    Out.vPosition = In.vWorldPos;

    
    return Out;
}

#endif // _3DNORMALCUBE_HLSL_