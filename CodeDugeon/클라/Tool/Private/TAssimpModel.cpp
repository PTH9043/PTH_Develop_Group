#include "ToolDefines.h"
#include "TAssimpModel.h"
#include "TAssimpAnimation.h"
#include "TAssimpBoneNode.h"
#include "TAssimpChannel.h"
#include "UGameInstance.h"
#include "TAssimpMeshContainer.h"
#include "UTexture.h"
#include "UTexGroup.h"
#include "UMethod.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

TAssimpModel::TAssimpModel(CSHPTRREF<UDevice> _spDevice) :
	UResource(_spDevice),
	m_spImporter{ nullptr },
	m_vecMeshContainers{},
	m_iNumMeshContainers{ 0 },
	m_BoneNodeContainer{},
	m_iNumBoneNodes{ 0 },
	m_vecAnimations{},
	m_iNumAnimation{ 0 },
	m_MaterialContainer{},
	m_iNumMaterials{ 0 },
	m_mScaleMatrix{ _float4x4::Identity },
	m_spFileGroup{ nullptr },
	m_spFileData{ nullptr },
	m_eModelType{ TYPE::NONANIM },
	m_wstrModelName{ L"" },
	m_wstrTextureFolderName{ L"" },
	m_pAIScene{ nullptr },
	m_wstrTexturePath{ L"" },
	m_isLoadAnimationCountIsZero{false}
{
}

TAssimpModel::TAssimpModel(const TAssimpModel& _rhs) :
	UResource(_rhs),
	m_spImporter{ _rhs.m_spImporter },
	m_vecMeshContainers{ _rhs.m_vecMeshContainers },
	m_iNumMeshContainers{ _rhs.m_iNumMeshContainers },
	m_BoneNodeContainer{ _rhs.m_BoneNodeContainer },
	m_iNumBoneNodes{ _rhs.m_iNumBoneNodes },
	m_vecAnimations{ _rhs.m_vecAnimations },
	m_iNumAnimation{ _rhs.m_iNumAnimation },
	m_MaterialContainer{ _rhs.m_MaterialContainer },
	m_iNumMaterials{ _rhs.m_iNumMaterials },
	m_mScaleMatrix{ _rhs.m_mScaleMatrix },
	m_spFileGroup{ _rhs.m_spFileGroup },
	m_spFileData{ _rhs.m_spFileData },
	m_eModelType{ _rhs.m_eModelType },
	m_wstrModelName{ L"" },
	m_wstrTextureFolderName{ L"" },
	m_pAIScene{ nullptr },
	m_wstrTexturePath{ L"" },
	m_isLoadAnimationCountIsZero{ false }
{
}

SHPTR<TAssimpBoneNode> TAssimpModel::FindBoneNode(const _wstring& _strBoneNode)
{
	const BONENODES::iterator& it = std::find_if(m_BoneNodeContainer.begin(), m_BoneNodeContainer.end(), [&](const SHPTR<TAssimpBoneNode>& p) {
		if (p->GetBoneName() == _strBoneNode)
			return true;

		return false;
		});
	RETURN_CHECK(m_BoneNodeContainer.end() == it, nullptr);
	return *it;
}

void TAssimpModel::Free()
{
	m_vecMeshContainers.clear();
}

HRESULT TAssimpModel::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT TAssimpModel::NativeConstruct(const TYPE& _eModelType, const _wstring& _wstrPath, const _float4x4& _mPivotMatrix, const _wstring& _wstrTextureFolder)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_wstrTextureFolderName = _wstrTextureFolder;
	m_eModelType = _eModelType;
	m_mScaleMatrix = _mPivotMatrix;
	return 	CreateModel(_wstrPath);
}

HRESULT TAssimpModel::NativeConstruct(const TYPE& _eModelType, CSHPTRREF<FILEGROUP> _spFileGroup, CSHPTRREF<FILEDATA> _spFileData, const _float4x4& _mPivotMatrix, const _wstring& _wstrTextureFolder)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_wstrTextureFolderName = _wstrTextureFolder;
	RETURN_CHECK(nullptr == _spFileGroup || nullptr == _spFileData, E_FAIL);
	m_spFileGroup = _spFileGroup; m_spFileData = _spFileData;
	m_eModelType = _eModelType;
	m_mScaleMatrix = _mPivotMatrix;
	return 	CreateModel(m_spFileData->wstrfilePath);
}

