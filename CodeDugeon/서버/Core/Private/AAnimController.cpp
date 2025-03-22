#include "CoreDefines.h"
#include "AAnimController.h"
#include "AAnimator.h"
#include "APawn.h"
#include "ATransform.h"
#include "AAnimation.h"

namespace Core {

	AAnimController::AAnimController(OBJCON_CONSTRUCTOR, SHPTR<APawn> _spPawn,
		const _string& _strFolderPath, const _string& _strFileName, const _float4x4& _PivotMatrix) :
		ACoreObject(OBJCON_CONDATA), m_spAnimator{ Create<AAnimator>(OBJCON_CONDATA,
			_strFolderPath, _strFileName, _PivotMatrix)}, m_wpOwnerPawn{_spPawn},
		m_dAccumulator{0},m_dElapsedTime{0}, m_strInputTrigger{""}, m_iPawnState{0}, m_iAnimState{0}
	{
	}

	void AAnimController::Tick(const _double& _dTimeDelta)
	{
		SHPTR<APawn> spPawn = m_wpOwnerPawn.lock();
		SHPTR<ATransform> spTransform = spPawn->GetTransform();

		m_spAnimator->TickEvent(spPawn.get(), m_strInputTrigger, _dTimeDelta);
		m_spAnimator->TickAnimChangeTransform(spTransform, _dTimeDelta);
	}

	SHPTR<ABoneNode> AAnimController::FindBoneNode(const _string& _strBoneNode) const
	{
		return m_spAnimator->FindBoneNode(_strBoneNode);
	}

	void AAnimController::UpdateAttackData(const _bool _isCanAttackSituation, CSHPTRREF<ACollider> _spCollider)
	{
		m_spAnimator->UpdateAttackData(_isCanAttackSituation, _spCollider);
	}

	void AAnimController::ResetCurAnimEvent()
	{
		m_spAnimator->ResetCurAnimEvent();
	}

	void AAnimController::SetAnimation(const _int& _iAnimIndex)
	{
		m_spAnimator->SetAnimation(_iAnimIndex);
	}

	void AAnimController::SetAnimation(const _string& _strAnimName)
	{
		m_spAnimator->SetAnimation(_strAnimName);
	}

	void AAnimController::SetAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc)
	{
		m_spAnimator->SetAnimation(_iAnimIndex, _dNextTimeAcc);
	}

	void AAnimController::SetAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc)
	{
		m_spAnimator->SetAnimation(_strAnimName, _dNextTimeAcc);
	}

	void AAnimController::ChangeAnimation(const _int& _iAnimIndex)
	{
		m_spAnimator->ChangeAnimation(_iAnimIndex);
	}

	void AAnimController::ChangeAnimation(const _string& _strAnimName)
	{
		m_spAnimator->ChangeAnimation(_strAnimName);
	}

	void AAnimController::ChangeAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc)
	{
		m_spAnimator->ChangeAnimation(_iAnimIndex, _dNextTimeAcc);
	}

	void AAnimController::ChangeAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc)
	{
		m_spAnimator->ChangeAnimation(_strAnimName, _dNextTimeAcc);
	}

	SHPTR<ACollider> AAnimController::BringAttackCollider(_int _iColliderType)
	{
		return m_spAnimator->BringAttackCollider(_iColliderType);
	}

	SHPTR<ACollider> AAnimController::GetAttackCollider()
	{
		return m_spAnimator->GetAttackCollider();
	}

	_bool AAnimController::IsCollisionAttackCollider(SHPTR<ACollider> _spEnemyCollider)
	{
		return m_spAnimator->IsCollisionAttackCollider(_spEnemyCollider);
	}

	void AAnimController::UpdateState(const _string& _strTrigger, const _int _AnimState)
	{
		SHPTR<AAnimator> spAnimator = GetAnimator();
		spAnimator->ResetCurAnimEvent();
		{
			WRITE_SPINLOCK(m_TriggerLock);
			m_strInputTrigger = _strTrigger;
		}
		m_iAnimState = _AnimState;
	}

	void AAnimController::ClearState()
	{
		{
			WRITE_SPINLOCK(m_TriggerLock);
			m_strInputTrigger = "";
		}
		m_iAnimState = -1;
	}

	const _int AAnimController::GetCurAnimIndex() const
	{
		return m_spAnimator->GetCurAnimIndex();
	}

	SHPTR<AAnimation> AAnimController::GetCurAnimation() const
	{
		return m_spAnimator->GetCurAnimation();
	}

	SHPTR<AAnimation> AAnimController::GetNextAnimation() const
	{
		return m_spAnimator->GetNextAnimation();
	}

	void AAnimController::SetSupplyLerpValue(const _float _fSupplyLerpValue)
	{
		m_spAnimator->SetSupplyLerpValue(_fSupplyLerpValue);
	}

	void AAnimController::SetOwnerPawnActiveStrong(_bool _isOwnerPawnActive)
	{
		bool isActive = m_isOwnerPawnActive.load();
		if (m_isOwnerPawnActive.compare_exchange_strong(isActive, _isOwnerPawnActive))
			return;
	}

	void AAnimController::SetOwnerPawnActiveWeak(_bool _isOwnerPawnActive)
	{
		if (IsOwnerPawnActive() == _isOwnerPawnActive)
			return;

		while (true)
		{
			bool isActive = m_isOwnerPawnActive.load();
			if (m_isOwnerPawnActive.compare_exchange_strong(isActive, _isOwnerPawnActive))
				break;
		}
	}

	void AAnimController::SetPawnState(_int _State)
	{
		if (GetPawnState() == _State)
			return;

		while (true)
		{
			_int iPawnState = m_iPawnState.load();
			if (true == m_iPawnState.compare_exchange_strong(iPawnState, _State))
				break;
		}
	}

	void AAnimController::SetAnimState(_int _iAnimState)
	{
		m_iAnimState = _iAnimState;
	}

	void AAnimController::Free()
	{
	}
}