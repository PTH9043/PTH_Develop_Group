#include "CoreDefines.h"
#include "AAnimator.h"
#include "ABoneNode.h"
#include "ARootBoneNode.h"
#include "AAnimation.h"
#include "ACollider.h"
#include "ATransform.h"

namespace Core
{
	AAnimator::AAnimator(SHPTR<ACoreInstance> _spCoreInstance, const _string& _strFolderPath, const _string& _strFileName,
		const _float4x4& _PivotMatrix) :
		ACoreObject(_spCoreInstance),  m_iCurAnimIndex{0}, m_iNextAnimIndex{0}, m_PivotMatirx{ _PivotMatrix }, m_fSupplyLerpValue{0},
	m_isCanAttackSituation{false}, m_BoneContainer{}, m_BoneSelector{}, m_AnimContainer{}, m_AnimSelector{},
	m_spRootBoneNode{nullptr},	m_spCurAnimation{nullptr},	m_spNextAnimation{nullptr},	m_AnimEventColliderContainer{},
		m_spAttackCollisionCollider{nullptr}
	{
		_string RealPath = _strFolderPath +  _strFileName;
		std::ifstream read{ RealPath, std::ios::binary };

		size_t boneNodeCnt = 0;
		size_t AnimCnt = 0;
		_int AnimIndex = 0;

		read.read((_char*)&boneNodeCnt, sizeof(size_t));
		read.read((_char*)&AnimCnt, sizeof(size_t));
		read.read((_char*)&AnimIndex, sizeof(_int));
		// Root Bone 할당
		for (_int i = 0; i < boneNodeCnt; ++i)
		{
			SHPTR<ABoneNode> spBoneNode = nullptr;
			if (AnimIndex == i)
			{
				m_spRootBoneNode = CreateInitNative<ARootBoneNode>(read);
				spBoneNode = m_spRootBoneNode;
			}
			else
			{
				spBoneNode = CreateInitNative<ABoneNode>(read);
			}
			m_BoneContainer.push_back(spBoneNode);
			m_BoneSelector.emplace(MakePair(spBoneNode->GetName(), i));
		}

		for (_int i = 0; i < AnimCnt; ++i)
		{
			SHPTR<AAnimation> spAnimation = CreateInitNative<AAnimation>(this, read);
			spAnimation->LoadAnimEventData(this, _strFolderPath);
			spAnimation->LoadAnimSectionData(_strFolderPath);
			m_AnimContainer.push_back(spAnimation);
			m_AnimSelector.emplace(MakePair(spAnimation->GetAnimName(), i));
		}

		ACollider::COLLIDERDESC Desc{};
		m_AnimEventColliderContainer.emplace(MakePair(ACollider::TYPE_AABB, Create<ACollider>(ACollider::TYPE_AABB, Desc)));
		m_AnimEventColliderContainer.emplace(MakePair(ACollider::TYPE_OBB, Create<ACollider>(ACollider::TYPE_OBB, Desc)));
		m_AnimEventColliderContainer.emplace(MakePair(ACollider::TYPE_SPHERE, Create<ACollider>(ACollider::TYPE_SPHERE, Desc)));

		m_spRootBoneNode->OnRootBoneNode();
		SetAnimation(0);
	}

	AAnimator::AAnimator(const AAnimator& _rhs) : 
		ACoreObject(_rhs), m_iCurAnimIndex{ 0 }, m_iNextAnimIndex{ 0 }, m_PivotMatirx{ _float4x4::Identity }, m_fSupplyLerpValue{ 0 },
		m_isCanAttackSituation{ false }, m_BoneContainer{}, m_BoneSelector{}, m_AnimContainer{}, m_AnimSelector{},
		m_spRootBoneNode{ nullptr }, m_spCurAnimation{ nullptr }, m_spNextAnimation{ nullptr }, m_AnimEventColliderContainer{},
		m_spAttackCollisionCollider{ nullptr }
	{
	}


