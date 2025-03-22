#ifndef _COMPUTE2DPARTICLE_
#define _COMPUTE2DPARTICLE_

#include "ComputeFunc.hlsli"

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


struct ComputeParticleType {
    int fParticleType;
    int fParticleLifeTimeType;
    float2 fPadding;
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
    float2 vAnimUV;     //원래 vAnimUV
    float fTransparency; //원래 padding
};

struct COMPUTESHARED
{
    int iAddCount;
    float3 padding;
};

// All Particle

cbuffer ALLPARTICLEBUFFER : register(b13)
{
    GROBALPARTICLEINFO g_GrobalParticleInfo;
};


RWStructuredBuffer<PARTICLE> g_ParticleWritedata : register(u0);
RWStructuredBuffer<COMPUTESHARED> g_SharedData : register(u1);

float3 g_UpForce = float3(0.0, 5.8, 0.0);
float3 g_Wind = float3(2.0, 0.0, 0.0); // 오른쪽으로 바람

// CS_Main
// g_vec2_1 : DeltaTime / AccTime
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed
[numthreads(50, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_GrobalParticleInfo.iMaxCount)
        return;
    
    g_SharedData[0].iAddCount = g_GrobalParticleInfo.iAddCount;
    // 모두 한 번 기다리는 것
    GroupMemoryBarrierWithGroupSync();

    if (g_ParticleWritedata[threadIndex.x].iAlive == 0)
    {      
        while (true)
        {
            int remaining = g_SharedData[0].iAddCount;
            if (remaining <= 0)
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            // Atomic 같은 느낌 한 번에 한 번만 실행된다. 
            // 1인자와 2인자가 같으면 desire을 다르면 OriginValue를 넣어준다. 
            InterlockedCompareExchange(g_SharedData[0].iAddCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_ParticleWritedata[threadIndex.x].iAlive = 1;
                break;
            }
        }

        // 랜덤 부분 간단하게 추출 Noise TexTexture 이용
        if (g_ParticleWritedata[threadIndex.x].iAlive == 1)
        {
            float x = ((float) threadIndex.x / (float) g_GrobalParticleInfo.iMaxCount) + g_GrobalParticleInfo.fAccTime;

            // [0~1] -> [-1~1]
            float r1 = Rand(float2(x, g_GrobalParticleInfo.fAccTime));
            float r2 = Rand(float2(x * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime));
            float r3 = Rand(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
           
            float3 noise =
            {
                (2 * r1 - 1),
                 0,
                0
            };
            //if (fmod(threadIndex.x, 2) == 1)
            //{
            //    g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticleDirection +g_GrobalParticleInfo.fParticleThickness* float3(1, 0, 0);
      
            //}
            //else
            //{
            //    g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticleDirection + g_GrobalParticleInfo.fParticleThickness * float3(1, 0, 0);
     
            //}
            g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticlePosition + g_GrobalParticleInfo.fParticleThickness * g_GrobalParticleInfo.fParticleDirection;
     
            g_ParticleWritedata[threadIndex.x].vWorldDir = float3(0, 0, -0.1);
            g_ParticleWritedata[threadIndex.x].fLifeTime = g_GrobalParticleInfo.fMaxLifeTime;
          
            g_ParticleWritedata[threadIndex.x].fCurTime = 0.f;
            g_ParticleWritedata[threadIndex.x].fTransparency = 1.f;
        }
    }
    else
    {
        // 나머지 쓰기
        g_ParticleWritedata[threadIndex.x].fCurTime += g_GrobalParticleInfo.fDeltaTime;
        if (g_ParticleWritedata[threadIndex.x].fLifeTime < g_ParticleWritedata[threadIndex.x].fCurTime)
        {
            g_ParticleWritedata[threadIndex.x].iAlive = 0;
            return;
        }
        float3 externalForces = g_UpForce * g_GrobalParticleInfo.fDeltaTime; // +g_Wind * g_GrobalParticleInfo.fDeltaTime;
       
        g_ParticleWritedata[threadIndex.x].vWorldDir += externalForces;
        g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(g_ParticleWritedata[threadIndex.x].vWorldDir);


        float ratio = g_ParticleWritedata[threadIndex.x].fCurTime / g_ParticleWritedata[threadIndex.x].fLifeTime;
        float speed = (g_GrobalParticleInfo.fMaxSpeed - g_GrobalParticleInfo.fMinSpeed) * ratio + g_GrobalParticleInfo.fMinSpeed;
        g_ParticleWritedata[threadIndex.x].vWorldPos += g_ParticleWritedata[threadIndex.x].vWorldDir * speed * g_GrobalParticleInfo.fDeltaTime;
   
        //if (ratio < 0.5f)
        //{
        //    g_ParticleWritedata[threadIndex.x].fTransparency = ratio * 2.0f; // 0 ~ 1
        //}
        //else
        //{
        //    g_ParticleWritedata[threadIndex.x].fTransparency = (1.0f - ratio) * 2.0f; // 1 ~ 0
        //}
        g_ParticleWritedata[threadIndex.x].fTransparency = (1.0f - ratio);

    }
   
}




#endif // _COMPUTE2DPARTICLE_