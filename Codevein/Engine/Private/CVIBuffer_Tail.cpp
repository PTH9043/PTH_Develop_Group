#include "CVIBuffer_Tail.h"

CVIBuffer_Tail::CVIBuffer_Tail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Tail::CVIBuffer_Tail(const CVIBuffer_Tail& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Tail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	if (FAILED(Create_Index_Tail()))
		return E_FAIL;

	m_iRenderVertexCount = 10;
	return S_OK;
}

HRESULT CVIBuffer_Tail::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(Create_Vertex_Tail()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		BUFFERTAILDESC tDesc = *static_cast<BUFFERTAILDESC*>(_pArg);
		m_tShowTimer = CUSTIMER(tDesc.fShowTime);
	}

	return S_OK;
}

void CVIBuffer_Tail::Tail_Update(const _double& _dTimeDelta, const _uint& _iMatrixCount, _matrix* _vecMatrix)
{
	if (m_tShowTimer.Is_Over(_dTimeDelta))
	{
		


		m_tShowTimer.Reset_Timer();
	}
}

void CVIBuffer_Tail::Reset_Value()
{
	m_tShowTimer.Reset_Timer();
}

HRESULT CVIBuffer_Tail::Create_Vertex_Tail()
{
	// Need To Data
	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
#pragma region Vertex_Buffer
	m_iStride = sizeof(VTXTAIL);
	m_iNumVertices = 12;
	m_iNumBuffers = 1;
	// Buffer
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.StructureByteStride = m_iStride;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.MiscFlags = 0;
	// Vertex
	VTXTAIL* pVertices = new VTXTAIL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTAIL) * m_iNumVertices);

	_uint iIndex = 0;
	
	_float3 vPosArr[4] = { _float3(-0.5f, 0.5f, 0.f) ,  _float3(-0.3f, 0.5f, 0.f), _float3(-0.3f, -0.5f, 0.f) , _float3(-0.5f, -0.5f, 0.f) };
	_float2 vUVTex[4] = { _float2(0.0f, 0.f) , _float2(0.1f, 0.f) , _float2(0.1f, 1.f), _float2(0.0f, 1.f) };
	for (_uint i = 0; i < 3; ++i)
	{
		pVertices[iIndex].vPosition  = vPosArr[0];
		pVertices[iIndex].vTexUV = vUVTex[0];
		++iIndex;
		pVertices[iIndex].vPosition = vPosArr[1];
		pVertices[iIndex].vTexUV = vUVTex[1];
		++iIndex;
		pVertices[iIndex].vPosition  = vPosArr[2];
		pVertices[iIndex].vTexUV = vUVTex[2];
		++iIndex;
		pVertices[iIndex].vPosition  = vPosArr[3];
		pVertices[iIndex].vTexUV = vUVTex[3];
		++iIndex;

		for (_uint k = 0; k < 4; ++k)
		{
			pVertices[iIndex].vPosition.x += 0.4f;
			pVertices[iIndex].vTexUV.x += 0.2f;
		}
	}

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion Vertex_Buffer 
	return S_OK;
}

HRESULT CVIBuffer_Tail::Create_Index_Tail()
{
	// Need To Data
	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;

	m_iNumPrimitive = 10;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES16);
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.StructureByteStride = 0;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.MiscFlags = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);
	
	// 0 1 2, 0 2 3
	{
		pIndices[0]._1 = 0;
		pIndices[0]._2 = 1;
		pIndices[0]._3 = 2;

		pIndices[1]._1 = 0;
		pIndices[1]._2 = 2;
		pIndices[1]._3 = 3;
	}
	// 1 4 7 / 1 2 7
	{
		pIndices[2]._1 = 1;
		pIndices[2]._2 = 4;
		pIndices[2]._3 = 7;

		pIndices[3]._1 = 1;
		pIndices[3]._2 = 2;
		pIndices[3]._3 = 7;
	}
	{
		pIndices[4]._1 = 4;
		pIndices[4]._2 = 5;
		pIndices[4]._3 = 6;

		pIndices[5]._1 = 4;
		pIndices[5]._2 = 7;
		pIndices[5]._3 = 6;
	}
	{
		pIndices[6]._1 = 5;
		pIndices[6]._2 = 8;
		pIndices[6]._3 = 11;

		pIndices[7]._1 = 5;
		pIndices[7]._2 = 6;
		pIndices[7]._3 = 11;
	}
	{
		pIndices[8]._1 = 8;
		pIndices[8]._2 = 9;
		pIndices[8]._3 = 10;

		pIndices[9]._1 = 8;
		pIndices[9]._2 = 11;
		pIndices[9]._3 = 10;
	}

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	m_pIndices = (void*)pIndices;
	return S_OK;
}

CVIBuffer_Tail* CVIBuffer_Tail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_Tail* pInstance = new CVIBuffer_Tail(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Tail::Create To Failed");
	}
	return pInstance;
}

CComponent* CVIBuffer_Tail::Clone(void* _pArg)
{
	CVIBuffer_Tail* pInstance = new CVIBuffer_Tail(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Tail::Clone To Failed");
	}
	return pInstance;
}

void CVIBuffer_Tail::Free()
{
	__super::Free();
}
