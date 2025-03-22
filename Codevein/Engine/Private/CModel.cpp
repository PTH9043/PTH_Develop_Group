#include "CModel.h"
#include "CGameInstance.h"
#include "CMeshContainer.h"
#include "CHierarchyNode.h"
#include "CAnimation.h"

CHierarchyNode* CModel::Get_HierarchyNodes(const char* pHierarchyNodeName)
{
	auto	iter = find_if(m_vecHierachyNodes.begin(), m_vecHierachyNodes.end(), [&](CHierarchyNode* _pNode)
	{
		return !strcmp(pHierarchyNodeName, _pNode->Get_Name().c_str());
	});

	if (m_vecHierachyNodes.end() == iter)
		return nullptr;

	return *iter;
}

CAnimation* CModel::Get_NextAnimation()
{
	if (nullptr == m_pNextAnimations)
		return m_pCurAnimations;

	return m_pNextAnimations;
}

CAnimation* CModel::Get_PrevAnimation()
{
	return m_pPrevAnimation;
}

void CModel::Set_Texture(CTexture* _pTexture, const TEXTYPE& _eTexType, const _uint& _iMaterialIndex)
{
	if (nullptr == _pTexture)
		return;

	if (TEXTYPE::TextureType_UNKNOWN <= _eTexType)
		return;

	if (m_vecMaterials.size() <= 0)
		m_vecMaterials.resize(m_vecHierachyNodes.size());

	Safe_Release(m_vecMaterials[_iMaterialIndex]->pMaterialTexture[_eTexType]);
	m_vecMaterials[_iMaterialIndex]->pMaterialTexture[_eTexType] = _pTexture;
	Safe_AddRef(_pTexture);
}

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& _rhs)
	: CComponent(_rhs), m_vecMeshContainers(_rhs.m_vecMeshContainers), m_iNumMeshContainers(_rhs.m_iNumMeshContainers),
	m_iNumMatrials(_rhs.m_iNumMatrials), m_vecMaterials(_rhs.m_vecMaterials), 
	m_vecHierachyNodes(_rhs.m_vecHierachyNodes), m_vecAnimations(_rhs.m_vecAnimations),
	m_pModelFileGroup(_rhs.m_pModelFileGroup), m_pFileData(_rhs.m_pFileData), m_PivotMatrix(_rhs.m_PivotMatrix)
{ 
	for (auto& iter : m_vecMeshContainers)
		Safe_AddRef(iter);

	for (auto& iter : m_vecMaterials)
	{
		for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
			Safe_AddRef(iter->pMaterialTexture[i]);
	}

	for (auto& iter : m_vecHierachyNodes)
		Safe_AddRef(iter);

	for (auto& iter : m_vecAnimations)
		Safe_AddRef(iter);
}

void CModel::Save_Animation(FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return;

	HANDLE hFile = NULL;
	_wstring strPath = _pFileGroup->strPath;
	strPath.append(L"\\");
	strPath.append(L"Animations");
	_wmkdir(strPath.c_str());
	for (auto& iter : m_vecAnimations)
	{
		_string str = iter->Get_Name();
		_wstring wstr;
		wstr.assign(str.begin(), str.end());
		_wstring Path = strPath;
		Path.append(L"\\");
		Path.append(wstr);
		Path.append(L".bin");
		HANDLE hFile = CreateFile(Path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwByte;
		if (INVALID_HANDLE_VALUE == hFile)
		{
			CloseHandle(hFile);
			return;
		}

		iter->Save_Data(hFile, dwByte);
		CloseHandle(hFile);
	}
}

void CModel::Load_Animation(FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return;

	HANDLE hFile = NULL;
	for (auto& iter : _pFileGroup->vecFileDataList)
	{
		hFile = CreateFile(iter.strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwByte;
		if (INVALID_HANDLE_VALUE == hFile)
		{
			CloseHandle(hFile);
			return;
		}
		ANIMDESC tAnimDesc;
		tAnimDesc.Load_Data(hFile, dwByte);

		CloseHandle(hFile);

		tAnimDesc.strName.assign(iter.strFileName.begin(), iter.strFileName.end());
		CAnimation* _pAnimation = CAnimation::Create(this, tAnimDesc);
		if (nullptr == _pAnimation)
			return;

		_pAnimation->Load_AnimationFastSection(_pFileGroup);
		_pAnimation->Load_AnimationClipSection(_pFileGroup);
		m_vecAnimations.push_back(_pAnimation);
	}
}

HRESULT CModel::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CModel::NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, FILEDATA* _pFileData, _fmatrix _mPivotMatrix)
{
	if (nullptr == _pFileGroup && nullptr == _pFileData)
		return E_FAIL;

	m_pModelFileGroup = _pFileGroup;
	m_pFileData = _pFileData;

	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);

	if (TYPE_NONANIM == _eModelType)
	{
		MODELDESC tData;
		if (FAILED(Load_Data_NonAnim(tData)))
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

		if (FAILED(Create_MeshConstainers_Anim(tData, _mPivotMatrix)))
			return E_FAIL;

		if (FAILED(Create_Materials(tData)))
			return E_FAIL;

		if (FAILED(Create_Animation(tData.AnimList)))
			return E_FAIL;


	}
	return S_OK;
}

