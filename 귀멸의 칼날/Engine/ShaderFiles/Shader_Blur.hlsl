Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);

cbuffer cb0
{
	int2 Res : packoffset(c0);
	float Blur : packoffset(c0.z);
}

void DownScale4x4(uint2 CurPixel, uint groupThreadId)
{
	if (CurPixel.y < (uint)Res.y)
	{
		int3 nFullResPos = int3(CurPixel * 4, 0);
		float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);
		[unroll]
		for (int i = 0; i < 4; i++)
		{
			[unroll]
			for (int j = 0; j < 4; j++)
			{
				downScaled += Input.Load(nFullResPos, int2(j, i));
			}
		}
		downScaled /= 16.0;
		Output[CurPixel.xy] = downScaled;
	}
}

void UpScale4x4(uint2 CurPixel, uint groupThreadId)
{
	if (CurPixel.y < (uint)Res.y)
	{
		int3 nScaleResPos = int3(CurPixel / 4, 0);
		float4 upScaled = Input.Load(nScaleResPos, int2(0, 0));

		[unroll]
		for (int i = 0; i < 4; i++)
		{
			[unroll]
			for (int j = 0; j < 4; j++)
			{
				uint2 pixel = CurPixel + uint2(j, i);
				Output[pixel] = upScaled;
			}
		}
	}
}

// SV_GroupID : 그룹ID (0 ~ Gx-1, 0 ~ Gy-1, 0 ~ Gz-1) 
// SV_GroupThreadID : 그룹 내에 고유 ID (0 ~ X-1, 0 ~ Y-1, 0 ~ Z-1)
// SV_GroupIndex : 그룹 스레드ID의 1차원 ID 
// SV_DispatchThreadID = SV_GroupID.xyz * ThreadGroupSize.xyz + SV_GroupThreadID.xyz;
[numthreads(1024, 1, 1)]
void CS_MAIN_DOWNSCALE4X4(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadID.x % Res.x, dispatchThreadID.x / Res.x);
	DownScale4x4(CurPixel, groupThreadID.x);
}

[numthreads(1024, 1, 1)]
void CS_MAIN_UPSCALE4X4(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadID.x % Res.x, dispatchThreadID.x / Res.x);
	UpScale4x4(CurPixel, groupThreadID.x);
}

static const float SampleWeights[13] = {
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

#define kernelhalf 4
#define groupthreads 128
groupshared float4 SharedInput[groupthreads];

[numthreads(groupthreads, 1, 1)]
void CS_MAIN_VERTICALBLUR(uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	int2 coord = int2(Gid.x, GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.y);
	coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[GI] = Input.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (GI >= kernelhalf && GI < (groupthreads - kernelhalf) &&
		((GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.y) < (uint)Res.y))
	{
		float4 vOut = 0;
		float vSumWeight = 0.f;
		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i)
		{
			vOut += SharedInput[GI + i * 2] * SampleWeights[i + kernelhalf];
			vSumWeight += SampleWeights[i + kernelhalf];
		}
		//vSumWeight *= 0.5f;
		Output[coord] = vOut / vSumWeight; //float4(vOut.rgb, 1.0f);
	}
}

[numthreads(groupthreads, 1, 1)]
void CS_MAIN_HORIZONBLUR(uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	int2 coord = int2(GI - kernelhalf + (groupthreads - kernelhalf * 2) * Gid.x, Gid.y);
	coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[GI] = Input.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (GI >= kernelhalf && GI < (groupthreads - kernelhalf) &&
		((Gid.x * (groupthreads - 2 * kernelhalf) + GI - kernelhalf) < (uint)Res.x))
	{
		float4 vOut = 0;
		float vSumWeight = 0.f;
		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i)
		{
			vOut += SharedInput[GI + i * 2] * SampleWeights[i + kernelhalf];
			vSumWeight += SampleWeights[i + kernelhalf];
		}
		//vSumWeight *= 0.5f;
		Output[coord] = vOut / vSumWeight; //float4(vOut.rgb, 1.0f);
	}
}


//=========================================================================================



