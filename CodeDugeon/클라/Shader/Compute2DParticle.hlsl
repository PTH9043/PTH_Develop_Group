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
    float2 vAnimUV;
    float padding;
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

// CS_Main
// g_vec2_1 : DeltaTime / AccTime
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed
[numthreads(1024, 1, 1)]
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
            if (g_ParticleType.fParticleType == 0) {//0이면 일반
                g_ParticleWritedata[threadIndex.x].vWorldPos = (noise.xyz - 0.5f);// * g_GrobalParticleInfo.fParticleThickness;
                g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(g_GrobalParticleInfo.fParticleDirection);
            }
            else if (g_ParticleType.fParticleType == 1) {//1이면 버퍼로 받은 위치값으로.
                g_ParticleWritedata[threadIndex.x].vWorldPos = g_GrobalParticleInfo.fParticlePosition;

                g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(dir);
            }

            if (g_ParticleType.fParticleLifeTimeType == 0)//0 Default 
            {
                if (g_GrobalParticleInfo.fMaxLifeTime > 0) {
                    g_ParticleWritedata[threadIndex.x].fLifeTime = g_GrobalParticleInfo.fMaxLifeTime;
                }
                else {
                    g_ParticleWritedata[threadIndex.x].fLifeTime =  g_GrobalParticleInfo.fMinLifeTime;
                }
              
            }
            else if (g_ParticleType.fParticleLifeTimeType == 1) {//1 Auto
                //g_ParticleWritedata[threadIndex.x].fLifeTime = (noise.x) + noise.y + noise.z;
                g_ParticleWritedata[threadIndex.x].fLifeTime = ((g_GrobalParticleInfo.fMaxLifeTime - g_GrobalParticleInfo.fMinLifeTime) * noise.x)
            + g_GrobalParticleInfo.fMinLifeTime;
            }
          
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
        float frameTime = floor(g_ParticleWritedata[threadIndex.x].fCurTime / g_GrobalParticleInfo.fNextAnimTime);
        int frameX = (int) fmod(frameTime, g_GrobalParticleInfo.fAnimSizeX);
        int frameY = (int) fmod((frameTime / g_GrobalParticleInfo.fAnimSizeX), g_GrobalParticleInfo.fAnimSizeY);
        g_ParticleWritedata[threadIndex.x].vAnimUV = float2(frameX, frameY);
        
        
        float ratio = g_ParticleWritedata[threadIndex.x].fCurTime / g_ParticleWritedata[threadIndex.x].fLifeTime;
        float speed = (g_GrobalParticleInfo.fMaxSpeed - g_GrobalParticleInfo.fMinSpeed) * ratio + g_GrobalParticleInfo.fMinSpeed;
        g_ParticleWritedata[threadIndex.x].vWorldPos += g_ParticleWritedata[threadIndex.x].vWorldDir * speed * g_GrobalParticleInfo.fDeltaTime;
   
        
        }
   
}


