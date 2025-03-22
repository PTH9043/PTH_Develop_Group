#ifndef _LIGHTPOINT_HLSL_
#define _LIGHTPOINT_HLSL_

#include "LightShaderFunc.hlsli"

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
//해상도 1280 1080
VS_OUT VS_Main(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = mul(float4(In.vPosition, 1.f), g_tLightParam.mScreenWorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_tLightParam.mScreenViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_tLightParam.mScreenProjMatrix);
 
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
    float4 vAmbient : SV_TARGET0;
    float4 vShade : SV_TARGET1;
    float4 vSpecular : SV_TARGET2;
};

PS_OUT PS_Main(PS_In Input)
{
    PS_OUT Out = (PS_OUT) 0;

    //월드에서는 vNormal을 텍스쳐로부터 읽고 그대로 쓰는게 조명이 확실히 DiffuseFactor계산이 됨.
    // 뷰공간에선 노멀라이즈하는게 좋아보임.
    // 
    //// 0, 1, 3
    float4 vNormal = g_Texture0.Sample(g_Sampler_Normal, Input.vTexUV);
   //vNormal = normalize(vector(vNormal.xyz * 2.f - 1.f, 0.f));
    float4 vDepth = g_Texture1.Sample(g_Sampler_Normal, Input.vTexUV);
    float4 vWorldPosition = g_Texture2.Sample(g_Sampler_Normal, Input.vTexUV);
    float4 vWorldNormal = normalize(vector(vNormal.xyz * 2.f - 1.f, 0.f));
    float4 vViewPosition = mul(vWorldPosition, g_ViewProjInfoArr[MAIN_CAM_ID].mViewMatrix);
    float4 vViewNormal = mul(vWorldNormal, g_ViewProjInfoArr[MAIN_CAM_ID].mViewMatrix);
    vViewNormal = normalize(vViewNormal);
  

    LIGHTCOLOR tLightColor = (LIGHTCOLOR) 0.f;
 
    tLightColor = LightingInWorld(vWorldPosition.xyz, vWorldNormal.xyz); //용빠 월드
    
  
    if (length(tLightColor.vDiffuse) != 0)
    { // Shadow Map 카메라 기준 View/Projection 변환
        float4 shadowViewPos = mul(vWorldPosition, g_ViewProjInfoArr[3].mViewMatrix);
        float4 shadowClipPos = mul(shadowViewPos, g_ViewProjInfoArr[3].mProjMatrix);
        float depth = 0;
        if (shadowClipPos.w > 0.0f)
        { // 카메라 뒤 영역 필터링
            depth = shadowClipPos.z / shadowClipPos.w;
        }
     
        // x [-1 ~ 1] -> u [0 ~ 1]
        // y [1 ~ -1] -> v [0 ~ 1]
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5 + 0.5;

        if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
        {
            float shadowDepth = g_Texture3.Sample(g_Sampler_Normal, uv).x;
            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
            {
                // 월드 좌표와 카메라 좌표 간 거리 계산
                float distance = length(vWorldPosition.xyz - g_ViewProjInfoArr[3].vCamPosition);

                // 거리 기반 계수 계산 (RangeMin, RangeMax 설정)
                float rangeMin = 0.05f; // 최소 거리
                float rangeMax = 100.f; // 최대 거리
                float distanceFactor = saturate( (distance - rangeMin) / (rangeMax - rangeMin));
                float diffuseFactor = lerp(0.05f, 1.f, distanceFactor); // 보간 수행

                // 조명 계수 조정
                tLightColor.vDiffuse *= diffuseFactor;
                
                
                tLightColor.vSpecular *=(float4) diffuseFactor*0.9f;
                tLightColor.vAmbient *= (float4) diffuseFactor * 0.9f;
            }
        }
    }
    
   // if (length(tLightColor.vDiffuse) != 0)
   // {
       
   //     float width, height, numMips;
   //     g_Texture3.GetDimensions(0, width, height, numMips);
       
   //     float4 shadowViewPos = mul(vWorldPosition, g_ViewProjInfoArr[3].mViewMatrix);
   //     float4 shadowClipPos = mul(shadowViewPos, g_ViewProjInfoArr[3].mProjMatrix);
   //     float depth = shadowClipPos.z / shadowClipPos.w - 0.0001f;
        
   //     if (depth < 0.0f || depth > 1.0f)
   //     {
   //         continue;
   //     }
   //     // x [-1 ~ 1] -> u [0 ~ 1]
   //     // y [1 ~ -1] -> v [0 ~ 1]
   //     float2 uv = shadowClipPos.xy / shadowClipPos.w;
   //     uv.y = -uv.y;
   //     uv = uv * 0.5 + 0.5;
        
   //     if (uv.x < 0.0f || uv.x > 1.0f ||
			//uv.y < 0.0f || uv.y > 1.0f)
   //     {
   //         continue;
   //     }
        
   //     float dx = 1.0f / width;
   //     float percentLit = 0.0f;
   //     const float2 offsets[9] =
   //     {
   //         float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
			//float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
			//float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
   //     };
   //     [unroll]
   //     for (int j = 0; j < 9; ++j)
   //     {
   //         percentLit += g_Texture3.SampleCmpLevelZero(g_Sampler_Border, uv.xy + offsets[j], depth).r;
   //     }
   //     percentLit = percentLit / 9.0f;
        
   //     if (percentLit < 0.33f)
   //         percentLit = 0.33f;
   //     else if (percentLit < 0.66f)
   //         percentLit = 0.66f;
   //     else
   //         percentLit = 0.99f;
        
     
   //     if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
   //     {
   //         tLightColor.vDiffuse *= percentLit;
   //         tLightColor.vSpecular = (float4) 0.f;
   //         tLightColor.vAmbient = (float4) 0.f;
   //     }
   // }
  
    
    Out.vAmbient = tLightColor.vAmbient;
    Out.vShade = tLightColor.vDiffuse;
    Out.vSpecular = tLightColor.vSpecular ;
    
    
    float deltaTimeRepeat = frac(fGrobalDeltaTime);
// 범위가 sin 함수를 사용하여 매 프레임마다 변하도록 설정됩니다.
    float radius = 72 + 30 * sin(fGrobalDeltaTime);
    float radiusSqrt = sqrt(radius); // 미리 계산

// 감쇠 시작 거리를 조정합니다.
    float attenuationDistance = radiusSqrt * 0.45f;

    [unroll(MAX_LIGHTS)]
    for (int i = 0; i < nLights; i++)
    {
        if (g_tLightInfo[i].eLightActive)
        {
            if (g_tLightInfo[i].eLightType == POINT_LIGHT)
            {
                float3 vToLight = g_tLightInfo[i].vPosition - vWorldPosition;
                float fDistance = length(vToLight);

        
                float fAttenuationFactor = 1.0f;
                if (fDistance > attenuationDistance)
                {
                    float normalizedDistance = (fDistance - attenuationDistance) / (radiusSqrt - attenuationDistance);
                    fAttenuationFactor = 1.0f - normalizedDistance;

            
                    fAttenuationFactor = max(fAttenuationFactor, 0.0f);
                }

        
                if (fDistance <= radiusSqrt)
                {
                    Out.vAmbient += g_tLightInfo[i].vAmbient * fAttenuationFactor;
                    Out.vShade += g_tLightInfo[i].vDiffuse * fAttenuationFactor;
                    Out.vSpecular += g_tLightInfo[i].vSpecular * fAttenuationFactor;
                }
            }
        }
    }
    
    
    
	return Out;
}

#endif // _LIGHTDEFFERED_HLSL_