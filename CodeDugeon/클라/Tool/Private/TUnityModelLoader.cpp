#include "ToolDefines.h"
#include "TUnityModelLoader.h"
#include "UMethod.h"

#pragma region LOADUNITY

static void LoadMeshData(IN std::ifstream& _Read, IN ANIMMODELDESC& _AnimModelDesc)
{
	_Read.read((_char*)&_AnimModelDesc.iMeshContainersCount, sizeof(_int));
	if (0 != _AnimModelDesc.iMeshContainersCount)
	{
		for (_uint i = 0; i < _AnimModelDesc.iMeshContainersCount; ++i)
		{
			ANIMMESHDESC AnimMeshDesc;
			UMethod::ReadStringUnity(_Read, AnimMeshDesc.wstrName);
			_Read.read((_char*)&AnimMeshDesc.iNumVertices, sizeof(_int));
			_Read.read((_char*)&AnimMeshDesc.iNumIndices, sizeof(_int));
			// VTXANIMMODEL 
			AnimMeshDesc.pVtxModel = Make::AllocBuffer<VTXANIMMODEL>(AnimMeshDesc.iNumVertices);
			{
				for (_uint j = 0; j < AnimMeshDesc.iNumVertices; ++j)
				{
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vPosition, sizeof(_float3));
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vNormal, sizeof(_float3));
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vTexUV, sizeof(_float2) * 5);
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vTangent, sizeof(_float3));
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vBlendIndex, sizeof(_float4));
					_Read.read((_char*)&AnimMeshDesc.pVtxModel[i].vBlendWeight, sizeof(_float4));
				}
			}
			_int NUMINDICIES = static_cast<_int>(((_float)(AnimMeshDesc.iNumIndices) / 3));
			AnimMeshDesc.pIndices = Make::AllocBuffer<INDICIES32>(NUMINDICIES + 1);
			{
				_Read.read((_char*)AnimMeshDesc.pIndices, sizeof(INDICIES32) * NUMINDICIES);
			}
			AnimMeshDesc.pPosition = Make::AllocBuffer<_float3>(AnimMeshDesc.iNumVertices + 1 );
			{
				_Read.read((_char*)AnimMeshDesc.pPosition, sizeof(_float3) * AnimMeshDesc.iNumVertices);
			}
			// BoneNodeCnt
			_Read.read((_char*)&AnimMeshDesc.iBoneNodeCnt, sizeof(_int));
			if (0 != AnimMeshDesc.iBoneNodeCnt)
			{
				for (_uint j= 0; j < AnimMeshDesc.iBoneNodeCnt; ++j)
				{
					_wstring str;
					UMethod::ReadStringUnity(_Read, str);
					AnimMeshDesc.BoneNodeNameList.push_back(str);
				}
			}
			_Read.read((_char*)&AnimMeshDesc.iMatrialIndex, sizeof(_int));
			_Read.read((_char*)&AnimMeshDesc.vMaxVertex, sizeof(_float3));
			_Read.read((_char*)&AnimMeshDesc.vMinVertex, sizeof(_float3));

			_AnimModelDesc.Meshes.push_back(AnimMeshDesc);

			ThreadNanoRelax(100);
		}
	}
}

static void LoadBoneNode(IN std::ifstream& _Read, IN ANIMMODELDESC& _AnimModelDesc)
{
	_Read.read((_char*)&_AnimModelDesc.iNodeCnt, sizeof(_int));
	if (0 != _AnimModelDesc.iNodeCnt)
	{
		for (_uint i = 0; i < _AnimModelDesc.iNodeCnt; ++i)
		{
			BONENODEDESC BoneNodeDesc;
			_Read.read((_char*)&BoneNodeDesc.mOffsetMatrix, sizeof(_float4x4));
			_Read.read((_char*)&BoneNodeDesc.mTransformMatrix, sizeof(_float4x4));
			_Read.read((_char*)&BoneNodeDesc.fDepth, sizeof(_float));
			UMethod::ReadStringUnity(_Read, BoneNodeDesc.wstrParentsName);
			UMethod::ReadStringUnity(_Read, BoneNodeDesc.wstrName);
			_AnimModelDesc.BNodeDatas.push_back(BoneNodeDesc);
		}
	}
}

static void LoadMaterial(IN std::ifstream& _Read, IN ANIMMODELDESC& _AnimModelDesc)
{
	_Read.read((_char*)&_AnimModelDesc.iMatrialCount, sizeof(_int));
	if (0 != _AnimModelDesc.iMatrialCount)
	{
		for (_uint i = 0; i < _AnimModelDesc.iMatrialCount; ++i)
		{
			_int Key{ 0 }, Value{ 0 };
			_Read.read((_char*)&Key, sizeof(_int));
			_Read.read((_char*)&Value, sizeof(_int));
			VECTOR<_wstring> MaterialVector;
			MaterialVector.reserve(Value);
			for (_int j = 0; j < Value; ++j)
			{
				_wstring str;
				UMethod::ReadStringUnity(_Read, str);
				MaterialVector.push_back(str);
			}
			_AnimModelDesc.MatrialData.emplace(Key, MaterialVector);
		}
	}
}

