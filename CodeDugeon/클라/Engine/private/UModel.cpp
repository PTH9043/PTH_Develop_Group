#include "EngineDefine.h"
#include "UModel.h"
#include "UMethod.h"
#include "UTexGroup.h"
#include "UBoneNode.h"
#include "UGameInstance.h"
#include "UShader.h"
#include "UMeshContainer.h"
#include "UTexture.h"
#include "URootBoneNode.h"
#include "UModelMaterial.h"
#include "UShaderConstantBuffer.h"

UModel::UModel(CSHPTRREF<UDevice> _spDevice, const TYPE& _eType) :
	UResource(_spDevice),
	m_MeshContainer{},
	m_iMeshContainerCnt{ 0 },
	m_BoneNodeContainer{},
	m_iBoneNodeCnt{ 0 },
	m_MaterialContainer{},
	m_iMaterialCnt{ 0 },
	m_spFileGroup{ nullptr },
	m_spFileData{ nullptr },
	m_eType{ _eType },
	m_wstrModelName{L""},
	m_spRootBoneNode{nullptr}

{
}

UModel::UModel(const UModel& _rhs) :
	UResource(_rhs),
	m_MeshContainer{ _rhs.m_MeshContainer },
	m_iMeshContainerCnt{ _rhs.m_iMeshContainerCnt },
	m_BoneNodeContainer{ _rhs.m_BoneNodeContainer },
	m_iBoneNodeCnt{ _rhs.m_iBoneNodeCnt },
	m_MaterialContainer{ _rhs.m_MaterialContainer },
	m_iMaterialCnt{ _rhs.m_iMaterialCnt },
	m_spFileGroup{ _rhs.m_spFileGroup },
	m_spFileData{ _rhs.m_spFileData },
	m_eType{ _rhs.m_eType },
	m_wstrModelName{ L"" },
	m_spRootBoneNode{ _rhs.m_spRootBoneNode}
{
}


SHPTR<UBoneNode> UModel::FindBoneNode(const _wstring& _strBoneNode) const
{
	auto it = std::find_if(m_BoneNodeContainer.begin(), m_BoneNodeContainer.end(), [&](const SHPTR<UBoneNode>& p) {
		if (p->GetName() == _strBoneNode)
			return true;

		return false;
		});
	RETURN_CHECK(m_BoneNodeContainer.end() == it, nullptr);
	return *it;
}

_float3 UModel::GetMaxVertexPos()
{
	_float3 maxVertexPos = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto& MeshContainer : m_MeshContainer)
	{
		const _float3& maxMeshVertexPos = MeshContainer->GetMaxVertexPos();
		maxVertexPos.x = max(maxVertexPos.x, maxMeshVertexPos.x);
		maxVertexPos.y = max(maxVertexPos.y, maxMeshVertexPos.y);
		maxVertexPos.z = max(maxVertexPos.z, maxMeshVertexPos.z);
	}

	return maxVertexPos; // 최대 버텍스 반환
}

_float3 UModel::GetMinVertexPos()
{
	_float3 minVertexPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);

	for (auto& MeshContainer : m_MeshContainer)
	{
		const _float3& minMeshVertexPos = MeshContainer->GetMinVertexPos();
		minVertexPos.x = min(minVertexPos.x, minMeshVertexPos.x);
		minVertexPos.y = min(minVertexPos.y, minMeshVertexPos.y);
		minVertexPos.z = min(minVertexPos.z, minMeshVertexPos.z);
	}

	return minVertexPos; // 최대 버텍스 반환
}

_float3 UModel::GetCenterPos()
{
	// 최소 버텍스와 최대 버텍스를 얻어옴
	const _float3& minVertexPos = GetMinVertexPos();
	const _float3& maxVertexPos = GetMaxVertexPos();

	// 중심 위치 계산
	_float3 centerPos;
	centerPos.x = (maxVertexPos.x + minVertexPos.x) * 0.5f;
	centerPos.y = (maxVertexPos.y + minVertexPos.y) * 0.5f;
	centerPos.z = (maxVertexPos.z + minVertexPos.z) * 0.5f;

	return centerPos;
}