	void AAnimator::TickEvent(APawn* _pPawn, const _string& _strInputTrigger, const _double& _TimeDelta)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		spCurAnim->TickAnimEvent(_pPawn, this, _TimeDelta, _strInputTrigger);
	}

	void AAnimator::TickEventToRatio(APawn* _pPawn, const _string& _strInputTrigger, const _double& _dRatio, const _double& _TimeDelta)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		spCurAnim->TickAnimEvent(_pPawn, this, _TimeDelta, _dRatio, _strInputTrigger);
	}

	void AAnimator::TickAnimation(const _double& _dTimeDelta)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		if (nullptr != m_spNextAnimation)
		{
			spCurAnim->UpdateNextAnimTransformMatrices(_dTimeDelta, m_fSupplyLerpValue, m_spNextAnimation);
			if (false == spCurAnim->IsSupplySituation())
			{
				spCurAnim = m_spNextAnimation;
				m_spNextAnimation = nullptr;
			}
		}
		else
		{
			spCurAnim->UpdateBoneMatrices(_dTimeDelta);
		}

		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		{
			for (auto& BoneNode : m_BoneContainer)
				BoneNode->UpdateCombinedMatrix(m_PivotMatirx);
		}
	}

	void AAnimator::TickAnimChangeTransform(CSHPTRREF<ATransform> _spTransform, const _double& _dTimeDelta)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		ASSERT_CRASH(nullptr != _spTransform && nullptr != spCurAnim);
		TickAnimation(_dTimeDelta);

		RETURN_CHECK(true == spCurAnim->IsSupplySituation(), ;);

		SHPTR<ARootBoneNode> spRootBoneNode = m_spRootBoneNode;
		if (true == spCurAnim->IsFinishAnim())
		{
			spRootBoneNode->ResetRootBoneInfo();
			return;
		}

		if (true == spCurAnim->IsApplyRootBoneMove())
		{
			Vector3 Position = Vector3::TransformCoord(spRootBoneNode->GetMoveRootBonePos(), m_PivotMatirx * _spTransform->GetWorldMatrix());
			_spTransform->SetPos(Position);
		}
	}

	void AAnimator::TickAnimToTimeAccChangeTransform(CSHPTRREF<ATransform> _spTransform, const _double& _dTimeDelta, const _double& _TimeAcc)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		ASSERT_CRASH(nullptr != _spTransform && nullptr != spCurAnim);
		UpdateCurAnimationToRatio(_TimeAcc);

		if (true == spCurAnim->IsApplyRootBoneMove())
		{
			SHPTR<ARootBoneNode> spRootBoneNode = m_spRootBoneNode;
			Vector3 Position = Vector3::TransformCoord(spRootBoneNode->GetMoveRootBonePos(), m_PivotMatirx * _spTransform->GetWorldMatrix());
			_spTransform->SetPos(Position);
		}
	}

	void AAnimator::UpdateCurAnimationToRatio(const _double& _dRatio)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		spCurAnim->UpdateboneMatricesToRatio(_dRatio);
		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		{
			for (auto& BoneNode : m_BoneContainer)
				BoneNode->UpdateCombinedMatrix(m_PivotMatirx);
		}
	}

	SHPTR<ABoneNode> AAnimator::FindBoneNode(const _string& _strBoneNode) const
	{
		const auto& iter = m_BoneSelector.find(_strBoneNode);
		RETURN_CHECK(m_BoneSelector.end() == iter, nullptr);
		return m_BoneContainer[iter->second];
	}

	void AAnimator::UpdateAttackData(const _bool _isCanAttackSituation, CSHPTRREF<ACollider> _spCollider)
	{
		m_isCanAttackSituation = _isCanAttackSituation;
		m_spAttackCollisionCollider = _spCollider;
	}

	void AAnimator::ResetCurAnimEvent()
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		if (nullptr != spCurAnim)
			spCurAnim->ResetAnimChangeEventNode();
	}

	void AAnimator::SetAnimation(const _int& _iAnimIndex)
	{
		ChangeAnimIndex(_iAnimIndex, m_iCurAnimIndex);
		// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
		SettingCurAnimSituation();
	}

	void AAnimator::SetAnimation(const _string& _strAnimName)
	{
		ChangeAnimIndex(_strAnimName, m_iCurAnimIndex);
		// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
		SettingCurAnimSituation();
	}

	void AAnimator::SetAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		SetAnimation(_iAnimIndex);
		// Change Time Acc
		spCurAnim->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
	}

	void AAnimator::SetAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc)
	{
		SHPTR<AAnimation> spCurAnim = m_spCurAnimation;
		SetAnimation(_strAnimName);
		// Change Time Acc
		spCurAnim->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
	}

	void AAnimator::ChangeAnimation(const _int& _iAnimIndex)
	{
		ChangeAnimation(_iAnimIndex, 0.0);
	}

	void AAnimator::ChangeAnimation(const _string& _strAnimName)
	{
		ChangeAnimation(_strAnimName, 0.0);
	}

	void AAnimator::ChangeAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc)
	{
		SHPTR<AAnimation> spNextAnim = m_spNextAnimation;
		ChangeAnimIndex(_iAnimIndex, m_iNextAnimIndex);
		// 다음 애니메이션이 세팅되는 상황일 때의 함수 실행
		SettingNextAnimSituation();
		// Change Time Acc
		spNextAnim->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
	}

	void AAnimator::ChangeAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc)
	{
		SHPTR<AAnimation> spNextAnim = m_spNextAnimation;
		ChangeAnimIndex(_strAnimName, m_iCurAnimIndex);
		// 현재 애니메이션이 세팅되는 상황일 때의 함수 실행
		SettingNextAnimSituation();
		// Change Time Acc
		spNextAnim->UpdateTimeAccToChannelIndex(_dNextTimeAcc);
	}

	SHPTR<ACollider> AAnimator::BringAttackCollider(_int _iColliderType)
	{
		SHPTR<ACollider> spCollider = m_spAttackCollisionCollider;
		const auto& iter = m_AnimEventColliderContainer.find(_iColliderType);
		RETURN_CHECK(m_AnimEventColliderContainer.end() == iter, nullptr);
		spCollider = iter->second;
		return iter->second;
	}

	_bool AAnimator::IsCollisionAttackCollider(SHPTR<ACollider> _spEnemyCollider)
	{
		SHPTR<ACollider> spCollider = m_spAttackCollisionCollider;
		RETURN_CHECK(false == m_isCanAttackSituation, false);
		RETURN_CHECK(nullptr == m_spAttackCollisionCollider, false);
		return spCollider->IsCollision(_spEnemyCollider);
	}

	void AAnimator::ChangeAnimIndex(const _int& _iAnimIndex, ATOMIC<_int>& _iIndex)
	{
		if (m_AnimContainer.size() <= _iAnimIndex)
		{
			_iIndex = 0;
		}
		else
		{
			_iIndex = _iAnimIndex;
		}
	}

	void AAnimator::ChangeAnimIndex(const _string& _strAnimName, ATOMIC<_int>& _iIndex)
	{
		const auto& FindIterator = m_AnimSelector.find(_strAnimName);
		if (m_AnimSelector.end() == FindIterator)
		{
			_iIndex = 0;
		}
		else
		{
			_iIndex = FindIterator->second;
		}
	}

	void AAnimator::SettingCurAnimSituation()
	{
		ASSERT_CRASH(nullptr != m_AnimContainer[m_iCurAnimIndex]);
		m_spCurAnimation = m_AnimContainer[m_iCurAnimIndex];
		m_spCurAnimation->ResetData();
		m_spNextAnimation = nullptr;

		SHPTR<ARootBoneNode> spRootBoneNode = m_spRootBoneNode;
		if (nullptr != spRootBoneNode)
			spRootBoneNode->ResetRootBoneInfo();
	}

	void AAnimator::SettingNextAnimSituation()
	{
	}

	void AAnimator::Free()
	{
	}
}