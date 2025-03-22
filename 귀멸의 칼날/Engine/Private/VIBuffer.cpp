#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& _rhs)
	: CComponent(_rhs),
	m_pVB(_rhs.m_pVB), m_iNumBuffers(_rhs.m_iNumBuffers), m_iStride(_rhs.m_iStride), m_iNumVertices(_rhs.m_iNumVertices),
	m_pIB(_rhs.m_pIB), m_iNumPrimitive(_rhs.m_iNumPrimitive), m_iNumIndicesPerPrimitive(_rhs.m_iNumIndicesPerPrimitive),
	m_iBytePerPrimitive(_rhs.m_iBytePerPrimitive), m_eFormat(_rhs.m_eFormat), m_eTopology(_rhs.m_eTopology),
	m_pVerticesPos(_rhs.m_pVerticesPos),
	m_pIndices(_rhs.m_pIndices)
{
	Safe_AddRef_DV(m_pVB);
	Safe_AddRef_DV(m_pIB);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct_Clone(void* _pArg)
{
	m_bCloned = true;
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVB };

	_uint				iStrides[] = { m_iStride };

	_uint				iOffset[] = { 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

void* CVIBuffer::ReCalculate_Value(void* _pData)
{
	return nullptr;
}

HRESULT CVIBuffer::Create_VertexBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&_tBufferDesc, &_tBufferSubResourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&_tBufferDesc, &_tBufferSubResourceData, &m_pIB)))
		return E_FAIL;
	 
	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_bCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release_DV(m_pVB);
	Safe_Release_DV(m_pIB);
}
