#ifndef _SHADOW_HLSL_
#define _SHADOW_HLSL_

#include "ShaderGrobalFunc.hlsli"
#include "LightShaderParam.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION;
   
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
  
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;

  
    Out.vPosition = Compute_FinalMatrix(In.vPosition);
  

    return Out;
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_Main(VS_OUT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = float4(In.vPosition.w / GetViewProjInfo().fCamFar, In.vPosition.z / In.vPosition.w, 1.f, In.vPosition.w);
    //Out.vColor = float4(0,0,0,0);
    return Out;

}



#endif // _SHADOW_HLSL_