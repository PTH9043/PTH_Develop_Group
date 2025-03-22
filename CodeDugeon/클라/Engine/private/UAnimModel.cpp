#include "EngineDefine.h"
#include "UAnimModel.h"
#include "UMethod.h"
#include "UAnimation.h"
#include "UShader.h"
#include "UMeshContainer.h"
#include "UBoneNode.h"
#include "UShaderConstantBuffer.h"
#include "UTexture.h"
#include "UDevice.h"
#include "UTexGroup.h"
#include "UGpuCommand.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "URootBoneNode.h"
#include "UModelMaterial.h"
#include "UMeshFilter.h"
#include "UCollider.h"

namespace fs = std::filesystem;
UAnimModel::UAnimModel(CSHPTRREF<UDevice> _spDevice) :
	UModel(_spDevice, TYPE::ANIM),
	m_vecAnimations{},
	m_vecSetupBonMatrix{},
	m_AnimNamesGroup{},
	m_spAnimShaderConstantBuffer{ nullptr },
	m_stAnimParam{},
	m_spBoneMatrixShaderConstantBuffer{ nullptr },
	m_spPrevBoneMatrixShaderConstantBuffer{ nullptr },
	m_spDrawRimLightBuffer{ nullptr },
	m_cpUpLoadResource{ nullptr },
	m_spCurAnimation{ nullptr },
	m_spNextAnimation{ nullptr },
	m_DrawRim{ 0 },
	m_iCurAnimIndex{ 0 },
	m_iNextAnimIndex{ 0 },
	m_fSupplyLerpValue{ 0.f },
	m_isChangeAnim{ false },
	m_mPivotMatrix{_float4x4::Identity},
	m_spMeshFilterController{nullptr},
	m_isCanAttackSituation{false},
	m_spAttackCollisionCollider{nullptr},
	m_isNotApplyAnimPosition{false}
{
}

UAnimModel::UAnimModel(const UAnimModel& _rhs) :
	UModel(_rhs),
	m_vecAnimations{ _rhs.m_vecAnimations },
	m_AnimNamesGroup{ _rhs.m_AnimNamesGroup },
	m_spAnimShaderConstantBuffer{ nullptr },
	m_stAnimParam{},
	m_spBoneMatrixShaderConstantBuffer{ nullptr },
	m_spPrevBoneMatrixShaderConstantBuffer{ nullptr },
	m_cpUpLoadResource{ nullptr },
	m_spCurAnimation{ nullptr },
	m_spNextAnimation{ nullptr },
	m_iCurAnimIndex{ 0 },
	m_iNextAnimIndex{ 0 },
	m_fSupplyLerpValue{ 0.f },
	m_isChangeAnim{ false },
	m_mPivotMatrix{_rhs.m_mPivotMatrix},
	m_spMeshFilterController{ _rhs.m_spMeshFilterController },
	m_isNotApplyAnimPosition{ false }
{

}

void UAnimModel::Free()
{
}

HRESULT UAnimModel::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UAnimModel::NativeConstruct(const _wstring& _wstrPath, const _float4x4& _PivotMatrix)
{
	m_mPivotMatrix = _PivotMatrix;
	return NativeConstruct(_wstrPath);
}

HRESULT UAnimModel::NativeConstruct(CSHPTRREF<FILEGROUP> _spFileGroup, CSHPTRREF<FILEDATA> _spFileData, const _float4x4& _PivotMatrix)
{
	m_mPivotMatrix = _PivotMatrix;
	return NativeConstruct(_spFileGroup, _spFileData);
}

HRESULT UAnimModel::NativeConstruct(const PATHS& _vecPaths, const _wstring& _wstrFileName, const _float4x4& _PivotMatrix)
{
	m_mPivotMatrix = _PivotMatrix;
	return NativeConstruct(_vecPaths, _wstrFileName);
}

