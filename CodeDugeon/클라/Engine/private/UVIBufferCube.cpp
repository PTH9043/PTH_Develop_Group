#include "EngineDefine.h"
#include "UVIBufferCube.h"


UVIBufferCube::UVIBufferCube(CSHPTRREF<UDevice> _spDevice, const VIBUFFERTYPE& _eCubeBufferType)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_3D, VIINSTANCE_TYPE::SINGLE, _eCubeBufferType)
{
}

UVIBufferCube::UVIBufferCube(const UVIBufferCube& _rhs) :
	UVIBuffer(_rhs)
{
}

void UVIBufferCube::Free()
{
}

HRESULT UVIBufferCube::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);

	const _uint VERTEX_CNT{ 8 };
	// Vertex
	{
		if (VIBUFFERTYPE::GENERIC == GetBufferType())
		{
			VECTOR<VTXCUBE>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);
			{
				VertexPos[0] = Vertecies[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
				Vertecies[0].vTexUV = _float2(0.0f, 1.0f);

				VertexPos[1] = Vertecies[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
				Vertecies[1].vTexUV = _float2(1.0f, 1.0f);

				VertexPos[2] = Vertecies[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
				Vertecies[2].vTexUV = _float2(1.0f, 0.0f);

				VertexPos[3] = Vertecies[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
				Vertecies[3].vTexUV = _float2(0.0f, 0.0f);

				VertexPos[4] = Vertecies[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
				Vertecies[4].vTexUV = _float2(0.0f, 1.0f);

				VertexPos[5] = Vertecies[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
				Vertecies[5].vTexUV = _float2(1.0f, 1.0f);

				VertexPos[6] = Vertecies[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
				Vertecies[6].vTexUV = _float2(1.0f, 0.0f);

				VertexPos[7] = Vertecies[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
				Vertecies[7].vTexUV = _float2(0.0f, 0.0f);
			}

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXCUBE), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
		else
		{
			VECTOR<VTXNORCUBE>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);
			VertexPos[0] = Vertecies[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
			Vertecies[0].vTexUV = _float2(0.0f, 1.0f);

			VertexPos[1] = Vertecies[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
			Vertecies[1].vTexUV = _float2(1.0f, 1.0f);

			VertexPos[2] = Vertecies[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
			Vertecies[2].vTexUV = _float2(1.0f, 0.0f);

			VertexPos[3] = Vertecies[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
			Vertecies[3].vTexUV = _float2(0.0f, 0.0f);

			VertexPos[4] = Vertecies[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
			Vertecies[4].vTexUV = _float2(0.0f, 1.0f);

			VertexPos[5] = Vertecies[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
			Vertecies[5].vTexUV = _float2(1.0f, 1.0f);

			VertexPos[6] = Vertecies[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
			Vertecies[6].vTexUV = _float2(1.0f, 0.0f);

			VertexPos[7] = Vertecies[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
			Vertecies[7].vTexUV = _float2(0.0f, 0.0f);

			for (_uint i = 0; i < 8; ++i)
			{
				Vertecies[i].vNormal = _float3{ 1.f, 1.f, 1.f };
			}

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXNORCUBE), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	// IndexBuffer
	{
		const _uint INDICES_CNT = 12;
		INDICIES16* pIndices = Make::AllocBuffer<INDICIES16>(INDICES_CNT);
		ZeroMemory(pIndices, sizeof(INDICIES16) * INDICES_CNT);
		// 인덱스 데이터 초기화
	/* +x */
		pIndices[0]._1 = 1; pIndices[0]._2 = 5; pIndices[0]._3 = 6;
		pIndices[1]._1 = 1; pIndices[1]._2 = 6; pIndices[1]._3 = 2;
		/* -x */
		pIndices[2]._1 = 4; pIndices[2]._2 = 0; pIndices[2]._3 = 3;
		pIndices[3]._1 = 4; pIndices[3]._2 = 3; pIndices[3]._3 = 7;
		/* +y */
		pIndices[4]._1 = 4; pIndices[4]._2 = 5; pIndices[4]._3 = 1;
		pIndices[5]._1 = 4; pIndices[5]._2 = 1; pIndices[5]._3 = 0;
		/* -y */
		pIndices[6]._1 = 3; pIndices[6]._2 = 2; pIndices[6]._3 = 6;
		pIndices[7]._1 = 3; pIndices[7]._2 = 6; pIndices[7]._3 = 7;
		/* +z */
		pIndices[8]._1 = 5; pIndices[8]._2 = 4; pIndices[8]._3 = 7;
		pIndices[9]._1 = 5; pIndices[9]._2 = 7; pIndices[9]._3 = 6;
		/* -z */
		pIndices[10]._1 = 0; pIndices[10]._2 = 1; pIndices[10]._3 = 2;
		pIndices[11]._1 = 0; pIndices[11]._2 = 2; pIndices[11]._3 = 3;

		RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
			sizeof(INDICIES16), pIndices, DXGI_FORMAT_R16_UINT), E_FAIL);
	}
	return S_OK;
}

HRESULT UVIBufferCube::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}
