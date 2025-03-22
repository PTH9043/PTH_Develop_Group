#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Cube::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	// Need To Data
	D3D11_BUFFER_DESC tVertexBufferDesc;
	D3D11_SUBRESOURCE_DATA tVertexBufferSubResourceData;
#pragma region Vertex_Buffer
	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;
	m_iNumBuffers = 1;
	// Buffer
	ZeroMemory(&tVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVertexBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVertexBufferDesc.StructureByteStride = m_iStride;
	tVertexBufferDesc.CPUAccessFlags = 0;
	tVertexBufferDesc.MiscFlags = 0;
	// Vertex
	VTXCUBETEX* pVertices = new VTXCUBETEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBETEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pVerticesPos[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vUvPosition = pVertices[0].vPosition;

	m_pVerticesPos[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vUvPosition = pVertices[1].vPosition;

	m_pVerticesPos[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vUvPosition = pVertices[2].vPosition;

	m_pVerticesPos[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vUvPosition = pVertices[3].vPosition;

	m_pVerticesPos[4] = pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vUvPosition = pVertices[4].vPosition;

	m_pVerticesPos[5] = pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vUvPosition = pVertices[5].vPosition;

	m_pVerticesPos[6] = pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vUvPosition = pVertices[6].vPosition;

	m_pVerticesPos[7] = pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vUvPosition = pVertices[7].vPosition;

	ZeroMemory(&tVertexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVertexBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer(tVertexBufferDesc, tVertexBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion Vertex_Buffer

	// Need To Data
	D3D11_BUFFER_DESC tIndexBufferDesc;
	D3D11_SUBRESOURCE_DATA tIndexBufferSubResourceData;
#pragma region Index_Buffer
	m_iNumPrimitive = 12;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES16);
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&tIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tIndexBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tIndexBufferDesc.StructureByteStride = 0;
	tIndexBufferDesc.CPUAccessFlags = 0;
	tIndexBufferDesc.MiscFlags = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

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

	ZeroMemory(&tIndexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tIndexBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer(tIndexBufferDesc, tIndexBufferSubResourceData)))
		return E_FAIL;

#pragma endregion Index_Buffer

	m_pIndices = (void*)pIndices;
	return S_OK;
}

HRESULT CVIBuffer_Cube::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_Cube::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Cube::Create To Failed");
	}
	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* _pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Cube::Clone To Failed");
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
