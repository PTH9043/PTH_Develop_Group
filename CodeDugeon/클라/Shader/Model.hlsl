#ifndef _MODEL_HLSL_
#define _MODEL_HLSL_

#include "ShaderGrobalFunc.hlsli"
cbuffer CheckPointBuffer : register(b6)
{
    bool isCheckPoint;
};

cbuffer HasNormalBuffer : register(b7)
{
    int HasBuffer[4];
};

//cbuffer MODELDATAPARAM : register(b10)
//{
//    int g_iMaterialIndex;
//    float3 PaddingValue;
//};

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
    float3 vViewDir : TEXCOORD8;
    float4 vVelocity : TEXCOORD9;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    Out.vPosition = Compute_FinalMatrix(In.vPosition);

    vector vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), g_WorldMatrix);

    Out.vNormal = normalize(vNormal);
    Out.vTangent = normalize(vTangent);
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
    Out.vTexUV0 = In.vTexUV0;
    Out.vTexUV1 = In.vTexUV1;
    Out.vTexUV2 = In.vTexUV2;
    Out.vTexUV3 = In.vTexUV3;
    Out.vTexUV4 = In.vTexUV4;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vViewDir = normalize(g_ViewProjInfoArr[g_CamID].vCamPosition - Out.vWorldPos.xyz); // Compute the view direction

   // if (true == g_isObjectMotionBlur)
   // {
   //     vector vPrevPosition = float4(In.vPosition, 1.f);
        
   //     VIEWPROJINFO tMainViewProj = GetViewProjInfo();
        
   //     vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
   //     vPrevPosition = mul(vPrevPosition, tMainViewProj.mPrevViewMatirx);
   //     vPrevPosition = mul(vPrevPosition, tMainViewProj.mProjMatrix);
        
   //     float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
   //     float4 NORMAL = mul(Out.vNormal, tMainViewProj.mPrevViewMatirx);
   //     float a = dot(normalize(dir), normalize(NORMAL.xyz));
        
   //     if (a < 0.f)
   //         Out.vPosition = vPrevPosition;
        
   //     float2 velocity = ((Out.vPosition.xy / Out.vPosition.w -
			//(vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
   //     velocity.y *= -1.f;
   //     Out.vVelocity.xy = velocity;
   //     Out.vVelocity.zw = Out.vPosition.zw;
   // }

    
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
    float3 vViewDir : TEXCOORD8;
    float4 vVelocity : TEXCOORD9;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
    float4 vNormal : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vPosition : SV_Target4;
    float4 vGlow : SV_Target5;
    float4 vVelocity : SV_Target6;
};

PS_OUT PS_Main(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    VIEWPROJINFO tMainViewProj = GetViewProjInfo();
    // Out Color    

    if (HasBuffer[0])
        Out.vDiffuse = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV0);
    //if (HasBuffer[1])
    //    Out.vSpecular = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV0);

    
    

    if (HasBuffer[2])
    {
        vector vNormalDesc = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV0);
        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // [0,1] ->[-1,1]


        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vNormal = mul(vNormal, WorldMatrix);

        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f); //[-1,1]->[0,1]

        Out.vNormal = normalize(float4(vNormal, 0.f));
    }
    else
    {
        Out.vNormal = In.vNormal;
    }
    
    
    float g_RimLightIntensity = 0.8; // Example intensity value
    float g_RimLightPower = 6.0; // Example power value
    float4 g_RimLightColor = float4(0.6, 0.25, 0, 1); // Example color (white)
    float4 g_RimLightColor2 = float4(1, 0, 0, 1); // Example color (white)
    float rimFactor = 1.0 - saturate(dot(normalize(In.vViewDir), normalize(In.vNormal.xyz)));
    rimFactor = pow(rimFactor, g_RimLightPower) * g_RimLightIntensity;
    float4 rimLight = rimFactor * g_RimLightColor;
    float4 rimLight2 = rimFactor * g_RimLightColor2;

    // Combine rim lighting with the final color
    Out.vDiffuse.rgb += 1.25*rimLight.rgb;
    Out.vDepth = float4(In.vProjPos.w / tMainViewProj.fCamFar, In.vProjPos.z / In.vProjPos.w, 1.f, In.vPosition.w);
    Out.vPosition = In.vWorldPos;

    if (isCheckPoint)
        Out.vDiffuse.rgb += 10 * rimLight2.rgb;
    
    //if (true == g_isObjectMotionBlur)
    //{
    //    Out.vVelocity = In.vVelocity;
    //    Out.vVelocity.z = 1.f;
    //    Out.vVelocity.w = In.vVelocity.z / In.vVelocity.w;
    //}
    
    return Out;
}

#endif // _MODEL_HLSL_