#ifndef _TRAIL_HLSL_
#define _TRAIL_HLSL_

#include "ShaderGrobalFunc.hlsli"


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION0;
	float2 vTexUV : TEXCOORD0;
    float2 vScreenTex : TEXCOORD1;
};


cbuffer BloodTimerBuffer : register(b14)
{
    float gfFullTime;
    float gfLeftTime;
};

VS_OUT VS_Main(VS_IN In) //이름 VS_Main PS_Main 유지.
{
	VS_OUT Out = (VS_OUT)0;

    //float bias = 15.0;
    //float scaleFactor = (gfFullTime - gfLeftTime) / gfFullTime;
    //scaleFactor = 1 + scaleFactor * bias;
    //float3 scaledPosition = float3(In.vPosition.x * scaleFactor, In.vPosition.y * scaleFactor, In.vPosition.z);
    //Out.vPosition = Compute_FinalMatrix(scaledPosition);
   
   
    Out.vPosition = Compute_FinalMatrix(In.vPosition);
	Out.vTexUV = In.vTexUV;
    Out.vScreenTex = Out.vPosition.xy / Out.vPosition.w;

	return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float2 vTexUV : TEXCOORD0;
    float2 vScreenTex : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
    float4 vNormal : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vPosition : SV_Target4;
    float4 vGlow : SV_Target5;
};

PS_OUT PS_Main(PS_In In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fTransparency = gfLeftTime / gfFullTime;
    
    Out.vColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    // g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
   
    //
    
    float2 ScreenTex;
    ScreenTex.x = (In.vScreenTex.x) * 0.5 + 0.5f;
    ScreenTex.y = (In.vScreenTex.y) * -0.5 + 0.5f;
    
    vector vDepthDesc = g_Texture1.Sample(g_Sampler_Normal, ScreenTex);

    float fViewZ = vDepthDesc.x * GetViewProjInfo().fCamFar;


    float depthDifference = abs(fViewZ - In.vPosition.w);


    float fDistance = smoothstep(0.0, 1.0, depthDifference * 0.05); 
    Out.vColor.rgb *= float3(0.4, 0.4, 0.4);
    Out.vColor.a *= fDistance ;
    Out.vColor.a *= fTransparency;
    Out.vSpecular = float4(Out.vColor.rgb, 0.5f);
    
    return Out;
}

#endif // _TRAIL_HLSL_