HRESULT UAnimModel::NativeConstruct(const _wstring& _wstrModelFolder, const _wstring& _wstrFileName, const _float4x4& _PivotMatrix)
{
	m_mPivotMatrix = _PivotMatrix;
	return NativeConstruct(_wstrModelFolder, _wstrFileName);
}

HRESULT UAnimModel::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

		UCollider::COLLIDERDESC Desc{};
		m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_AABB,
			std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_ABBCOLLIDER, { &Desc }))));

		m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_OBB,
			std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &Desc }))));

		m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_SPHERE,
			std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_SPHERECOLLIDER, { &Desc }))));
	}
	UMeshFilter::DESC desc{ GetMeshContainers() };
	m_spMeshFilterController = std::static_pointer_cast<UMeshFilter>(m_spMeshFilterController->Clone({ &desc }));

	SHPTR<UAnimModel> spAnimModel = ThisShared<UAnimModel>();
	ANIMATIONS vecAnims;
	vecAnims.reserve(m_vecAnimations.size());
	for (auto& iter : m_vecAnimations)
	{
		vecAnims.push_back(iter->Clone(spAnimModel));		
	}
	m_vecAnimations = vecAnims;


	if (m_vecAnimations.size() > 0)
	{
		// 애니메이션이 nullptr일 때, Animation 0으로 초기화 
		SetAnimation(0);
	}
	// BondMatrix
	m_vecSetupBonMatrix.resize(GetMeshContainerCnt());
	for (_uint i = 0; i < GetMeshContainerCnt(); ++i)
	{
		for (auto& iter : m_vecSetupBonMatrix[i])
			iter = _float4x4::Identity;
	}
	GetRootBoneNode()->OnRootBoneNode();
	RETURN_CHECK_FAILED(CreateShaderConstantBuffer(), E_FAIL);
	{
		UMeshFilter::DESC tDesc{ GetMeshContainers() };
	}
	return S_OK;
}

void UAnimModel::TickEvent(UPawn* _pPawn, const _wstring& _wstrInputTrigger, const _double& _TimeDelta)
{
	m_spCurAnimation->TickAnimEvent(_pPawn, this, _TimeDelta, _wstrInputTrigger);
}

void UAnimModel::TickEventToRatio(UPawn* _pPawn, const _wstring& _wstrInputTrigger, const _double& _dRatio, const _double& _TimeDelta)
{
	m_spCurAnimation->TickAnimEvent(_pPawn, this, _TimeDelta, _dRatio, _wstrInputTrigger);
}

void UAnimModel::TickAnimation(const _double& _dTimeDelta)
{
	if (nullptr != m_spNextAnimation)
	{
		m_spCurAnimation->UpdateNextAnimTransformMatrices(_dTimeDelta, m_fSupplyLerpValue, m_spNextAnimation);
		if (false == m_spCurAnimation->IsSupplySituation())
		{
			m_spCurAnimation = m_spNextAnimation;
			m_spNextAnimation = nullptr;
		}
		m_isChangeAnim = true;
	}
	else
	{
		m_spCurAnimation->UpdateBoneMatrices(_dTimeDelta);
		m_isChangeAnim = false;
	}

	/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
	{
		for (auto& BoneNode : GetBoneNodes())
			BoneNode->UpdateCombinedMatrix(m_mPivotMatrix);
	}
}

void UAnimModel::UpdateCurAnimationToRatio(const _double& _dRatio)
{
	m_spCurAnimation->UpdateboneMatricesToRatio(_dRatio);
	/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
	{
		for (auto& BoneNode : GetBoneNodes())
			BoneNode->UpdateCombinedMatrix(m_mPivotMatrix);
	}
}

void UAnimModel::TickAnimChangeTransform(CSHPTRREF<UTransform> _spTransform, const _double& _dTimeDelta)
{
	assert(_spTransform && m_spCurAnimation);
	TickAnimation(_dTimeDelta);

	RETURN_CHECK(true == m_spCurAnimation->IsSupplySituation(), ;);

	if (true == m_spCurAnimation->IsFinishAnim())
	{
		GetRootBoneNode()->ResetRootBoneInfo();
		return;
	}

	if (false == m_isNotApplyAnimPosition && true == m_spCurAnimation->IsApplyRootBoneMove())
	{
		_float3 Position = _float3::TransformCoord(GetRootBoneNode()->GetMoveRootBonePos(), m_mPivotMatrix * _spTransform->GetWorldMatrix());
		_spTransform->SetPos(Position);
	}
}

