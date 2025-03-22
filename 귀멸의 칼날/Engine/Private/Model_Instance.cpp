#include "Model_Instance.h"
#include "GameInstance.h"
#include "MeshContainer_Instance.h"
#include "HierarchyNode_Instance.h"
#include "Animation_Instance.h"
#include "Method.h"


CHierarchyNode_Instance* CModel_Instance::Get_HierarchyNodes_Proto(const _char* _pHierarchyNodeNames)
{
	auto	iter = find_if(m_vecHierachyNodes.begin(), m_vecHierachyNodes.end(), [&](CHierarchyNode_Instance* _pNode)
	{
		return !strcmp(_pHierarchyNodeNames, _pNode->Get_Name().c_str());
	});

	if (m_vecHierachyNodes.end() == iter)
		return nullptr;

	return (*iter);
}

CModel_Instance::HIERARCHYNODES CModel_Instance::Get_HierachyNodeList(const _uint& _iGroupID)
{
	if(_iGroupID >= m_iGroupNumber)
		return HIERARCHYNODES();

	return m_vecHierarchyGroup[_iGroupID];
}

CHierarchyNode_Instance* CModel_Instance::Get_HierarchyNodes_Clone(const _uint& _iGroupID, const _char* _pHierarchyNodeNames)
{
	HIERARCHYNODES Nodes = Get_HierachyNodeList(_iGroupID);
	if (Nodes.size() <= 0)
		return nullptr;

	auto	iter = find_if(Nodes.begin(), Nodes.end(), [&](CHierarchyNode_Instance* _pNode)
	{
		return !strcmp(_pHierarchyNodeNames, _pNode->Get_Name().c_str());
	});

	if (Nodes.end() == iter)
		return nullptr;

	return (*iter);
}

CModel_Instance::CModel_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext), m_PivotMatrix(CMethod::XMMatrixIdentity_P())
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& _rhs)
	: CComponent(_rhs), m_vecMeshContainers(_rhs.m_vecMeshContainers), m_iNumMeshContainers(_rhs.m_iNumMeshContainers),
	m_iNumMatrials(_rhs.m_iNumMatrials), m_vecMaterials(_rhs.m_vecMaterials),
	m_vecHierachyNodes(_rhs.m_vecHierachyNodes), m_vecAnimations(_rhs.m_vecAnimations),
	m_pFileGroup(_rhs.m_pFileGroup), m_pFileData(_rhs.m_pFileData), m_PivotMatrix(_rhs.m_PivotMatrix), 
	m_eModelType(_rhs.m_eModelType)
{
	for (auto& iter : m_vecMeshContainers)
		Safe_AddRef(iter);

	for (auto& iter : m_vecMaterials)
	{
		for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
			Safe_AddRef(iter.pMaterialTexture[i]);
	}

	for (auto& iter : m_vecHierachyNodes)
		Safe_AddRef(iter);

	for (auto& iter : m_vecAnimations)
		Safe_AddRef(iter);
}

