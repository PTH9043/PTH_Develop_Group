#include "CVIBuffer_Point_Instance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& _rhs) 
    : CVIBuffer(_rhs)
    , m_iNumInstance(_rhs.m_iNumInstance)
    , m_iStrideInstance(_rhs.m_iStrideInstance)
    , m_pSpeeds(_rhs.m_pSpeeds)
{
}

HRESULT CVIBuffer_Point_Instance::NativeConstruct_Prototype(const _uint& _iNumInstance)
{
	m_iNumInstance = _iNumInstance;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumBuffers = 2;

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

		VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

		pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
		pVertices[0].vPSize = _float2(0.3f, 0.3f);
		D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
		ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tBufferSubResourceData.pSysMem = pVertices;

		if (FAILED(__super::Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}

#pragma endregion

	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicesPerPrimitive = 1;
	m_iBytePerPrimitive = sizeof(_ushort);
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region INDEX_BUFFER
	{
		D3D11_BUFFER_DESC tBufferDesc;
		ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
		tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
		tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tBufferDesc.StructureByteStride = 0;
		tBufferDesc.CPUAccessFlags = 0;
		tBufferDesc.MiscFlags = 0;

		_ushort* pIndices = new _ushort[m_iNumPrimitive];
		ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);
		D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
		ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tBufferSubResourceData.pSysMem = pIndices;

		if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
	}
#pragma endregion

	m_pSpeeds = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeeds[i] = rand() % 5 + 1.f;
	}
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::NativeConstruct_Clone(void* pArg)
{
#pragma region INSTANCE_BUFFER

	m_bCloned = true;
	m_iStrideInstance = sizeof(VTXRECTINSTANCE);
	D3D11_BUFFER_DESC tBuffDesc;
	ZeroMemory(&tBuffDesc, sizeof(D3D11_BUFFER_DESC));
	tBuffDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	tBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBuffDesc.StructureByteStride = m_iStrideInstance;
	tBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBuffDesc.MiscFlags = 0;

	VTXRECTINSTANCE* pInstanceVertices = new VTXRECTINSTANCE[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXRECTINSTANCE) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(0.3f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 0.3f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 0.3f, 0.f);
		pInstanceVertices[i].vTranslation = _float4((_float)(rand() % 2), 0.f, (_float)(rand() % 2), 1.f);
	}
	D3D11_SUBRESOURCE_DATA tBuffSubResourceData;
	ZeroMemory(&tBuffSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBuffSubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&tBuffDesc, &tBuffSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {m_pVB,	m_pVBInstance};
	_uint				iStrides[] = {	m_iStride, m_iStrideInstance};
	_uint				iOffset[] = { 0,	0 };
	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(1, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CVIBuffer_Point_Instance::Update(const _double& _dTimeDelta)
{
	if (nullptr == m_pContext)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeeds[i] * (_float)(_dTimeDelta);

		if (((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y > 5.f)
			((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _uint& _iNumInstance)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(_iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_bCloned)
		Safe_Delete_Array(m_pSpeeds);

	Safe_Release(m_pVBInstance);
}
