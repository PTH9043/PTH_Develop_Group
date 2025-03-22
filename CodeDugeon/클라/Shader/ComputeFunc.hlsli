#ifndef _COMPUTEPARAM_
#define _COMPUTEPARAM_


float Rand(float2 co)//0.5~1.0
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

float RandNormalized(float2 co)
{
    // 기존 Rand 함수의 결과
    float randValue = Rand(co);
    
    // 0.5와 1.0 사이의 값을 0과 1 사이로 변환
    return (randValue - 0.5f) / 0.5f;
}



#endif 