HRESULT CModel::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	m_bCloned = true;

	MESHCONTAINERS		MeshContainers;
	HIERARCHYNODES		HierachyNodes;
	ANIMATIONS				Animations;

	for (auto& iter : m_vecHierachyNodes)
	{
		HierachyNodes.push_back(iter->Clone_Node());
		Safe_Release(iter);
	}
	m_vecHierachyNodes.clear();
	m_vecHierachyNodes = HierachyNodes;

	for (auto& iter : m_vecHierachyNodes)
		iter->Find_Parents(this);

	for (auto& iter : m_vecMeshContainers)
	{
		MeshContainers.push_back((CMeshContainer*)iter->Clone(this));
		Safe_Release(iter);
	}
	m_vecMeshContainers.clear();
	m_vecMeshContainers = MeshContainers;

	for (auto& iter : m_vecAnimations)
	{
		Animations.push_back(iter->Clone_Animation(this));
		Safe_Release(iter);
	}
	m_vecAnimations.clear();
	m_vecAnimations = Animations;
	return S_OK;
}

void CModel::Update_Animation(const _double& _dTimeDelta)
{
	if (m_iCurAnimIndex >= (_uint)m_vecAnimations.size())
		return;

	if (nullptr == m_pCurAnimations)
		m_pCurAnimations = m_vecAnimations[0];


	if (nullptr != m_pNextAnimations)
	{
		m_pCurAnimations->Updatet_NextAnim_TransformMatrix(_dTimeDelta, m_pNextAnimations);
		if (false == m_pCurAnimations->Is_SupplementSituation())
		{
			m_pPrevAnimation = m_pCurAnimations;
			m_pCurAnimations = m_pNextAnimations;
			m_pNextAnimations = nullptr;
			m_bChangeAnimation = true;
			/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
			m_pCurAnimations->Update_TransformMatrices();
		}
	}
	else
	{
		/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
		m_pCurAnimations->Update_TransformMatrices(_dTimeDelta);
		m_bChangeAnimation = false;
	}

	/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
	for (auto& pHierarchyNode : m_vecHierachyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix();
}

HRESULT CModel::Bind_SRV(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType)
{
	if (m_vecMaterials.size() <= 0)
		return E_FAIL;

	if (nullptr == m_vecMeshContainers[_iMeshContainerIndex])
		return E_FAIL;

	_uint iMatrialIndex = m_vecMeshContainers[_iMeshContainerIndex]->Get_MaterialIndex();

	if (nullptr != m_vecMaterials[iMatrialIndex]->pMaterialTexture[_eTextureType])
	{
		m_vecMaterials[iMatrialIndex]->pMaterialTexture[_eTextureType]->SetUp_ConstantTable_Index(_pShader, _pConstantName, 0);
	}

	return S_OK;
}

HRESULT CModel::Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const  _uint& _iPassIndex)
{
	if (nullptr != m_vecMeshContainers[_iMeshContainersIndex])
	{
		_float4x4		BoneMatrices[256];

		for (_uint i = 0; i < 256; ++i)
			XMStoreFloat4x4(&BoneMatrices[i], XMMatrixIdentity());

		if (0 != m_vecAnimations.size())
		{
			m_vecMeshContainers[_iMeshContainersIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
		}

		_pShader->Set_RawValue("g_Bones", BoneMatrices, sizeof(_float4x4) * 256);
		_pShader->Begin(_iPassIndex);
		m_vecMeshContainers[_iMeshContainersIndex]->Render();
	}
	return S_OK;
}

HRESULT CModel::Render_NonAnim(const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex)
{
	if (nullptr != m_vecMeshContainers[_iMeshContainersIndex])
	{
		_pShader->Begin(_iPassIndex);
		m_vecMeshContainers[_iMeshContainersIndex]->Render();
	}
	return S_OK;
}

void CModel::Set_Animation(const _uint& _iAnimIndex)
{
	this->m_iCurAnimIndex = _iAnimIndex;  	

	if (m_vecAnimations.size() <= _iAnimIndex)
		m_iCurAnimIndex = 0;
	m_iCurAnimIndex = _iAnimIndex;
	m_pCurAnimations = m_vecAnimations[m_iCurAnimIndex]; 
	m_pCurAnimations->Reset_Data();
	m_pNextAnimations = nullptr;
}

void CModel::Set_Animation(const char* _pName)
{
	for(_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			m_pCurAnimations = m_vecAnimations[i];
			m_iCurAnimIndex = i;
			m_pNextAnimations = nullptr;
			break;
		}
	}
}

