#include "AssimpFIleLoad_ Tool.h" 
#include "MeshContainer_Tool.h"
#include "Model_Tool.h"
#include "HierarchyNode_Tool.h"


CMeshContainer_Tool::CMeshContainer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMeshContainer_Tool::CMeshContainer_Tool(const CMeshContainer_Tool& _rhs)
	: CVIBuffer(_rhs), m_iMaterialIndex(_rhs.m_iMaterialIndex)
	, m_HierarchyNodes(_rhs.m_HierarchyNodes)
	, m_iNumBones(_rhs.m_iNumBones)
	, m_strName(_rhs.m_strName)
	, m_UVTexCoords(_rhs.m_UVTexCoords)
{
	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_AddRef(pHierarchyNode);
}

_float2 CMeshContainer_Tool::Get_UVTexCoords(const _uint& _iIndex)
{
	if (m_UVTexCoords.size() <= 0)
		return _float2(0.f, 0.f);

	if (m_UVTexCoords.size() >= _iIndex)
		return m_UVTexCoords[0];

	return m_UVTexCoords[_iIndex];
}

HRESULT CMeshContainer_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMeshContainer_Tool::NativeConstruct_Prototype(aiMesh* _pAIMesh, const _uint& _iType, CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix)
{
	if (nullptr == _pAIMesh)
		return E_FAIL;

	m_iMaterialIndex = _pAIMesh->mMaterialIndex;
	m_strName = _pAIMesh->mName.data;

	if (CModel_Tool::TYPE_NONANIM == static_cast<CModel_Tool::TYPE>(_iType))
	{
		if (FAILED(SetUp_Vertices(_pAIMesh, _pModel, _mPivotMatrix)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(SetUp_AnimVertices(_pAIMesh, _pModel)))
			return E_FAIL;
	}

	if (FAILED(SetUp_Indices(_pAIMesh)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer_Tool::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	CModel_Tool* pModel = static_cast<CModel_Tool*>(_pArg);
	if (nullptr == pModel)
		return E_FAIL;

	vector<CHierarchyNode_Tool*>		HierarchyNodes;
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		CHierarchyNode_Tool* pNewNode = pModel->Get_HierarchyNodes(pHierarchyNode->Get_Name());
		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, pHierarchyNode->Get_OffsetMatrix());
		pNewNode->Set_OffsetMatrix(OffsetMatrix);
		HierarchyNodes.push_back(pNewNode);
		Safe_Release(pHierarchyNode);
	}
	m_HierarchyNodes.clear();
	m_HierarchyNodes = HierarchyNodes;
	return S_OK;
}

HRESULT CMeshContainer_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer_Tool::SetUp_BoneMatrices(_float4x4* _pBoneMatrices, const _fmatrix& _PivotMatrix)
{
	_uint			iBoneIndex = 0;

	if (m_HierarchyNodes.empty())
	{
		XMStoreFloat4x4(&_pBoneMatrices[0], XMMatrixIdentity());
		return S_OK;
	}

	for (auto& pHierarchyNodes : m_HierarchyNodes)
	{
		_float4x4 _matBone;
		XMStoreFloat4x4(&_matBone,
			XMMatrixTranspose(pHierarchyNodes->Get_OffsetMatrix() *
				pHierarchyNodes->Get_CombinedTransformationMatrix() * _PivotMatrix));

		//   XMStoreFloat4x4(&_matBone, XMMatrixIdentity());

		_pBoneMatrices[iBoneIndex++] = _matBone;
	}

	return S_OK;
}

HRESULT CMeshContainer_Tool::SetUp_Vertices(aiMesh* _pMesh, CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix)
{
	m_iStride = sizeof(VTXMODEL);
	m_iNumVertices = _pMesh->mNumVertices;
	m_iNumBuffers = 1;

	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerticesBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStride;
	tVerticesBufferDesc.CPUAccessFlags = 0;
	tVerticesBufferDesc.MiscFlags = 0;

	VTXMODEL* pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
	_float2 v2 = _float2(0.f, 0.f);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &_pMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _mPivotMatrix));
		m_pVerticesPos[i] = pVertices[i].vPosition;
		memcpy(&pVertices[i].vNormal, &_pMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _mPivotMatrix));
		memcpy(&pVertices[i].vTangent, &_pMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV0, &_pMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer(tVerticesBufferDesc, tVerticesBufferSubResourceData)))
		return E_FAIL;

	m_pVertexData = (void*)pVertices;
	return S_OK;
}