[numthreads(groupthreads, 1, 1)]
void CS_MAIN_VERTICALBLUR1(uint3 Gid : SV_GroupID, uint3 DispatchID : SV_DispatchThreadID, uint GI : SV_GroupIndex)
{
	float4 vOut = 0;
	float vSumWeight = 0.f;
	uint2 CurPixel = uint2(DispatchID.x % Res.x, DispatchID.x / Res.x);
	[unroll]
	for (int i = -kernelhalf; i <= kernelhalf; ++i)
	{
		if (0.f == Blur)
		{
			float4 vColor = Input.Load(int3(CurPixel, 0), int2(0, i)) * SampleWeights[i + kernelhalf];
			if (!(vColor.r == 0.f && vColor.g == 0.f && vColor.b == 0.f))
			{
				vOut += vColor;
				vSumWeight += SampleWeights[i + kernelhalf];
			}
		}
		else
		{
			if (CurPixel.y + i >= 0.f && CurPixel.y + i <= Res.y)
			{
				vOut += Input.Load(int3(CurPixel, 0), int2(0, i));
				vSumWeight += Blur;
			}
		}
	}
	//vSumWeight *= 0.5f;
	Output[CurPixel] = vOut / vSumWeight;
}

[numthreads(groupthreads, 1, 1)]
void CS_MAIN_HORIZONBLUR1(uint3 Gid : SV_GroupID, uint3 DispatchID : SV_DispatchThreadID, uint GI : SV_GroupIndex)
{
	float4 vOut = 0;
	float vSumWeight = 0.f;
	uint2 CurPixel = uint2(DispatchID.x % Res.x, DispatchID.x / Res.x);
	[unroll]
	for (int i = -kernelhalf; i <= kernelhalf; ++i)
	{
		if (0.f == Blur)
		{
			float4 vColor = Input.Load(int3(CurPixel, 0), int2(i, 0)) * SampleWeights[i + kernelhalf];
			if (!(vColor.r == 0.f && vColor.g == 0.f && vColor.b == 0.f))
			{
				vOut += vColor;
				vSumWeight += SampleWeights[i + kernelhalf];
			}
		}
		else
		{
			if (CurPixel.x + i >= 0.f && CurPixel.x + i <= Res.x)
			{
				vOut += Input.Load(int3(CurPixel, 0), int2(i, 0));
				vSumWeight += Blur;
			}
		}
	}
	//vSumWeight *= 0.5f;
	Output[CurPixel] = vOut / vSumWeight;
}

//===========================================================================================

[numthreads(16, 16, 1)]
void CS_MAIN(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint i = groupID.x + groupID.y;
	if (i % 2 == 0)
		Output[dispatchThreadID.xy] = vector(0.f, 0.f, 1.f, 1.f);
	else
		Output[dispatchThreadID.xy] = vector(1.f, 0.f, 0.f, 1.f);
}

struct BONE
{
	float4 v1;
	float4 v2;
	float4 v3;
	float4 v4;
};

StructuredBuffer<BONE> gInput;
RWStructuredBuffer<BONE> gOutput;

[numthreads(32,1,1)]
void CS_TEST(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	gOutput[dispatchThreadID.x].v1 = gInput[dispatchThreadID.x].v1 * dispatchThreadID.x;
	gOutput[dispatchThreadID.x].v2 = gInput[dispatchThreadID.x].v2 * dispatchThreadID.x;
	gOutput[dispatchThreadID.x].v3 = gInput[dispatchThreadID.x].v3 * dispatchThreadID.x;
	gOutput[dispatchThreadID.x].v4 = gInput[dispatchThreadID.x].v4 * dispatchThreadID.x;
}


technique11 DefaultTechnique
{
	pass Vertical//0
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_VERTICALBLUR();
	}

	pass Horizon//1
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_HORIZONBLUR();
	}

	pass DownScale//2
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_DOWNSCALE4X4();
	}

	pass UpScale//3
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_UPSCALE4X4();
	}

	pass Vertical1//4
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_VERTICALBLUR1();
	}

	pass Horizon1//5
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MAIN_HORIZONBLUR1();
	}
}