HRESULT TAssimpModel::NativeConstruct(const TYPE& _eModelType, const PATHS& _vecPaths, const _wstring& _wstrFileName, const _float4x4& _mPivotMatrix, const _wstring& _wstrTextureFolder)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_wstrTextureFolderName = _wstrTextureFolder;
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spFileGroup = spGameInstance->FindFolder(_vecPaths);
	RETURN_CHECK(nullptr == m_spFileGroup, E_FAIL);
	m_spFileData = m_spFileGroup->FindData(_wstrFileName);
	RETURN_CHECK(nullptr == m_spFileData, E_FAIL);
	m_eModelType = _eModelType;
	m_mScaleMatrix = _mPivotMatrix;
	return 	CreateModel(m_spFileData->wstrfilePath);
}

HRESULT TAssimpModel::NativeConstruct(const TYPE& _eModelType, const _wstring& _wstrModelFolder, const _wstring& _wstrFileName, const _float4x4& _mPivotMatrix, const _wstring& _wstrTextureFolder)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_wstrTextureFolderName = _wstrTextureFolder;
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spFileGroup = spGameInstance->FindFolder(_wstrModelFolder);
	RETURN_CHECK(nullptr == m_spFileGroup, E_FAIL);
	m_spFileData = m_spFileGroup->FindData(_wstrFileName);
	RETURN_CHECK(nullptr == m_spFileData, E_FAIL);
	m_mScaleMatrix = _mPivotMatrix;
	return 	CreateModel(m_spFileData->wstrfilePath);
}

HRESULT TAssimpModel::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	return S_OK;
}


void TAssimpModel::Render(const _uint _iMeshIndex, const SRV_REGISTER _eRegister, const aiTextureFlags& _eTextureType, CSHPTRREF<UShader> _spShade)
{

}

void TAssimpModel::GetData(MODELDESC& _tModelDesc)
{
	// MeshContainery
	for (auto& iter : m_vecMeshContainers) {
		MESHDESC tMeshDesc;
		iter->GetData(tMeshDesc);
		_tModelDesc.Meshes.push_back(tMeshDesc);
	}
	// Materials
	int i = 0;
	for (auto& iter : m_MaterialContainer) {
		VECTOR<_wstring> strTextureFileName{};
		for (auto& value : iter->arrMaterialTexture) {
			if (nullptr != value)
			{
				// Extenstion 
				_wstring ext = m_lsTextureExts.front();

				_wstring TexturePath = m_wstrTexturePath;
				TexturePath += value->GetTextureName(0);
				TexturePath += ext;
				strTextureFileName.push_back(TexturePath);
				// 앞을 날린다. 
				m_lsTextureExts.pop_front();
			}
			else
			{
				strTextureFileName.push_back(L"NULL");
			}
		}
		_tModelDesc.MatrialData.insert(std::pair<_uint, VECTOR<_wstring>>(i++, strTextureFileName));
	}
	// BoneNode
	for (auto& iter : m_BoneNodeContainer) {
		BONENODEDESC tDesc;
		iter->GetData(tDesc);
		_tModelDesc.BNodeDatas.push_back(tDesc);
	}
	_tModelDesc.iNodeCnt = (_uint)m_BoneNodeContainer.size();
	_tModelDesc.iMeshContainersCount = m_iNumMeshContainers;
	_tModelDesc.iMatrialCount = m_iNumMaterials;
}

