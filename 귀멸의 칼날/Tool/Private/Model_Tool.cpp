#include "AssimpFIleLoad_ Tool.h" 
#include "Model_Tool.h"
#include "MeshContainer_Tool.h"
#include "HierarchyNode_Tool.h"
#include "Animation_Tool.h"
#include "GameInstance.h"

CModel_Tool::CModel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());
}

CModel_Tool::CModel_Tool(const CModel_Tool& _rhs)
	: CComponent(_rhs)
	, m_pScene(_rhs.m_pScene)
	, m_MeshContainers(_rhs.m_MeshContainers)
	, m_Animations(_rhs.m_Animations)
	, m_Materials(_rhs.m_Materials)
	, m_iNumMeshContainers(_rhs.m_iNumMeshContainers)
	, m_iNumMaterials(_rhs.m_iNumMaterials)
	, m_PivotMatrix(_rhs.m_PivotMatrix)
	, m_iNumAnimations(_rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(_rhs.m_iCurrentAnimIndex)
{
	for (auto& pMeshContainer : m_MeshContainers)
		Safe_AddRef(pMeshContainer);

	for (auto& pMaterialDesc : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterialDesc->pMaterialTexture[i]);
	}

	for (auto& iter : m_Animations)
		Safe_AddRef(iter);
}

CHierarchyNode_Tool* CModel_Tool::Get_HierarchyNodes(const char* pHierarchyNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode_Tool* pNode)
	{
		return !strcmp(pHierarchyNodeName, pNode->Get_Name());
	});

	if (m_HierarchyNodes.end() == iter)
		return nullptr;

	return *iter;
}