void CModel::Change_Animation(const _uint& _iAnimation)
{
	if (m_vecAnimations.size() <= _iAnimation)
		m_iNextAnimIndex = 0;

	if (m_pCurAnimations == m_vecAnimations[_iAnimation])
		return;

	this->m_iNextAnimIndex = _iAnimation;
	m_pNextAnimations = m_vecAnimations[m_iCurAnimIndex];
	m_pCurAnimations->Set_Finished(false);
	m_pCurAnimations->Set_SupplementSituation(true);
	m_pNextAnimations->Set_Finished(false);
}

void CModel::Change_Animation(const _char* _pName)
{
	for (_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			if (m_pCurAnimations == m_vecAnimations[i])
				return;

			if (nullptr == m_pCurAnimations)
			{
				m_pCurAnimations = m_vecAnimations[i];
				m_iCurAnimIndex = i;
				m_pCurAnimations->Set_Finished(false);
				m_pCurAnimations->Set_SupplementSituation(true);
			}
			else
			{
				m_pNextAnimations = m_vecAnimations[i];
				m_iNextAnimIndex = i;
				m_pCurAnimations->Set_Finished(false);
				m_pCurAnimations->Set_SupplementSituation(true);
				m_pNextAnimations->Set_Finished(false);
		//		m_pNextAnimations->Reset_Data();
			}
			break;
		}
	}
}

void CModel::Change_SameAnimation(const _char* _pName)
{
	for (_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			m_pCurAnimations = m_vecAnimations[i];
			m_iCurAnimIndex = i;
			m_pCurAnimations->Reset_Data();
			m_pCurAnimations->Update_TransformMatrices();
			break;
		}
	}
}

void CModel::Change_SameAnimationSup(const char* _pName)
{
	for (_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			m_pNextAnimations = m_vecAnimations[i];
			m_iNextAnimIndex = i;
			m_pCurAnimations->Set_Finished(false);
			m_pCurAnimations->Set_SupplementSituation(true);
			m_pNextAnimations->Set_Finished(false);
			break;
		}
	}
}

void CModel::Repeat_Animation(const _char* _pName)
{
	for (_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			if (m_pCurAnimations == m_vecAnimations[i])
				return;

			m_pCurAnimations = m_vecAnimations[i];
			m_iCurAnimIndex = i;
			break;
		}
	}
}

void CModel::Set_NextAnimation(const _char* _pName)
{
	for (_uint i = 0; i < (_uint)m_vecAnimations.size(); ++i)
	{
		if (!strcmp(_pName, m_vecAnimations[i]->Get_Name().c_str()))
		{
			if (m_pNextAnimations == m_vecAnimations[i])
				return;

			m_pNextAnimations = m_vecAnimations[i];
			m_iNextAnimIndex = i;
			m_pCurAnimations->Set_Finished(false);
			break;
		}
	}
}