void TAssimpModel::GetData(ANIMMODELDESC& _tAnimModelDesc)
{
	// MeshContainery
	for (auto& iter : m_vecMeshContainers) {
		ANIMMESHDESC tMeshDesc;
		iter->GetData(tMeshDesc);
		_tAnimModelDesc.Meshes.push_back(tMeshDesc);
	}
	// Materials
	int i = 0;
	for (auto& iter : m_MaterialContainer) {
		VECTOR<_wstring> strTextureFileName{};
		for (auto& value : iter->arrMaterialTexture) {
			if (nullptr != value)
			{
				// Extenstion 
				_wstring ext = m_lsTextureExts.front();

				_wstring TexturePath = m_wstrTexturePath;
				TexturePath += value->GetTextureName(0);
				TexturePath += ext;
				strTextureFileName.push_back(TexturePath);
				// 앞을 날린다. 
				m_lsTextureExts.pop_front();
			}
			else
			{
				strTextureFileName.push_back(L"NULL");
			}
		}
		_tAnimModelDesc.MatrialData.insert(std::pair<_uint, VECTOR<_wstring>>(i++, strTextureFileName));
	}
	// BoneNode
	for (auto& iter : m_BoneNodeContainer) {
		BONENODEDESC tDesc;
		iter->GetData(tDesc);
		_bool bIsTrue{ false };
		for (auto& Mesh : m_vecMeshContainers)
		{
			if (Mesh->GetMeshName() == tDesc.wstrName)
			{
				bIsTrue = true;
				break;
			}
		}
		if (bIsTrue)
			continue;

		_tAnimModelDesc.BNodeDatas.push_back(tDesc);
	}
	// Animation
	for (auto& iter : m_vecAnimations) {
		ANIMDESC tDesc;
		iter->GetData(tDesc);
		_tAnimModelDesc.Animes.push_back(tDesc);
	}

	_tAnimModelDesc.iNodeCnt = (_uint)m_BoneNodeContainer.size();
	_tAnimModelDesc.iMeshContainersCount = m_iNumMeshContainers;
	_tAnimModelDesc.iMatrialCount = m_iNumMaterials;
}

void TAssimpModel::SaveNonAnimModel(const _wstring& _wstrPath, _wstring& _wstrConvertPath)
{
	MODELDESC stModelDesc;
	GetData(stModelDesc);
	_wstring dir = _wstrPath;
	ConvertPath(dir);
	_wstrConvertPath = dir;

	using namespace std;
	ofstream save{ dir, ios::binary };
	RETURN_CHECK(!save, ;);
	{
		// MESH
		{
			size_t SIZE = stModelDesc.Meshes.size();
			save.write((_char*)&SIZE, sizeof(size_t));
			if (0 != SIZE)
			{
				for (auto& iter : stModelDesc.Meshes) {
					// Name 
					UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrName));
					// Vertex Model
					save.write((_char*)&iter.iNumVertices, sizeof(iter.iNumVertices));
					// Index Model
					save.write((_char*)&iter.iNumIndices, sizeof(iter.iNumIndices));
					for (_uint i = 0; i < iter.iNumVertices; ++i) {
						save.write((_char*)&iter.pVtxModel[i], sizeof(VTXMODEL));
					}
					for (_uint i = 0; i < iter.iNumIndices; ++i) {
						save.write((_char*)&iter.pIndices[i], sizeof(INDICIES32));
					}
					for (_uint i = 0; i < iter.iNumVertices; ++i) {
						save.write((_char*)&iter.pVtxModel[i].vPosition, sizeof(_float3));
					}
					{
						save.write((_char*)&iter.vMinVertex, sizeof(_float3));
						save.write((_char*)&iter.vMaxVertex, sizeof(_float3));
					}
					// BonCnt
					save.write((_char*)&iter.iBoneNodeCnt, sizeof(iter.iBoneNodeCnt));
					if (0 != iter.iBoneNodeCnt) {
						for (auto& BoneNodeNames : iter.BoneNodeNameList)
						{
							UMethod::SaveString(save, UMethod::ConvertWToS(BoneNodeNames));
						}
					}
					// Material Index
					save.write((_char*)&iter.iMatrialIndex, sizeof(iter.iMatrialIndex));
				}
			}
		}
		// BoneNode
		{
			size_t SIZE = stModelDesc.BNodeDatas.size();
			save.write((_char*)&SIZE, sizeof(size_t));
			if (0 != SIZE) {
				for (auto& iter : stModelDesc.BNodeDatas) {
					// Name
					save.write((_char*)&iter.mOffsetMatrix, sizeof(_float4x4));
					save.write((_char*)&iter.mTransformMatrix, sizeof(_float4x4));
					save.write((_char*)&iter.fDepth, sizeof(_float));
					UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrParentsName));
					UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrName));
				}
			}
		}
		// Material
		{
			size_t SIZE = stModelDesc.MatrialData.size();
			save.write((_char*)&SIZE, sizeof(size_t));
			if (0 != SIZE) {
				for (auto& iter : stModelDesc.MatrialData) {
					save.write((_char*)&iter.first, sizeof(iter.first));
					// Materials
					size_t STRING_SIZE = iter.second.size();
					save.write((_char*)&STRING_SIZE, sizeof(size_t));

					for (auto& TextureName : iter.second) {
						UMethod::SaveString(save, UMethod::ConvertWToS(TextureName));
					}
				}

				for (auto& iter : m_MaterialInfoContainer)
				{
					save.write((_char*)&iter, sizeof(MODELMATERIALINFO));
				}
			}
		}
	}
}