void UModel::Free()
{
	m_BoneNodeContainer.clear();
	m_MeshContainer.clear();
	m_MaterialContainer.clear();
}

HRESULT UModel::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UModel::NativeConstruct(const _wstring& _wstrPath)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	LoadToData(_wstrPath);
	return S_OK;
}

HRESULT UModel::NativeConstruct(CSHPTRREF<FILEGROUP> _spFileGroup, CSHPTRREF<FILEDATA> _spFileData)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	RETURN_CHECK(nullptr == _spFileGroup || nullptr == _spFileData, E_FAIL);
	m_spFileGroup = _spFileGroup; m_spFileData = _spFileData;
	LoadToData(m_spFileData->wstrfilePath);
	return S_OK;
}

HRESULT UModel::NativeConstruct(const PATHS& _vecPaths, const _wstring& _wstrFileName)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spFileGroup = spGameInstance->FindFolder(_vecPaths);
	RETURN_CHECK(nullptr == m_spFileGroup, E_FAIL);
	m_spFileData = m_spFileGroup->FindData(_wstrFileName);
	RETURN_CHECK(nullptr == m_spFileData, E_FAIL);
	LoadToData(m_spFileData->wstrfilePath);
	return S_OK;
}

HRESULT UModel::NativeConstruct(const _wstring& _wstrModelFolder, const _wstring& _wstrFileName)
{
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spFileGroup = spGameInstance->FindFolder(_wstrModelFolder);
	RETURN_CHECK(nullptr == m_spFileGroup, E_FAIL);
	m_spFileData = m_spFileGroup->FindData(_wstrFileName);
	RETURN_CHECK(nullptr == m_spFileData, E_FAIL);
	LoadToData(m_spFileData->wstrfilePath);
	return S_OK;
}

HRESULT UModel::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	// Create Model ConstantBuffer 
	/*m_spModelDataConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::MODELDATA, 
		GetTypeSize<MODELDATAPARAM>());*/

	MESHCONTAINERS MeshContainers{};
	BONENODES BoneNodes{};

	for (auto& iter : m_BoneNodeContainer) {
		BoneNodes.push_back(iter->Clone());
	}
	m_BoneNodeContainer.clear();
	m_BoneNodeContainer = BoneNodes;

	m_spRootBoneNode = std::static_pointer_cast<URootBoneNode>(FindBoneNode(m_spRootBoneNode->GetName()));
	std::sort(m_BoneNodeContainer.begin(), m_BoneNodeContainer.end(), [&](SHPTR<UBoneNode>& p1, SHPTR<UBoneNode>& p2) {
		return p1->GetDepth() < p2->GetDepth();
		});
	for (auto& iter : m_BoneNodeContainer) {
		iter->FindParents(ThisShared<UModel>());
	}

	VOIDDATAS tDatas;
	// Insert Data
	{
		UModel* pModel = this;
		tDatas.push_back(&pModel);
	}

	for (auto& iter : m_MeshContainer) {
		MeshContainers.push_back(static_pointer_cast<UMeshContainer>(iter->Clone(tDatas)));
	}
	m_MeshContainer.clear();
	m_MeshContainer = MeshContainers;
	ThreadMiliRelax(10);
	return S_OK;
}

