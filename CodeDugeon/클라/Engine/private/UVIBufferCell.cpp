#include "EngineDefine.h"
#include "UVIBufferCell.h"

UVIBufferCell::UVIBufferCell(CSHPTRREF<UDevice> _spDevice)
	:UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_2D, VIINSTANCE_TYPE::SINGLE)
{
}

UVIBufferCell::UVIBufferCell(const UVIBufferCell& _rhs)
	: UVIBuffer(_rhs)
{
}

void UVIBufferCell::Free()
{
}

HRESULT UVIBufferCell::NativeConstruct()
{
	return __super::NativeConstruct();
}


HRESULT UVIBufferCell::NativeConstruct(ARRAY<_float3, 3>& _Points)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	// Vertex 
	{
		const _uint VERTEX_CNT = 3;
		if (VIBUFFERTYPE::GENERIC == GetBufferType())
		{
			VECTOR<VTXDEFAULT>	Vertices;
			Vertices.resize(VERTEX_CNT);

			POSVECTOR VertexPos;

			VertexPos.resize(VERTEX_CNT);
			Vertices[0] = VTXDEFAULT{ _Points[0], _float2(0.f, 0.f) };
			Vertices[1] = VTXDEFAULT{ _Points[1], _float2(0.f, 0.f) };
			Vertices[2] = VTXDEFAULT{ _Points[2], _float2(0.f, 0.f) };

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
			Vertices[0] = VTXNORMAL{ _Points[0], _float3{0.f, 1.f, 0.f}, _float2(0.f, 0.f) };
			Vertices[1] = VTXNORMAL{ _Points[1], _float3{0.f, 1.f, 0.f}, _float2(0.f, 0.f) };
			Vertices[2] = VTXNORMAL{ _Points[2], _float3{0.f, 1.f, 0.f}, _float2(0.f, 0.f) };

			for (_uint i = 0; i < VERTEX_CNT; ++i)
				VertexPos[i] = Vertices[i].vPosition;

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertices.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	// Index
	{
		INDICIES16* pIndices = Make::AllocBuffer<INDICIES16>(1);
		ZeroMemory(pIndices, sizeof(INDICIES16) * 1);

		pIndices[0] = { 0, 1, 2 };

		RETURN_CHECK_FAILED(CreateIndexBuffer(1,
			sizeof(INDICIES16), pIndices, DXGI_FORMAT_R16_UINT), E_FAIL);

	}
	return S_OK;
}

HRESULT UVIBufferCell::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	return __super::NativeConstructClone(_vecDatas);
}