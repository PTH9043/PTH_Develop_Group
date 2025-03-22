#ifndef _SHADOW_HLSL_
#define _SHADOW_HLSL_

#include "ShaderGrobalFunc.hlsli"
#include "LightShaderParam.hlsli"


cbuffer HasNormalBuffer : register(b7)
{
    int HasBuffer[4];
};

cbuffer SOCKETMATRIXPARAM : register(b13)
{
    float4x4 g_SocketMatrix;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV0 : TEXCOORD0;
    float2 vTexUV1 : TEXCOORD1;
    float2 vTexUV2 : TEXCOORD2;
    float2 vTexUV3 : TEXCOORD3;
    float2 vTexUV4 : TEXCOORD4;
    float3 vTangent : TANGENT;

};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vclipPos : POSITION;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
  
  //  Out.vPosition = mul(float4(In.vPosition, 1.f), g_PivotMatrix);
    float4 OutPos = float4(In.vPosition, 1.f);

    matrix matWS, matWSV, matWSVP;
    matWS = mul(g_WorldMatrix, g_SocketMatrix);
    matWSV = mul(matWS, g_ViewProjInfoArr[3].mViewMatrix);
    matWSVP = mul(matWSV, g_ViewProjInfoArr[3].mProjMatrix);

    Out.vPosition = mul(OutPos, matWSVP);

    return Out;
}

struct PS_OUT
{
    float vColor : SV_TARGET0;
};


PS_OUT PS_Main(VS_OUT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = (In.vPosition.z);
    return Out;

}



#endif // _SHADOW_HLSL_