void UAnimModel::TickAnimToTimeAccChangeTransform(CSHPTRREF<UTransform> _spTransform, const _double& _dTimeDelta, const _double& _TimeAcc)
{
	assert(_spTransform && m_spCurAnimation);
	UpdateCurAnimationToRatio(_TimeAcc);

	if (false == m_isNotApplyAnimPosition && true == m_spCurAnimation->IsApplyRootBoneMove())
	{
		_float3 Position = _float3::TransformCoord(GetRootBoneNode()->GetMoveRootBonePos(), m_mPivotMatrix *  _spTransform->GetWorldMatrix());
		_spTransform->SetPos(Position);
	}
}

void UAnimModel::ApplyRootBoneTransform(CSHPTRREF<UTransform> _spTransform)
{
	assert(nullptr != _spTransform);
	if (true == m_spCurAnimation->IsApplyRootBoneMove())
	{
		_float3 Position = _float3::TransformCoord(GetRootBoneNode()->GetMoveRootBonePos(), m_mPivotMatrix * _spTransform->GetWorldMatrix());
		_spTransform->SetPos(Position);
	}
}

void UAnimModel::ApplyRootBoneMatrix(_float4x4& _matrix)
{
	if (true == m_spCurAnimation->IsApplyRootBoneMove())
	{
		_float3 Position = _float3::TransformCoord(GetRootBoneNode()->GetMoveRootBonePos(), m_mPivotMatrix * _matrix);
		_matrix.Set_Pos(Position);
	}
}

HRESULT UAnimModel::Render(const _uint _iMeshIndex, CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommand)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);   
	RETURN_CHECK(GetMeshContainerCnt() <= _iMeshIndex, E_FAIL);
	RETURN_CHECK(nullptr == GetMeshContainers()[_iMeshIndex], E_FAIL);
	CSHPTRREF<UMeshContainer> spMeshContainer{ GetMeshContainers()[_iMeshIndex] };
	// 이전 본 값 세팅
	_spShader->BindCBVBuffer(m_spPrevBoneMatrixShaderConstantBuffer, m_vecSetupBonMatrix[_iMeshIndex].data(), GetTypeSize<BONEMATRIXPARAM>());
	spMeshContainer->SetUpBoneMatrix(m_vecSetupBonMatrix[_iMeshIndex]);
	// 현재 본 값 세팅
	_spShader->BindCBVBuffer(m_spBoneMatrixShaderConstantBuffer, m_vecSetupBonMatrix[_iMeshIndex].data(), GetTypeSize<BONEMATRIXPARAM>());
	// 애니메이션 세팅
	_spShader->BindCBVBuffer(m_spAnimShaderConstantBuffer, &m_stAnimParam, GetTypeSize<ANIMATIONPARAM>());
	
	_spShader->BindCBVBuffer(m_spDrawRimLightBuffer, &m_DrawRim, GetTypeSize<RIMDRAW>());
	spMeshContainer->RenderAnimModel(m_spMeshFilterController, _spShader, _spCommand, _iMeshIndex);
	return S_OK;
}

void UAnimModel::SetAnimation(const _uint& _iAnimIndex)
{
	ChangeAnimIndex(_iAnimIndex, m_iCurAnimIndex);
	// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
	SettingCurAnimSituation();
}

void UAnimModel::SetAnimation(const _wstring& _wstrAnimName)
{
	ChangeAnimIndex(_wstrAnimName, m_iCurAnimIndex);
	// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
	SettingCurAnimSituation();
}

