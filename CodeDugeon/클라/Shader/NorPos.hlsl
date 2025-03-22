#ifndef _NORPOS_HLSL_
#define _NORPOS_HLSL_

#include "ShaderGrobalFunc.hlsli"
#include "LightShaderParam.hlsli"

cbuffer USESCALEPARAM : register(b14)
{
    bool ifUseScale;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;
    if (ifUseScale)
    {
        float gScale = 1.01; // 예를 들어 1.1로 설정하여 10% 확대
        matrix scaleMatrix =
        {
            gScale, 0.0f, 0.0f, 0.0f,
                           0.0f, gScale, 0.0f, 0.0f,
                           0.0f, 0.0f, gScale, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f
        };
        vector vPosition = mul(float4(In.vPosition, 1.f), scaleMatrix);
        Out.vPosition = Compute_FinalMatrix(vPosition);
    }
    else
    {
        float gSize = 1.25;
        In.vPosition.xyz += In.vNormal * gSize;
        Out.vPosition = Compute_FinalMatrix(In.vPosition);
    }
   
    
   
  

    return Out;
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_Main(VS_OUT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor.xyz = float3(204, 255, 0);
    Out.vColor.w = In.vPosition.z / In.vPosition.w;
    return Out;

}



#endif // _NORPOS_HLSL_