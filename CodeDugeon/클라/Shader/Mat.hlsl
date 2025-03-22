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
 

	return Out;
}

struct PS_In
{
    float4 vPosition : SV_POSITION0;
    float2 vTexUV : TEXCOORD0;
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
    if (Out.vColor.a < 0.7)
        discard;
   // Out.vColor.a *= fTransparency;
    Out.vSpecular = float4(Out.vColor.rgb, 0.5f);
    Out.vGlow = float4(Out.vColor.rgb, 0.5f);
    
    return Out;
}

#endif // _TRAIL_HLSL_