HRESULT CModel_Instance::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CModel_Instance::NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, FILEDATA* _pFileData,  
	_fmatrix _mPivotMatrix)
{
	if (nullptr == _pFileGroup && nullptr == _pFileData)
		return E_FAIL;

	m_pFileGroup = _pFileGroup;
	m_pFileData = _pFileData;

	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);
	m_eModelType = _eModelType;

	m_vecHierachyNodes.resize(m_iModelInstanceCount);
	m_vecAnimations.resize(m_iModelInstanceCount);

	if (TYPE_NONANIM == _eModelType)
	{
		MODELDESC tData;
		if (FAILED(Load_Data_NonAnim(tData)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers(tData, _mPivotMatrix)))
			return E_FAIL;

		Create_Materials(tData);
	}
	else
	{
		ANIMMODELDESC tData;
		if (FAILED(Load_Data_Anim(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers_Anim(tData, _mPivotMatrix)))
			return E_FAIL;

		if (FAILED(Create_Animation(tData.AnimList)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel_Instance::NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, const _wstring& _strName, 
	_fmatrix _mPivotMatrix)
{
	if (nullptr == _pFileGroup)
		return E_FAIL;

	m_pFileData = _pFileGroup->FindData(_strName);
	if (nullptr == m_pFileData)
		return E_FAIL;

	m_pFileGroup = _pFileGroup;

	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);
	m_eModelType = _eModelType;

	m_vecHierachyNodes.resize(m_iModelInstanceCount);
	m_vecAnimations.resize(m_iModelInstanceCount);

	if (TYPE_NONANIM == _eModelType)
	{
		MODELDESC tData;
		if (FAILED(Load_Data_NonAnim(tData)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers(tData, _mPivotMatrix)))
			return E_FAIL;

		Create_Materials(tData);
	}
	else
	{
		ANIMMODELDESC tData;
		if (FAILED(Load_Data_Anim(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers_Anim(tData, _mPivotMatrix)))
			return E_FAIL;

		if (FAILED(Create_Animation(tData.AnimList)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CModel_Instance::NativeConstruct_Prototype(const TYPE& _eModelType, PATHS* _pPaths, const _wstring& _strName,
 _fmatrix _mPivotMatrix)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pFileGroup = pGameInstance->Find_Folder(_pPaths);
	}
	RELEASE_INSTANCE(CGameInstance);

	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);

	if (nullptr == m_pFileGroup)
		return E_FAIL;

	m_eModelType = _eModelType;

	_bool bIsFindFBX = false;
	for (auto& iter : m_pFileGroup->lsUnderList)
	{
		if (!lstrcmp(iter->strUppderName.c_str(), L"FBX_Converter"))
		{
			bIsFindFBX = true;
			break;
		}
	}

	if (false == bIsFindFBX)
	{
		m_pFileData = m_pFileGroup->FindData(_strName);
		if (nullptr == m_pFileData)
			return E_FAIL;
	}
	else
	{
		FILEGROUP* _pFileGroup = m_pFileGroup->Find_Group(L"FBX_Converter");
		if (nullptr == _pFileGroup)
			return E_FAIL;

		m_pFileData = _pFileGroup->FindData(_strName);
		if (nullptr == m_pFileData)
			return E_FAIL;
	}

	if (TYPE_NONANIM == _eModelType)
	{
		MODELDESC tData;
		if (FAILED(Load_Data_NonAnim(tData)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers(tData, _mPivotMatrix)))
			return E_FAIL;

		Create_Materials(tData);
	}
	else
	{
		ANIMMODELDESC tData;
		if (FAILED(Load_Data_Anim(tData)))
			return E_FAIL;

		if (FAILED(Create_HierachyNodes(tData.HNodeDataList)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_MeshConstainers_Anim(tData, _mPivotMatrix)))
			return E_FAIL;

		if (FAILED(Create_Animation(tData.AnimList)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel_Instance::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	if (m_eModelType == TYPE_ANIM)
	{
		MODEL_INSTNACE_DESC tDesc = *static_cast<MODEL_INSTNACE_DESC*>(pArg);
		m_iGroupNumber = tDesc.iGroupNumber;
		m_bCloned = true;
		// HierachyNode
		{
			for (_uint i = 0; i < tDesc.iGroupNumber; ++i)
			{
				HIERARCHYNODES		HierachyNodes;
				for (auto& iter : m_vecHierachyNodes)
				{
					HierachyNodes.push_back(iter->Clone_Node(i));
				}

				m_vecHierarchyGroup.push_back(HierachyNodes);
			}

			for (auto& iter : m_vecHierarchyGroup)
			{
				for (auto& value : iter)
					value->Find_Parents(this);
			}
		}
		// Mesh Container
		{
			MESHCONTAINERS		MeshContainers;
			CMeshContainer_Instance::MESHINFO tInfo;
			tInfo.iInstanceNum = tDesc.iModelInstanceCount;
			tInfo.iGroupNum = tDesc.iGroupNumber;
			tInfo.iType = m_eModelType;
			tInfo.pInstance = this;

			for (auto& iter : m_vecMeshContainers)
			{
				MeshContainers.push_back((CMeshContainer_Instance*)iter->Clone(&tInfo));
				Safe_Release(iter);
			}
			m_vecMeshContainers.clear();
			m_vecMeshContainers = MeshContainers;
		}
		// Animation
		{
			for (_uint i = 0; i < tDesc.iGroupNumber; ++i)
			{
				ANIMATIONS				Animations;
				for (auto& iter : m_vecAnimations)
				{
					Animations.push_back(iter->Clone_Animation(this, i));
				}
				m_vecAnimationGroup.push_back(Animations);
			}

			m_CurIndexGroup.resize(m_iGroupNumber);
			m_CurrentAnimationGroup.resize(m_iGroupNumber);
			m_NextAnimationGroup.resize(m_iGroupNumber);
			m_PrevAnimationGroup.resize(m_iGroupNumber);
		}

		if (FAILED(Create_BoneTexture()))
			return E_FAIL;
	}
	else
	{
		MODEL_INSTNACE_DESC tDesc = *static_cast<MODEL_INSTNACE_DESC*>(pArg);
		m_iGroupNumber = tDesc.iGroupNumber;
		m_bCloned = true;
		// HierachyNode
		{
			for (_uint i = 0; i < tDesc.iGroupNumber; ++i)
			{
				HIERARCHYNODES		HierachyNodes;
				for (auto& iter : m_vecHierachyNodes)
				{
					HierachyNodes.push_back(iter->Clone_Node(i));
				}

				m_vecHierarchyGroup.push_back(HierachyNodes);
			}

			for (auto& iter : m_vecHierarchyGroup)
			{
				for (auto& value : iter)
					value->Find_Parents(this);
			}
		}
		// Mesh Container
		{
			MESHCONTAINERS		MeshContainers;
			CMeshContainer_Instance::MESHINFO tInfo;
			tInfo.iInstanceNum = tDesc.iModelInstanceCount;
			tInfo.iGroupNum = tDesc.iGroupNumber;
			tInfo.iType = m_eModelType;
			tInfo.pInstance = this;

			for (auto& iter : m_vecMeshContainers)
			{
				MeshContainers.push_back((CMeshContainer_Instance*)iter->Clone(&tInfo));
				Safe_Release(iter);
			}
			m_vecMeshContainers.clear();
			m_vecMeshContainers = MeshContainers;
		}
	}
	return S_OK;
}

void CModel_Instance::Update_Animation(const _double& _dTimeDelta)
{
	for (_uint i = 0; i < m_iGroupNumber; ++i)
	{
		if (m_CurIndexGroup[i] >= (_uint)m_vecAnimationGroup[i].size())
			return;

		if (nullptr == m_CurrentAnimationGroup[i])
			m_CurrentAnimationGroup[i] = m_vecAnimationGroup[i][0];

		if (nullptr != m_NextAnimationGroup[i])
		{
			m_CurrentAnimationGroup[i]->Updatet_NextAnim_TransformMatrix(_dTimeDelta, m_NextAnimationGroup[i]);
			if (false == m_CurrentAnimationGroup[i]->Is_SupplementSituation())
			{
				m_PrevAnimationGroup[i] = m_CurrentAnimationGroup[i];
				m_CurrentAnimationGroup[i] = m_NextAnimationGroup[i];
				m_NextAnimationGroup[i] = nullptr;
				m_bChangeAnimation = true;
				/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
				m_CurrentAnimationGroup[i]->Update_TransformMatrices();
			}
		}
		else
		{
			/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
			m_CurrentAnimationGroup[i]->Update_TransformMatrices(_dTimeDelta);
			m_bChangeAnimation = false;
		}

		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : m_vecHierarchyGroup[i])
			pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

HRESULT CModel_Instance::Update_Value(TRANSFORMVECTOR Transforms, const _uint& _iIndex)
{
	for (auto& iter : m_vecMeshContainers)
		iter->Update_Value(m_eModelType, Transforms, CMethod::XMMatrixLoadFloat4x4_P(m_PivotMatrix), _iIndex);
	return S_OK;
}

HRESULT CModel_Instance::Update_Value(F4POSVECTOR f4PosVector, const _uint& _iIndex)
{
	for (auto& iter : m_vecMeshContainers)
		iter->Update_Value(m_eModelType, f4PosVector, CMethod::XMMatrixLoadFloat4x4_P(m_PivotMatrix), _iIndex);
	return S_OK;
}

HRESULT CModel_Instance::Update_Value(MATRIXES _Matrix, const _uint& _iIndex)
{
	for (auto& iter : m_vecMeshContainers)
		iter->Update_Value(m_eModelType, _Matrix, CMethod::XMMatrixLoadFloat4x4_P(m_PivotMatrix), _iIndex);
	return S_OK;
}

HRESULT CModel_Instance::Bind_SRV(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType)
{
	if (m_vecMaterials.size() <= 0)
		return E_FAIL;

	if (nullptr == m_vecMeshContainers[_iMeshContainerIndex])
		return E_FAIL;

	_uint iMatrialIndex = m_vecMeshContainers[_iMeshContainerIndex]->Get_MaterialIndex();

	if (nullptr != m_vecMaterials[iMatrialIndex].pMaterialTexture[_eTextureType])
	{
		m_vecMaterials[iMatrialIndex].pMaterialTexture[_eTextureType]->SetUp_ConstantTable_Index(_pShader, _pConstantName, 0);
	}

	return S_OK;
}

_bool CModel_Instance::Check_Texture(const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType)
{
	if (m_vecMaterials.size() <= 0)
		return false;

	if (nullptr == m_vecMeshContainers[_iMeshContainerIndex])
	{
		return false;
	}

	_uint iMatrialIndex = m_vecMeshContainers[_iMeshContainerIndex]->Get_MaterialIndex();


	if (nullptr != m_vecMaterials[iMatrialIndex].pMaterialTexture[_eTextureType])
	{
		return true;
	}

	return false;
}

HRESULT CModel_Instance::Render( const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex)
{
	if (nullptr != m_vecMeshContainers[_iMeshContainersIndex])
	{
		_float4x4* m_pBoneMatrices = new _float4x4[m_iGroupNumber * 1024];
		for (_uint i = 0; i < m_iGroupNumber; ++i)
		{
			if (i >= 64)
				break;
			_float4x4 BoneMatrices[1024];
			if (0 != m_vecAnimations.size())
			{
				m_vecMeshContainers[_iMeshContainersIndex]->SetUp_BoneMatrices(i, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			}
			memcpy(&m_pBoneMatrices[i * 1024], BoneMatrices, sizeof(_float4x4) * 1024);
		}
		if (FAILED(Bind_BoneTexture(m_pBoneMatrices, _pShader, m_iGroupNumber * 1024)))
			return E_FAIL;
		Safe_Delete_Array(m_pBoneMatrices);
		//_pShader->Set_RawValue("g_Bones", BoneMatrices, sizeof(_float4x4) * 512);
		_pShader->Begin(_iPassIndex);
         m_vecMeshContainers[_iMeshContainersIndex]->Render();
	}
	return S_OK;
}

HRESULT CModel_Instance::Render_NonAnim(const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex)
{
	if (nullptr != m_vecMeshContainers[_iMeshContainersIndex])
	{
		_pShader->Begin(_iPassIndex);
		m_vecMeshContainers[_iMeshContainersIndex]->Render();
	}
	return S_OK;
}

HRESULT CModel_Instance::Bind_BoneTexture(_float4x4* BoneData, CShader* _pShader, _uint iSize)
{
	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRESULT		hr = 0;

	hr = m_pContext->Map(m_pBoneTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	if (FAILED(hr))
		return E_FAIL;

	memcpy(MappedSubResource.pData, BoneData, sizeof(_float4x4) * iSize);

	m_pContext->Unmap(m_pBoneTexture, 0);

	if (FAILED(_pShader->Set_SRV("g_BoneTexture", m_pBoneSRV)))
		return E_FAIL;

	return S_OK;
}

void CModel_Instance::Set_Animation(const _uint& _iIndex, const _uint& _iAnimIndex)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	if (m_vecAnimationGroup[_iIndex].size() <= _iAnimIndex)
		m_CurIndexGroup[_iIndex] = 0;
	m_CurIndexGroup[_iIndex] = _iAnimIndex;
	m_CurrentAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][_iAnimIndex];
	m_CurrentAnimationGroup[_iIndex]->Reset_Data();
	m_NextAnimationGroup[_iIndex] = nullptr;
}

void CModel_Instance::Set_Animation(const _uint& _iIndex, const char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			m_CurrentAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
			m_CurIndexGroup[_iIndex] = i;
			m_NextAnimationGroup[_iIndex] = nullptr;
			break;
		}
	}
}

void CModel_Instance::Change_Animation(const _uint& _iIndex, const _char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			if (m_CurrentAnimationGroup[_iIndex] == m_vecAnimationGroup[_iIndex][i])
				return;

			if (nullptr == m_CurrentAnimationGroup[_iIndex])
			{
				m_CurrentAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
				m_CurIndexGroup[_iIndex] = i;
				// Finish 초기화
				m_CurrentAnimationGroup[_iIndex]->Set_Finished(false);
				m_CurrentAnimationGroup[_iIndex]->Set_SupplementSituation(true);
			}
			else
			{
				m_NextAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
				// Finish 초기화
				m_CurrentAnimationGroup[_iIndex]->Set_Finished(false);
				m_CurrentAnimationGroup[_iIndex]->Set_SupplementSituation(true);
				m_NextAnimationGroup[_iIndex]->Set_Finished(false);
			}
			break;
		}
	}
}

void CModel_Instance::Change_SameAnimation(const _uint& _iIndex, const _char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			m_CurrentAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
			m_CurIndexGroup[_iIndex] = i;
			// Finish 초기화
			m_CurrentAnimationGroup[_iIndex]->Reset_Data();
			m_CurrentAnimationGroup[_iIndex]->Update_TransformMatrices();
			break;
		}
	}
}

void CModel_Instance::Change_SameAnimationSup(const _uint& _iIndex, const _char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			m_NextAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
			// Finish 초기화
			m_CurrentAnimationGroup[_iIndex]->Set_Finished(false);
			m_CurrentAnimationGroup[_iIndex]->Set_SupplementSituation(true);
			m_NextAnimationGroup[_iIndex]->Set_Finished(false);
			break;
		}
	}
}

