#ifndef _2DFOG_
#define _2DFOG_

#include "ShaderGrobalFunc.hlsli"



cbuffer FogBuffer : register(b14)
{
    float fFogStart;
    float fFogEnd;
    float fDensity;
    uint fType; 
    float4 fFogColor;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION0;
    float2 vTexCoord : TEXCOORD0;
    float vFogFactor : FOG;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;
    float4 fCameraPosition;
   
    float4 Position = float4(In.vPosition, 1.f);

    Position= mul(Position, g_WorldMatrix);
    Position= mul(Position, g_ViewProjInfoArr[g_CamID].mViewMatrix);
    Position= mul(Position, g_ViewProjInfoArr[g_CamID].mProjMatrix);
   
    Out.vPosition=Position;
    Out.vTexCoord = In.vTexCoord;
    
    float4 CamPos = float4(In.vPosition, 1.f);
    fCameraPosition = mul(CamPos, g_WorldMatrix);
    fCameraPosition = mul(fCameraPosition, g_ViewProjInfoArr[g_CamID].mViewMatrix);

    if (fType == 0) {
        Out.vFogFactor = saturate((fFogEnd - fCameraPosition.z) / (fFogEnd - fFogStart));
    }
    else if (fType == 1) {
        Out.vFogFactor = 1 / pow(2.781828,fCameraPosition.z * fDensity);
    }
    else if (fType == 2) {
        Out.vFogFactor = 1 / pow(2.781828, (fCameraPosition.z * fDensity)* (fCameraPosition.z * fDensity));
    }
   

    return Out;
}


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_Main(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 TexColor;
    float4 FogColor;
    float4 FinalColor;

    TexColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexCoord);

    FogColor = float4(0.0f, 0.0f, 1.f, 0.0f);

    FinalColor = In.vFogFactor * TexColor + (1.0f - In.vFogFactor) * fFogColor;

    Out.vColor = FinalColor;
    
    return Out;
}

#endif 