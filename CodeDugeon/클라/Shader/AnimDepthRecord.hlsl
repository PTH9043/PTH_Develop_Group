#ifndef _SHADOW_HLSL_
#define _SHADOW_HLSL_

#include "ShaderGrobalFunc.hlsli"
#include "LightShaderParam.hlsli"

struct BONEMATRIX
{
    float4x4 BoneMatrix[512];
};

//cbuffer MODELDATAPARAM : register(b10)
//{
//    int g_iMaterialIndex;
//    float3 PaddingValue;
//};

cbuffer ANIMATIONPARAM : register(b11)
{
    bool g_isOutLineExist = false;
    bool g_isNomralExist = true;
    bool g_isAnimPadding;
    bool g_isAnimPadding2;
};

cbuffer BONEMATRIXPARAM : register(b12)
{
    BONEMATRIX g_BoneMatrix;
};

cbuffer PREVBONEMATRIXPARAM : register(b13)
{
    BONEMATRIX g_PrevBoneMatrix;
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
    uint4 vBlendIndex : BLENDINDEX; /* 현재 정점에 영향ㅇ르 주는 뼈의 인덱스(메시커넽이너에 영햐을 주는 뼈들 중), 최대 4개. */
    float4 vBlendWeight : BLENDWEIGHT; /* 위에서 사용되고 있는 인덱스번째 뼈의 영향비율(최대 1) . */

};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
  
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = (g_BoneMatrix.BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x) +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.w] * fWeightW;
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

    
    vPosition = Compute_FinalMatrix(vPosition);
    Out.vPosition = vPosition;
  

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
    return Out;

}



#endif // _SHADOW_HLSL_