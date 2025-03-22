#include "Shader_Defines.hpp"

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
};

int MAX_STEPS = 64;
float MIN_DIST = 1.0f;
float MAX_DIST = 100.0f;
float EPSILON = 1e-6f;

float2  iResolution = float2(1280.f, 720.f);
float   iTime = 0.1f;

//https://www.shadertoy.com/view/3s3GDn
float getGlow(float dist, float radius, float intensity) {
    return pow(radius / max(dist, 1e-6), intensity);
}

//----------------------------- Camera ------------------------------

float3 rayDirection(float fieldOfView, float2 vTexUV) 
{
    float2 fragCoord = vTexUV * iResolution;
    float2 xy = fragCoord - iResolution.xy / 2.0;
    float z = (0.5 * iResolution.y) / tan(radians(fieldOfView) / 2.0);
    return normalize(float3(xy, -z));
}

//https://www.geertarien.com/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/
float3x3 lookAt(float3 camera, float3 targetDir, float3 up) {
    float3 zaxis = normalize(targetDir);
    float3 xaxis = normalize(cross(zaxis, up));
    float3 yaxis = cross(xaxis, zaxis);

    return float3x3(xaxis, yaxis, -zaxis);
}


//-------------------------- SDF and scene ---------------------------

float3 rotate(float3 p, float4 q) {
    return 2.0 * cross(q.xyz, p * q.w + cross(q.xyz, p)) + p;
}

//https://iquilezles.org/articles/distfunctions
float torusSDF(float3 p, float2 t) {
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float getSDF(float3 position) {
    float angle = iTime;
    float3 axis = normalize(float3(1.0, 1.0, 1.0));
    position = rotate(position, float4(axis * sin(-angle * 0.5), cos(-angle * 0.5)));
    return torusSDF(position, float2(1.0, 0.2));

}


//---------------------------- Raymarching ----------------------------

// Glow variable is passed in by reference using the keyword inout. The result written in this
// function can be read afterwards from where it was called.
float distanceToScene(float3 cameraPos, float3 rayDir, float start, float end, inout float glow) {

    // Start at a predefined distance from the camera in the ray direction
    float depth = start;

    // Variable that tracks the distance to the scene at the current ray endpoint
    float dist;

    // For a set number of steps
    for (int i = 0; i < MAX_STEPS; i++) {

        // Get the SDF value at the ray endpoint, giving the maximum 
        // safe distance we can travel in any direction without hitting a surface
        //dist = getSDF(cameraPos + depth * rayDir);
        dist = length(cameraPos + depth * rayDir) - 0.3;

        // Calculate the glow at the current distance using the distance based glow function
        // Accumulate this value over the whole view ray
        // The smaller the step size, the smoother the final result
        glow += getGlow(dist, 1e-3, 0.55);

        // If the distance is small enough, we have hit a surface
        // Return the depth that the ray travelled through the scene
        if (dist < EPSILON) {
            return depth;
        }

        // Else, march the ray by the sdf value
        depth += dist;

        // Test if we have left the scene
        if (depth >= end) {
            return end;
        }
    }

    // Return max value if we hit nothing but remain in the scene after max steps
    return end;
}


//----------------------- Tonemapping and render ------------------------

//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x) {
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

float4 mainImage(float2 vTexUV) {

    // Get the default direction of the ray (along the negative Z direction)
    float3 rayDir = rayDirection(60.0, vTexUV);

    //----------------- Define a camera -----------------

    float3 cameraPos = float3(2.f, 2.f, 2.f);

    float3 target = -normalize(cameraPos);
    float3 up = float3(0.0, 1.0, 0.0);

    // Get the view matrix from the camera orientation
    float3x3 viewMatrix = lookAt(cameraPos, target, up);

    //---------------------------------------------------

    // Transform the ray to point in the correct direction
    //rayDir = viewMatrix * rayDir;
    rayDir = mul(rayDir, viewMatrix);

    // Initialize glow to 0
    float glow = 0.0;

    // Find the distance to where the ray stops, pass in the glow variable to be accumulated
    float dist = distanceToScene(cameraPos, rayDir, MIN_DIST, MAX_DIST, glow);

    // Dist can now be used to render surfaces in the scene. We will only render the glow

    float3 glowColour = float3(0.2, 0.5, 1.0);
    // float3 glowColour = float3(1.0, 1.0, 1.0);

     // Glow now holds the value from the ray marching
    float3 col = glow * glowColour;

    // Tonemapping
    //= ACESFilm(col);

    // Gamma
    //l = pow(col, float3(0.4545));

    float4 fragColor = float4(col, 1.0);
    return fragColor;
}

PS_OUT PS_3DGLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = mainImage(In.vTexUV);

    return Out;
}

technique11			DefaultTechnique
{
	pass EdgeGlow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_3DGLOW();
	}
}