void UAnimModel::SetAnimation(const _uint& _iAnimIndex, const _double& _dNextTimeAcc)
{
	ResetCurAnimEvent();
	SetAnimation(_iAnimIndex);
	// Change Time Acc
	m_spCurAnimation->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
}

void UAnimModel::SetAnimation(const _wstring& _wstrAnimName, const _double& _dNextTimeAcc)
{
	SetAnimation(_wstrAnimName);
	// Change Time Acc
	m_spCurAnimation->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
}

void UAnimModel::ChangeAnimation(const _uint& _iAnimIndex)
{
	ChangeAnimation(_iAnimIndex, 0.0);
}

void UAnimModel::ChangeAnimation(const _wstring& _wstrAnimName)
{
	ChangeAnimation(_wstrAnimName, 0.0);
}

void UAnimModel::ChangeAnimation(const _uint& _iAnimIndex, const _double& _dNextTimeAcc)
{
	ResetCurAnimEvent();
	ChangeAnimIndex(_iAnimIndex, m_iNextAnimIndex);
	// 다음 애니메이션이 세팅되는 상황일 때의 함수 실행
	SettingNextAnimSituation();
	// Change Time Acc
	m_spNextAnimation->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
}

void UAnimModel::ChangeAnimation(const _wstring& _wstrAnimName, const _double& _dNextTimeAcc)
{
	ChangeAnimIndex(_wstrAnimName, m_iCurAnimIndex);
	// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
	SettingNextAnimSituation();
	// Change Time Acc
	m_spNextAnimation->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
}

void UAnimModel::OnShowOriginAnimation()
{
	SHPTR<URootBoneNode> spRootBoneNode{ GetRootBoneNode() };
	if (nullptr != spRootBoneNode)
		spRootBoneNode->OffRootBoneNode();
}

void UAnimModel::OnAdjustTransformToAnimation()
{
	SHPTR<URootBoneNode> spRootBoneNode{ GetRootBoneNode() };
	if (nullptr != spRootBoneNode)
		spRootBoneNode->OnRootBoneNode();
}

void UAnimModel::ResetCurAnimEvent()
{
	m_spCurAnimation->ResetAnimChangeEventNode();
}

SHPTR<UCollider> UAnimModel::BringAttackCollider(_int _iColliderType)
{
	const auto& iter = m_AnimEventColliderContainer.find(_iColliderType);
	RETURN_CHECK(m_AnimEventColliderContainer.end() == iter, nullptr);
	return iter->second;
}

_bool UAnimModel::IsCollisionAttackCollider(CSHPTRREF<UCollider> _spEnemyCollider)
{
	RETURN_CHECK(false == m_isCanAttackSituation, false);
	RETURN_CHECK(nullptr == m_spAttackCollisionCollider, false);
	return m_spAttackCollisionCollider->IsCollision(_spEnemyCollider);
}

void UAnimModel::OutAnimationDataToServer(const _wstring& _wstrPath)
{
	_wstring str = _wstrPath;
	str.append(L"\\ServerAnim");
	if (0 != _wmkdir(str))
	{
		str.append(L"\\");
		str.append(GetModelName());

		std::ofstream Saves{ str, std::ios::binary };
		RETURN_CHECK(!Saves, ;);

		size_t boneNodeCnt = GetBoneNodes().size();
		size_t AnimCnt = GetAnimations().size();
		Saves.write((_char*)&boneNodeCnt, sizeof(size_t));
		Saves.write((_char*)&AnimCnt, sizeof(size_t));

		_int i = 0; 
		for (auto& iter : GetBoneNodes())
		{
			if (true == iter->IsRootBoneNode())
			{
				Saves.write((_char*)&i, sizeof(_int));
				break;
			}
			++i;
		}

		for (auto& iter : GetBoneNodes())
		{
			iter->SaveServerData(Saves);
		}

		for (auto& iter : m_vecAnimations)
		{
			iter->SaveAnimToServerData(Saves);
		}
	}
}

