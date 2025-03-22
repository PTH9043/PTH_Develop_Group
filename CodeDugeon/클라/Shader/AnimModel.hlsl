#ifndef _ANIMMODEL_HLSL_
#define _ANIMMODEL_HLSL_

#include "ShaderGrobalFunc.hlsli"


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
    int             g_iOutLineExist = 0;
    int             g_iNomralExist = 1;
    int             g_iObjectDissolve = 0;
    float         g_fDissolveTimer = 0;
};

cbuffer BONEMATRIXPARAM : register(b12)
{
    BONEMATRIX g_BoneMatrix;
};

cbuffer PREVBONEMATRIXPARAM : register(b13)
{
    BONEMATRIX g_PrevBoneMatrix;
};

cbuffer DrawRim : register(b14)
{
    int g_DrawRim;
    float3 RimColor;
};

static const float dissolveAmount = 0.3f;
static const float4 edgeColor = float4(1.f, 0.f, 0.f, 0.f);
static const float noiseTiling = 2.f;
static const float edgeThickness = 0.1f;

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
	float4 vVelocity : TEXCOORD7;
    float3 vViewDir : TEXCOORD8;
};

VS_OUT VS_Main(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = (g_BoneMatrix.BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x) +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrix.BoneMatrix[In.vBlendIndex.w] * fWeightW;
    

    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
   // vector vPosition = (float4(In.vPosition, 1.f));
    
    vPosition = Compute_FinalMatrix(vPosition);
   

    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
    Out.vTexUV0 = In.vTexUV0;
    Out.vTexUV1 = In.vTexUV1;
    Out.vTexUV2 = In.vTexUV2;
    Out.vTexUV3 = In.vTexUV3;
    Out.vTexUV4 = In.vTexUV4;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vViewDir = normalize(g_ViewProjInfoArr[g_CamID].vCamPosition - Out.vWorldPos.xyz); // Compute the view direction

   
    
    if (true == g_isObjectMotionBlur || 0 != g_TransformMotionBlurOn)
    {
        matrix PrevBoneMatrix = (g_PrevBoneMatrix.BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x) +
                        (g_PrevBoneMatrix.BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y) +
                        (g_PrevBoneMatrix.BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z) +
                        (g_PrevBoneMatrix.BoneMatrix[In.vBlendIndex.w] * fWeightW);
        
        vector vPrevPosition = mul(float4(In.vPosition, 1.f), PrevBoneMatrix);
        
        VIEWPROJINFO tMainViewProj = GetViewProjInfo();
        
        vPrevPosition = mul(vPrevPosition, g_PrevWorldMatrix);
        vPrevPosition = mul(vPrevPosition, tMainViewProj.mPrevViewMatirx);
        vPrevPosition = mul(vPrevPosition, tMainViewProj.mProjMatrix);
        
        float3 dir = Out.vPosition.xyz - vPrevPosition.xyz;
        float4 NORMAL = mul(Out.vNormal, tMainViewProj.mPrevViewMatirx);
        float a = dot(normalize(dir), normalize(NORMAL.xyz));
     //   if (a < 0.f)
      //      Out.vPosition = vPrevPosition;
        
        float2 velocity = ((Out.vPosition.xy / Out.vPosition.w - (vPrevPosition.xy / vPrevPosition.w)) * 0.5f);
        velocity.y *= -1.f;
        Out.vVelocity.xy = velocity;
        Out.vVelocity.zw = Out.vPosition.zw;
    }
    
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
    float4 vVelocity : TEXCOORD7;
    float3 vViewDir : TEXCOORD8;
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

/* 픽셀의 색을 결정한다. */
// float4 PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_Main(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    VIEWPROJINFO tMainViewProj = GetViewProjInfo();
    // Out Color    
   
    Out.vDiffuse = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV0);
 
    if (Out.vDiffuse.a <= 0.05)
        discard;
    
    if (g_DrawRim)
    {
        float rimFactor = 1.0 - saturate(dot(normalize(In.vViewDir), normalize(In.vNormal.xyz)));
        rimFactor = pow(rimFactor, 3.5) * 3;
        float4 rimLight = rimFactor * float4(RimColor, 1);

    // Combine rim lighting with the final color
        Out.vDiffuse.rgb += rimLight.rgb;
    }
   
    Out.vNormal = float4(float3(In.vNormal.xyz) * 0.5f + 0.5f, 1.f);
  
    Out.vDepth = float4(In.vProjPos.w / tMainViewProj.fCamFar, In.vProjPos.z / In.vProjPos.w, 1.f, In.vPosition.w);
    Out.vPosition = In.vWorldPos;
    
    if (true == g_isObjectMotionBlur || 0 != g_TransformMotionBlurOn)
    {
        Out.vVelocity = In.vVelocity;
        Out.vVelocity.z = 1.f;
        Out.vVelocity.w = In.vVelocity.z / In.vVelocity.w;
    }
    
    //if (1 == g_iObjectDissolve)
    //{
        
    //    vector vDisorveColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV0 * noiseTiling);

    //    float fColorLength = vDisorveColor.r + vDisorveColor.g + vDisorveColor.b;
        
    //    if (fColorLength <= g_fDissolveTimer)
    //    {
    //        if (fColorLength - 0.2f <= g_fDissolveTimer)
    //        {
    //            discard;
    //        }
    //        float4 vColor = float4(1.f, -0.22f, -0.22f, Out.vDiffuse.a);
    //        Out.vDiffuse = vColor;
    //    }
    //}
    
    if (1 == g_iObjectDissolve)
    {
        vector vDisolveColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV0 * noiseTiling);

        float fColorLength = vDisolveColor.r + vDisolveColor.g + vDisolveColor.b;    

        float edgeThickness = 0.05f; 
   
        float4 baseEdgeColor = float4(1.0f, 0.5f, 0.0f, 1.0f); 
        float4 burnColor = float4(1.0f, 0.0f, 0.0f, 1.0f); 
        float brightness = 4.0f;
        
        float4 dissolveColor = float4(1.f, -0.22f, -0.22f, Out.vDiffuse.a);

        if (fColorLength <= g_fDissolveTimer)
        {
            if (fColorLength >= g_fDissolveTimer - edgeThickness)
            {
                float factor = (g_fDissolveTimer - fColorLength) / edgeThickness;
                float4 edgeColor = lerp(baseEdgeColor, burnColor, factor); 
                edgeColor.rgb *= brightness; 
                Out.vDiffuse = edgeColor;
                Out.vGlow = edgeColor;
            }
            else
            {
                discard;
            }
        }
        else
        {
            Out.vDiffuse = Out.vDiffuse;
        }
    }

        
    return Out;
}



#endif // _ANIMMODEL_HLSL_