void TAssimpModel::SaveAnimModel(const _wstring& _wstrPath, _wstring& _wstrConvertPath)
{
	ANIMMODELDESC stModelDesc;
	GetData(stModelDesc);
	_wstring dir = _wstrPath;
	ConvertPath(dir);
	_wstrConvertPath = dir;

	using namespace std;
	ofstream save{ dir, ios::binary };
	RETURN_CHECK(!save, ;);
	{
		{
			// MESH
			{
				size_t SIZE = stModelDesc.Meshes.size();
				save.write((_char*)&SIZE, sizeof(size_t));
				if (0 != SIZE)
				{
					for (auto& iter : stModelDesc.Meshes) {
						// Name 
						UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrName));
						// Vertex Model
						save.write((_char*)&iter.iNumVertices, sizeof(iter.iNumVertices));
						// Index Model
						save.write((_char*)&iter.iNumIndices, sizeof(iter.iNumIndices));
						for (_uint i = 0; i < iter.iNumVertices; ++i) {
							save.write((_char*)&iter.pVtxModel[i], sizeof(VTXANIMMODEL));
						}
						for (_uint i = 0; i < iter.iNumIndices; ++i) {
							save.write((_char*)&iter.pIndices[i], sizeof(INDICIES32));
						}
						for (_uint i = 0; i < iter.iNumVertices; ++i) {
							save.write((_char*)&iter.pVtxModel[i].vPosition, sizeof(_float3));
						}
						{
							save.write((_char*)&iter.vMinVertex, sizeof(_float3));
							save.write((_char*)&iter.vMaxVertex, sizeof(_float3));
						}
						// BonCnt
						save.write((_char*)&iter.iBoneNodeCnt, sizeof(iter.iBoneNodeCnt));
						if (0 != iter.iBoneNodeCnt) {
							for (auto& BoneNodeNames : iter.BoneNodeNameList)
							{
								UMethod::SaveString(save, UMethod::ConvertWToS(BoneNodeNames));
							}
						}
						// Material Index
						save.write((_char*)&iter.iMatrialIndex, sizeof(iter.iMatrialIndex));
					}
				}
			}
			// BoneNode
			{
				size_t SIZE = stModelDesc.BNodeDatas.size();
				save.write((_char*)&SIZE, sizeof(size_t));
				if (0 != SIZE) {
					for (auto& iter : stModelDesc.BNodeDatas) {
						// Name
						save.write((_char*)&iter.mOffsetMatrix, sizeof(_float4x4));
						save.write((_char*)&iter.mTransformMatrix, sizeof(_float4x4));
						save.write((_char*)&iter.fDepth, sizeof(_float));
						UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrParentsName));
						UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrName));
					}
				}
			}
			// Material
			{
				size_t SIZE = stModelDesc.MatrialData.size();
				save.write((_char*)&SIZE, sizeof(size_t));
				if (0 != SIZE) {
					for (auto& iter : stModelDesc.MatrialData) {
						save.write((_char*)&iter.first, sizeof(iter.first));
						// Materials
						size_t STRING_SIZE = iter.second.size();
						save.write((_char*)&STRING_SIZE, sizeof(size_t));

						for (auto& TextureName : iter.second) {
							UMethod::SaveString(save, UMethod::ConvertWToS(TextureName));
						}
					}

					for (auto& iter : m_MaterialInfoContainer)
					{
						save.write((_char*)&iter, sizeof(MODELMATERIALINFO));
					}
				}
			}
		}
		// Animation 
		{
			_uint iSize = (_uint)stModelDesc.Animes.size();
			save.write((_char*)&iSize, sizeof(_uint));
			if (iSize != 0)
			{
				for (auto& iter : stModelDesc.Animes)
				{
					save.write((_char*)&iter.stExtraData, sizeof(ANIMEXTRADATA));
					UMethod::SaveString(save, UMethod::ConvertWToS(iter.wstrName));
					_uint iChannelSize = (_uint)iter.Channels.size();
					save.write((_char*)&iChannelSize, sizeof(_uint));
					for (auto& Channel : iter.Channels)
					{
						UMethod::SaveString(save, UMethod::ConvertWToS(Channel.wstrBoneName));
						save.write((_char*)&Channel.iNumMaxKeyFrames, sizeof(_uint));
						for (_uint i = 0; i < Channel.iNumMaxKeyFrames; ++i)
						{
							save.write((_char*)&Channel.pKeyFrames[i], sizeof(KEYFRAME));
						}
					}
				}
			}
		}
	}
}

