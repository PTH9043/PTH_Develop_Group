#include "MeshContainer_Instance.h"
#include "GameInstance.h"
#include "HierarchyNode_Instance.h"


CMeshContainer_Instance::CMeshContainer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMeshContainer_Instance::CMeshContainer_Instance(const CMeshContainer_Instance& _rhs)
	: CVIBuffer(_rhs),
	m_iMaterialIndex(_rhs.m_iMaterialIndex), m_iNumBones(_rhs.m_iNumBones),
	m_HierarchyNodes(_rhs.m_HierarchyNodes), m_vMaxVertex(_rhs.m_vMaxVertex), m_vMinVertex(_rhs.m_vMinVertex)
	, m_iNumIndices(_rhs.m_iNumIndices)
{
	for (auto& value : m_HierarchyNodes)
		Safe_AddRef(value);
}

HRESULT CMeshContainer_Instance::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMeshContainer_Instance::NativeConstruct_Prototype(CModel_Instance* _pModel, const MESHDESC& _tData,
	_fmatrix _mPivotMatrix)
{
	m_strName = CMethod::ConvertWToS(_tData.strName);

	if (FAILED(SetUp_Vertices(_pModel, _tData, _mPivotMatrix)))
		return E_FAIL;

	m_pIndices = (void*)_tData.pIndices;
	m_iNumIndices = _tData.iNumIndices;
	return S_OK;
}

HRESULT CMeshContainer_Instance::NativeConstruct_Prototype_Anim(CModel_Instance* _pModel, const ANIMMESHDESC& _tData,
 _fmatrix _mPivotMatrix)
{
	m_strName = CMethod::ConvertWToS(_tData.strName);
	m_iNumPrimitive = m_iNumInstance * _tData.iNumIndices;
	m_iNumIndices = _tData.iNumIndices;

	if (FAILED(SetUp_AnimVertices(_pModel, _tData, _mPivotMatrix)))
		return E_FAIL;

	m_pIndices = (void*)_tData.pIndices;
	return S_OK;
}