void UAnimModel::UpdateDissolveTImer(const _double& _dTimeDelta)
{
	m_stAnimParam.iObjectDissolve = 1;
	m_stAnimParam.fDissolveTimer += static_cast<_float>(_dTimeDelta);
}

void UAnimModel::ResetDissolveTimer()
{
	m_stAnimParam.fDissolveTimer = 0;
	m_stAnimParam.iObjectDissolve = 0;
}

void UAnimModel::UpdateAttackData(const _bool _isCanAttackSituation, CSHPTRREF<UCollider> _spCollider)
{
	m_isCanAttackSituation = _isCanAttackSituation;
	m_spAttackCollisionCollider = _spCollider;
}

HRESULT UAnimModel::CreateAnimation(const  VECTOR<ANIMDESC>& _convecAnimDesc, const _wstring& _wstrPath)
{
	m_vecAnimations.reserve(_convecAnimDesc.size());
	_uint iIndex{ 0 };

	SHPTR<UAnimModel> spAnimModel = ThisShared<UAnimModel>();

	for (auto& iter : _convecAnimDesc)
	{
		SHPTR<UAnimation> spAnimation{ CreateNative<UAnimation>(ThisShared<UAnimModel>(), iter, iIndex) };
		RETURN_CHECK(nullptr == spAnimation, E_FAIL);

		m_vecAnimations.push_back(spAnimation);

		m_AnimNamesGroup.insert(std::pair<_wstring, _uint>(spAnimation->GetAnimName(), iIndex++));
		spAnimation->LoadAnimSectionDataPathIsFolder(_wstrPath);
		spAnimation->LoadAnimEventDataPathIsFolder(spAnimModel, _wstrPath);
	}
	return S_OK;
}

void UAnimModel::LoadToData(const _wstring& _wstrPath)
{
	std::ifstream read{ _wstrPath, std::ios::binary};
	RETURN_CHECK(!read, ;);
	ANIMMODELDESC tDesc;
	{
		BringModelName(_wstrPath); 
		// MaterialInfos 
		MATERIALINFOS		vecMaterialInfo;
		// MESH
		LoadAnimMeshData(read, tDesc.Meshes);
		// BoneNode
		LoadBoneData(read, tDesc.BNodeDatas);
		// Material
		LoadMaterial(read, tDesc.MatrialData, vecMaterialInfo);
		// Animations
		LoadAnimationData(read, tDesc.Animes);

		_wstring wstrPath = _wstrPath.c_str();
		_uint iFindIndex{ static_cast<_uint>(wstrPath.find_last_of(L"\\")) };
		wstrPath = wstrPath.substr(0, iFindIndex);

		_wstring RootBoneNodeName{L""};
		size_t AnimSize = tDesc.Animes.size();
		if (AnimSize > 0)
		{
			RootBoneNodeName = tDesc.Animes[0].Channels[0].wstrBoneName;
		}
		// Create
		CreateBoneNode(&tDesc, RootBoneNodeName);
		CreateMeshContainers(&tDesc);
		CreateMaterial(&tDesc, vecMaterialInfo);
		CreateAnimation(tDesc.Animes, wstrPath);
		// BondMatrix
		m_vecSetupBonMatrix.resize(GetMeshContainerCnt());
		for (_uint i = 0; i < GetMeshContainerCnt(); ++i)
		{
			for (auto& iter : m_vecSetupBonMatrix[i])
				iter = _float4x4::Identity;
		}

		CreateShaderConstantBuffer();
		SetAnimation(0);

		// Create MeshFilter 
		m_spMeshFilterController = CreateConstructorNative<UMeshFilter>(GetDevice(), wstrPath, GetMeshContainers());
	}
}

void UAnimModel::LoadAnimMeshData(std::ifstream& _ifRead, VECTOR<ANIMMESHDESC>& _convecMeshes)
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
				iter.pVtxModel = Make::AllocBuffer<VTXANIMMODEL>(iter.iNumVertices);
				size_t vtxModel = sizeof(VTXANIMMODEL) * iter.iNumVertices;
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