void TAssimpModel::LoadAnimationFBX(const _wstring& _wstrPath)
{
	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	// FilePath
	_string strFilePath = UMethod::ConvertWToS(_wstrPath);

	SHPTR<Assimp::Importer>		spImporter{ nullptr };
	const aiScene* pAIScene{ nullptr };

	// Read Improter 
	spImporter = std::make_shared<Assimp::Importer>();
	pAIScene = spImporter->ReadFile(strFilePath, iFlag);
	RETURN_CHECK(nullptr == pAIScene, ;);
	// 애니메이션 만들어서 집어 넣는다.
	CreateAnimation(pAIScene);
}

void TAssimpModel::LoadAnimation(const _wstring& _wstrPath)
{
	std::ifstream ifRead{ _wstrPath, std::ios::binary };
	VECTOR<ANIMDESC> AnimDesc;
	RETURN_CHECK(!ifRead, ;);
	{
		_uint iSize{ 0 };
		ifRead.read((_char*)&iSize, sizeof(_uint));
		if (0 != iSize)
		{
			AnimDesc.resize(iSize);
			for (auto& Animation : AnimDesc)
			{
				ifRead.read((_char*)&Animation.stExtraData.iNumChannels, sizeof(int));
				ifRead.read((_char*)&Animation.stExtraData.dDuration, sizeof(_double));
				ifRead.read((_char*)&Animation.stExtraData.dTickPerSeconds, sizeof(_double));
				UMethod::ReadStringUnity(ifRead, Animation.wstrName);
				_int iChannelSize{ 0 };
				ifRead.read((_char*)&iChannelSize, sizeof(_int));
				Animation.Channels.resize(iChannelSize);
				for (auto& Channel : Animation.Channels)
				{
					UMethod::ReadStringUnity(ifRead, Channel.wstrBoneName);
					ifRead.read((_char*)&Channel.iNumMaxKeyFrames, sizeof(_int));
					Channel.pKeyFrames = Make::AllocBuffer<KEYFRAME>(Channel.iNumMaxKeyFrames);
					for (_uint i = 0; i < Channel.iNumMaxKeyFrames; ++i)
					{
						KEYFRAME& KeyFrame = Channel.pKeyFrames[i];
						ifRead.read((_char*)&KeyFrame.dTime, sizeof(_double));

						ifRead.read((_char*)&KeyFrame.vScale.x, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vScale.y, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vScale.z, sizeof(_float));

						ifRead.read((_char*)&KeyFrame.vRotation.x, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vRotation.y, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vRotation.z, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vRotation.w, sizeof(_float));

						ifRead.read((_char*)&KeyFrame.vPosition.x, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vPosition.y, sizeof(_float));
						ifRead.read((_char*)&KeyFrame.vPosition.z, sizeof(_float)); 
						KeyFrame.vPosition.w = 1.f;
						//KeyFrame.vPosition.z *= -1.f;
					}
				}
			}
		}

		for (auto& iter : AnimDesc)
		{
			SHPTR<TAssimpAnimation> spAnimation{ CreateNative<TAssimpAnimation>(ThisShared<TAssimpModel>(), iter) };
			RETURN_CHECK(nullptr == spAnimation, ;);
			m_vecAnimations.push_back(spAnimation);
		}
	}

}