HRESULT CMeshContainer_Instance::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	if (nullptr == _pArg)
		return E_FAIL;

	MESHINFO tDesc = *static_cast<MESHINFO*>(_pArg);
	m_iGroupNum = tDesc.iGroupNum;
	m_iNumInstance = tDesc.iInstanceNum;
	if (tDesc.iType == (_uint)CModel_Instance::TYPE_ANIM)
	{
		m_iNumPrimitive = m_iNumIndices * m_iNumInstance;

		if (FAILED(SetUp_AnimVertices_Instance(tDesc.iInstanceNum)))
			return E_FAIL;

		FACEINDICES32* pIndices = (FACEINDICES32*)m_pIndices;
		if (FAILED(SetUp_Indices(tDesc.iInstanceNum, m_iNumIndices, pIndices)))
			return E_FAIL;
	
		CModel_Instance* pModel = (CModel_Instance*)tDesc.pInstance;

		for (_uint i = 0; i < m_iGroupNum; ++i)
		{
			vector<CHierarchyNode_Instance*>		HierarchyNodes;

			for (auto& pHierarchyNode : m_HierarchyNodes)
			{
				CHierarchyNode_Instance* pNewNode = pModel->Get_HierarchyNodes_Clone(i, pHierarchyNode->Get_Name().c_str());
				_float4x4		OffsetMatrix;
				XMStoreFloat4x4(&OffsetMatrix, pHierarchyNode->Get_OffsetMatrix());
				pNewNode->Set_OffsetMatrix(OffsetMatrix);
				HierarchyNodes.push_back(pNewNode);
				Safe_AddRef(pNewNode);
			}
			m_HierackyGroup.push_back(HierarchyNodes);
		}
	}
	else
	{
		m_iNumPrimitive = m_iNumIndices * m_iNumInstance;

		if (FAILED(SetUp_Vertices_Instance(tDesc.iInstanceNum)))
			return E_FAIL;

		FACEINDICES32* pIndices = (FACEINDICES32*)m_pIndices;
		if (FAILED(SetUp_Indices(tDesc.iInstanceNum, m_iNumIndices, pIndices)))
			return E_FAIL;

		CModel_Instance* pModel = (CModel_Instance*)_pArg;
		vector<CHierarchyNode_Instance*>		HierarchyNodes;

		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			CHierarchyNode_Instance* pNewNode = pModel->Get_HierarchyNodes_Proto(pHierarchyNode->Get_Name().c_str());
			_float4x4		OffsetMatrix;
			XMStoreFloat4x4(&OffsetMatrix, pHierarchyNode->Get_OffsetMatrix());
			pNewNode->Set_OffsetMatrix(OffsetMatrix);
			HierarchyNodes.push_back(pNewNode);
			Safe_AddRef(pNewNode);
			Safe_Release(pHierarchyNode);
		}
		m_HierarchyNodes.clear();
		m_HierarchyNodes = HierarchyNodes;
	}
	
	return S_OK;
}
// Render 
HRESULT CMeshContainer_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance };

	_uint				iStrides[] = { m_iStride,   m_iStrideInstance };

	_uint				iOffset[] = { 0,   0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iNumIndices * 3, m_iNumInstance, 0, 0, 0);

	return S_OK;
}
// Bone Matrices
HRESULT CMeshContainer_Instance::Update_Value(const _uint& _iType, TRANSFORMVECTOR Transforms, _fmatrix _PivotMatrix, const _uint& _iIndex )
{
	if (Transforms.size() <= 0)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint iIndex = 0;

	if (CModel_Instance::TYPE_ANIM == _iType)
	{
		for (_uint i = 0; i < Transforms.size(); ++i)
		{
			_float4 vPos;
			_float4 vRight;
			_float4 vUp;
			_float4 vLook;
			vPos.Set_Vector(Transforms[i]->Get_Pos());
			vRight.Set_Vector(Transforms[i]->Get_Right());
			vUp.Set_Vector(Transforms[i]->Get_Up());
			vLook.Set_Vector(Transforms[i]->Get_Look());
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vTranslation = vPos;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vRight = vRight;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vUp = vUp;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vLook = vLook;
			_uint ID = ((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].iGroupID;
			++iIndex;
		}
	}
	else
	{
		for (_uint i = 0; i < Transforms.size(); ++i)
		{
			_float4 vPos;
			_float4 vRight;
			_float4 vUp;
			_float4 vLook;
			vPos.Set_Vector(Transforms[i]->Get_Pos());
			vRight.Set_Vector(Transforms[i]->Get_Right());
			vUp.Set_Vector(Transforms[i]->Get_Up());
			vLook.Set_Vector(Transforms[i]->Get_Look());
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vTranslation = vPos;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vRight = vRight;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vUp = vUp;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vLook = vLook;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return S_OK;
}

HRESULT CMeshContainer_Instance::Update_Value(const _uint& _iType, F4POSVECTOR f4PosVector, _fmatrix _PivotMatrix, const _uint& _iIndex )
{
	if (f4PosVector.size() <= 0)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	_uint iIndex = 0;

	if (CModel_Instance::TYPE_ANIM == _iType)
	{
		for (_uint i = 0; i < f4PosVector.size(); ++i)
		{
			_float4 vPos = f4PosVector[i];
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vTranslation = vPos;
			_uint ID = ((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].iGroupID;
			++iIndex;
		}
	}
	else
	{
		for (_uint i = 0; i < f4PosVector.size(); ++i)
		{
			_float4 vPos = f4PosVector[i];
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return S_OK;
}

HRESULT CMeshContainer_Instance::Update_Value(const _uint& _iType, MATRIXES _Matrix, _fmatrix _PivotMatrix, const _uint& _iIndex)
{
	if (_Matrix.size() <= 0)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint iIndex = 0;

	if (CModel_Instance::TYPE_ANIM == _iType)
	{
		for (_uint i = 0; i < _Matrix.size(); ++i)
		{
			_float4 vPos;
			_float4 vRight;
			_float4 vUp;
			_float4 vLook;
			vPos.Set_Vector(_Matrix[i].r[3]);
			vRight.Set_Vector(_Matrix[i].r[0]);
			vUp.Set_Vector(_Matrix[i].r[1]);
			vLook.Set_Vector(_Matrix[i].r[2]);
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vTranslation = vPos;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vRight = vRight;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vUp = vUp;
			((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].vLook = vLook;
			_uint ID = ((VTXANIMMODEL_INSTANCE*)SubResource.pData)[m_GroupIDS[_iIndex][iIndex]].iGroupID;
			++iIndex;
		}
	}
	else
	{
		for (_uint i = 0; i < _Matrix.size(); ++i)
		{
			_float4 vPos;
			_float4 vRight;
			_float4 vUp;
			_float4 vLook;
			vPos.Set_Vector(_Matrix[i].r[3]);
			vRight.Set_Vector(_Matrix[i].r[0]);
			vUp.Set_Vector(_Matrix[i].r[1]);
			vLook.Set_Vector(_Matrix[i].r[2]);
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vTranslation = vPos;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vRight = vRight;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vUp = vUp;
			((VTXMODEL_INSTANCE*)SubResource.pData)[i].vLook = vLook;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return S_OK;
}

HRESULT CMeshContainer_Instance::SetUp_BoneMatrices(const _uint& _iIndex, _float4x4* _pBoneMatrices, _fmatrix _PivotMatrix)
{
	if (m_HierackyGroup[_iIndex].empty())
	{
		XMStoreFloat4x4(&_pBoneMatrices[0], XMMatrixIdentity());
		return S_OK;
	}

	_uint			iBoneIndex = 0;

	for (auto& pHierarchyNodes : m_HierackyGroup[_iIndex])
	{
		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixTranspose(pHierarchyNodes->Get_OffsetMatrix() *
			pHierarchyNodes->Get_CombinedTransformationMatrix() * _PivotMatrix));

		_pBoneMatrices[iBoneIndex++] = matrix;
	}

	return S_OK;
}

// SetUp Vertices
HRESULT CMeshContainer_Instance::SetUp_Vertices(CModel_Instance* _pModel, const MESHDESC& _tMeshDesc, _fmatrix _mPivotMatrix)
{
	m_iMaterialIndex = _tMeshDesc.iMatrialIndex;
	m_iStride = sizeof(VTXMODEL);
	m_iNumVertices = _tMeshDesc.iNumVertices;
	m_iNumBuffers = 2;

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
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _mPivotMatrix));
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
// SetUp AnimVertice
HRESULT CMeshContainer_Instance::SetUp_AnimVertices(CModel_Instance* _pModel, const ANIMMESHDESC& _tAnimMeshDesc, _fmatrix _mPivotMatrix)
{
	m_iMaterialIndex = _tAnimMeshDesc.iMatrialIndex;
	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVertices = _tAnimMeshDesc.iNumVertices;
	/* 현재 메시에 영향을 주는 뼈들의 갯수. */
	m_iNumBones = _tAnimMeshDesc.iNumBones;
	m_iNumBuffers = 2;

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
		CHierarchyNode_Instance* pNode = _pModel->Get_HierarchyNodes_Proto(CMethod::ConvertWToS(iter).c_str());
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
// SetUp Vertices Instance
HRESULT CMeshContainer_Instance::SetUp_Vertices_Instance(const _uint& _iInstanceCount)
{
	if (0 == _iInstanceCount)
		return E_FAIL;

	m_iStrideInstance = sizeof(VTXMODEL_INSTANCE);

	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	tVerticesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStrideInstance;
	tVerticesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tVerticesBufferDesc.MiscFlags = 0;

	VTXMODEL_INSTANCE* pVertices = new VTXMODEL_INSTANCE[m_iNumInstance];
	ZeroMemory(pVertices, sizeof(VTXMODEL_INSTANCE) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vRight = _float4(0.1f, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, 0.1f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 0.1f, 0.f);
		pVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	}
	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&tVerticesBufferDesc, &tVerticesBufferSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	return S_OK;
}
// SetUp AnimVertices Instance
HRESULT CMeshContainer_Instance::SetUp_AnimVertices_Instance(const _uint& _iInstanceCount)
{
	if (0 == _iInstanceCount)
		return E_FAIL;

	m_iStrideInstance = sizeof(VTXANIMMODEL_INSTANCE);

	D3D11_BUFFER_DESC tVerticesBufferDesc;
	ZeroMemory(&tVerticesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tVerticesBufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	tVerticesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tVerticesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVerticesBufferDesc.StructureByteStride = m_iStrideInstance;
	tVerticesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tVerticesBufferDesc.MiscFlags = 0;

	if (FAILED(Divine_Group(_iInstanceCount, m_iGroupNum)))
		return E_FAIL;

	VTXANIMMODEL_INSTANCE* pVertices = new VTXANIMMODEL_INSTANCE[m_iNumInstance];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL_INSTANCE) * m_iNumInstance);

	_uint iGroupID = 0;
	for(auto& iter : m_GroupIDS)
	{
		for (auto& value : iter)
		{
			pVertices[value].vTranslation = _float4(0, 0, 0, 1.f);
			pVertices[value].vRight = _float4(1, 0.f, 0.f, 0.f);
			pVertices[value].vUp = _float4(0.f, 1.f, 0.f, 0.f);
			pVertices[value].vLook = _float4(0.f, 0.f, 1.f, 0.f);
			pVertices[value].iGroupID = iGroupID;
		}
		++iGroupID;
	}
	D3D11_SUBRESOURCE_DATA tVerticesBufferSubResourceData;
	ZeroMemory(&tVerticesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tVerticesBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&tVerticesBufferDesc, &tVerticesBufferSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	return S_OK;
}
// Create Indices 
HRESULT CMeshContainer_Instance::SetUp_Indices(const _uint& _iInstanceCount, const _uint& _iIndicesCount, FACEINDICES32* _pData)
{
	if (0 == _iInstanceCount)
		return E_FAIL;

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

	_uint iIndex = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		memcpy((FACEINDICES32*)pIndices + i * _iIndicesCount, _pData, sizeof(FACEINDICES32) * _iIndicesCount);
	}

	D3D11_SUBRESOURCE_DATA tIndicesBufferSubResourceData;
	ZeroMemory(&tIndicesBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tIndicesBufferSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer(tIndicesBufferDesc, tIndicesBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CMeshContainer_Instance::Divine_Group(const _uint& _iModelInstanceNum, const _uint& _iGroupNum)
{
	_uint iID = 0;
	for (_uint i = 0; i < _iGroupNum; ++i)
	{
		vector<_uint> GROUPS;
		for (_uint j = 0; j < _iModelInstanceNum / _iGroupNum; ++j)
		{
			if (iID >= _iModelInstanceNum)
				return S_OK;

			GROUPS.push_back(iID++);
		}
		m_GroupIDS.push_back(GROUPS);
	}

	if (iID < _iModelInstanceNum)
	{
		for (_uint i = iID; i < _iModelInstanceNum; ++i)
		{
			m_GroupIDS[_iGroupNum - 1].push_back(iID);
		}
	}
	return S_OK;
}

CMeshContainer_Instance* CMeshContainer_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel_Instance* _pModel,
	const MESHDESC& _tData,  _fmatrix _mPivotMatrix)
{
	CMeshContainer_Instance* pInstance = new CMeshContainer_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pModel, _tData, _mPivotMatrix)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMeshContainer_Instance::Create To Failed");
#endif
	}
	return pInstance;
}

CMeshContainer_Instance* CMeshContainer_Instance::Create_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
	CModel_Instance* _pModel, const ANIMMESHDESC& _tData,  _fmatrix _mPivotMatrix)
{
	CMeshContainer_Instance* pInstance = new CMeshContainer_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype_Anim(_pModel, _tData, _mPivotMatrix)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMeshContainer_Instance::Create To Failed");
#endif
	}
	return pInstance;
}

CComponent* CMeshContainer_Instance::Clone(void* _pArg)
{
	CMeshContainer_Instance* pInstance = new CMeshContainer_Instance(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMeshContainer_Instance::Clone To Failed");
#endif
	}
	return pInstance;
}

void CMeshContainer_Instance::Free()
{
	if (true == m_bCloned)
	{
		for (auto& iter : m_HierackyGroup)
		{
			for (auto& value : iter)
				Safe_Release(value);
		}
		m_HierackyGroup.clear();
	}

	for (auto& value : m_HierarchyNodes)
		Safe_Release(value);

	Safe_Release_DV(m_pVBInstance);

	__super::Free();
}
