#include "EngineDefine.h"
#include "UVIBufferGrid.h"

UVIBufferGrid::UVIBufferGrid(CSHPTRREF<UDevice> _spDevice, const VIBUFFERTYPE _eBufferType)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_2D, VIINSTANCE_TYPE::SINGLE, _eBufferType)
{
}

UVIBufferGrid::UVIBufferGrid(const UVIBufferGrid& _rhs) :
	UVIBuffer(_rhs)
{
}

void UVIBufferGrid::Free()
{
}

HRESULT UVIBufferGrid::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	// Vertex 
	{
		// Vertex 개수
		const _uint VERTEX_CNT = GRID_SIZE * GRID_SIZE;
		if (VIBUFFERTYPE::GENERIC == GetBufferType())
		{
			VECTOR<VTXDEFAULT>	Vertices;
			Vertices.resize(VERTEX_CNT);

			POSVECTOR VertexPos;

			VertexPos.resize(VERTEX_CNT);
			for (int i = 0, z = 0; z < GRID_SIZE; z++)
				for (int x = 0; x < (GRID_SIZE); x++, i++)
					Vertices[i] = VTXDEFAULT{ _float3(static_cast<_float>(x), 0, static_cast<_float>(z)), _float2(0.f, 0.f) };

			for (_uint i = 0; i < VERTEX_CNT; ++i)
				VertexPos[i] = Vertices[i].vPosition;

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertices.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
		else
		{
			VECTOR<VTXNORMAL>	Vertices;
			Vertices.resize(VERTEX_CNT);

			POSVECTOR VertexPos;

			VertexPos.resize(VERTEX_CNT);
			for (int i = 0, z = 0; z < GRID_SIZE; z++)
				for (int x = 0; x < (GRID_SIZE); x++, i++)
					Vertices[i] = VTXNORMAL{ _float3(static_cast<_float>(x), 0, static_cast<_float>(z)), _float3{0.f, 1.f, 0.f}, _float2(0.f, 0.f) };

			for (_uint i = 0; i < VERTEX_CNT; ++i)
				VertexPos[i] = Vertices[i].vPosition;

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertices.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	// Index
	{
		// Index 개수
		const _uint INDICES_CNT = (GRID_SIZE - 1) * (GRID_SIZE - 1) * 6; // 각 정점마다 6개의 인덱스가 필요함 (2개의 삼각형을 이룸)

		// 목록
		INDICIES32* pIndices = Make::AllocBuffer<INDICIES32>(INDICES_CNT);
		ZeroMemory(pIndices, sizeof(INDICIES32) * INDICES_CNT);

		for (int z = 0, index = 0; z < GRID_SIZE - 1; ++z) {
			for (int x = 0; x < GRID_SIZE - 1; ++x) {
				// 현재 정점의 인덱스
				_uint currentIndex = z * GRID_SIZE + x;

				// 첫 번째 삼각형
				pIndices[index++] = { currentIndex, currentIndex + 1, currentIndex + GRID_SIZE };
				// 두 번째 삼각형
				pIndices[index++] = { currentIndex + 1, currentIndex + GRID_SIZE + 1, currentIndex + GRID_SIZE };
			}
		}

		RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
			sizeof(INDICIES32), pIndices, DXGI_FORMAT_R32_UINT), E_FAIL);

	}
	return S_OK;
}

HRESULT UVIBufferGrid::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	return __super::NativeConstructClone(_vecDatas);
}