HRESULT TAssimpModel::CreateModel(const _wstring& _wstrPath)
{
	std::filesystem::path PATH{ _wstrPath };
	_wstring str = PATH.extension();
	// FBX 
	RETURN_CHECK(str != L".fbx" && str != L".FBX", E_FAIL);

	_wstring Path = _wstrPath;
	// FilePath
	_string strFilePath = UMethod::ConvertWToS(Path);

	_uint iFlag = 0;
	if (TYPE::NONANIM == m_eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_Triangulate 
		| aiProcess_CalcTangentSpace | aiProcess_FlipUVs;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	// Read Improter 
	m_spImporter = std::make_shared<Assimp::Importer>();
	m_pAIScene = m_spImporter->ReadFile(strFilePath, iFlag);
	RETURN_CHECK(nullptr == m_pAIScene, E_FAIL);
	std::filesystem::path path = Path;

	m_wstrModelName = std::filesystem::path(Path).filename();
	size_t Index = m_wstrModelName.find(L".");
	m_wstrModelName = m_wstrModelName.substr(0, Index);

	CreateBoneNodes(m_pAIScene->mRootNode);
	// Create MeshContainers
	RETURN_CHECK_FAILED(CreateMeshContainers(m_mScaleMatrix), E_FAIL);
	RETURN_CHECK_FAILED(CreateMaterials(m_spFileGroup), E_FAIL);
	RETURN_CHECK_FAILED(CreateAnimation(m_pAIScene), E_FAIL);

	std::sort(m_BoneNodeContainer.begin(), m_BoneNodeContainer.end(), [&](SHPTR<TAssimpBoneNode>& p1, SHPTR<TAssimpBoneNode>& p2) {
		return p1->GetDepth() < p2->GetDepth();
		});

	return S_OK;
}

HRESULT TAssimpModel::CreateMeshContainers(const _xmmatrix& _pivotMatrix)
{
	// MeshContainers
	m_iNumMeshContainers = m_pAIScene->mNumMeshes;
	m_vecMeshContainers.reserve(m_iNumMeshContainers);
	// NumMeshContainers
	for (_uint i = 0; i < m_iNumMeshContainers; ++i) {

		SHPTR<TAssimpMeshContainer> pAssimpMeshContainers =
			CreateConstructorNative<TAssimpMeshContainer>(GetDevice(), m_pAIScene->mMeshes[i],
				(_uint)m_eModelType, ThisShared<TAssimpModel>(), _pivotMatrix);

		m_vecMeshContainers.push_back(pAssimpMeshContainers);
	}
	return S_OK;
}

HRESULT TAssimpModel::CreateMaterials(CSHPTRREF<FILEGROUP> _spFileGroup)
{
	SHPTR<FILEGROUP> spFileGroup = _spFileGroup->FindGroup(m_wstrTextureFolderName);
	if (nullptr == spFileGroup)
	{
		spFileGroup = _spFileGroup;
		_bool isFindTexture{ false };
		// Upper Finder
		{
			while (!isFindTexture)
			{
				spFileGroup = spFileGroup->spParentsUpper;
				for (auto& iter : spFileGroup->UnderFileGroupList)
				{
					if (iter.first == m_wstrTextureFolderName)
					{
						spFileGroup = iter.second;
						isFindTexture = true;
						break;
					}
				}
				if (nullptr == _spFileGroup)
					break;
			}
		}
		if (false == isFindTexture)
			// down Folder
		{
			spFileGroup = _spFileGroup;
			for (auto& iter : _spFileGroup->UnderFileGroupList)
			{
				if (iter.first == m_wstrTextureFolderName)
				{
					spFileGroup = iter.second;
					break;
				}
			}
			RETURN_CHECK(spFileGroup == _spFileGroup, E_FAIL);
		}
	}

	m_iNumMaterials = m_pAIScene->mNumMaterials;
	m_MaterialContainer.reserve(m_iNumMaterials);
	_wstring wstrPath = spFileGroup->wstrPath;
	m_wstrTexturePath = wstrPath;

	for (_uint i = 0; i < m_iNumMaterials; ++i) {
		MODELMATERIALINFO MaterialInfo;
		// Matrail 
		aiMaterial* pAiMaterial = m_pAIScene->mMaterials[i];

		aiColor4D color(0.f, 0.f, 0.f, 0.f);
		if (AI_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			MaterialInfo.vDiffuse = _float4{ color.r, color.g, color.b, color.a };
		}

		_uint iCnt{ 0 };
		SHPTR<MODELMATRIALDESC> pModelMaterial = std::make_shared<MODELMATRIALDESC>();
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) {
			aiString strTexturePath{};
			char szName[MAX_PATH]{ "" };
			if (FAILED(pAiMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			if (!strcmp(strTexturePath.C_Str(), ""))
				continue;

			fs::path ps{ strTexturePath.data };

			_wstring strRealName = ps.filename();
			_wstring strName = strRealName.substr(0, strRealName.find_last_of(L"."));
			_wstring strLoadName = strName + L".dds";

			_wstring Path = wstrPath;
			Path += L"\\";
			Path += strLoadName;
			// Create Texture 
			pModelMaterial->arrMaterialTexture[j] = CreateConstructorNativeNotMsg<UTexGroup>(GetDevice(), Path);
			if (nullptr == pModelMaterial->arrMaterialTexture[j])
			{
				// dds로 로드 실패시 다른 texture로 로드 시도
				Path = wstrPath;
				Path += L"\\";
				Path += strRealName;
				// Create Texture 
				pModelMaterial->arrMaterialTexture[j] = CreateConstructorNativeNotMsg<UTexGroup>(GetDevice(), Path);
				m_lsTextureExts.push_back(ps.extension());
				if (nullptr == pModelMaterial->arrMaterialTexture[j])
					continue;
			}
			else
			{
				m_lsTextureExts.push_back(L".dds");
			}
			++iCnt;
		}
		if (iCnt > 0)
		{
			m_MaterialContainer.push_back(pModelMaterial);
			m_MaterialInfoContainer.push_back(MaterialInfo);
		}
	}

	m_wstrTexturePath += L"\\";
	return S_OK;
}

HRESULT TAssimpModel::CreateBoneNodes(aiNode* _pNode, SHPTR<TAssimpBoneNode> _pParents, const _float _fDepths)
{
	RETURN_CHECK(nullptr == _pNode, E_FAIL);

	_float4x4 TransformMatrix;
	memcpy(&TransformMatrix, &_pNode->mTransformation, sizeof(_float4x4));

	SHPTR<TAssimpBoneNode> pBoneNode = CreateNative<TAssimpBoneNode>(_pParents,
		_pNode->mName.data, TransformMatrix, _fDepths);

	m_BoneNodeContainer.push_back({ pBoneNode });

	for (_uint i = 0; i < _pNode->mNumChildren; ++i) {
		CreateBoneNodes(_pNode->mChildren[i], pBoneNode, _fDepths + 0.1f);
	}
	return S_OK;
}

HRESULT TAssimpModel::CreateAnimation(const aiScene* _pAIScene)
{
	RETURN_CHECK(nullptr == _pAIScene, E_FAIL);

	m_iNumAnimation = _pAIScene->mNumAnimations;
	SHPTR<TAssimpModel> pModel = ThisShared<TAssimpModel>();

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		if (_pAIScene->mAnimations[i]->mNumChannels <= 1)
			continue;

		aiAnimation* pAIAnim = _pAIScene->mAnimations[i];

		SHPTR<TAssimpAnimation> pAnimation = CreateNative<TAssimpAnimation>(pAIAnim, pModel);
		if (nullptr == pAnimation)
			return S_OK;

		m_vecAnimations.push_back(pAnimation);
	}
	return S_OK;
}

void TAssimpModel::ConvertPath(_wstring& _wstrConvertPath)
{
	_wstrConvertPath.append(L"\\Convert\\");
	int MakeDir = _wmkdir(_wstrConvertPath.c_str());
	_wstrConvertPath.append(m_wstrModelName);
	_wstrConvertPath += L"_FBX";
	_wstrConvertPath += L".bin";
}
