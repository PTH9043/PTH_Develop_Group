#ifndef _NORMALOBJECT_HLSL_
#define _NORMALOBJECT_HLSL_

#include "ShaderGrobalFunc.hlsli"

cbuffer RECTCOLOR : register(b4)
{
    float4 g_RectColor;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION0;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN In) //이름 VS_Main PS_Main 유지.
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = Compute_FinalMatrix(In.vPosition);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPosition : SV_Target3;
};

PS_OUT PS_Main(PS_In Input)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_RectColor;
    Out.vNormal = Input.vNormal;
    return Out;
}

#endif // _NORMALOBJECT_HLSL_