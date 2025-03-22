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

    // View Space
    output[0].vPosition = Vertex.vViewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].vPosition = Vertex.vViewPos + float4(scale, scale, 0.f, 0.f);
    output[2].vPosition = Vertex.vViewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].vPosition = Vertex.vViewPos + float4(-scale, -scale, 0.f, 0.f);
    
    output[0].vWorldPos = Vertex.vWorldPos + float4(-scale, scale, 0.f, 0.f);
    output[1].vWorldPos = Vertex.vWorldPos + float4(scale, scale, 0.f, 0.f);
    output[2].vWorldPos = Vertex.vWorldPos + float4(scale, -scale, 0.f, 0.f);
    output[3].vWorldPos = Vertex.vWorldPos + float4(-scale, -scale, 0.f, 0.f);

    VIEWPROJINFO stViewProjInfo = GetViewProjInfo();
    
    // Projection Space
    output[0].vPosition = mul(output[0].vPosition, stViewProjInfo.mProjMatrix);
    output[1].vPosition = mul(output[1].vPosition, stViewProjInfo.mProjMatrix);
    output[2].vPosition = mul(output[2].vPosition, stViewProjInfo.mProjMatrix);
    output[3].vPosition = mul(output[3].vPosition, stViewProjInfo.mProjMatrix);

    output[0].vTexUV = float2(0.f, 0.f);
    output[1].vTexUV = float2(1.f, 0.f);
    output[2].vTexUV = float2(1.f, 1.f);
    output[3].vTexUV = float2(0.f, 1.f);

    output[0].iInstanceID = id;
    output[1].iInstanceID = id;
    output[2].iInstanceID = id;
    output[3].iInstanceID = id;

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
    if (fmod(In.iInstanceID, 8) == 0)
    {
        Out.vColor = g_Texture0.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if (fmod(In.iInstanceID, 8) == 1)
    {
        Out.vColor = g_Texture1.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if(fmod(In.iInstanceID, 8) == 2)
    {
        Out.vColor = g_Texture2.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if(fmod(In.iInstanceID, 8) == 3)
    {
        Out.vColor = g_Texture3.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if(fmod(In.iInstanceID, 8) == 4)
    {
        Out.vColor = g_Texture4.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if(fmod(In.iInstanceID, 8) == 5)
    {
        Out.vColor = g_Texture5.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if (fmod(In.iInstanceID, 8) == 6)
    {
        Out.vColor = g_Texture6.Sample(g_Sampler_Normal, In.vTexUV);
    }
    else if (fmod(In.iInstanceID, 8) == 7)
    {
        Out.vColor = g_Texture7.Sample(g_Sampler_Normal, In.vTexUV);
    }
    float particleTransparency = g_ParticleData[In.iInstanceID].fTransparency;
    Out.vColor.a *= particleTransparency;
    if (Out.vColor.a < 0.1)
        discard;
    Out.vGlow = Out.vColor;
   
        return Out;
}

#endif 