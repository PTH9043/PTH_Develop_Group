#ifndef _SHADOW_HLSL_
#define _SHADOW_HLSL_

#include "ShaderGrobalFunc.hlsli"
#include "LightShaderParam.hlsli"

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
    float4 vNormal : NORMAL;
    float4 vclipPos : POSITION;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;

  
    float4 Position = float4(In.vPosition, 1.f);

    Position = mul(Position, g_WorldMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(vNormal);
    
    
    Position = mul(Position, g_ViewProjInfoArr[3].mViewMatrix);
    Position = mul(Position, g_ViewProjInfoArr[3].mProjMatrix);
    //Position = mul(Position, g_tLightParam.mViewMatrix);
    //Position = mul(Position, g_tLightParam.mProjMatrix);
   
    Out.vPosition = Position;
  
   

    return Out;
}

struct PS_OUT
{
    float vColor : SV_TARGET0;
};


PS_OUT PS_Main(VS_OUT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    //float3 vToCam = g_ViewProjInfoArr[3].vCamPosition.xyz - In.vPosition.xyz;
    //float fDiffuseFactor = dot(vToCam, In.vNormal.xyz);
    //if (fDiffuseFactor > 0.0f)
    //{
    //    Out.vColor = (In.vPosition.z);
    //}
    Out.vColor = (In.vPosition.z);
    return Out;

}



#endif // _SHADOW_HLSL_