void UAnimModel::LoadAnimationData(std::ifstream& _ifRead, VECTOR<ANIMDESC>& _convecAnimes)
{
	_uint iSize{ 0 };
	_ifRead.read((_char*)&iSize, sizeof(_uint));
	if (0 != iSize)
	{
		_convecAnimes.resize(iSize);
		for (auto& Animation : _convecAnimes)
		{
			_ifRead.read((_char*)&Animation.stExtraData, sizeof(ANIMEXTRADATA));
			UMethod::ReadString(_ifRead, Animation.wstrName);
			_uint iChannelSize{ 0 };
			_ifRead.read((_char*)&iChannelSize, sizeof(_uint));
			if (iChannelSize == 0)
				continue;
			Animation.Channels.resize(iChannelSize);
			for (auto& Channel : Animation.Channels)
			{
				UMethod::ReadString(_ifRead, Channel.wstrBoneName);
				_ifRead.read((_char*)&Channel.iNumMaxKeyFrames, sizeof(_uint));
				Channel.pKeyFrames = Make::AllocBuffer<KEYFRAME>(Channel.iNumMaxKeyFrames);
				_ifRead.read((_char*)Channel.pKeyFrames, sizeof(KEYFRAME) * Channel.iNumMaxKeyFrames);
			}
		}
	}
}

void UAnimModel::ChangeAnimIndex(const _uint& _iAnimIndex, _uint& _iIndex)
{
	if (m_vecAnimations.size() <= _iAnimIndex)
	{
		_iIndex = 0;
	}
	else
	{
		_iIndex = _iAnimIndex;
	}
}

void UAnimModel::ChangeAnimIndex(const _wstring& _wstrAnimName, _uint& _iIndex)
{
	const auto& FindIterator = m_AnimNamesGroup.find(_wstrAnimName);
	if (m_AnimNamesGroup.end() == FindIterator)
	{
		_iIndex = 0;
	}
	else
	{
		_iIndex = FindIterator->second;
	}
}

void UAnimModel::SettingCurAnimSituation()
{
	assert(nullptr != m_vecAnimations[m_iCurAnimIndex]);
	m_spCurAnimation = m_vecAnimations[m_iCurAnimIndex];
	m_spCurAnimation->ResetData();
	m_spNextAnimation = nullptr;

	SHPTR<URootBoneNode> spRootBoneNode{ GetRootBoneNode() };
	if(nullptr != spRootBoneNode)
		spRootBoneNode->ResetRootBoneInfo();
}

void UAnimModel::SettingNextAnimSituation()
{
	m_spNextAnimation = m_vecAnimations[m_iNextAnimIndex];
	// Reset
	m_spCurAnimation->ResetData();
	m_spNextAnimation->ResetData();
	SHPTR<URootBoneNode> spRootBoneNode{ GetRootBoneNode() };
	if (nullptr != spRootBoneNode)
		spRootBoneNode->ResetRootBoneInfo();
}

HRESULT UAnimModel::CreateShaderConstantBuffer()
{
	m_spAnimShaderConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::ANIMMODEL, GetTypeSize<ANIMATIONPARAM>());
	RETURN_CHECK(nullptr == m_spAnimShaderConstantBuffer, E_FAIL);

	m_spBoneMatrixShaderConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BONEMATRIX, GetTypeSize<BONEMATRIXPARAM>());
	RETURN_CHECK(nullptr == m_spBoneMatrixShaderConstantBuffer, E_FAIL);

	m_spPrevBoneMatrixShaderConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PREVBONEMATRIX, GetTypeSize<BONEMATRIXPARAM>());
	RETURN_CHECK(nullptr == m_spPrevBoneMatrixShaderConstantBuffer, E_FAIL);
	
	m_spDrawRimLightBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::ANIMMODELRIMDRAW, GetTypeSize<RIMDRAW>());
	RETURN_CHECK(nullptr == m_spDrawRimLightBuffer, E_FAIL);
	return S_OK;
}