void CModel_Instance::Repeat_Animation(const _uint& _iIndex, const _char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			if (m_CurrentAnimationGroup[_iIndex] == m_vecAnimationGroup[_iIndex][i])
				return;

			m_CurrentAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
			m_CurIndexGroup[_iIndex] = i;
			break;
		}
	}
}

void CModel_Instance::Set_NextAnimation(const _uint& _iIndex, const _char* _pName)
{
	if (m_iGroupNumber <= _iIndex)
		return;

	for (_uint i = 0; i < (_uint)m_vecAnimationGroup[_iIndex].size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimationGroup[_iIndex][i]->Get_Name().c_str()))
		{
			if (m_NextAnimationGroup[_iIndex] == m_vecAnimationGroup[_iIndex][i])
				return;

			m_NextAnimationGroup[_iIndex] = m_vecAnimationGroup[_iIndex][i];
			m_NextAnimationGroup[_iIndex]->Set_Finished(false);
			break;
		}
	}
}

HRESULT CModel_Instance::Create_MeshConstainers(const MODELDESC& _pData, _fmatrix _mPivotMatrix)
{
	m_iNumMeshContainers = _pData.iMeshContainersCount;
	m_vecMeshContainers.reserve(m_iNumMeshContainers + 1);
	for (auto& iter : _pData.MeshList)
	{
		CMeshContainer_Instance* pMeshContainer = CMeshContainer_Instance::Create(m_pDevice, m_pContext, this, iter
			, _mPivotMatrix);

		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_vecMeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel_Instance::Create_MeshConstainers_Anim(const ANIMMODELDESC& _pData, _fmatrix _mPivotMatrix)
{
	m_iNumMeshContainers = _pData.iMeshContainersCount;
	m_vecMeshContainers.reserve(m_iNumMeshContainers + 1);
	for (auto& iter : _pData.MeshList)
	{
		CMeshContainer_Instance* pMeshContainer = CMeshContainer_Instance::Create_Anim(m_pDevice, m_pContext, this, iter
			, _mPivotMatrix);

		if (nullptr == pMeshContainer)
			return E_FAIL;
		m_vecMeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel_Instance::Create_Materials(const COMMODELDATA& _pData)
{
	m_iNumMatrials = _pData.iMatrialCount;

	for (auto& iter : _pData.MatrialData)
	{
		MODELMATRIALDESC ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATRIALDESC));

		for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
		{
			if (lstrcmp(iter.second[i].c_str(), L""))
			{
				ModelMaterial.pMaterialTexture[i] = CTexture::Create(m_pDevice, m_pContext, iter.second[i]);
				if (nullptr == ModelMaterial.pMaterialTexture[i])
					continue;
			}
		}
		m_vecMaterials.push_back(ModelMaterial);
	}
	return S_OK;
}

HRESULT CModel_Instance::Create_HierachyNodes(const vector<HNODEDESC>& _tDescList)
{
	for (auto& iter : _tDescList)
	{
		CHierarchyNode_Instance* pNode = CHierarchyNode_Instance::Create(this, iter);
		if (nullptr == pNode)
			return E_FAIL;

		m_vecHierachyNodes.push_back(pNode);
	}
	return S_OK;
}

HRESULT CModel_Instance::Create_Animation(const vector<ANIMDESC>& _tDescList)
{
	for (auto& iter : _tDescList)
	{
		CAnimation_Instance* pNode = CAnimation_Instance::Create(this, iter);
		if (nullptr == pNode)
			return E_FAIL;

		m_vecAnimations.push_back(pNode);
	}
	return S_OK;
}

HRESULT CModel_Instance::Create_BoneTexture()
{
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 512;
	TextureDesc.Height = 512;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_float4* pPixel = new _float4[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_float4) * TextureDesc.Width * TextureDesc.Height);

	for (_uint i = 0; i < TextureDesc.Width * TextureDesc.Height; ++i)
	{
		pPixel[i] = _float4(0.f, 0.f, 0.f, 0.f);
	}

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = TextureDesc.Width * sizeof(_float4);

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &m_pBoneTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBoneTexture, nullptr, &m_pBoneSRV)))
		return E_FAIL;
	Safe_Delete_Array(pPixel);

	return S_OK;
}

