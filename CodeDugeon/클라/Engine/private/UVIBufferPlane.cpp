#include "EngineDefine.h"
#include "UVIBufferPlane.h"

UVIBufferPlane::UVIBufferPlane(CSHPTRREF<UDevice> _spDevice, const VIBUFFERTYPE _eBufferType)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_3D, VIINSTANCE_TYPE::SINGLE, _eBufferType)
{
}

UVIBufferPlane::UVIBufferPlane(const UVIBufferPlane& _rhs) : 
	UVIBuffer(_rhs)
{
}

void UVIBufferPlane::Free()
{
}

HRESULT UVIBufferPlane::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	// Vertex 
	{
		// Vertex 개수
		const _uint VERTEX_CNT = 4;
		if (VIBUFFERTYPE::GENERIC == GetBufferType())
		{
			VECTOR<VTXDEFAULT>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);
			{
				// 1 -X, +Y                                                                                   
				Vertecies[0] = VTXDEFAULT{ _float3(-0.5f, 0.f, 0.5f), _float2(0.f, 0.f) };
				// 2 +X, +Y
				Vertecies[1] = VTXDEFAULT{ _float3(0.5f, 0.f, 0.5f), _float2(1.f, 0.f) };
				// 3 +X, -Y
				Vertecies[2] = VTXDEFAULT{ _float3(0.5f, 0.f, -0.5f), _float2(1.f, 1.f) };
				// 4 -X, -Y
				Vertecies[3] = VTXDEFAULT{ _float3(-0.5f, 0.f, -0.5f), _float2(0.f, 1.f) };
			}
			// Position 복제
			for (_uint i = 0; i < VERTEX_CNT; ++i)
				VertexPos[i] = Vertecies[i].vPosition;

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
		else
		{
			VECTOR<VTXNORMAL>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);
			{
				// 1 -X, +Y                                                                                   
				Vertecies[0] = VTXNORMAL{ _float3(-0.5f, 0.f, 0.5f), _float3{0.f, 1.f, 0.f}, _float2(0.f, 0.f) };
				// 2 +X, +Y
				Vertecies[1] = VTXNORMAL{ _float3(0.5f, 0.f, 0.5f), _float3{0.f, 1.f, 0.f}, _float2(1.f, 0.f) };
				// 3 +X, -Y
				Vertecies[2] = VTXNORMAL{ _float3(0.5f, 0.f, -0.5f),  _float3{0.f, 1.f, 0.f},_float2(1.f, 1.f) };
				// 4 -X, -Y
				Vertecies[3] = VTXNORMAL{ _float3(-0.5f, 0.f, -0.5f),  _float3{0.f, 1.f, 0.f}, _float2(0.f, 1.f) };

			}
			// Position 복제
			for (_uint i = 0; i < VERTEX_CNT; ++i)
				VertexPos[i] = Vertecies[i].vPosition;

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXNORMAL), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	// Index
	{
		// Index 개수
		const _uint INDICES_CNT = 2;
		// 목록
		INDICIES16* pIndices = Make::AllocBuffer<INDICIES16>(INDICES_CNT);
		ZeroMemory(pIndices, sizeof(INDICIES16) * INDICES_CNT);
		{
			pIndices[0] = { 0, 1, 2 };
			pIndices[1] = { 0, 2, 3 };
		}
		RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
			sizeof(INDICIES16), pIndices, DXGI_FORMAT_R16_UINT), E_FAIL);
	}
	return S_OK;
}

HRESULT UVIBufferPlane::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	return __super::NativeConstructClone(_vecDatas);
}