HRESULT CModel::Create_MeshConstainers(const MODELDESC& _pData, _fmatrix _mPivotMatrix)
{
	m_iNumMeshContainers = _pData.iMeshContainersCount;
	m_vecMeshContainers.reserve(m_iNumMeshContainers + 1);
	for (auto& iter : _pData.MeshList)
	{
		CMeshContainer* pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, this, iter, _mPivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;
		m_vecMeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel::Create_MeshConstainers_Anim(const ANIMMODELDESC& _pData,  _fmatrix _mPivotMatrix)
{
	m_iNumMeshContainers = _pData.iMeshContainersCount;
	m_vecMeshContainers.reserve(m_iNumMeshContainers + 1);
	for (auto& iter : _pData.MeshList)
	{
		CMeshContainer* pMeshContainer = CMeshContainer::Create_Anim(m_pDevice, m_pContext, this, iter, _mPivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;
		m_vecMeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel::Create_Materials(const COMMODELDATA& _pData)
{
	m_iNumMatrials = _pData.iMatrialCount;

	for (auto& iter : _pData.MatrialData)
	{
		MODELMATRIALDESC* pModelMaterial = new MODELMATRIALDESC;
		ZeroMemory(pModelMaterial, sizeof(MODELMATRIALDESC));

		for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
		{
			if (lstrcmp(iter.second[i].c_str(), L""))
			{
				pModelMaterial->pMaterialTexture[i] = CTexture::Create(m_pDevice, m_pContext, iter.second[i]);
				if (nullptr == pModelMaterial->pMaterialTexture[i])
					return E_FAIL;
			}
		}
		m_vecMaterials.push_back(pModelMaterial);
	}
	return S_OK;
}

HRESULT CModel::Create_HierachyNodes(const vector<HNODEDESC>& _tDescList)
{
	for (auto& iter : _tDescList)
	{
		CHierarchyNode* pNode = CHierarchyNode::Create(this, iter);
		if (nullptr == pNode)
			return E_FAIL;

		m_vecHierachyNodes.push_back(pNode);
	}
	return S_OK;
}

HRESULT CModel::Create_Animation(const vector<ANIMDESC>& _tDescList)
{
	for (auto& iter : _tDescList)
	{
		CAnimation* pNode = CAnimation::Create(this, iter);
		if (nullptr == pNode)
			return E_FAIL;

		m_vecAnimations.push_back(pNode);
	}
	return S_OK;
}

#pragma region LOAD

HRESULT CModel::Load_Data_NonAnim(MODELDESC& _tModelDesc)
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

HRESULT CModel::Load_Data_Anim(ANIMMODELDESC& _tModelDesc)
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
			for(_uint i = 0; i < tData.iAnimCount; ++i)
			{
				ANIMDESC tAnimDesc;
				ReadFile(hFile, &tAnimDesc.iNumChannels, sizeof(_uint), &dwByte, 0);
				ReadFile(hFile, &tAnimDesc.dTickPerSeconds, sizeof(_double), &dwByte, 0);
				ReadFile(hFile, &tAnimDesc.dDuration, sizeof(_double), &dwByte, 0);
				{
					_uint iLength = 0;
					ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
					_tchar* pName = new _tchar[iLength];
					ReadFile(hFile, pName, sizeof(_tchar)* iLength, &dwByte, nullptr);
					tAnimDesc.strName = pName;
					Safe_Delete_Array(pName);
				}
				for(_uint j = 0; j < tAnimDesc.iNumChannels; ++j)
				{
					CHANNELDESC tChannelDesc;
					ReadFile(hFile, &tChannelDesc.iNumMaxKeyFrames, sizeof(_uint), &dwByte, 0);
					{
						_uint iLength = 0;
						ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
						_tchar* pName = new _tchar[iLength];
						ReadFile(hFile, pName, sizeof(_tchar)* iLength, &dwByte, nullptr);
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

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
	FILEDATA* _pFileData,  _fmatrix _mPivotMatrix)
{
	CModel* pModel = new CModel(_pDevice, _pContext);
	if (FAILED(pModel->NativeConstruct_Prototype(_eModelType, _pFileGroup, _pFileData, _mPivotMatrix)))
	{
		Safe_Release(pModel);
		MSG_BOX("CModel::Create To Failed");
	}
	return pModel;
}

CComponent* CModel::Clone(void* _pArg)
{
	CModel* pModel = new CModel(*this);
	if (FAILED(pModel->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pModel);
		MSG_BOX("CModel::Create To Failed");
	}
	return pModel;
}

void CModel::Free()
{
	if (m_vecMaterials.size() > 0)
	{
		for (auto& pMaterialDesc : m_vecMaterials)
		{
			for (_uint i = 0; i < TEXTYPE::TextureType_UNKNOWN; ++i)
				Safe_Release(pMaterialDesc->pMaterialTexture[i]);
		}

		for (auto& pMaterial : m_vecMaterials)
			if (false == m_bCloned)
				Safe_Delete_Array(pMaterial);
	}

	for (auto& iter : m_vecAnimations)
	{
		iter->Save_AnimationFastSection(m_pModelFileGroup);
		iter->Save_AnimationClipSection(m_pModelFileGroup);
		Safe_Release(iter);
	}

	for (auto& pMeshContainer : m_vecMeshContainers)
		Safe_Release(pMeshContainer);

	for (auto& iter : m_vecHierachyNodes)
		Safe_Release(iter);

	m_vecAnimations.clear();
	m_vecHierachyNodes.clear();
	m_vecMeshContainers.clear();
	__super::Free();
}
