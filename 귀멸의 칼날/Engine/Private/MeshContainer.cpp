#include "MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"
#include  "Method.h"

CMeshContainer::CMeshContainer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer& _rhs)
	: CVIBuffer(_rhs),
	m_iMaterialIndex(_rhs.m_iMaterialIndex), m_iNumBones(_rhs.m_iNumBones),
	m_HierarchyNodes(_rhs.m_HierarchyNodes), m_vMaxVertex(_rhs.m_vMaxVertex), m_vMinVertex(_rhs.m_vMinVertex)
{
	for (auto& iter : m_HierarchyNodes)
		Safe_AddRef(iter);
}

HRESULT CMeshContainer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel* _pModel, const MESHDESC& _tData, _fmatrix _mPivotMatrix)
{
	if (FAILED(SetUp_Vertices(_pModel, _tData, _mPivotMatrix)))
		return E_FAIL;

	m_iNumPrimitive = _tData.iNumIndices;
	m_strName = CMethod::ConvertWToS(_tData.strName);
	if (FAILED(SetUp_Indices(_tData.pIndices)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct_Prototype_Anim(CModel* _pModel, const ANIMMESHDESC& _tData, _fmatrix _mPivotMatrix)
{
	if (FAILED(SetUp_AnimVertices(_pModel, _tData, _mPivotMatrix)))
		return E_FAIL;

	m_iNumPrimitive = _tData.iNumIndices;
	m_strName = CMethod::ConvertWToS(_tData.strName);
	if (FAILED(SetUp_Indices(_tData.pIndices)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	CModel* pModel = (CModel*)_pArg;
	vector<CHierarchyNode*>		HierarchyNodes;

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		CHierarchyNode* pNewNode = pModel->Get_HierarchyNodes(pHierarchyNode->Get_Name().c_str());
		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, pHierarchyNode->Get_OffsetMatrix());
		pNewNode->Set_OffsetMatrix(OffsetMatrix);
		HierarchyNodes.push_back(pNewNode);
		Safe_AddRef(pNewNode);
		Safe_Release(pHierarchyNode);
	}
	m_HierarchyNodes.clear();
	m_HierarchyNodes = HierarchyNodes;
	return S_OK;
}

HRESULT CMeshContainer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMeshContainer::SetUp_BoneMatrices(_float4x4* _pBoneMatrices, _fmatrix _PivotMatrix)
{
	_uint			iBoneIndex = 0;

	if (m_HierarchyNodes.empty())
	{
		XMStoreFloat4x4(&_pBoneMatrices[0], XMMatrixIdentity());
		return S_OK;
	}

	for (auto& pHierarchyNodes : m_HierarchyNodes)
	{
		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixTranspose(pHierarchyNodes->Get_OffsetMatrix() *
			pHierarchyNodes->Get_CombinedTransformationMatrix() * _PivotMatrix));

		_pBoneMatrices[iBoneIndex++] = matrix;
	}
	return S_OK;
}

HRESULT CMeshContainer::SetUp_Vertices(CModel* _pModel, const MESHDESC& _tMeshDesc, _fmatrix _mPivotMatrix)
{
	m_iMaterialIndex = _tMeshDesc.iMatrialIndex;
	m_iStride = sizeof(VTXMODEL);
	m_iNumVertices = _tMeshDesc.iNumVertices;
	m_iNumBuffers = 1;

	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerticesBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStride;
	tVerticesBufferDesc.CPUAccessFlags = 0;
	tVerticesBufferDesc.MiscFlags = 0;

	VTXMODEL* pVertices = _tMeshDesc.pVtxModel;
	m_pVerticesPos = _tMeshDesc.pVerticesPos;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _mPivotMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _mPivotMatrix)));
		m_pVerticesPos[i] = pVertices[i].vPosition;

		m_vMinVertex = _float3{ min(m_vMinVertex.x, m_pVerticesPos[i].x) , min(m_vMinVertex.y, m_pVerticesPos[i].y), min(m_vMinVertex.z, m_pVerticesPos[i].z) };
		m_vMaxVertex = _float3{ max(m_vMaxVertex.x, m_pVerticesPos[i].x) , max(m_vMaxVertex.y, m_pVerticesPos[i].y), max(m_vMaxVertex.z, m_pVerticesPos[i].z) };
	}
	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer(tVerticesBufferDesc, tVerticesBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	return S_OK;
}