HRESULT UModel::BindTexture(const _uint _iMeshIndex, const SRV_REGISTER _eRegister,
	const TEXTYPE& _eTextureType, CSHPTRREF<UShader> _spShader)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);
	RETURN_CHECK(GetMeshContainerCnt() <= _iMeshIndex, E_FAIL);
	RETURN_CHECK(nullptr == GetMeshContainers()[_iMeshIndex], E_FAIL);
	// BindSrvBuffer
	CSHPTRREF<UMeshContainer> spMeshContainer{ GetMeshContainers()[_iMeshIndex] };
	if (GetMaterials().size() > 0)
	{
		if (nullptr != GetMaterials()[spMeshContainer->GetMaterialIndex()])
		{
			SHPTRREF<UModelMaterial> spMaterial = GetMaterials()[spMeshContainer->GetMaterialIndex()];
			if (nullptr != spMaterial)
			{
				spMaterial->BindTexture(_spShader, _eRegister, _eTextureType);
			}
		}
	}
	return S_OK;
}

HRESULT UModel::Render(const _uint _iMeshIndex, CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommand)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);
	RETURN_CHECK(GetMeshContainerCnt() <= _iMeshIndex, E_FAIL);
	RETURN_CHECK(nullptr == GetMeshContainers()[_iMeshIndex], E_FAIL);
	CSHPTRREF<UMeshContainer> spMeshContainer{ GetMeshContainers()[_iMeshIndex] };
	spMeshContainer->Render(_spShader, _spCommand);
	return S_OK;
}

HRESULT UModel::CreateBoneNode(void* _pData, const _wstring& _wstrBoneNodeName)
{
	MODELDESC* tDesc = static_cast<MODELDESC*>(_pData);

	_int Index{ 0 };

	if (true == _wstrBoneNodeName.empty())
	{
		CreateRootBoneToOrder(tDesc);
	}
	else
	{
		// 만약 애니메이션 뼈가 존재한다면
		_bool isFind = tDesc->BNodeDatas.end()!= (std::find_if(tDesc->BNodeDatas.begin(), tDesc->BNodeDatas.end(), [&](const BONENODEDESC& _Desc)	{
				return _wstrBoneNodeName == _Desc.wstrName;
			}));

		if (true == isFind)
		{
			for (auto& iter : tDesc->BNodeDatas) {
				SHPTR<UBoneNode> spBoneNode{ nullptr };
				if (iter.wstrName == _wstrBoneNodeName)
				{
					m_spRootBoneNode = CreateNative<URootBoneNode>(iter);
					m_spRootBoneNode->OnRootBoneNode();
					spBoneNode = m_spRootBoneNode;
				}
				else
					spBoneNode = CreateNative<UBoneNode>(iter);

				m_BoneNodeContainer.push_back(spBoneNode);
			}
		}
		else
		{
			// 아예 하나 새로운 루트 본을 만든다.  
			{
				m_spRootBoneNode = CreateNative<URootBoneNode>(_wstrBoneNodeName);
				m_BoneNodeContainer.push_back(m_spRootBoneNode);
			}
			_int Index{ 0 };
			// 루트본에 대한 처리를 한다. 
			for (auto& iter : tDesc->BNodeDatas) {
				SHPTR<UBoneNode> spBoneNode{ nullptr };
				if (0 == Index++)
				{
					// 맨 첫번째와 연결해줌으로써 루트본을 사용할 수 있도록 만든다. 
					iter.wstrParentsName = _wstrBoneNodeName;
					spBoneNode = CreateNative<UBoneNode>(iter);
				}
				else
				{
					spBoneNode = CreateNative<UBoneNode>(iter);
				}
				m_BoneNodeContainer.push_back(spBoneNode);
			}
		}
	}

	std::sort(m_BoneNodeContainer.begin(), m_BoneNodeContainer.end(), [&](SHPTR<UBoneNode>& p1, SHPTR<UBoneNode>& p2) {
		return p1->GetDepth() < p2->GetDepth();
		});

	for (auto& iter : m_BoneNodeContainer) {
		iter->FindParents(ThisShared<UModel>());
	}
	m_iBoneNodeCnt = (_uint)(m_BoneNodeContainer.size());
	return S_OK;
}