#pragma region LOAD

HRESULT CModel_Instance::Load_Data_NonAnim(MODELDESC& _tModelDesc)
{
	_wstring strFilePath = m_pFileData->strFilePath;
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CModel::Load_Data_NonAnim To Failed");
		return E_FAIL;
	}
	MODELDESC tData;
	DWORD dwByte = 0;

	ReadFile(hFile, &tData.iMatrialCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &tData.iMeshContainersCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &tData.iHierachyCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < tData.iMatrialCount; ++i)
	{
		_uint MaterialIndex = 0;
		ReadFile(hFile, &MaterialIndex, sizeof(_uint), &dwByte, nullptr);
		vector<_wstring> Textures;
		_uint iTextureSize = 0;
		ReadFile(hFile, &iTextureSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < iTextureSize; ++j)
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			Textures.push_back(pTextureName);
			Safe_Delete_Array(pTextureName);
		}
		tData.MatrialData.insert(pair<_uint, vector<_wstring>>(MaterialIndex, Textures));
	}

	for (_uint i = 0; i < tData.iMeshContainersCount; ++i)
	{
		MESHDESC tMesh;
		ReadFile(hFile, &tMesh.iMatrialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tMesh.iNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tMesh.iNumIndices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tMesh.iHierachyCount, sizeof(_uint), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tMesh.strName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		tMesh.pVtxModel = new VTXMODEL[tMesh.iNumVertices];
		ReadFile(hFile, tMesh.pVtxModel, sizeof(VTXMODEL) * tMesh.iNumVertices, &dwByte, nullptr);

		tMesh.pIndices = new FACEINDICES32[tMesh.iNumIndices];
		ReadFile(hFile, tMesh.pIndices, sizeof(FACEINDICES32) * tMesh.iNumIndices, &dwByte, nullptr);

		tMesh.pVerticesPos = new _float3[tMesh.iNumVertices];
		ReadFile(hFile, tMesh.pVerticesPos, sizeof(_float3) * tMesh.iNumVertices, &dwByte, nullptr);

		for (_uint j = 0; j < tMesh.iHierachyCount; ++j)
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pName = new _tchar[iLength];
			ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tMesh.HierachyNameList.push_back(pName);
			Safe_Delete_Array(pName);
		}

		tData.MeshList.push_back(tMesh);
	}

	for (_uint i = 0; i < tData.iHierachyCount; ++i)
	{
		HNODEDESC tHNodeDesc;
		ReadFile(hFile, &tHNodeDesc.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &tHNodeDesc.OffsetMatrix, sizeof(_float4x4), &dwByte, 0);
		ReadFile(hFile, &tHNodeDesc.fDepth, sizeof(_float), &dwByte, nullptr);
		// Name
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pName = new _tchar[iLength];
			ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tHNodeDesc.strName = pName;
			Safe_Delete_Array(pName);
		}
		// Parents Name
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pName = new _tchar[iLength];
			ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tHNodeDesc.strParentsName = pName;
			Safe_Delete_Array(pName);
		}

		tData.HNodeDataList.push_back(tHNodeDesc);
	}
	CloseHandle(hFile);
	// Model Data
	_tModelDesc = tData;
	return S_OK;
}