HRESULT CModel_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CModel_Tool::NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, 
	const _uint& _iIndex, const _wstring& _strTextureFolderName,  const _fmatrix& _mPivotMatrix)
{
	if (nullptr == _pFileGroup)
		return E_FAIL;

	if (_pFileGroup->vecFileDataList.size() <= _iIndex)
		return E_FAIL;

	m_pFileData = &_pFileGroup->vecFileDataList[_iIndex];
	_wstring strFilePath = _pFileGroup->vecFileDataList[_iIndex].strFilePath;
	_string strPath = CMethod::ConvertWToS(strFilePath);

	m_strTextureFolderName = _strTextureFolderName;

	m_pModelFileGroup = _pFileGroup;
	_uint iFlag = 0;

	if (TYPE_NONANIM == _eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	
	m_Importer = new Importer;
	m_pScene = m_Importer->ReadFile(strPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;
	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);

	if (FAILED(Create_HierachyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode_Tool* pSour, CHierarchyNode_Tool* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Create_MeshConstainers(_eModelType, _mPivotMatrix)))
		return E_FAIL;
	if (TYPE_NONANIM == _eModelType)
	{
		if (FAILED(Create_Materials(_pFileGroup->pParentsUpper)))
		{
			Create_Materials(_pFileGroup);
		}
	}
	else
	{
		if (FAILED(Create_Materials(_pFileGroup)))
			return E_FAIL;
	}

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Tool::NativeConstruct_Prototype(const TYPE& _eModelType, 
	FILEGROUP* _pFileGroup, FILEDATA* _pFileData, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	if (nullptr == _pFileGroup || nullptr == _pFileData)
		return E_FAIL;

	_wstring strFilePath = _pFileData->strFilePath;
	_string strPath = CMethod::ConvertWToS(strFilePath);

	m_strTextureFolderName = _strTextureFolderName;
	m_pModelFileGroup = _pFileGroup;
	m_pFileData = _pFileData;

	_uint iFlag = 0;
	if (TYPE_NONANIM == _eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_Importer = new Importer;
	m_pScene = m_Importer->ReadFile(strPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);
	if (FAILED(Create_HierachyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode_Tool* pSour, CHierarchyNode_Tool* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Create_MeshConstainers(_eModelType, _mPivotMatrix)))
		return E_FAIL;

	if (FAILED(Create_Materials(_pFileGroup->pParentsUpper)))
	{
		Create_Materials(_pFileGroup);
	}

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Tool::NativeConstruct_Prototype(const TYPE& _eModelType, 
	FILEGROUP* _pFileGroup, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	if (nullptr == _pFileGroup)
		return E_FAIL;

	if (_pFileGroup->vecFileDataList.size() <= 0)
		return E_FAIL;

	m_pFileData = _pFileGroup->FindData(_strModelName.c_str());
	_wstring strFilePath = m_pFileData->strFilePath;
	_string strPath = CMethod::ConvertWToS(strFilePath);

	m_strTextureFolderName = _strTextureFolderName;

	m_pModelFileGroup = _pFileGroup;
	_uint iFlag = 0;

	if (TYPE_NONANIM == _eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_Importer = new Importer;
	m_pScene = m_Importer->ReadFile(strPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;
	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);

	if (FAILED(Create_HierachyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode_Tool* pSour, CHierarchyNode_Tool* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Create_MeshConstainers(_eModelType, _mPivotMatrix)))
		return E_FAIL;
	if (TYPE_NONANIM == _eModelType)
	{
		if (FAILED(Create_Materials(_pFileGroup->pParentsUpper)))
		{
			Create_Materials(_pFileGroup);
		}
	}
	else
	{
		if (FAILED(Create_Materials(_pFileGroup)))
			return E_FAIL;
	}

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Tool::NativeConstruct_Prototype(const TYPE& _eModelType, PATHS* _pPaths, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pModelFileGroup = pGameInstance->Find_Folder(_pPaths);
	}
	RELEASE_INSTANCE(CGameInstance);
	m_pFileData = m_pModelFileGroup->FindData(_strModelName.c_str());
	_wstring strFilePath = m_pFileData->strFilePath;
	_string strPath = CMethod::ConvertWToS(strFilePath);

	m_strTextureFolderName = _strTextureFolderName;
	_uint iFlag = 0;

	if (TYPE_NONANIM == _eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_Importer = new Importer;
	m_pScene = m_Importer->ReadFile(strPath, iFlag);

	if (nullptr == m_pScene)
		return E_FAIL;
	XMStoreFloat4x4(&m_PivotMatrix, _mPivotMatrix);

	if (FAILED(Create_HierachyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode_Tool* pSour, CHierarchyNode_Tool* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Create_MeshConstainers(_eModelType, _mPivotMatrix)))
		return E_FAIL;
	if (TYPE_NONANIM == _eModelType)
	{
		if (FAILED(Create_Materials(m_pModelFileGroup->pParentsUpper)))
		{
			Create_Materials(m_pModelFileGroup);
		}
	}
	else
	{
		if (FAILED(Create_Materials(m_pModelFileGroup)))
			return E_FAIL;
	}

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Tool::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	m_bCloned = true;

	if (FAILED(Create_HierachyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode_Tool* pSour, CHierarchyNode_Tool* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	vector<CMeshContainer_Tool*> MeshContainers;
	for (auto& iter : m_MeshContainers)
	{
		CMeshContainer_Tool* pMesh = static_cast<CMeshContainer_Tool*>(iter->Clone(this));
		Safe_Release(iter);
		MeshContainers.push_back(pMesh);
	}
	m_MeshContainers.clear();
	m_MeshContainers = MeshContainers;

	ANIMATIONS				Animations;
	for (auto& iter : m_Animations)
	{
		Animations.push_back(iter->Clone_Animation(this));
		Safe_Release(iter);
	}

	m_Animations.clear();
	m_Animations = Animations;
	return S_OK;
}

void CModel_Tool::Update_Animation(const _double& _dDeltaTime)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	if (nullptr == m_pCurAnimation)
		m_pCurAnimation = m_Animations[0];

	/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
	m_Animations[m_iCurrentAnimIndex]->Update_TransformMatrices(_dDeltaTime);

	/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix();
}

HRESULT CModel_Tool::Bind_SRV(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshContainerIndex, const aiTextureType& _eTextureType)
{
	if (m_Materials.size() <= 0)
		return E_FAIL;

	_uint iMatrialIndex = m_MeshContainers[_iMeshContainerIndex]->Get_MaterialIndex();

	if (nullptr == m_Materials[iMatrialIndex])
		return E_FAIL;

	if (nullptr == m_Materials[iMatrialIndex]->pMaterialTexture)
		return E_FAIL;

	if (nullptr != m_Materials[iMatrialIndex]->pMaterialTexture[_eTextureType])
		m_Materials[iMatrialIndex]->pMaterialTexture[_eTextureType]->SetUp_ConstantTable_Index(_pShader, _pConstantName, 0);

	return S_OK;
}

HRESULT CModel_Tool::Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex)
{
	if (nullptr != m_MeshContainers[_iMeshContainersIndex])
	{
		_float4x4* BoneMatrices = new _float4x4[256];

		m_MeshContainers[_iMeshContainersIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		_pShader->Set_RawValue("g_Bones", BoneMatrices, sizeof(_float4x4) * 256);

		_pShader->Begin(_iPassIndex);
		m_MeshContainers[_iMeshContainersIndex]->Render();

		Safe_Delete_Array(BoneMatrices);
	}
	return   S_OK;
}

HRESULT CModel_Tool::Create_Animation(FILEGROUP* _pFileGroup, FILEDATA* _pFileData)
{
	const aiScene* pScene = nullptr;

	string str = CMethod::ConvertWToS(_pFileData->strFilePath);
	pScene = m_Importer->ReadFile(str.c_str(), aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (nullptr == pScene)
		return E_FAIL;

	for (_uint i = 0; i < pScene->mNumAnimations; ++i)
	{
		++m_iNumAnimations;
		aiAnimation* pAIAnim = pScene->mAnimations[i];
		CAnimation_Tool* pAnimation = CAnimation_Tool::Create(pAIAnim, this, _pFileData->strFileName);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	m_Importer->FreeScene();

	return S_OK;
}

void CModel_Tool::Set_Animation(const _uint& _iAnimIndex)
{
	this->m_iCurrentAnimIndex = _iAnimIndex;

	if (m_Animations.size() <= _iAnimIndex)
		m_iCurrentAnimIndex = 0;
	else
		return;

	if (0 == m_Animations.size())
		return;

	m_pCurAnimation = m_Animations[m_iCurrentAnimIndex];
}

void CModel_Tool::Set_Animation(const char* _pName)
{
	for (_uint i = 0; i < (_uint)m_Animations.size(); ++i)
	{
		if (!strcmp(_pName, m_Animations[i]->Get_Name()))
		{
			m_pCurAnimation = m_Animations[i];
			m_iCurrentAnimIndex = i;
			break;
		}
	}
}

HRESULT CModel_Tool::Create_MeshConstainers(const TYPE& _eType, const _fmatrix& _mPivotMatrix)
{
	m_iNumMeshContainers = m_pScene->mNumMeshes;
	m_MeshContainers.reserve(m_iNumMeshContainers + 1);
	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer_Tool* pMeshContainer = CMeshContainer_Tool::Create(m_pDevice, m_pContext,
			m_pScene->mMeshes[i], _eType, this, _mPivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel_Tool::Create_Materials(FILEGROUP*& _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return E_FAIL;

	FILEGROUP* pFileGroup = _pFileGroup->Find_Group(m_strTextureFolderName.c_str());
	if (nullptr == pFileGroup)
	{
		FILEGROUP* pNextFind = _pFileGroup;
		while (true)
		{
			pNextFind = pNextFind->pParentsUpper;
			if (nullptr == pNextFind)
				break;
			pFileGroup = pNextFind->Find_Group(L"Textures");
			if (nullptr != pFileGroup)
				break;
		}

		if (nullptr == pFileGroup)
			return E_FAIL;
	}

	if (pFileGroup->vecFileDataList.size() <= 0)
		return E_FAIL;

	m_iNumMaterials = m_pScene->mNumMaterials;
	_wstring strPath = pFileGroup->strPath;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial* _pAIMaterial = m_pScene->mMaterials[i];

		MODELMATRIALDESC* pModelMaterial = new MODELMATRIALDESC;
		ZeroMemory(pModelMaterial, sizeof(MODELMATRIALDESC));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString strTexturePath;
			char szName[MAX_PATH] = "";

			if (FAILED(_pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			strcat_s(szFileName, szExt);
			_tchar		szRealName[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFileName, (_uint)strlen(szFileName), szRealName, MAX_PATH);

			_wstring Path = strPath;
			Path.append(L"\\");
			Path.append(szRealName);

			pModelMaterial->pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, Path.c_str());
			if (nullptr == pModelMaterial->pMaterialTexture[j])
			{
				Safe_Delete_Array(pModelMaterial);
				return E_FAIL;
			}
		}
		m_Materials.push_back(pModelMaterial);
	}
	return S_OK;
}

HRESULT CModel_Tool::Create_HierachyNodes(aiNode* _pNode, CHierarchyNode_Tool* _pParents, const _uint& _iDepths)
{
	if (nullptr == _pNode)
		return S_OK;

	_float4x4		TransformationMatrix;
	memcpy(&TransformationMatrix, &_pNode->mTransformation, sizeof(_float4x4));

	CHierarchyNode_Tool* pHierarchyNode = CHierarchyNode_Tool::Create(_pParents, _pNode->mName.data, XMLoadFloat4x4(&TransformationMatrix), _iDepths);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < _pNode->mNumChildren; ++i)
	{
		Create_HierachyNodes(_pNode->mChildren[i], pHierarchyNode, _iDepths + 1);
	}

	return S_OK;
}

HRESULT CModel_Tool::Create_Animation()
{
	m_iNumAnimations = m_pScene->mNumAnimations;
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation* pAIAnim = m_pScene->mAnimations[i];

		CAnimation_Tool* pAnimation = CAnimation_Tool::Create(pAIAnim, this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

void CModel_Tool::Get_Data(MODELDESC& _tModelDesc)
{
	for (auto& iter : m_MeshContainers)
	{
		MESHDESC tMeshDesc;
		iter->Get_Data(tMeshDesc);

		_tModelDesc.MeshList.push_back(tMeshDesc);
	}
	if (m_Materials.size() > 0)
	{
		for (_uint i = 0; i < (_uint)m_Materials.size(); ++i)
		{
			vector<_wstring> strTextureFileName;

			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
			{
				CTexture* pTexture = m_Materials[i]->pMaterialTexture[j];
				if (nullptr != pTexture)
					strTextureFileName.push_back(pTexture->Get_TextureName(0));
				else
					strTextureFileName.push_back(L"");
			}

			_tModelDesc.MatrialData.insert(pair<_uint, vector<_wstring>>(i, strTextureFileName));
		}
	}

	for (_uint i = 0; i < (_uint)m_HierarchyNodes.size(); ++i)
	{
		HNODEDESC tNode;
		m_HierarchyNodes[i]->Get_Data(tNode);
		_tModelDesc.HNodeDataList.push_back(tNode);
	}

	_tModelDesc.iHierachyCount = (_uint)m_HierarchyNodes.size();
	_tModelDesc.iMeshContainersCount = m_iNumMeshContainers;
	_tModelDesc.iMatrialCount = m_iNumMaterials;
	_tModelDesc.pFileData = m_pFileData;
	_tModelDesc.pFileGroup = m_pModelFileGroup;
}

void CModel_Tool::Get_Data(ANIMMODELDESC& _tAnimModelDesc)
{
	for (auto& iter : m_MeshContainers)
	{
		ANIMMESHDESC tMeshDesc;
		iter->Get_Data(tMeshDesc);

		_tAnimModelDesc.MeshList.push_back(tMeshDesc);
	}

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		vector<_wstring> strTextureFileName;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			CTexture* pTexture = m_Materials[i]->pMaterialTexture[j];
			if (nullptr != pTexture)
				strTextureFileName.push_back(pTexture->Get_TextureName(0));
			else
				strTextureFileName.push_back(L"");
		}

		_tAnimModelDesc.MatrialData.insert(pair<_uint, vector<_wstring>>(i, strTextureFileName));
	}

	for (_uint i = 0; i < (_uint)m_HierarchyNodes.size(); ++i)
	{
		HNODEDESC tNode;
		m_HierarchyNodes[i]->Get_Data(tNode);
		_tAnimModelDesc.HNodeDataList.push_back(tNode);
	}

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		ANIMDESC tAnimDesc;
		m_Animations[i]->Get_Data(tAnimDesc);
		_tAnimModelDesc.AnimList.push_back(tAnimDesc);
	}

	_tAnimModelDesc.iAnimCount = m_iNumAnimations;
	_tAnimModelDesc.iHierachyCount = (_uint)m_HierarchyNodes.size();
	_tAnimModelDesc.iMeshContainersCount = m_iNumMeshContainers;
	_tAnimModelDesc.iMatrialCount = m_iNumMaterials;
	_tAnimModelDesc.pFileData = m_pFileData;
	_tAnimModelDesc.pFileGroup = m_pModelFileGroup;
}

void CModel_Tool::Save_NonAnimModel()
{
	MODELDESC tData;
	Get_Data(tData);

	FILEGROUP* pFileGroup = m_pModelFileGroup;
	FILEDATA* pFileData = m_pFileData;

	_wstring strPath = pFileGroup->strPath, strFileName = pFileData->strFileName;

	strPath.append(L"\\");
	strPath.append(L"FBX_Converter");
	_wmkdir(strPath.c_str());
	strPath.append(L"\\");
	strPath.append(strFileName);
	strPath.append(L".bin");
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CModel_FBX::Save_NonAnimModel to Failed");
		cout << strFileName.c_str() << endl;
		return;
	}

	DWORD dwByte;
	// Matrial Count
	tData.iMatrialCount = static_cast<_uint>(tData.MatrialData.size());
	WriteFile(hFile, &tData.iMatrialCount, sizeof(_uint), &dwByte, 0);
	// MeshCount
	WriteFile(hFile, &tData.iMeshContainersCount, sizeof(_uint), &dwByte, 0);
	// MeshCount
	WriteFile(hFile, &tData.iHierachyCount, sizeof(_uint), &dwByte, 0);
	// Material
	for (auto& MATERIAL : tData.MatrialData)
	{
		WriteFile(hFile, &MATERIAL.first, sizeof(_uint), &dwByte, 0);
		_uint iTextureSize = (_uint)MATERIAL.second.size();
		WriteFile(hFile, &iTextureSize, sizeof(_uint), &dwByte, 0);
		for (auto& TEXTURENAME : MATERIAL.second)
		{
			_uint Length = (_uint)lstrlen(TEXTURENAME.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, TEXTURENAME.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}

	for (auto& MESH : tData.MeshList)
	{
		WriteFile(hFile, &MESH.iMatrialIndex, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iNumVertices, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iNumIndices, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iHierachyCount, sizeof(_uint), &dwByte, 0);
		{
			_uint Length = (_uint)lstrlen(MESH.strName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, MESH.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		for (_uint i = 0; i < MESH.iNumVertices; ++i)
		{
			WriteFile(hFile, &MESH.pVtxModel[i], sizeof(VTXMODEL), &dwByte, 0);
		}

		for (_uint i = 0; i < MESH.iNumIndices; ++i)
		{
			WriteFile(hFile, &MESH.pIndices[i], sizeof(FACEINDICES32), &dwByte, 0);
		}

		for (_uint i = 0; i < MESH.iNumVertices; ++i)
		{
			WriteFile(hFile, &MESH.pVerticesPos[i], sizeof(_float3), &dwByte, 0);
		}

		for (auto& HIERNAME : MESH.HierachyNameList)
		{
			_uint Length = (_uint)lstrlen(HIERNAME.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, HIERNAME.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}

	for (auto& Hier : tData.HNodeDataList)
	{
		WriteFile(hFile, &Hier.TransformMatrix, sizeof(_float4x4), &dwByte, 0);
		WriteFile(hFile, &Hier.OffsetMatrix, sizeof(_float4x4), &dwByte, 0);
		WriteFile(hFile, &Hier.fDepth, sizeof(_float), &dwByte, 0);
		{
			_uint Length = (_uint)lstrlen(Hier.strName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, Hier.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		{
			_uint Length = (_uint)lstrlen(Hier.strParentsName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, Hier.strParentsName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}
	CloseHandle(hFile);
}

void CModel_Tool::Save_AnimModel()
{
	ANIMMODELDESC tData;
	Get_Data(tData);

	FILEGROUP* pFileGroup = m_pModelFileGroup;
	FILEDATA* pFileData = m_pFileData;

	_wstring strPath = pFileGroup->strPath, strFileName = pFileData->strFileName;

	strPath.append(L"\\");
	strPath.append(L"FBX_Converter");
	_wmkdir(strPath.c_str());
	strPath.append(L"\\");
	strPath.append(strFileName);
	strPath.append(L".bin");
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CModel_FBX::Save_NonAnimModel to Failed");
		cout << strFileName.c_str() << endl;
		return;
	}

	DWORD dwByte;
	// Matrial Count
	WriteFile(hFile, &tData.iMatrialCount, sizeof(_uint), &dwByte, 0);
	// MeshCount
	WriteFile(hFile, &tData.iMeshContainersCount, sizeof(_uint), &dwByte, 0);
	// MeshCount
	WriteFile(hFile, &tData.iHierachyCount, sizeof(_uint), &dwByte, 0);
	// MeshCount
	WriteFile(hFile, &tData.iAnimCount, sizeof(_uint), &dwByte, 0);
	// Material
	for (auto& MATERIAL : tData.MatrialData)
	{
		WriteFile(hFile, &MATERIAL.first, sizeof(_uint), &dwByte, 0);
		_uint iTextureSize = (_uint)MATERIAL.second.size();
		WriteFile(hFile, &iTextureSize, sizeof(_uint), &dwByte, 0);
		for (auto& TEXTURENAME : MATERIAL.second)
		{
			_uint Length = (_uint)lstrlen(TEXTURENAME.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, TEXTURENAME.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}
	// MeshData
	for (auto& MESH : tData.MeshList)
	{
		WriteFile(hFile, &MESH.iMatrialIndex, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iNumBones, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iNumVertices, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iNumIndices, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &MESH.iHierachyCount, sizeof(_uint), &dwByte, 0);
		{
			_uint Length = (_uint)lstrlen(MESH.strName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, MESH.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, MESH.pVtxModel, sizeof(VTXANIMMODEL) * MESH.iNumVertices, &dwByte, 0);
		WriteFile(hFile, MESH.pIndices, sizeof(FACEINDICES32) * MESH.iNumIndices, &dwByte, 0);
		WriteFile(hFile, MESH.pVerticesPos, sizeof(_float3) * MESH.iNumVertices, &dwByte, 0);

		for (auto& HIERNAME : MESH.HierachyNameList)
		{
			_uint Length = (_uint)lstrlen(HIERNAME.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, HIERNAME.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}

	for (auto& Hier : tData.HNodeDataList)
	{
		WriteFile(hFile, &Hier.TransformMatrix, sizeof(_float4x4), &dwByte, 0);
		WriteFile(hFile, &Hier.OffsetMatrix, sizeof(_float4x4), &dwByte, 0);
		WriteFile(hFile, &Hier.fDepth, sizeof(_float), &dwByte, 0);
		{
			_uint Length = (_uint)lstrlen(Hier.strName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, Hier.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		{
			_uint Length = (_uint)lstrlen(Hier.strParentsName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, Hier.strParentsName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}

	for (auto& ANIM : tData.AnimList)
	{
		WriteFile(hFile, &ANIM.iNumChannels, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, &ANIM.dTickPerSeconds, sizeof(_double), &dwByte, 0);
		WriteFile(hFile, &ANIM.dDuration, sizeof(_double), &dwByte, 0);
		_uint Length = (_uint)lstrlen(ANIM.strName.c_str()) + 1;
		WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
		WriteFile(hFile, ANIM.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);

		for (auto& CHANNEL : ANIM.vecChannels)
		{
			WriteFile(hFile, &CHANNEL.iNumMaxKeyFrames, sizeof(_uint), &dwByte, 0);
			_uint Length = (_uint)lstrlen(CHANNEL.strName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, CHANNEL.strName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);

			for (_uint i = 0; i < CHANNEL.iNumMaxKeyFrames; ++i)
				WriteFile(hFile, &CHANNEL.pKeyFrames[i], sizeof(KEYFRAME), &dwByte, 0);
		}
	}

	CloseHandle(hFile);
}

CModel_Tool* CModel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup, const _uint& _iIndex, 
	const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	CModel_Tool* pModel = new CModel_Tool(_pDevice, _pContext);
	if (FAILED(pModel->NativeConstruct_Prototype(_eModelType, _pFileGroup, _iIndex, _strTextureFolderName, _mPivotMatrix)))
	{
		Safe_Release(pModel);
#ifdef _DEBUG
	     MSG_BOX("CModel_Tool::Create To Failed");
#endif
	}
	return pModel;
}

CModel_Tool* CModel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
	FILEDATA* _pFileData, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	CModel_Tool* pModel = new CModel_Tool(_pDevice, _pContext);
	if (FAILED(pModel->NativeConstruct_Prototype(_eModelType, _pFileGroup, _pFileData, _strTextureFolderName, _mPivotMatrix)))
	{
		Safe_Release(pModel);
#ifdef _DEBUG
		MSG_BOX("CModel_Tool::Create To Failed");
#endif
	}
	return pModel;
}

CModel_Tool* CModel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, 
	FILEGROUP* _pFileGroup, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	CModel_Tool* pModel = new CModel_Tool(_pDevice, _pContext);
	if (FAILED(pModel->NativeConstruct_Prototype(_eModelType, _pFileGroup, _strModelName, _strTextureFolderName, _mPivotMatrix)))
	{
		Safe_Release(pModel);
#ifdef _DEBUG
		MSG_BOX("CModel_Tool::Create To Failed");
#endif
	}
	return pModel;
}

CModel_Tool* CModel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, PATHS* _pFileGroup, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix)
{
	return nullptr;
}

CComponent* CModel_Tool::Clone(void* _pArg)
{
	CModel_Tool* pModel = new CModel_Tool(*this);
	if (FAILED(pModel->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pModel);
#ifdef _DEBUG
		MSG_BOX("CModel_Tool::Clone To Failed");
#endif
	}
	return pModel;
}

void CModel_Tool::Free()
{
	for (auto& pMaterialDesc : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMaterialDesc->pMaterialTexture[i]);

		if (false == m_bCloned)
			Safe_Delete(pMaterialDesc);
	}

	for (auto& iter : m_Animations)
		Safe_Release(iter);

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);

	for (auto& iter : m_HierarchyNodes)
		Safe_Release(iter);

	if(nullptr != m_Importer)
	{
		m_Importer->FreeScene();
		Safe_Delete(m_Importer);
	}

	__super::Free();
}