HRESULT UModel::CreateMeshContainers(void* _pData)
{
	MODELDESC* tDesc = static_cast<MODELDESC*>(_pData);

	_int iMeshIndex{ 0 };
	for (auto& iter : tDesc->Meshes)
	{
		SHPTR<UMeshContainer> pMeshContainer{ CreateConstructorNative<UMeshContainer>(
		GetDevice(),  (void*)&iter, ThisShared<UModel>(), iMeshIndex++) };

		m_MeshContainer.push_back(pMeshContainer);
	}
	m_iMeshContainerCnt = static_cast<_uint>(m_MeshContainer.size());
	return S_OK;
}

HRESULT UModel::CreateMaterial(void* _pData, const MATERIALINFOS& _vecMaterialInfo)
{
 	MODELDESC* tDesc = static_cast<MODELDESC*>(_pData);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (nullptr != tDesc) {
		m_MaterialContainer.resize(tDesc->MatrialData.size());
		_int MaterialIndex{ 0 };
		for (auto& iter : tDesc->MatrialData) {
			VECTOR<SHPTR<UTexGroup>> vecMaterialTexGroup;
			vecMaterialTexGroup.resize(TEXTYPE::TextureType_END);
			for (_uint i = 0; i < TEXTYPE::TextureType_END; ++i)
			{
				if (iter.second[i] != L"NULL")
				{
					vecMaterialTexGroup[i] = CreateConstructorNative<UTexGroup>(GetDevice(), iter.second[i]);
				}
			}
			UModelMaterial::DESC Desc;
			Desc.MaterialInfo = _vecMaterialInfo[MaterialIndex++];
			Desc.MaterialTexContainer = vecMaterialTexGroup;
			// MaterialContainer 
			m_MaterialContainer[iter.first] = CreateConstructorToNative<UModelMaterial>(GetDevice(), Desc);
		}
	}
	return S_OK;
}

void UModel::LoadToData(const _wstring& _wstrPath)
{
	std::ifstream read{ _wstrPath , std::ios::binary};
	RETURN_CHECK(!read, ;);
	ThreadMiliRelax(10);
	MODELDESC tDesc;

	MATERIALINFOS		vecMaterialInfo(TEXTYPE::TextureType_END);
	{
		BringModelName(_wstrPath);

		// MESH
		LoadMeshData(REF_OUT read, REF_OUT tDesc.Meshes);
		// BoneNode
		LoadBoneData(REF_OUT read, REF_OUT tDesc.BNodeDatas);
		// Material
		LoadMaterial(REF_OUT read, REF_OUT tDesc.MatrialData, REF_OUT vecMaterialInfo);

		CreateBoneNode(&tDesc);
		CreateMeshContainers(&tDesc);
		CreateMaterial(&tDesc, vecMaterialInfo);
	}
}

void UModel::LoadMeshData(REF_IN std::ifstream& _ifRead, REF_IN VECTOR<MESHDESC>& _convecMeshes)
{
	size_t SIZE = 0;
	_ifRead.read((_char*)&SIZE, sizeof(size_t));
	_convecMeshes.resize(SIZE);
	if (0 != SIZE) {
		for (auto& iter : _convecMeshes) {
			// Name 
			UMethod::ReadString(_ifRead, iter.wstrName);
			// Vertex Model
			_ifRead.read((_char*)&iter.iNumVertices, sizeof(iter.iNumVertices));
			_ifRead.read((_char*)&iter.iNumIndices, sizeof(iter.iNumIndices));
			// Positions
			{
				iter.pVtxModel = Make::AllocBuffer<VTXMODEL>(iter.iNumVertices);
				size_t vtxModel = sizeof(VTXMODEL) * iter.iNumVertices;
				_ifRead.read((_char*)iter.pVtxModel, vtxModel);
			}
			// Indexies
			{
				size_t index = sizeof(INDICIES32) * iter.iNumIndices;
				iter.pIndices = Make::AllocBuffer<INDICIES32>(iter.iNumIndices);
				_ifRead.read((_char*)iter.pIndices, index);
			}
			// Positions 
			{
				size_t Positions = sizeof(_float3) * iter.iNumVertices;
				iter.pPosition = Make::AllocBuffer<_float3>(Positions);
				_ifRead.read((_char*)iter.pPosition, Positions);
			}
			{
				_ifRead.read((_char*)&iter.vMinVertex, sizeof(_float3));
				_ifRead.read((_char*)&iter.vMaxVertex, sizeof(_float3));
			}
			// BonCnt 
			_ifRead.read((_char*)&iter.iBoneNodeCnt, sizeof(iter.iBoneNodeCnt));
			iter.BoneNodeNameList.resize(iter.iBoneNodeCnt);
			if (0 != iter.iBoneNodeCnt) {
				for (auto& name : iter.BoneNodeNameList)
				{
					UMethod::ReadString(_ifRead, name);
				}
			}
			// Material Index
			_ifRead.read((_char*)&iter.iMatrialIndex, sizeof(iter.iMatrialIndex));

		}
	}
}