HRESULT CMeshContainer::SetUp_AnimVertices(CModel* _pModel, const ANIMMESHDESC& _tAnimMeshDesc, _fmatrix _mPivotMatrix)
{
	m_iMaterialIndex = _tAnimMeshDesc.iMatrialIndex;
	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVertices = _tAnimMeshDesc.iNumVertices;
	/* 현재 메시에 영향을 주는 뼈들의 갯수. */
	m_iNumBones = _tAnimMeshDesc.iNumBones;
	m_iNumBuffers = 1;
	
	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerticesBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStride;
	tVerticesBufferDesc.CPUAccessFlags = 0;
	tVerticesBufferDesc.MiscFlags = 0;

	VTXANIMMODEL* pVertices = _tAnimMeshDesc.pVtxModel;
	m_pVerticesPos = _tAnimMeshDesc.pVerticesPos;

	for (auto& iter : _tAnimMeshDesc.HierachyNameList)
	{
		CHierarchyNode* pNode = _pModel->Get_HierarchyNodes(CMethod::ConvertWToS(iter).c_str());
		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer(tVerticesBufferDesc, tVerticesBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	return S_OK;
}

HRESULT CMeshContainer::SetUp_Indices(FACEINDICES32* _pData)
{
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC tIndicesBufferDesc;
	ZeroMemory(&tIndicesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tIndicesBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
 	tIndicesBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tIndicesBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tIndicesBufferDesc.StructureByteStride = 0;
	tIndicesBufferDesc.CPUAccessFlags = 0;
	tIndicesBufferDesc.MiscFlags = 0;

	FACEINDICES32* pIndices = _pData;

	D3D11_SUBRESOURCE_DATA tIndicesBufferSubResourceData;
 	ZeroMemory(&tIndicesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tIndicesBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer(tIndicesBufferDesc, tIndicesBufferSubResourceData)))
		return E_FAIL;

	m_pIndices = (void*)_pData;

	return S_OK;
}

CMeshContainer* CMeshContainer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	CModel* _pModel, const MESHDESC& _tData, _fmatrix _mPivotMatrix)
{
	CMeshContainer* pMeshContainer = new CMeshContainer(_pDevice, _pContext);
	if (FAILED(pMeshContainer->NativeConstruct_Prototype(_pModel, _tData, _mPivotMatrix)))
	{
		Safe_Release(pMeshContainer);
		MSG_BOX("CMeshContainer::Create To Failed");
	}
	return pMeshContainer;
}

CMeshContainer* CMeshContainer::Create_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	CModel* _pModel, const ANIMMESHDESC& _tData, _fmatrix _mPivotMatrix)
{
	CMeshContainer* pMeshContainer = new CMeshContainer(_pDevice, _pContext);
	if (FAILED(pMeshContainer->NativeConstruct_Prototype_Anim(_pModel, _tData, _mPivotMatrix)))
	{
		Safe_Release(pMeshContainer);
		MSG_BOX("CMeshContainer::Create To Failed");
	}
	return pMeshContainer;
}

CComponent* CMeshContainer::Clone(void* _pArg)
{
	CMeshContainer* pMeshContainer = new CMeshContainer(*this);
	if (FAILED(pMeshContainer->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pMeshContainer);
		MSG_BOX("CMeshContainer::Clone To Failed");
	}
	return pMeshContainer;
}

void CMeshContainer::Free()
{
	for (auto& iter : m_HierarchyNodes)
		Safe_Release(iter);

	__super::Free();
}