//float3x3 RotationMatrix(float3 axis, float angle)
//{
//    axis = normalize(axis);
//    float s = sin(angle);
//    float c = cos(angle);
//    float oc = 1.0f - c;
//
//    float3x3 rotationMatrix;
//    rotationMatrix[0] = float3(
//        oc * axis.x * axis.x + c,
//        oc * axis.x * axis.y - axis.z * s,
//        oc * axis.x * axis.z + axis.y * s
//        );
//    rotationMatrix[1] = float3(
//        oc * axis.x * axis.y + axis.z * s,
//        oc * axis.y * axis.y + c,
//        oc * axis.y * axis.z - axis.x * s
//        );
//    rotationMatrix[2] = float3(
//        oc * axis.x * axis.z - axis.y * s,
//        oc * axis.y * axis.z + axis.x * s,
//        oc * axis.z * axis.z + c
//        );
//
//    return rotationMatrix;
//}
//
//void RotateAroundAxis(float3 axis, float angle, inout float3 position)
//{
//    float3x3 rotationMatrix = RotationMatrix(axis, angle);
//    position = mul(rotationMatrix, position);
//}
//
//[numthreads(1024, 1, 1)]
//void CS_Main(int3 threadIndex : SV_DispatchThreadID)
//{
//    if (threadIndex.x >= g_GrobalParticleInfo.iMaxCount)
//        return;
//    
//    g_SharedData[0].iAddCount = g_GrobalParticleInfo.iAddCount;
//    // 모두 한 번 기다리는 것
//    GroupMemoryBarrierWithGroupSync();
//
//    if (g_ParticleWritedata[threadIndex.x].iAlive == 0)
//    {      
//        while (true)
//        {
//            int remaining = g_SharedData[0].iAddCount;
//            if (remaining <= 0)
//                break;
//
//            int expected = remaining;
//            int desired = remaining - 1;
//            int originalValue;
//            // Atomic 같은 느낌 한 번에 한 번만 실행된다. 
//            // 1인자와 2인자가 같으면 desire을 다르면 OriginValue를 넣어준다. 
//            InterlockedCompareExchange(g_SharedData[0].iAddCount, expected, desired, originalValue);
//
//            if (originalValue == expected)
//            {
//                g_ParticleWritedata[threadIndex.x].iAlive = 1;
//                break;
//            }
//        }
//
//        // 랜덤 부분 간단하게 추출 Noise TexTexture 이용
//        if (g_ParticleWritedata[threadIndex.x].iAlive == 1)
//        {
//            float x = ((float) threadIndex.x / (float) g_GrobalParticleInfo.iMaxCount) + g_GrobalParticleInfo.fAccTime;
//
//            float r1 = Rand(float2(x, g_GrobalParticleInfo.fAccTime));
//            float r2 = Rand(float2(x * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime));
//            float r3 = Rand(float2(x * g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime, g_GrobalParticleInfo.fAccTime * g_GrobalParticleInfo.fAccTime));
//
//            // [0.5~1] -> [0~1]
//            float3 noise =
//            {
//                2 * r1 - 1,
//                2 * r2 - 1,
//                2 * r3 - 1
//            };
//
//            // [0~1] -> [-1~1]
//            float3 dir = (noise - 0.5f) * 2.f;
//           
//            g_ParticleWritedata[threadIndex.x].vWorldDir = normalize(g_GrobalParticleInfo.fParticleDirection);
//            g_ParticleWritedata[threadIndex.x].vWorldPos = (noise.xyz - 0.5f) * g_GrobalParticleInfo.fParticleThickness;
//            g_ParticleWritedata[threadIndex.x].fLifeTime = ((g_GrobalParticleInfo.fMaxLifeTime - g_GrobalParticleInfo.fMinLifeTime) * noise.x)
//            + g_GrobalParticleInfo.fMinLifeTime;
//            g_ParticleWritedata[threadIndex.x].fCurTime = 0.f;
//        }
//    }
//    else
//    {
//        float rotationAngle = 10 * g_GrobalParticleInfo.fDeltaTime;
//
//        // 주어진 축 주위로 파티클을 회전시킵니다.
//        RotateAroundAxis(g_ParticleWritedata[threadIndex.x].vWorldDir, rotationAngle, g_ParticleWritedata[threadIndex.x].vWorldPos);
//
//        // 회전된 이동 방향을 사용하여 파티클을 이동시킵니다.
//        float ratio = g_ParticleWritedata[threadIndex.x].fCurTime / g_ParticleWritedata[threadIndex.x].fLifeTime;
//        float speed = (g_GrobalParticleInfo.fMaxSpeed - g_GrobalParticleInfo.fMinSpeed) * ratio + g_GrobalParticleInfo.fMinSpeed;
//        g_ParticleWritedata[threadIndex.x].vWorldPos += g_ParticleWritedata[threadIndex.x].vWorldDir * 5 * g_GrobalParticleInfo.fDeltaTime;
//
//        // 파티클의 수명이 다 되었을 경우 파티클을 제거합니다.
//        if (g_ParticleWritedata[threadIndex.x].fLifeTime < g_ParticleWritedata[threadIndex.x].fCurTime)
//        {
//            g_ParticleWritedata[threadIndex.x].iAlive = 0;
//            return;
//        }
//    }
//   
//}


#endif // _COMPUTE2DPARTICLE_