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


//struct ComputeParticleType {
//    int fParticleType;
//    int fParticleLifeTimeType;
//    float2 fPadding;
//};

struct PARTICLEPLUS
{
    float3 vWorldPos;
    float fCurTime;
    // ===============
    float3 vWorldDir; //Velocity.xyz로도 쓰일것.
    float fLifeTime;
    // ==============
    int iAlive;
    float3 fScaleFactor; //Scale .x,.y를 StartScale, EndScale로
    //============
    float fStartTime;
    float fAmp;
    float fPeriod;
    float paddingplus;
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

//cbuffer PARTICLETYPEBUFFER : register(b14)
//{
//    ComputeParticleType g_ParticleType;
//}

RWStructuredBuffer<PARTICLEPLUS> g_ParticleWritedata : register(u0);
RWStructuredBuffer<COMPUTESHARED> g_SharedData : register(u1);

const float3 v_Gravity = float3(0, -4.9, 0);
const float c_PI = 3.141592;
// CS_Main
// g_vec2_1 : DeltaTime / AccTime
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed
[numthreads(116, 1, 1)]
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

            float r1 = Rand(float2(x, g_GrobalParticleInfo.fAccTime));
            float r2 = Rand(float2(x * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime));
            float r3 = Rand(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
            float noise4 = RandNormalized(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
           
            // [0.5~1] -> [0~1]
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // [0~1] -> [-1~1]
            float3 dir = (noise - 0.5f) * 2.f;
           
            //
           
                g_ParticleWritedata[threadIndex.x].vWorldPos = float3(0.0, 0.0, 0);
                g_ParticleWritedata[threadIndex.x].vWorldDir = float3(0.0, 0.5, 0);
                g_ParticleWritedata[threadIndex.x].fStartTime = 0.4 * noise4;
                g_ParticleWritedata[threadIndex.x].fLifeTime = 1.6;
            g_ParticleWritedata[threadIndex.x].fAmp = noise.x+0.3;
                g_ParticleWritedata[threadIndex.x].fPeriod =noise.y*2.5;
            g_ParticleWritedata[threadIndex.x].fScaleFactor.x = r3-0.2;
            g_ParticleWritedata[threadIndex.x].fScaleFactor.y = r2 -0.5;
                
            
           
          
            g_ParticleWritedata[threadIndex.x].fCurTime = -g_ParticleWritedata[threadIndex.x].fStartTime;
            //;
        }
    }
    else
    {
        float3 newPosition = g_ParticleWritedata[threadIndex.x].vWorldPos;
        // 나머지 쓰기
        g_ParticleWritedata[threadIndex.x].fCurTime += g_GrobalParticleInfo.fDeltaTime;
        //g_ParticleWritedata[threadIndex.x].fCurTime += 0.016f; //이게 훨씬 빨리 올라감
        float t = g_ParticleWritedata[threadIndex.x].fCurTime;
       
        float r1 = Rand(float2(threadIndex.x, threadIndex.x));
    
        float r3 = Rand(float2(threadIndex.x * 3, threadIndex.x * 3));
        float3 noise =
        {
            2 * r1 - 1,
              0,
                2 * r3 - 1
        };
        float3 dir = (noise - 0.5f) * 2.f;
           
    
            if (g_ParticleWritedata[threadIndex.x].fCurTime < 0)
            {
                g_ParticleWritedata[threadIndex.x].vWorldPos = float3(-100000, -100000, 0);
            g_ParticleWritedata[threadIndex.x].paddingplus = -1;

            }
            else
            {
            g_ParticleWritedata[threadIndex.x].paddingplus = 1;
            newPosition = float3(0, 0, 0);
            
           
           float3 newDir = float3(-g_ParticleWritedata[threadIndex.x].vWorldDir.y, g_ParticleWritedata[threadIndex.x].vWorldDir.x, g_ParticleWritedata[threadIndex.x].vWorldDir.y);
                newDir = normalize(newDir);
               
              
                newPosition.xyz = newPosition.xyz+g_ParticleWritedata[threadIndex.x].vWorldDir.xyz * t*10;
            newPosition.xyz = newPosition.xyz + dir.xyz * sin(t * g_ParticleWritedata[threadIndex.x].fPeriod) * g_ParticleWritedata[threadIndex.x].fAmp * 2;

              
            }
          
           

        
        g_ParticleWritedata[threadIndex.x].vWorldPos.x = newPosition.x;
        g_ParticleWritedata[threadIndex.x].vWorldPos.y = newPosition.y;
        g_ParticleWritedata[threadIndex.x].vWorldPos.z = newPosition.z;
        
        if (g_ParticleWritedata[threadIndex.x].fLifeTime < g_ParticleWritedata[threadIndex.x].fCurTime)
        {
            g_ParticleWritedata[threadIndex.x].iAlive = 0;
            return;
        }
        
    }
   
}




#endif // _COMPUTE2DPARTICLE_