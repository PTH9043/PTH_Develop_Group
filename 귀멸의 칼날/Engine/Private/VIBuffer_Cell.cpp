#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& _rhs)
	: CVIBuffer(_rhs)

{
}

HRESULT CVIBuffer_Cell::NativeConstruct_Prototype(const _float3* _pPosition)
{
	if (nullptr == _pPosition)
		return E_FAIL;

	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = 3;
	m_iNumBuffers = 1;

#pragma region VERTEX_BUFFER
	{
		D3D11_BUFFER_DESC tBufferDesc;
		ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

		tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
		tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		tBufferDesc.StructureByteStride = m_iStride;
		tBufferDesc.CPUAccessFlags = 0;
		tBufferDesc.MiscFlags = 0;

		VTXCOL* pVertices = new VTXCOL[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXCOL) * m_iNumVertices);

		m_pVerticesPos = new _float3[m_iNumVertices];

		pVertices[0].vPosition = _pPosition[0];
		m_pVerticesPos[0] = _pPosition[0];
		pVertices[0].vColor = _float4(1.0f, 1.f, 1.0f, 1.f);

		pVertices[1].vPosition = _pPosition[1];
		m_pVerticesPos[1] = _pPosition[1];
		pVertices[1].vColor = _float4(1.0f, 1.f, 1.0f, 1.f);

		pVertices[2].vPosition = _pPosition[2];
		m_pVerticesPos[2] = _pPosition[2];
		pVertices[2].vColor = _float4(1.0f, 1.f, 1.0f, 1.f);

		D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
		ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tBufferSubResourceData.pSysMem = pVertices;

		if (FAILED(__super::Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}
#pragma endregion

#pragma region INDEX_BUFFER
	{
		m_eFormat = DXGI_FORMAT_R16_UINT;
		m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

		D3D11_BUFFER_DESC tBufferDesc;
		D3D11_SUBRESOURCE_DATA tBufferSubResourceData;

		ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
		tBufferDesc.ByteWidth = sizeof(_ushort) * 4;
		tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tBufferDesc.StructureByteStride = 0;
		tBufferDesc.CPUAccessFlags = 0;
		tBufferDesc.MiscFlags = 0;

		_ushort* pIndices = new _ushort[4];
		ZeroMemory(pIndices, sizeof(_ushort) * 4);

		pIndices[0] = 0;
		pIndices[1] = 1;
		pIndices[2] = 2;
		pIndices[3] = 0;

		ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tBufferSubResourceData.pSysMem = pIndices;

		if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
	}
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Cell::NativeConstruct_Clone(void* _pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Cell::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB
	};

	_uint				iStrides[] = {
		m_iStride
	};

	_uint				iOffset[] = {
		0
	};


	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _float3* _pPosition)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pPosition)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Cell::Create To Failed");
	}
	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* _pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Cell::Clone To Failed");
	}
	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();
}