HRESULT CMeshContainer_Tool::SetUp_AnimVertices(aiMesh* _pMesh, CModel_Tool* _pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVertices = _pMesh->mNumVertices;
	m_iNumBuffers = 1;

	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tVerticesBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStride;
	tVerticesBufferDesc.CPUAccessFlags = 0;
	tVerticesBufferDesc.MiscFlags = 0;

	VTXANIMMODEL* pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &_pMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &_pMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &_pMesh->mTangents[i], sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;

		memcpy(&pVertices[i].vTexUV0, &_pMesh->mTextureCoords[0][i], sizeof(_float2));

		if(nullptr != _pMesh->mTextureCoords[1])
			memcpy(&pVertices[i].vTexUV1, &_pMesh->mTextureCoords[1][i], sizeof(_float2));

		if (nullptr != _pMesh->mTextureCoords[2])
			memcpy(&pVertices[i].vTexUV2, &_pMesh->mTextureCoords[2][i], sizeof(_float2));

		if (nullptr != _pMesh->mTextureCoords[3])
			memcpy(&pVertices[i].vTexUV3, &_pMesh->mTextureCoords[3][i], sizeof(_float2));
	}

	m_iNumBones = _pMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone* pAIBone = _pMesh->mBones[i];

		_float4x4	OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		CHierarchyNode_Tool* pNode = _pModel->Get_HierarchyNodes(pAIBone->mName.data);
		pNode->Set_OffsetMatrix(OffsetMatrix);
		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);

		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer(tVerticesBufferDesc, tVerticesBufferSubResourceData)))
		return E_FAIL;

	m_pVertexData = (void*)pVertices;
	return S_OK;
}

HRESULT CMeshContainer_Tool::SetUp_Indices(aiMesh* _pMesh)
{
	m_iNumPrimitive = _pMesh->mNumFaces;
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

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._1 = _pMesh->mFaces[i].mIndices[0];
		pIndices[i]._2 = _pMesh->mFaces[i].mIndices[1];
		pIndices[i]._3 = _pMesh->mFaces[i].mIndices[2];
	}
	D3D11_SUBRESOURCE_DATA tIndicesBufferSubResourceData;
	ZeroMemory(&tIndicesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tIndicesBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer(tIndicesBufferDesc, tIndicesBufferSubResourceData)))
		return E_FAIL;

	m_pIndices = (void*)pIndices;
	return S_OK;
}

void CMeshContainer_Tool::Get_Data(MESHDESC& _tMeshDesc)
{
	_tMeshDesc.iMatrialIndex = m_iMaterialIndex;
	_tMeshDesc.iNumVertices = m_iNumVertices;
	_tMeshDesc.iNumIndices = m_iNumPrimitive;
	_tMeshDesc.strName.assign(m_strName.begin(), m_strName.end());
	_tMeshDesc.pVtxModel = static_cast<VTXMODEL*>(m_pVertexData);
	_tMeshDesc.pIndices = static_cast<FACEINDICES32*>(m_pIndices);
	_tMeshDesc.pVerticesPos = m_pVerticesPos;
	_tMeshDesc.iHierachyCount = (_uint)m_HierarchyNodes.size();
	for (auto& iter : m_HierarchyNodes)
	{
		string str = iter->Get_Name();
		_wstring strName;
		strName.assign(str.begin(), str.end());
		_tMeshDesc.HierachyNameList.push_back(strName);
	}
}

void CMeshContainer_Tool::Get_Data(ANIMMESHDESC& _tAnimMeshDesc)
{
	_tAnimMeshDesc.iMatrialIndex = m_iMaterialIndex;
	_tAnimMeshDesc.iNumBones = m_iNumBones;
	_tAnimMeshDesc.iNumVertices = m_iNumVertices;
	_tAnimMeshDesc.iNumIndices = m_iNumPrimitive;
	_tAnimMeshDesc.strName.assign(m_strName.begin(), m_strName.end());
	_tAnimMeshDesc.pVtxModel = static_cast<VTXANIMMODEL*>(m_pVertexData);
	_tAnimMeshDesc.pIndices = static_cast<FACEINDICES32*>(m_pIndices);
	_tAnimMeshDesc.pVerticesPos = m_pVerticesPos;
	_tAnimMeshDesc.iHierachyCount = (_uint)m_HierarchyNodes.size();
	for (auto& iter : m_HierarchyNodes)
	{
		string str = iter->Get_Name();
		_wstring strName;
		strName.assign(str.begin(), str.end());
		_tAnimMeshDesc.HierachyNameList.push_back(strName);
	}
}

CMeshContainer_Tool* CMeshContainer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, aiMesh* _pAIMesh, const _uint& _iType, CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix)
{
	CMeshContainer_Tool* pMeshContainer = new CMeshContainer_Tool(_pDevice, _pContext);
	if (FAILED(pMeshContainer->NativeConstruct_Prototype(_pAIMesh, _iType, _pModel)))
	{
		Safe_Release(pMeshContainer);
		MSG_BOX("CMeshContainer::Create To Failed");
	}
	return pMeshContainer;
}

CComponent* CMeshContainer_Tool::Clone(void* _pArg)
{
	CMeshContainer_Tool* pMeshContainer = new CMeshContainer_Tool(*this);
	if (FAILED(pMeshContainer->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pMeshContainer);
		MSG_BOX("CMeshContainer::Clone To Failed");
	}
	return pMeshContainer;
}

void CMeshContainer_Tool::Free()
{
	if (false == m_bCloned)
		Safe_Delete_Array(m_pVertexData);

	for (auto& iter : m_HierarchyNodes)
		Safe_Release(iter);
	__super::Free();
}
