#include "EngineDefine.h"
#include "UVIBufferSphere.h"

UVIBufferSphere::UVIBufferSphere(CSHPTRREF<UDevice> _spDevice, const VIBUFFERTYPE _eBufferType) :
	UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_3D, VIINSTANCE_TYPE::SINGLE, _eBufferType)
{
}

UVIBufferSphere::UVIBufferSphere(const UVIBufferSphere& _rhs) :
	UVIBuffer(_rhs)
{
}

void UVIBufferSphere::Free()
{
}

HRESULT UVIBufferSphere::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	// 구에 대한 정의 
	const _uint NUM_LOGITUDES{ 30 }; // 경도(수평) 방향 분할 수
	const _uint NUM_LATITUDES{ 20 };  // 위도(수직) 방향 분할 수
	const _float RADIUS{ 1.f };  // 구의 반지름

	if (GetBufferType() == VIBUFFERTYPE::GENERIC)
	{
		// Vertex 
		{
			// Vertex 개수
			const _uint VERTEX_CNT = (NUM_LOGITUDES + 1) * (NUM_LATITUDES + 1);
			VECTOR<VTXDEFAULT>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);

			const _float LATITUDE_DELTA{ DirectX::XM_PI / NUM_LATITUDES };
			const _float LONGTITUDE_DELTA{ 2.f * DirectX::XM_PI / NUM_LOGITUDES };
			_uint VERTEX_INDEX{ 0 };

			for (_uint i{ 0 }; i <= NUM_LATITUDES; ++i)
			{
				_float fLatitudeAngle = i * LATITUDE_DELTA;
				_float fLatitudeSin{ sin(fLatitudeAngle) }, fLatitudeCos{ cos(fLatitudeAngle) };

				for (_uint j = 0; j <= NUM_LOGITUDES; ++j)
				{
					_float fLongtidueAngle = j * LONGTITUDE_DELTA;
					_float fLongtidueSin{ sin(fLongtidueAngle) }, fLongtidueCos{ cos(fLongtidueAngle) };

					_float x = RADIUS * fLatitudeSin * fLongtidueCos;
					_float y = RADIUS * fLatitudeSin * fLongtidueSin;
					_float z = RADIUS * fLatitudeCos;

					Vertecies[VERTEX_INDEX].vPosition = _float3{ x, y, z };
					Vertecies[VERTEX_INDEX].vTexUV = DirectX::XMFLOAT2(static_cast<_float>(j) / NUM_LOGITUDES, static_cast<_float>(i) / NUM_LATITUDES);
					VertexPos[VERTEX_INDEX] = Vertecies[VERTEX_INDEX].vPosition;
					++VERTEX_INDEX;
				}
			}

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	else
	{
		// Vertex 
		{
			// Vertex 개수
			const _uint VERTEX_CNT = (NUM_LOGITUDES + 1) * (NUM_LATITUDES + 1);
			VECTOR<VTXNORMAL>	Vertecies;
			Vertecies.resize(VERTEX_CNT);

			POSVECTOR VertexPos;
			VertexPos.resize(VERTEX_CNT);

			const _float LATITUDE_DELTA{ DirectX::XM_PI / NUM_LATITUDES };
			const _float LONGTITUDE_DELTA{ 2.f * DirectX::XM_PI / NUM_LOGITUDES };
			_uint VERTEX_INDEX{ 0 };

			for (_uint i{ 0 }; i <= NUM_LATITUDES; ++i)
			{
				_float fLatitudeAngle = i * LATITUDE_DELTA;
				_float fLatitudeSin{ sin(fLatitudeAngle) }, fLatitudeCos{ cos(fLatitudeAngle) };

				for (_uint j = 0; j <= NUM_LOGITUDES; ++j)
				{
					_float fLongtidueAngle = j * LONGTITUDE_DELTA;
					_float fLongtidueSin{ sin(fLongtidueAngle) }, fLongtidueCos{ cos(fLongtidueAngle) };

					_float x = RADIUS * fLatitudeSin * fLongtidueCos;
					_float y = RADIUS * fLatitudeSin * fLongtidueSin;
					_float z = RADIUS * fLatitudeCos;
					// Position
					Vertecies[VERTEX_INDEX].vPosition = _float3{ x, y, z };
					// Normal 
					Vertecies[VERTEX_INDEX].vNormal = _float3{ x, y, z };
					Vertecies[VERTEX_INDEX].vNormal.Normalize();
					// Tex
					Vertecies[VERTEX_INDEX].vTexUV = DirectX::XMFLOAT2(static_cast<_float>(j) / NUM_LOGITUDES, static_cast<_float>(i) / NUM_LATITUDES);
					VertexPos[VERTEX_INDEX] = Vertecies[VERTEX_INDEX].vPosition;
					++VERTEX_INDEX;
				}
			}

			RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXNORMAL), Vertecies.data(),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
		}
	}
	// Index
	{
		// Index 개수
		const _uint INDICES_CNT = NUM_LATITUDES * NUM_LOGITUDES * 6;
		// 목록
		INDICIES32* pIndices = Make::AllocBuffer<INDICIES32>(INDICES_CNT);
		ZeroMemory(pIndices, sizeof(INDICIES32) * INDICES_CNT);
		{
			_uint iIndex{ 0 };
			for (_uint i{ 0 }; i < NUM_LATITUDES; ++i)
			{
				for (_uint j = 0; j < NUM_LOGITUDES; ++j)
				{
					_uint iTopLeft = static_cast<_uint>(i * (NUM_LOGITUDES + 1) + j);
					_uint iTopRight = static_cast<_uint>(iTopLeft + 1);
					_uint iBottomLeft = static_cast<_uint>((i + 1) * (NUM_LOGITUDES + 1) + j);
					_uint iBottomRight = static_cast<_uint>(iBottomLeft + 1);

					pIndices[iIndex++] = INDICIES32{ iTopLeft, iBottomLeft, iTopRight };
					pIndices[iIndex++] = INDICIES32{ iTopRight, iBottomLeft, iBottomRight };
				}
			}
		}
		RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
			sizeof(INDICIES32), pIndices, DXGI_FORMAT_R32_UINT), E_FAIL);
	}
	return S_OK;
}

HRESULT UVIBufferSphere::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	return __super::NativeConstructClone(_vecDatas);
}
