#include "EngineDefine.h"
#include "UMeshContainer.h"
#include "UBoneNode.h"
#include "UModel.h"
#include "UMethod.h"
#include "UMeshFilter.h"

UMeshContainer::UMeshContainer(CSHPTRREF<UDevice> _spDevice)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_3D, VIINSTANCE_TYPE::SINGLE),
	m_iMaterialIndex{ 0 },
	m_iNumBones{ 0 },
	m_iNumBuffers{ 0 },
	m_BoneNodeContainer{  },
	m_wstrName{ L"" },
	m_isMaterialShowEnable{false},
	m_iMeshIndex{0}
{
}

UMeshContainer::UMeshContainer(const UMeshContainer& _rhs) :
	UVIBuffer(_rhs),
	m_iMaterialIndex{ _rhs.m_iMaterialIndex },
	m_iNumBones{ _rhs.m_iNumBones },
	m_iNumBuffers{ _rhs.m_iNumBuffers },
	m_BoneNodeContainer{ _rhs.m_BoneNodeContainer },
	m_wstrName{ _rhs.m_wstrName },
	m_isMaterialShowEnable{ false },
	m_iMeshIndex{ _rhs.m_iMaterialIndex }
{
}

void UMeshContainer::Free()
{
	m_BoneNodeContainer.clear();
}

HRESULT UMeshContainer::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UMeshContainer::NativeConstruct(void* _pData, CSHPTRREF<UModel> _spModel, const _int _iMeshIndex)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);

	if (UModel::TYPE::ANIM == _spModel->GetType())
	{
		RETURN_CHECK_FAILED(ReadyAnimVertices(_pData, _spModel), E_FAIL);
	}
	else
	{
		RETURN_CHECK_FAILED(ReadyVertices(_pData, _spModel), E_FAIL);
	}
	RETURN_CHECK_FAILED(ReadyIndicies(_pData), E_FAIL);
	m_iMeshIndex = _iMeshIndex;
	return S_OK;
}

HRESULT UMeshContainer::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK(0 >= _vecDatas.size(), E_FAIL);
	UModel* pModel = UMethod::ConvertTemplate_Index<UModel*>(_vecDatas, 0);
	RETURN_CHECK(nullptr == pModel, E_FAIL);
	VECTOR<SHPTR<UBoneNode>> BoneModels{};

	for (auto& BoneNode : m_BoneNodeContainer) {
		_wstring wstrBoneName = BoneNode->GetName();
		SHPTR<UBoneNode> pBoneNode = pModel->FindBoneNode(wstrBoneName);
		BoneModels.push_back(pBoneNode);
	}

	m_BoneNodeContainer.clear();
	m_BoneNodeContainer = BoneModels;
	return S_OK;
}

HRESULT UMeshContainer::SetUpBoneMatrix(ARRAY<_float4x4, MAX_BONE_SIZE>& _arrBones)
{
	for (_uint i = 0; i < m_BoneNodeContainer.size(); ++i)
	{
		_arrBones[i] = m_BoneNodeContainer[i]->GetFinalTranformMatrix();
	}
	return S_OK;
}

void UMeshContainer::RenderAnimModel(CSHPTRREF<UMeshFilter> _spMeshShowController,
	CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommands, const _uint _iMeshIndex, const _uint& _iInstanceCnt)
{
	assert(nullptr != _spMeshShowController);
	RETURN_CHECK(false == _spMeshShowController->IsShowEnable(_iMeshIndex), ;);
	__super::Render(_spShader, _spCommands, _iInstanceCnt);
}

void UMeshContainer::SetMaterialShowEnable(CSHPTRREF<UMeshFilter> _spMeshShowController)
{
	assert(nullptr != _spMeshShowController);
}

HRESULT UMeshContainer::ReadyVertices(void* _pData, CSHPTRREF<UModel> _spModel)
{
	MESHDESC* tMeshDesc = static_cast<MESHDESC*>(_pData);
	ReceiveBoneNode(_spModel, tMeshDesc->BoneNodeNameList, tMeshDesc->iBoneNodeCnt);
	return MakeVertices(tMeshDesc->pVtxModel, (void*)tMeshDesc->pPosition, sizeof(VTXMODEL),
		tMeshDesc->iNumVertices, tMeshDesc->vMinVertex, tMeshDesc->vMaxVertex,
		tMeshDesc->wstrName, tMeshDesc->iMatrialIndex);
}

HRESULT UMeshContainer::ReadyAnimVertices(void* _pData, CSHPTRREF<UModel> _spModel)
{
	ANIMMESHDESC* tMeshDesc = static_cast<ANIMMESHDESC*>(_pData);
	ReceiveBoneNode(_spModel, tMeshDesc->BoneNodeNameList, tMeshDesc->iBoneNodeCnt);
	return MakeVertices(tMeshDesc->pVtxModel, (void*)tMeshDesc->pPosition, sizeof(VTXANIMMODEL),
		tMeshDesc->iNumVertices, tMeshDesc->vMinVertex, tMeshDesc->vMaxVertex,
		tMeshDesc->wstrName, tMeshDesc->iMatrialIndex);
}

HRESULT UMeshContainer::ReadyIndicies(void* _pData)
{
	MESHDESC* tMeshDesc = static_cast<MESHDESC*>(_pData);
	RETURN_CHECK_FAILED(CreateIndexBuffer(tMeshDesc->iNumIndices, sizeof(INDICIES32),
		tMeshDesc->pIndices, DXGI_FORMAT_R32_UINT), E_FAIL);
	return S_OK;
}
/* Make To Vertex */
HRESULT UMeshContainer::MakeVertices(void* _pVertices, void* _pPosition, const _uint _iVertexSize, const _uint _iVertexCnt
	, const _float3& _vMinPos, const _float3& _vMaxPos, const _wstring& _wstrName, const _uint& _iMaterialIndex)
{
	VECTOR<_float3> pVerticesPos;
	pVerticesPos.resize(_iVertexCnt);
	::memcpy(&pVerticesPos[0], _pPosition, sizeof(_float3) * _iVertexCnt);

	RETURN_CHECK_FAILED(CreateVtxBuffer(_iVertexCnt, _iVertexSize, _pVertices,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pVerticesPos, false), E_FAIL);

	m_wstrName = _wstrName;
	m_iMaterialIndex = _iMaterialIndex;

	SetMinPosition(_vMinPos);
	SetMaxPosition(_vMaxPos);

	Make::ReleaseBuffer(_pVertices);
	Make::ReleaseBuffer(_pPosition);
	//Safe_Delete_Array(_pVertices);
	//Safe_Delete_Array(_pPosition);
	return S_OK;
}

void UMeshContainer::ReceiveBoneNode(CSHPTRREF<UModel> _spModel, const VECTOR<_wstring>& _convecBoneNameList,
	const _uint& _iBoneNodeCnt)
{
	for (_uint i = 0; i < _iBoneNodeCnt; ++i)
	{
		SHPTR<UBoneNode> spBoneNode = _spModel->FindBoneNode(_convecBoneNameList[i]);
		m_BoneNodeContainer.push_back(spBoneNode);
	}
	m_iNumBones = _iBoneNodeCnt;
}
