#include "EngineDefine.h"
#include "UVIBufferPoint.h"

UVIBufferPoint::UVIBufferPoint(CSHPTRREF<UDevice> _spDevice)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_3D, VIINSTANCE_TYPE::SINGLE, VIBUFFERTYPE::GENERIC)
{
}

UVIBufferPoint::UVIBufferPoint(const UVIBufferPoint& _rhs) :
	UVIBuffer(_rhs)
{
}

void UVIBufferPoint::Free()
{
}

HRESULT UVIBufferPoint::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);

	{
		const _uint VERTEX_CNT{ 1 };

		VECTOR<VTXPOINT>	Vertecies;
		Vertecies.resize(VERTEX_CNT);

		POSVECTOR VertexPos;
		VertexPos.resize(VERTEX_CNT);

		Vertecies[0] = VTXPOINT(_float3(0, 0, 0), _float3(0.0f, 0.0f, -1.0f), _float3(1.0f, 0.0f, 0.0f), _float2(0.5f, 0.5f));
		VertexPos[0] = Vertecies[0].vPosition; 

		RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXPOINT), Vertecies.data(),
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST, VertexPos), E_FAIL);
	}

	{
		const _uint INDICES_CNT{ 1 };
		_ushort* pIndices = Make::AllocBuffer<_ushort>(INDICES_CNT);
		pIndices[0] = 0;
		RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
			sizeof(_ushort), pIndices, DXGI_FORMAT_R16_UINT, 1), E_FAIL);
	}
	return S_OK;
}

HRESULT UVIBufferPoint::NativeConstructClone(const VOIDDATAS& _convecDatas)
{
	return __super::NativeConstructClone(_convecDatas);
}