#ifndef _EQUIPMENT_HLSL_
#define _EQUIPMENT_HLSL_

#include "ShaderGrobalFunc.hlsli"


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
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexUV0 : TEXCOORD0;
    float2 vTexUV1 : TEXCOORD1;
    float2 vTexUV2 : TEXCOORD2;
    float2 vTexUV3 : TEXCOORD3;
    float2 vTexUV4 : TEXCOORD4;
    float4 vWorldPos : TEXCOORD5;
    float4 vProjPos : TEXCOORD6;
    float3 vViewDir : TEXCOORD7;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    float4x4 matWS = mul(g_WorldMatrix, g_SocketMatrix);
  //  Out.vPosition = mul(float4(In.vPosition, 1.f), g_PivotMatrix);
    Out.vPosition = Compute_FinalMatrix_Socket(In.vPosition, g_SocketMatrix);

    vector vNormal = mul(float4(In.vNormal, 0.f), matWS);
    vector vTangent = mul(float4(In.vTangent, 0.f), matWS);

    Out.vNormal = normalize(vNormal);
    Out.vTangent = normalize(vTangent);
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
    Out.vTexUV0 = In.vTexUV0;
    Out.vTexUV1 = In.vTexUV1;
    Out.vTexUV2 = In.vTexUV2;
    Out.vTexUV3 = In.vTexUV3;
    Out.vTexUV4 = In.vTexUV4;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWS);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexUV0 : TEXCOORD0;
    float2 vTexUV1 : TEXCOORD1;
    float2 vTexUV2 : TEXCOORD2;
    float2 vTexUV3 : TEXCOORD3;
    float2 vTexUV4 : TEXCOORD4;
    float4 vWorldPos : TEXCOORD5;
    float4 vProjPos : TEXCOORD6;
    float3 vViewDir : TEXCOORD7;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
    float4 vNormal : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vPosition : SV_Target4;
};

PS_OUT PS_Main(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    VIEWPROJINFO tMainViewProj = GetViewProjInfo();
    // Out Color    

    if (HasBuffer[0])
        Out.vDiffuse = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV0);
    //if (HasBuffer[1])
    //    Out.vSpecular = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV0);


    if (Out.vDiffuse.a <= 0.05)
        discard;

    if (HasBuffer[2])
    {
        vector vNormalDesc = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV0);
        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;


        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vNormal = mul(vNormal, WorldMatrix);

        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);

        Out.vNormal = normalize(float4(vNormal, 0.f));
    }
    else
    {
        Out.vNormal = In.vNormal;
    }
    //float rimFactor = 1.0 - saturate(dot(normalize(In.vViewDir), normalize(In.vNormal.xyz)));
    //rimFactor = pow(rimFactor, 32) * 3;
    //float4 rimLight = rimFactor * float4(0, 0, 1, 1);

    //// Combine rim lighting with the final color
    //Out.vDiffuse.rgb += 0.05*rimLight.rgb;

    Out.vDepth = float4(In.vProjPos.w / tMainViewProj.fCamFar, In.vProjPos.z / In.vProjPos.w, 1.f, In.vPosition.w);
    Out.vPosition = In.vWorldPos;

    return Out;
}

#endif // _EQUIPMENT_HLSL_