HRESULT CModel_Instance::Load_Data_Anim(ANIMMODELDESC& _tModelDesc)
{
	_wstring strFilePath = m_pFileData->strFilePath;
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CModel::Load_Data_NonAnim To Failed");
		return E_FAIL;
	}
	ANIMMODELDESC tData;
	DWORD dwByte = 0;

	ReadFile(hFile, &tData.iMatrialCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &tData.iMeshContainersCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &tData.iHierachyCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &tData.iAnimCount, sizeof(_uint), &dwByte, nullptr);
	// 머터리얼
	{
		for (_uint i = 0; i < tData.iMatrialCount; ++i)
		{
			_uint MaterialIndex = 0;
			ReadFile(hFile, &MaterialIndex, sizeof(_uint), &dwByte, nullptr);
			vector<_wstring> Textures;
			_uint iTextureSize = 0;
			ReadFile(hFile, &iTextureSize, sizeof(_uint), &dwByte, nullptr);

			for (_uint j = 0; j < iTextureSize; ++j)
			{
				_uint iLength = 0;
				ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
				_tchar* pTextureName = new _tchar[iLength];
				ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
				Textures.push_back(pTextureName);
				Safe_Delete_Array(pTextureName);
			}
			tData.MatrialData.insert(pair<_uint, vector<_wstring>>(MaterialIndex, Textures));
		}
	}
	// 메쉬
	{
		for (_uint i = 0; i < tData.iMeshContainersCount; ++i)
		{
			ANIMMESHDESC tMesh;
			ReadFile(hFile, &tMesh.iMatrialIndex, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &tMesh.iNumBones, sizeof(_uint), &dwByte, 0);
			ReadFile(hFile, &tMesh.iNumVertices, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &tMesh.iNumIndices, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &tMesh.iHierachyCount, sizeof(_uint), &dwByte, nullptr);
			{
				_uint iLength = 0;
				ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
				_tchar* pTextureName = new _tchar[iLength];
				ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
				tMesh.strName = pTextureName;
				Safe_Delete_Array(pTextureName);
			}
			tMesh.pVtxModel = new VTXANIMMODEL[tMesh.iNumVertices];
			ReadFile(hFile, tMesh.pVtxModel, sizeof(VTXANIMMODEL) * tMesh.iNumVertices, &dwByte, nullptr);

			tMesh.pIndices = new FACEINDICES32[tMesh.iNumIndices];
			ReadFile(hFile, tMesh.pIndices, sizeof(FACEINDICES32) * tMesh.iNumIndices, &dwByte, nullptr);

			tMesh.pVerticesPos = new _float3[tMesh.iNumVertices];
			ReadFile(hFile, tMesh.pVerticesPos, sizeof(_float3) * tMesh.iNumVertices, &dwByte, nullptr);

			for (_uint j = 0; j < tMesh.iHierachyCount; ++j)
			{
				_uint iLength = 0;
				ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
				_tchar* pName = new _tchar[iLength];
				ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
				tMesh.HierachyNameList.push_back(pName);
				Safe_Delete_Array(pName);
			}

			tData.MeshList.push_back(tMesh);
		}
	}
	// HNode
	{
		for (_uint i = 0; i < tData.iHierachyCount; ++i)
		{
			HNODEDESC tHNodeDesc;
			ReadFile(hFile, &tHNodeDesc.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &tHNodeDesc.OffsetMatrix, sizeof(_float4x4), &dwByte, 0);
			ReadFile(hFile, &tHNodeDesc.fDepth, sizeof(_float), &dwByte, nullptr);
			{
				_uint iLength = 0;
				ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
				_tchar* pName = new _tchar[iLength];
				ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
				tHNodeDesc.strName = pName;
				Safe_Delete_Array(pName);
			}
			{
				_uint iLength = 0;
				ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
				_tchar* pName = new _tchar[iLength];
				ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
				tHNodeDesc.strParentsName = pName;
				Safe_Delete_Array(pName);
			}

			tData.HNodeDataList.push_back(tHNodeDesc);
		}
		// Animation
		{
			for (_uint i = 0; i < tData.iAnimCount; ++i)
			{
				ANIMDESC tAnimDesc;
				ReadFile(hFile, &tAnimDesc.iNumChannels, sizeof(_uint), &dwByte, 0);
				ReadFile(hFile, &tAnimDesc.dTickPerSeconds, sizeof(_double), &dwByte, 0);
				ReadFile(hFile, &tAnimDesc.dDuration, sizeof(_double), &dwByte, 0);
				{
					_uint iLength = 0;
					ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
					_tchar* pName = new _tchar[iLength];
					ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
					tAnimDesc.strName = pName;
					Safe_Delete_Array(pName);
				}
				for (_uint j = 0; j < tAnimDesc.iNumChannels; ++j)
				{
					CHANNELDESC tChannelDesc;
					ReadFile(hFile, &tChannelDesc.iNumMaxKeyFrames, sizeof(_uint), &dwByte, 0);
					{
						_uint iLength = 0;
						ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
						_tchar* pName = new _tchar[iLength];
						ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
						tChannelDesc.strName = pName;
						Safe_Delete_Array(pName);
					}
					tChannelDesc.pKeyFrames = new KEYFRAME[tChannelDesc.iNumMaxKeyFrames];
					ReadFile(hFile, tChannelDesc.pKeyFrames, sizeof(KEYFRAME) * tChannelDesc.iNumMaxKeyFrames, &dwByte, 0);

					tAnimDesc.vecChannels.push_back(tChannelDesc);
				}
				tData.AnimList.push_back(tAnimDesc);
			}
		}
	}
	_tModelDesc = tData;
	CloseHandle(hFile);
	hFile = nullptr;
	return S_OK;
}

