#ifndef _SCREENRENDEROBJ_HLSL_
#define _SCREENRENDEROBJ_HLSL_

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

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseDesc = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    vector vShadeDesc = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
    vector vSpecularDesc = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);
    vector vAmbientDesc = g_Texture3.Sample(g_Sampler_Normal, In.vTexUV);

    vector vColor = vDiffuseDesc * (vShadeDesc + vAmbientDesc + vSpecularDesc);
    //(vShadeDesc + vAmbientDesc + vSpecularDesc)
    Out.vColor = vColor;
    Out.vColor.a = vDiffuseDesc.a;
    if (Out.vColor.a == 0)
        discard;
    
    Out.vColor += g_Texture4.Sample(g_Sampler_Normal, In.vTexUV);
    return Out;
}

#endif // _SCREENRENDEROBJ_HLSL_