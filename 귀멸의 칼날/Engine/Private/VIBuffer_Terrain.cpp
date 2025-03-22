#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TERRAINDESC& _tTerrianDesc)
	: CVIBuffer(_pDevice, _pContext), m_tTerrainDesc(_tTerrianDesc)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& _rhs)
	: CVIBuffer(_rhs), m_tTerrainDesc(_rhs.m_tTerrainDesc)
{

}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_tTerrainDesc.iNumVerticiesX * m_tTerrainDesc.iNumVerticiesZ;
	m_iNumBuffers = 1;

#pragma region VERTEX_BUFFER

	D3D11_BUFFER_DESC tVerterxBuffDesc;
	ZeroMemory(&tVerterxBuffDesc, sizeof(D3D11_BUFFER_DESC));
	tVerterxBuffDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerterxBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	tVerterxBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerterxBuffDesc.StructureByteStride = m_iStride;
	tVerterxBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tVerterxBuffDesc.MiscFlags = 0;


	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, /*(pPixel[iIndex] & 0x000000ff) / 10.f*/0.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_tTerrainDesc.iNumVerticiesX - 1.0f), i / (m_tTerrainDesc.iNumVerticiesZ - 1.f));

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition;
		}
	}
	D3D11_SUBRESOURCE_DATA tVertexBufferSubResourceData;
	ZeroMemory(&tVertexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVertexBufferSubResourceData.pSysMem = pVertices;

#pragma endregion  VERTEX_BUFFER

	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
	m_iNumPrimitive = (m_tTerrainDesc.iNumVerticiesX - 1) * (m_tTerrainDesc.iNumVerticiesZ - 1) * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region INDEX_BUFFER

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.StructureByteStride = 0;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBufferDesc.MiscFlags = 0;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX - 1; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;
			_uint		iIndices[] = {
				iIndex + m_tTerrainDesc.iNumVerticiesX,
				iIndex + m_tTerrainDesc.iNumVerticiesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[1];
			pIndices[iNumFaces]._3 = iIndices[2];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[2];
			pIndices[iNumFaces]._3 = iIndices[3];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pIndices;
#pragma endregion INDEX_BUFFER

	if (FAILED(__super::Create_VertexBuffer(tVerterxBuffDesc, tVertexBufferSubResourceData)))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	m_pIndices = (void*)pIndices;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* _pMapFilePath)
{
	HANDLE			hFile = CreateFile(_pMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	_ulong			dwByte = 0;

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_tTerrainDesc.iNumVerticiesX = ih.biWidth;
	m_tTerrainDesc.iNumVerticiesZ = ih.biHeight;
	_ulong iSize = m_tTerrainDesc.iNumVerticiesX * m_tTerrainDesc.iNumVerticiesZ;
	_ulong* pPixel = new _ulong[m_tTerrainDesc.iNumVerticiesX * m_tTerrainDesc.iNumVerticiesZ];
	ReadFile(hFile, pPixel, sizeof(_ulong) * iSize, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_tTerrainDesc.iNumVerticiesX * m_tTerrainDesc.iNumVerticiesZ;
	m_iNumBuffers = 1;

#pragma region VERTEX_BUFFER

	D3D11_BUFFER_DESC tVertexBufferDesc;
	ZeroMemory(&tVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVertexBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVertexBufferDesc.StructureByteStride = m_iStride;
	tVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tVertexBufferDesc.MiscFlags = 0;


	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_tTerrainDesc.iNumVerticiesX - 1.0f), i / (m_tTerrainDesc.iNumVerticiesZ - 1.f));

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition;
		}
		_uint k = 0;
	}
	D3D11_SUBRESOURCE_DATA tVertexBufferSubResourceData;
	ZeroMemory(&tVertexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVertexBufferSubResourceData.pSysMem = pVertices;

#pragma endregion  VERTEX_BUFFER

	m_iNumPrimitive = (m_tTerrainDesc.iNumVerticiesX - 1) * (m_tTerrainDesc.iNumVerticiesZ - 1) * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region INDEX_BUFFER

	D3D11_BUFFER_DESC tIndicesBufferDesc;
	ZeroMemory(&tIndicesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tIndicesBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tIndicesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tIndicesBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tIndicesBufferDesc.StructureByteStride = 0;
	tIndicesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tIndicesBufferDesc.MiscFlags = 0;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX - 1; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;
			_uint		iIndices[] = {
				iIndex + m_tTerrainDesc.iNumVerticiesX,
				iIndex + m_tTerrainDesc.iNumVerticiesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[1];
			pIndices[iNumFaces]._3 = iIndices[2];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[2];
			pIndices[iNumFaces]._3 = iIndices[3];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	D3D11_SUBRESOURCE_DATA tIndicesBufferSubResourceData;
	ZeroMemory(&tIndicesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tIndicesBufferSubResourceData.pSysMem = pIndices;

#pragma endregion INDEX_BUFFER

	if (FAILED(__super::Create_VertexBuffer(tVertexBufferDesc, tVertexBufferSubResourceData)))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer(tIndicesBufferDesc, tIndicesBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

	m_pIndices = (void*)pIndices;
	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const TERRAINDESC& _tTerrianDesc, _float3* _pVertices)
{
	m_tTerrainDesc = _tTerrianDesc;

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_tTerrainDesc.iNumVerticiesX * m_tTerrainDesc.iNumVerticiesZ;
	m_iNumBuffers = 1;

#pragma region VERTEX_BUFFER

	D3D11_BUFFER_DESC tVerterxBuffDesc;
	ZeroMemory(&tVerterxBuffDesc, sizeof(D3D11_BUFFER_DESC));
	tVerterxBuffDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerterxBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	tVerterxBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerterxBuffDesc.StructureByteStride = m_iStride;
	tVerterxBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tVerterxBuffDesc.MiscFlags = 0;


	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	if (nullptr != _pVertices)
	{
		m_pVerticesPos = _pVertices;
		for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ; ++i)
		{
			for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX; ++j)
			{
				_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;

				pVertices[iIndex].vPosition = _pVertices[iIndex];
				pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
				pVertices[iIndex].vTexUV = _float2((_float)j / (m_tTerrainDesc.iNumVerticiesX - 1.0f), i / (m_tTerrainDesc.iNumVerticiesZ - 1.f));
			}
		}
	}
	else
	{
		m_pVerticesPos = new _float3[m_iNumVertices];
		ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
		for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ; ++i)
		{
			for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX; ++j)
			{
				_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;

				pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.0f, (_float)i);
				pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
				pVertices[iIndex].vTexUV = _float2((_float)j / (m_tTerrainDesc.iNumVerticiesX - 1.0f), i / (m_tTerrainDesc.iNumVerticiesZ - 1.f));
			}
		}
	}
	D3D11_SUBRESOURCE_DATA tVertexBufferSubResourceData;
	ZeroMemory(&tVertexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVertexBufferSubResourceData.pSysMem = pVertices;

#pragma endregion  VERTEX_BUFFER

	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
	m_iNumPrimitive = (m_tTerrainDesc.iNumVerticiesX - 1) * (m_tTerrainDesc.iNumVerticiesZ - 1) * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region INDEX_BUFFER

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.StructureByteStride = 0;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBufferDesc.MiscFlags = 0;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX - 1; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;
			_uint		iIndices[] = {
				iIndex + m_tTerrainDesc.iNumVerticiesX,
				iIndex + m_tTerrainDesc.iNumVerticiesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[1];
			pIndices[iNumFaces]._3 = iIndices[2];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;

			pIndices[iNumFaces]._1 = iIndices[0];
			pIndices[iNumFaces]._2 = iIndices[2];
			pIndices[iNumFaces]._3 = iIndices[3];

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pIndices;
#pragma endregion INDEX_BUFFER

	if (FAILED(__super::Create_VertexBuffer(tVerterxBuffDesc, tVertexBufferSubResourceData)))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	m_pIndices = pIndices;

	return S_OK;
}


HRESULT CVIBuffer_Terrain::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

void* CVIBuffer_Terrain::ReCalculate_Value(void* _pData)
{
	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(_pData);

	_uint		iNumFaces = 0;

	const FACEINDICES32* pIndices = static_cast<FACEINDICES32*>(m_pIndices);
	for (_uint i = 0; i < m_tTerrainDesc.iNumVerticiesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_tTerrainDesc.iNumVerticiesX - 1; ++j)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticiesX + j;
			_uint		iIndices[] = {
				iIndex + m_tTerrainDesc.iNumVerticiesX,
				iIndex + m_tTerrainDesc.iNumVerticiesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;

			vSour = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition)) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vDest = XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._3].vPosition)) - XMLoadFloat3(&(pVertices[pIndices[iNumFaces]._2].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal, XMLoadFloat3(&pVertices[pIndices[iNumFaces]._3].vNormal) + vNormal);
			++iNumFaces;
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	return _pData;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TERRAINDESC& _tTerrianDesc)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext, _tTerrianDesc);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Terrain::Create To Failed");
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create_MapFile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pMapFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Terrain::Create_MapFile To Failed");
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create_ToLoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const TERRAINDESC& _tTerrianDesc, _float3* _pVertices)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_tTerrianDesc, _pVertices)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Terrain::Create_ToLoadData To Failed");
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* _pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CVIBuffer_Terrain::Clone To Failed");
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