#pragma endregion LOAD

CModel_Instance* CModel_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, 
	FILEGROUP* _pFileGroup, FILEDATA* _pFileData,  _fmatrix _mPivotMatrix)
{
	CModel_Instance* pInstance = new CModel_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_eModelType, _pFileGroup, _pFileData, _mPivotMatrix)))
	{
#ifdef _DEBUG
		MSG_BOX("CModel_Instance::Create To Failed");
#endif
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType,
	FILEGROUP* _pFileGroup, const _wstring& _strName,  _fmatrix _mPivotMatrix)
{
	CModel_Instance* pInstance = new CModel_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_eModelType, _pFileGroup, _strName, _mPivotMatrix)))
	{
#ifdef _DEBUG
		MSG_BOX("CModel_Instance::Create To Failed");
#endif
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, 
	PATHS* _pPaths, const _wstring& _strName, _fmatrix _mPivotMatrix)
{
	CModel_Instance* pInstance = new CModel_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_eModelType, _pPaths, _strName, _mPivotMatrix)))
	{
#ifdef _DEBUG
		MSG_BOX("CModel_Instance::Create To Failed");
#endif
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel_Instance::Clone(void* _pArg)
{
	CModel_Instance* pInstance = new CModel_Instance(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CModel_Instance::Clone To Failed");
#endif
	}
	return pInstance;
}

void CModel_Instance::Free()
{
	if (m_vecMaterials.size() > 0)
	{
		for (auto& MaterialDesc : m_vecMaterials)
		{
			for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
				Safe_Release(MaterialDesc.pMaterialTexture[i]);
		}
	}

	for (auto& pMeshContainer : m_vecMeshContainers)
		Safe_Release(pMeshContainer);

	if (true == m_bCloned)
	{
		Safe_Release_DV(m_pBoneSRV);
		Safe_Release_DV(m_pBoneTexture);
		for (auto& iter : m_vecHierarchyGroup)
		{
			for (auto& value : iter)
				Safe_Release(value);
		}
		m_vecHierarchyGroup.clear();
		for (auto& iter : m_vecAnimationGroup)
		{
			for (auto& value : iter)
				Safe_Release(value);
		}
		m_vecAnimationGroup.clear();
	}

	for (auto& iter : m_vecAnimations)
		Safe_Release(iter);

	for (auto& iter : m_vecHierachyNodes)
		Safe_Release(iter);

	m_vecAnimations.clear();
	m_vecHierachyNodes.clear();
	m_vecMeshContainers.clear();
	__super::Free();
}