static void LoadAnimation(IN std::ifstream& _Read, IN ANIMMODELDESC& _AnimModelDesc)
{
	_Read.read((_char*)&_AnimModelDesc.iAnimCount, sizeof(_int));
	if (0 != _AnimModelDesc.iAnimCount)
	{
		for (_uint i = 0; i < _AnimModelDesc.iAnimCount; ++i)
		{
			// AnimDesc
			ANIMDESC AnimDesc;
			_int NumChannels{ 0 };
			_Read.read((_char*)&NumChannels, sizeof(_int));
			_Read.read((_char*)&AnimDesc.stExtraData.dDuration, sizeof(_double));
			_Read.read((_char*)&AnimDesc.stExtraData.dTickPerSeconds, sizeof(_double));
			// Read Unity
			UMethod::ReadStringUnity(_Read, AnimDesc.wstrName);
			_Read.read((_char*)&NumChannels, sizeof(_int));
			for (_int j = 0; j < NumChannels; ++j)
			{
				CHANNELDESC ChannelDesc;
				UMethod::ReadStringUnity(_Read, ChannelDesc.wstrBoneName);
				_Read.read((_char*)&ChannelDesc.iNumMaxKeyFrames, sizeof(_int));
				ChannelDesc.pKeyFrames = Make::AllocBuffer<KEYFRAME>(ChannelDesc.iNumMaxKeyFrames);
				for (_uint k = 0; k < ChannelDesc.iNumMaxKeyFrames; ++k)
				{
					KEYFRAME KeyFrame;
					_Read.read((_char*)&KeyFrame.dTime, sizeof(_double));
					_Read.read((_char*)&KeyFrame.vScale, sizeof(_float3));
					_Read.read((_char*)&KeyFrame.vRotation, sizeof(_float4));
					_Read.read((_char*)&KeyFrame.vPosition, sizeof(_float3));
					ChannelDesc.pKeyFrames[k] = KeyFrame;
				}
				AnimDesc.Channels.push_back(ChannelDesc);
			}

			_AnimModelDesc.Animes.push_back(AnimDesc);
		}
	}
}

#pragma endregion LOADUNITY

#pragma region SAVEUNITY

static void SaveAnimModelData(IN std::ofstream& save, CSHPTRREF<FILEGROUP> _spFileGroup, 
	const  ANIMMODELDESC& stModelDesc)
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
						save.write((_char*)&iter.pPosition[i], sizeof(_float3));
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

					Make::ReleaseBuffer(iter.pVtxModel);
					Make::ReleaseBuffer(iter.pIndices);
					Make::ReleaseBuffer(iter.pPosition);
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
			SHPTR<FILEGROUP> spParents = _spFileGroup->spParentsUpper;
			SHPTR<FILEGROUP> spTextureGroup = spParents->FindGroup(L"Textures");
			assert(nullptr != spTextureGroup);
			size_t SIZE = stModelDesc.MatrialData.size();
			save.write((_char*)&SIZE, sizeof(size_t));
			if (0 != SIZE) {
				for (auto& iter : stModelDesc.MatrialData) {
					save.write((_char*)&iter.first, sizeof(iter.first));
					// Materials
					size_t STRING_SIZE = iter.second.size();
					save.write((_char*)&STRING_SIZE, sizeof(size_t));

					for (auto& TextureName : iter.second) {
						_wstring strName = L"";
						if (false == TextureName.empty())
						{
							strName = spTextureGroup->wstrPath;
							strName += L"\\";
							strName += TextureName;
							strName += L".dds";
						}
						else
						{
							strName = "NULL";
						}
						UMethod::SaveString(save, strName);
					}
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

					Make::ReleaseBuffer(Channel.pKeyFrames);
				}
			}
		}
	}
}


#pragma endregion SAVEUNITY

void TUnityModelLoader::LoadUnityAnimFBXData(CSHPTRREF<FILEGROUP> _spFileGroup, 
	CSHPTRREF<FILEDATA> _spFileData, IN _wstring& _wstrConvertLoadPath)
{
	ANIMMODELDESC AnimModelDesc;
	std::ifstream Read{ _spFileData->wstrfilePath, std::ios::binary };
	assert(true == Read.is_open());

	LoadMeshData(OUT Read, OUT AnimModelDesc);
	LoadBoneNode(OUT Read, OUT AnimModelDesc);
	LoadMaterial(OUT Read, OUT AnimModelDesc);
	LoadAnimation(OUT Read, OUT AnimModelDesc);

	_wstring wstrFilePath = L"";
	if (nullptr != _spFileGroup->spParentsUpper)
	{
		wstrFilePath = _spFileGroup->spParentsUpper->wstrPath;
	}
	wstrFilePath += L"\\Convert\\";
	int MakeDir = _wmkdir(wstrFilePath.c_str());

	wstrFilePath += _spFileData->wstrfileName;

	std::ofstream Save{ wstrFilePath, std::ios::binary };
	SaveAnimModelData(OUT Save, _spFileGroup, AnimModelDesc);

	_wstrConvertLoadPath = wstrFilePath;
}
