#include "Shader_Defines.hpp"

texture2D g_VelocityTexture;
int g_iNumBlurValue = 50;
int g_iDivineVelocityValue = 50;
float g_PassAlpha = 0.04f;
float g_VelocityAlpha = 0.f;

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


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT		Out = (VS_OUT)0;

    vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vJudgeDraw : SV_TARGET1;
};

PS_OUT PS_MOTIONBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
     
    int iNumBlurSampler = g_iNumBlurValue;
    float4 Velocity = g_VelocityTexture.Sample(DefaultSampler, In.vTexUV);

    int iCnt = 1;

    float4 OutColor = (float4)0.f;
    float4 OUT = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
    Velocity.xy /= float(g_iDivineVelocityValue);
    for (int i = iCnt; i < iNumBlurSampler; ++i)
    {
        OutColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + Velocity.xy * (float)i);
        if (Velocity.a < OutColor.a + g_PassAlpha) {
            iCnt++;
            OUT += OutColor;
            Out.vJudgeDraw.a = 1;
            Out.vJudgeDraw.r = 1;
        } 
    }

    Out.vColor = OUT;
    Out.vColor /= ((float)iCnt);

    return Out;
}

float fShakeAmount = 0.1f;
float fShakeDepth = 100.f;
float fFadeAmount = 0.f;
float4 vFadeColor = float4(0.f, 0.f, 0.f, 0.f);

PS_OUT PS_SHACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = 0;

    float Center = 0.5;
    float2 uv = In.vTexUV - Center;

    for (int i = 0; i < 10; i++)
    {
        float scale = 1.0 + fShakeAmount * (i / 9.0) * length(uv);
        vColor += g_DiffuseTexture.Sample(ClampSampler, uv * scale + Center);
    }

    vColor /= 10;

    Out.vColor = vColor;

    Out.vColor = lerp(Out.vColor, vFadeColor, fFadeAmount);

    return Out;
}

technique11			DefaultTechnique
{
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Debug, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTIONBLUR();
    }

    pass Shake
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Debug, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHACK();
    }
}
