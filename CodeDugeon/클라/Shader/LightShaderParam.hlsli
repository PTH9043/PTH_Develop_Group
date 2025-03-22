#ifndef _LIGHTSHADERPARAM_HLSLI_
#define _LIGHTSHADERPARAM_HLSLI_

#include "ShaderParams.hlsli"

#define MAX_LIGHTS 100
struct LIGHTINFO
{
    float4      vDiffuse;
    float4      vAmbient;
    float4      vSpecular;
    float4      vDirection;
    float4      vPosition;
    
    int            eLightType;
    int     eLightActive;
    float         fRange;
    float         fAngle;
    
    float         fLightPower;  
    float         fSpecularPowValue;
    float fFallOff;
    float fPhi;
    
    float fTheta;
    float3 vAttenuation;
};

struct LIGHTCOLOR
{
    float4 vDiffuse;
    float4 vAmbient;
    float4 vSpecular;
};



struct LIGHTPARAM
{
    float3       vLightCamPos;
    float          fCamFar;
    matrix       mScreenWorldMatrix;
    matrix       mScreenViewMatrix;
    matrix       mScreenProjMatrix;
    matrix       mViewMatrix;
    matrix       mProjMatrix;
    matrix       mViewMatrixInv;
    matrix       mProjMatrixInv;
};

cbuffer LIGHTBUFFER : register(b2)
{
    LIGHTPARAM g_tLightParam;
};

cbuffer LIGHTCONTROLPARAM : register(b4)
{
    LIGHTINFO                       g_tLightInfo[MAX_LIGHTS];
    int nLights;
};



float4 g_vMaterialSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMaterialAmbient = float4(1.f, 1.f, 1.f, 1.f);
float g_vGlobalAmbient = float4(0.2f, 0.2f, 0.2f, 1.0f);


#endif // _LIGHTSHADERPARAM_HLSLI_
