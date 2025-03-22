#include "VIBuffer_Trail.h"

#include "GameInstance.h"
#include "Method.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}
CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& _rhs)
	: CVIBuffer(_rhs)
{}

// Change Trail Buffer Desc
void CVIBuffer_Trail::Change_TrailBufferDesc(TRAILBUFFERDESC*& _pModify){
	_pModify = &m_tTrailBufferDesc;
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct_Clone(void* _pArg)
{
	if (Is_Nullptr(_pArg))
		return E_FAIL;
	
 	TRAILBUFFERSAVES Value = *static_cast<TRAILBUFFERSAVES*>(_pArg);
	m_tTrailBufferDesc = Value.tValue;
	m_iNumVertices = Value.iVerticesNum;

	if(Is_Nullptr(m_tTrailBufferDesc.pStartMatrix))
		return E_FAIL;

	if (Is_Nullptr(m_tTrailBufferDesc.pEndMatrix))
		return E_FAIL;


	if (m_iNumVertices % 2 == 1)
		++m_iNumVertices;

#pragma region			VERTEX_BUFFER
	{
		m_iStride = sizeof(VTXTEX);
		m_iNumBuffers = 1;

		D3D11_BUFFER_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(D3D11_BUFFER_DESC));
		tDesc.ByteWidth = m_iStride * m_iNumVertices;
		tDesc.StructureByteStride = m_iStride;
		tDesc.Usage = D3D11_USAGE_DYNAMIC;
		tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tDesc.MiscFlags = 0;

		VTXTEX* pVertices = new VTXTEX[m_iNumVertices];

		for (_uint i = 0; i < m_iNumVertices; i += 2)
		{
			_float3 vStartPos;
			XMStoreFloat3(&vStartPos, XMLoadFloat4x4(m_tTrailBufferDesc.pStartMatrix).r[3]);
			_float3 vEndPos;
			XMStoreFloat3(&vEndPos, XMLoadFloat4x4(m_tTrailBufferDesc.pEndMatrix).r[3]);

			pVertices[i].vPosition = vStartPos;
			pVertices[i + 1].vPosition = vEndPos;

			pVertices[i].vTexUV = _float2(0.f, 0.f);
			pVertices[i + 1].vTexUV = _float2(0.f, 1.f);
		}
		D3D11_SUBRESOURCE_DATA tSubResourceData;
		ZeroMemory(&tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tSubResourceData.pSysMem = pVertices;

		if (FAILED(Create_VertexBuffer(tDesc, tSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}
#pragma endregion	VERTEX_BUFFER

#pragma region INDEX_BUFFER
	{
		m_iBytePerPrimitive = sizeof(FACEINDICES16);
		m_iNumPrimitive = m_iNumVertices - 2;
		m_iNumIndicesPerPrimitive = 3;
		m_eFormat = DXGI_FORMAT_R16_UINT;
		m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		D3D11_BUFFER_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(D3D11_BUFFER_DESC));
		tDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
		tDesc.StructureByteStride = 0;
		tDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = 0;

		FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];

		for (_uint i = 0; i < m_iNumPrimitive; i += 2)
		{
			pIndices[i]._1 = i + 3;
			pIndices[i]._2 = i + 1;
			pIndices[i]._3 = i;

			pIndices[i + 1]._1 = (i) + 2;
			pIndices[i + 1]._2 = (i) + 3;
			pIndices[i + 1]._3 = (i);
		}

		D3D11_SUBRESOURCE_DATA tSubResourceData;
		ZeroMemory(&tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		tSubResourceData.pSysMem = pIndices;

		if (FAILED(Create_IndexBuffer(tDesc, tSubResourceData)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
	}
#pragma endregion INDEX_BUFFER
	return S_OK;
}

HRESULT CVIBuffer_Trail::Update(const _double& _dTimeDelta, const _float4x4& _WorldMatrix, const _bool& _bUpdate, 
	const _bool& _bShow)
{
	if (false == _bShow)
	{
		m_bStart = true;
		return E_FAIL;
	}

	_matrix BoneMatrixStart = XMLoadFloat4x4(m_tTrailBufferDesc.pStartMatrix);
	_matrix BoneMatrixEnd = XMLoadFloat4x4(m_tTrailBufferDesc.pEndMatrix);
	_matrix BonePivot = CMethod::XMMatrixLoadFloat4x4_P(m_tTrailBufferDesc.PivotMatrix);
	_matrix WorldMatrix = CMethod::XMMatrixLoadFloat4x4_P(_WorldMatrix);

	BoneMatrixStart.r[0] = XMVector3Normalize(BoneMatrixStart.r[0]);
	BoneMatrixStart.r[1] = XMVector3Normalize(BoneMatrixStart.r[1]);
	BoneMatrixStart.r[2] = XMVector3Normalize(BoneMatrixStart.r[2]);
	BoneMatrixEnd.r[0] = XMVector3Normalize(BoneMatrixEnd.r[0]);
	BoneMatrixEnd.r[1] = XMVector3Normalize(BoneMatrixEnd.r[1]);
	BoneMatrixEnd.r[2] = XMVector3Normalize(BoneMatrixEnd.r[2]);

	BoneMatrixStart = CMethod::XMMatrixMultiple_P(MATRIXES{ BoneMatrixStart, BonePivot, WorldMatrix });
	BoneMatrixEnd = CMethod::XMMatrixMultiple_P(MATRIXES{ BoneMatrixEnd, BonePivot, WorldMatrix });

	D3D11_MAPPED_SUBRESOURCE SubResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (nullptr == SubResource.pData)
	{
		m_pContext->Unmap(m_pVB, 0);
		return E_FAIL;
	}

	if (m_bStart == false)
	{
		m_bStart = true;
		for (_uint i = 0; i < m_iNumVertices; i += 2)
		{
			_vector vPosition;
			vPosition = BoneMatrixStart.r[3];
			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i].vPosition, vPosition);
			vPosition = BoneMatrixEnd.r[3];
			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i + 1].vPosition, vPosition);
		}
		m_iVertexIndex += 2;
	}

	if (m_iNumVertices  <= m_iVertexIndex - 2)
	{
		_uint iRemoveCount = m_iVertexIndex / 8;
		m_iVertexIndex -= iRemoveCount;
		for (_uint i = 0; i < m_iVertexIndex; i += 2)
		{
			((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
			((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
		}
	}

	_int iEndIndex = m_iCatmulRomMax * 2 + m_iVertexIndex;
	m_iCatmulRomArr[2] = iEndIndex - 2;
	m_iCatmulRomArr[3] = iEndIndex;

	((VTXTEX*)SubResource.pData)[iEndIndex - 2].vPosition = ((VTXTEX*)SubResource.pData)[m_iVertexIndex - 2].vPosition;
	((VTXTEX*)SubResource.pData)[iEndIndex - 1].vPosition = ((VTXTEX*)SubResource.pData)[m_iVertexIndex - 1].vPosition;

	_vector vPosition = BoneMatrixStart.r[3];
	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[iEndIndex].vPosition, vPosition);

	vPosition = BoneMatrixEnd.r[3];
	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[iEndIndex + 1].vPosition, vPosition);

	for (_uint i = 0; i < m_iCatmulRomMax; ++i)
	{
		_uint index = i * 2 + m_iVertexIndex - 2;
		_float fWeight = _float(i + 1) / (m_iCatmulRomMax + 1);

		_vector Pos = XMVectorCatmullRom(XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[0]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[1]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[2]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[3]].vPosition),
			fWeight);
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[index].vPosition, Pos);

		Pos = XMVectorCatmullRom(XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[0] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[1] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[2] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmulRomArr[3] + 1].vPosition),
			fWeight);
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[index + 1].vPosition, Pos);
	}
	m_iVertexIndex = iEndIndex + 2;

	for (_uint i = 0; i < m_iVertexIndex; i += 2)
	{
		XMStoreFloat2(&((VTXTEX*)SubResource.pData)[i].vTexUV, XMVectorSet((_float)i / ((_float)m_iVertexIndex), 1.f, 0.f, 0.f));
		XMStoreFloat2(&((VTXTEX*)SubResource.pData)[i + 1].vTexUV, XMVectorSet((_float)i / ((_float)m_iVertexIndex), 0.f, 0.f, 0.f));
	}

	m_iCatmulRomArr[0] = m_iCatmulRomArr[1];
	m_iCatmulRomArr[1] = m_iCatmulRomArr[2];

	m_pContext->Unmap(m_pVB, 0);
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CVIBuffer_Trail::Create To Failed");
	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* _pArg)    
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);
	CHECK_CREATE_CLONE(pInstance, _pArg, "CVIBuffer_Trail::Clone To Failed");
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
