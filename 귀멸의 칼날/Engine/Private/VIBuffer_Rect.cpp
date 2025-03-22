#include "..\Public\VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Rect::NativeConstruct_Prototype()
{
	// Need To Data
	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
#pragma region Vertex_Buffer
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumBuffers = 1;
	// Buffer
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.StructureByteStride = m_iStride;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.MiscFlags = 0;
	// Vertex
	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);
	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.0f, 0.f);
	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.0f, 1.f);
	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion Vertex_Buffer

#pragma region Index_Buffer
	m_iNumPrimitive = 2;
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

	pIndices[0]._1 = 0;
	pIndices[0]._2 = 1;
	pIndices[0]._3 = 2;

	pIndices[1]._1 = 0;
	pIndices[1]._2 = 2;
	pIndices[1]._3 = 3;

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

#pragma endregion Index_Buffer

	m_pIndices = (void*)pIndices;
	return S_OK;
}

HRESULT CVIBuffer_Rect::NativeConstruct_Clone(void* _pArg)
{
	return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("CVIBuffer_Rect::Create To Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* _pArg)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("CVIBuffer_Rect::Clone To Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
