#ifndef _2DPARTICLE_
#define _2DPARTICLE_

#include "ShaderGrobalFunc.hlsli"


struct GROBALPARTICLEINFO
{
    int iMaxCount;
    int iAddCount;
    float fDeltaTime;
    float fAccTime;
    // ==============
  
    float fMinLifeTime;
    float fMaxLifeTime;
    float fMinSpeed;
    float fMaxSpeed;
    // ===============
   
    float fStartScaleParticle;
    
    float fEndScaleParticle;
    float fParticleThickness;
    int fParticleKind;
    //===============
    float3 fParticleDirection;
    float fAnimSizeX;
    
     //================
    float fAnimSizeY;
    float3 fParticlePosition;
    //=================       
    float fNextAnimTime;
    float3 fPadding;
};


struct PARTICLE
{
    float3 vWorldPos;
    float fCurTime;
    // ===============
    float3 vWorldDir;
    float fLifeTime;
    // ==============
    int iAlive;
    float2 vAnimUV;
    float fTransparency; //원래 padding
};

// All Particle

cbuffer ALLPARTICLEBUFFER : register(b13)
{
    GROBALPARTICLEINFO g_GrobalParticleInfo;
};

StructuredBuffer<PARTICLE> g_ParticleData : register(t14);

// VS_MAIN
// g_float_0    : Start Scale
// g_float_1    : End Scale
// g_tex_0      : Particle Texture

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTagent : TANGENT;
    float2 vTexUV : TEXCOORD;
    uint iInstanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 vViewPos : POSITION;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    uint iInstanceID : ID;
};


VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.f;

    Out.vViewPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vViewPos += float4(g_ParticleData[In.iInstanceID].vWorldPos, 0.f);
    // 컴퓨트 셰이더에서 RWStructuredBuffer에 적어놓은 각자 파티클 numthreads 1024개 중 우리는 
    // maxParticle인 1000개 또는 500개 등 정한 수치만큼 해당 버퍼에 정보를 기입하였으므로
    // 해당 정보를 instanceID로 받아와 읽어 사용함.
    
    Out.vViewPos = mul(Out.vViewPos, GetViewProjInfo().mViewMatrix);
    Out.vTexUV = In.vTexUV;
    Out.iInstanceID = In.iInstanceID;
    return Out;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    uint iInstanceID : SV_InstanceID;
    float2 vScreenTex : TEXCOORD2; // 화면 좌표 추가

};


[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };

    VS_OUT Vertex = input[0];
    uint id = (uint) Vertex.iInstanceID;
    if (0 == g_ParticleData[id].iAlive)
        return;

    float ratio = g_ParticleData[id].fCurTime / g_ParticleData[id].fLifeTime;
    float scale = ((g_GrobalParticleInfo.fEndScaleParticle - g_GrobalParticleInfo.fStartScaleParticle) * ratio +
    g_GrobalParticleInfo.fStartScaleParticle) / 2.f;

    float ScaleX = g_ParticleData[id].vAnimUV.x * scale;
    float ScaleY = g_ParticleData[id].vAnimUV.y * scale;

    //float4 positions[4] =
    //{
    //    float4(-ScaleX, ScaleY, 0.f, 0.f),
    //float4(ScaleX, ScaleY, 0.f, 0.f),
    //float4(ScaleX, -ScaleY, 0.f, 0.f),
    //float4(-ScaleX, -ScaleY, 0.f, 0.f)
    //};
   float4 positions[4] =
   {
       float4(-scale, scale, 0.f, 0.f),
   float4(scale, scale, 0.f, 0.f),
   float4(scale, -scale, 0.f, 0.f),
   float4(-scale, -scale, 0.f, 0.f)
   };

    float2 texCoords[4] =
    {
        float2(0.f, 0.f),
        float2(1.f, 0.f),
        float2(1.f, 1.f),
        float2(0.f, 1.f)
    };

    for (int i = 0; i < 4; ++i)
    {
        float x = positions[i].x;
        float y = positions[i].y;
 

        output[i].vPosition = Vertex.vViewPos + positions[i];
        output[i].vWorldPos = Vertex.vWorldPos + positions[i];

        // Projection Space
        VIEWPROJINFO stViewProjInfo = GetViewProjInfo();
        output[i].vPosition = mul(output[i].vPosition, stViewProjInfo.mProjMatrix);

        output[i].vTexUV = texCoords[i];
        output[i].iInstanceID = id;
        output[i].vScreenTex = output[i].vPosition.xy / output[i].vPosition.w;
    }
    // 삼각형 두 개 실행
    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
    float4 vNormal : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vPosition : SV_Target4;
    float4 vGlow : SV_Target5;
};


PS_OUT PS_Main(GS_OUT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    float2 ScreenTex;
    ScreenTex.x = (In.vScreenTex.x) * 0.5 + 0.5f;
    ScreenTex.y = (In.vScreenTex.y) * -0.5 + 0.5f;

    vector vDepthDesc = g_Texture1.Sample(g_Sampler_Normal, ScreenTex);

    float fViewZ = vDepthDesc.x * GetViewProjInfo().fCamFar;

// 깊이 차이 계산
    float depthDifference = abs(fViewZ - In.vPosition.w);

// 깊이 차이에 기반하여 부드럽게 알파 값을 조정
    float fDistance = smoothstep(0.0, 1.0, depthDifference * 0.05); // 0.0과 0.1은 필요시 조정

// 텍스처 샘플링
    Out.vColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
   //Out.vColor.rgb *= 1.5;
// 최소 알파 값을 유지하여 전체적으로 파티클이 보이도록 함
    float minAlpha = 0.2; // 최소 알파 값, 필요시 조정
    Out.vColor.a *= fDistance;

    Out.vGlow = (Out.vColor.rgb,0.5);
    
    float particleTransparency = g_ParticleData[In.iInstanceID].fCurTime / g_ParticleData[In.iInstanceID].fLifeTime;
    Out.vColor.a *= particleTransparency;
   if (Out.vColor.a < 0.05)
       discard;
  
  
  
        return Out;
}

#endif 