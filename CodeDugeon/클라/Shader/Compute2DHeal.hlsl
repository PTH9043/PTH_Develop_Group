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


struct ComputeParticleType
{
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
    float2 vAnimUV;
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

cbuffer PARTICLETYPEBUFFER : register(b14)
{
    ComputeParticleType g_ParticleType;
}

RWStructuredBuffer<PARTICLE> g_ParticleWritedata : register(u0);
RWStructuredBuffer<COMPUTESHARED> g_SharedData : register(u1);

float3 g_Gravity = float3(0.0, -9.8, 0.0);


//[numthreads(512, 1, 1)]
//void CS_Main(int3 threadIndex : SV_DispatchThreadID)
//{
//    if (threadIndex.x >= g_GrobalParticleInfo.iMaxCount)
//        return;
    
//    g_SharedData[0].iAddCount = g_GrobalParticleInfo.iAddCount;
//    // 모두 한 번 기다리는 것
//    GroupMemoryBarrierWithGroupSync();

//    if (g_ParticleWritedata[threadIndex.x].iAlive == 0)
//    {
//        while (true)
//        {
//            int remaining = g_SharedData[0].iAddCount;
//            if (remaining <= 0)
//                break;

//            int expected = remaining;
//            int desired = remaining - 1;
//            int originalValue;
//            // Atomic 같은 느낌 한 번에 한 번만 실행된다. 
//            // 1인자와 2인자가 같으면 desire을 다르면 OriginValue를 넣어준다. 
//            InterlockedCompareExchange(g_SharedData[0].iAddCount, expected, desired, originalValue);

//            if (originalValue == expected)
//            {
//                g_ParticleWritedata[threadIndex.x].iAlive = 1;
//                break;
//            }
//        }

//        // 랜덤 부분 간단하게 추출 Noise TexTexture 이용
//        if (g_ParticleWritedata[threadIndex.x].iAlive == 1)
//        {
//            float x = ((float) threadIndex.x / (float) g_GrobalParticleInfo.iMaxCount) + g_GrobalParticleInfo.fAccTime;

//            float r1 = Rand(float2(x, g_GrobalParticleInfo.fAccTime));
//            float RRr1 = Rand(float2(x, g_GrobalParticleInfo.fDeltaTime));
//            float r2 = Rand(float2(x * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime));
//            float r3 = Rand(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
         
//            // [0.5~1] -> [0~1]
//            float3 noise =
//            {
//                2 * r1 - 1,
//                2 * r2 - 1,
//                2 * r3 - 1
//            };

//            // [0~1] -> [-1~1]
//            float3 dir;
//            dir.xz = (noise.xz - 0.5f) * 2.f;
//            dir.y = noise.y;
//            float offset = (noise - 0.5f) * 2.f;
  
//            dir.y *= 2;
//            g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticlePosition + (dir.xyz);
//            g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(dir);
           
         

         
//            g_ParticleWritedata[threadIndex.x].fLifeTime = ((g_GrobalParticleInfo.fMaxLifeTime - g_GrobalParticleInfo.fMinLifeTime))
//            + g_GrobalParticleInfo.fMinLifeTime - Rand(float2(((float) threadIndex.x / (float) g_GrobalParticleInfo.iMaxCount) + g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime) / 4);
          
//            g_ParticleWritedata[threadIndex.x].fTransparency = 1.f;
//            g_ParticleWritedata[threadIndex.x].fCurTime = 0.f;
//        }
//    }
//    else
//    {
//        // 나머지 쓰기
//        g_ParticleWritedata[threadIndex.x].fCurTime += g_GrobalParticleInfo.fDeltaTime;
//        if (g_ParticleWritedata[threadIndex.x].fLifeTime < g_ParticleWritedata[threadIndex.x].fCurTime)
//        {
//            g_ParticleWritedata[threadIndex.x].iAlive = 0;
//            return;
//        }

//        float ratio = g_ParticleWritedata[threadIndex.x].fCurTime / g_ParticleWritedata[threadIndex.x].fLifeTime;
//        float speed = (g_GrobalParticleInfo.fMaxSpeed - g_GrobalParticleInfo.fMinSpeed) * ratio + g_GrobalParticleInfo.fMinSpeed;
//       // Calculate circular motion
//        float angle = g_ParticleWritedata[threadIndex.x].fCurTime * 2.0f * 3.14159265f; // Full circle over the particle's lifetime
//        float radius = 0.01f; // Adjust the radius for the desired circular motion size
//        float3 circularMotion = float3(cos(angle) * radius, 0.0f, sin(angle) * radius);

//        if (ratio < 0.5f)
//        {
//            // 생명 주기의 절반 이전: 위로 튀는 효과
//            g_ParticleWritedata[threadIndex.x].vWorldPos += (g_ParticleWritedata[threadIndex.x].vWorldDir * speed * g_GrobalParticleInfo.fDeltaTime) + circularMotion;
           
//        }
//        else
//        {
//            // 생명 주기의 절반 이후: 중력의 영향을 받아 아래로 떨어짐
//            float3 gravityEffect = g_Gravity * 10.0f; // 중력 효과를 대폭 강화
//            g_ParticleWritedata[threadIndex.x].vWorldPos += gravityEffect + circularMotion;
//        }
//        g_ParticleWritedata[threadIndex.x].vWorldPos.y += 0.025;
//        g_ParticleWritedata[threadIndex.x].fTransparency = (1.0f - ratio);
        
//    }
   
//}






[numthreads(512, 1, 1)]
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
            float RRr1 = Rand(float2(x, g_GrobalParticleInfo.fDeltaTime));
            float r2 = Rand(float2(x * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime));
            float r3 = Rand(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
         
            // [0.5~1] -> [0~1]
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // [0~1] -> [-1~1]
            float3 dir;
            dir.xz = (noise.xz - 0.5f) * 2.f;
            dir.y = noise.y;
            float offset = (noise - 0.5f) * 2.f;
  
            dir.y *= 2;
            g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticlePosition + (dir.xyz);
            g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(dir);
           
         

         
            g_ParticleWritedata[threadIndex.x].fLifeTime = ((g_GrobalParticleInfo.fMaxLifeTime - g_GrobalParticleInfo.fMinLifeTime))
            + g_GrobalParticleInfo.fMinLifeTime - Rand(float2(((float) threadIndex.x / (float) g_GrobalParticleInfo.iMaxCount) + g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime) / 4);
          
            g_ParticleWritedata[threadIndex.x].fTransparency = 1.f;
            g_ParticleWritedata[threadIndex.x].fCurTime = 0.f;
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

        float ratio = g_ParticleWritedata[threadIndex.x].fCurTime / g_ParticleWritedata[threadIndex.x].fLifeTime;
        float speed = (g_GrobalParticleInfo.fMaxSpeed - g_GrobalParticleInfo.fMinSpeed) * ratio + g_GrobalParticleInfo.fMinSpeed;
       // Calculate circular motion
        float angle = g_ParticleWritedata[threadIndex.x].fCurTime * 4.0f * 3.14159265f; // Full circle over the particle's lifetime
        float radius = 0.1f; // Adjust the radius for the desired circular motion size
        float3 circularMotion = float3(cos(angle) * radius, 0.0f, sin(angle) * radius);

        if (ratio < 0.5f)
        {
            // 생명 주기의 절반 이전: 위로 튀는 효과
            g_ParticleWritedata[threadIndex.x].vWorldPos += (g_ParticleWritedata[threadIndex.x].vWorldDir * speed * g_GrobalParticleInfo.fDeltaTime) + circularMotion;
           
        }
        else
        {
            // 생명 주기의 절반 이후: 중력의 영향을 받아 아래로 떨어짐
            float3 gravityEffect = g_Gravity * 10.0f; // 중력 효과를 대폭 강화
            g_ParticleWritedata[threadIndex.x].vWorldPos += gravityEffect + circularMotion;
        }
        g_ParticleWritedata[threadIndex.x].vWorldPos.y += 0.025;
        g_ParticleWritedata[threadIndex.x].fTransparency = (1.0f - ratio);
        
    }
   
}
































#endif // _COMPUTE2DPARTICLE_