void UModel::LoadBoneData(REF_IN std::ifstream& _ifRead, REF_IN VECTOR<BONENODEDESC>& _convecBones)
{
	size_t SIZE = 0;
	_ifRead.read((_char*)&SIZE, sizeof(size_t));
	_convecBones.resize(SIZE);
	if (0 != SIZE) {
		for (auto& iter : _convecBones) {
			// Name
			_ifRead.read((_char*)&iter.mOffsetMatrix, sizeof(_float4x4));
			_ifRead.read((_char*)&iter.mTransformMatrix, sizeof(_float4x4));
			_ifRead.read((_char*)&iter.fDepth, sizeof(_float));
			UMethod::ReadString(_ifRead, iter.wstrParentsName);
			UMethod::ReadString(_ifRead, iter.wstrName);
		}
	}
}

void UModel::LoadMaterial(REF_IN std::ifstream& _ifRead, REF_IN UNORMAP<_uint, VECTOR<_wstring>>& _uomapMaterials,
	REF_IN MATERIALINFOS& _vecMaterialInfos)
{
	size_t SIZE = 0;
	_ifRead.read((_char*)&SIZE, sizeof(size_t));
	if (0 != SIZE) {
		for (int i = 0; i < SIZE; ++i) {

			_uint first{ 0 };
			_ifRead.read((_char*)&first, sizeof(first));
			// Materials
			size_t STRING_SIZE = 0;
			_ifRead.read((_char*)&STRING_SIZE, sizeof(size_t));

			VECTOR<_wstring> rtVec;
			rtVec.reserve(STRING_SIZE);
			for (int j = 0; j < STRING_SIZE; ++j) {
				_wstring str;
				UMethod::ReadString(_ifRead, str);
				rtVec.push_back(str);
			}
			_uomapMaterials.insert(std::pair<_uint, VECTOR<_wstring>>(first, rtVec));
		}
		_vecMaterialInfos.resize(SIZE);
		_ifRead.read((_char*)&_vecMaterialInfos[0], sizeof(MODELMATERIALINFO) * SIZE);
	}
}

void UModel::BringModelName(const _wstring& _wstrPath)
{
	m_wstrModelName = _wstrPath.substr(_wstrPath.find_last_of(L"\\") + 1, _wstrPath.find_last_of(L"."));
}

void UModel::CreateRootBoneToOrder(MODELDESC* _pDesc, _int _BoneOrder)
{
	_int Index{ 0 };
	for (auto& iter : _pDesc->BNodeDatas) {
		SHPTR<UBoneNode> spBoneNode{ nullptr };
		if (_BoneOrder == Index++)
		{
			m_spRootBoneNode = CreateNative<URootBoneNode>(iter);
			spBoneNode = m_spRootBoneNode;
		}
		else
		{
			spBoneNode = CreateNative<UBoneNode>(iter);
		}
		m_BoneNodeContainer.push_back(spBoneNode);
	}
}

