#include "EngineDefine.h"
#include "AnimSectionEvents.h"
#include "UMethod.h"
#include "UAnimModel.h"
#include "UCollider.h"
#include "UBoneNode.h"
#include "UGameInstance.h"
#include "UPawn.h"
#include "UTransform.h"
#include "UAnimation.h"
#include "USound.h"
#include "UCharacter.h"

UAnimChangeBetweenEvent::UAnimChangeBetweenEvent() : 
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN), 
	m_AnimChangeDesc{}
{
}

UAnimChangeBetweenEvent::UAnimChangeBetweenEvent(const UAnimChangeBetweenEvent& _rhs) : 
	UAnimSectionEvent(_rhs), m_AnimChangeDesc{_rhs.m_AnimChangeDesc }
{
}

UAnimChangeBetweenEvent::UAnimChangeBetweenEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) :
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN)
{ 
	LoadEvent(_spAnimModel, _load);
}

SHPTR<UAnimEvent> UAnimChangeBetweenEvent::Clone(UAnimModel* _pAnimModel)
{
	return std::move(CloneThis<UAnimChangeBetweenEvent>(*this));
}

const ANIMOTHEREVENTDESC*  UAnimChangeBetweenEvent::OutOtherEventDesc()
{
	return &m_AnimChangeDesc;
}

void UAnimChangeBetweenEvent::EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc)
{
	RETURN_CHECK(_pAnimModel->GetCurrentAnimIndex() == m_AnimChangeDesc.iNextAnimIndex, ;);

	// ���� EnableLastLerp�� ���� ���� �ʴٸ�
	if (false == m_AnimChangeDesc.isEnableLastSettingAnim)
	{
		// �ٲ�� �ϴ� ������ �ٲ۴�. 
		if (m_AnimChangeDesc.fAnimChangeTime <= _dTimeAcc)
		{
			ResetEventData();
			// ���� event ��Ȱ��ȭ
			_pAnimModel->SetSupplyLerpValue(m_AnimChangeDesc.fSupplyAnimValue);
			_pAnimModel->ChangeAnimation(m_AnimChangeDesc.iNextAnimIndex, m_AnimChangeDesc.dNextAnimTimeAcc);
		}
	}
	// ���� �ִٸ�
	else
	{
		// ����
		if (_pAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= m_AnimChangeDesc.fLastProgressValue)
		{
			ResetEventData();
			_pAnimModel->SetAnimation(m_AnimChangeDesc.iNextAnimIndex, m_AnimChangeDesc.dNextAnimTimeAcc);
		}
	}
}

void UAnimChangeBetweenEvent::SaveEvent(std::ofstream& _save)
{
	__super::SaveEvent(_save);
	_save.write((_char*)&m_AnimChangeDesc, sizeof(ANIMCHANGEDESC));
}

void UAnimChangeBetweenEvent::LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load)
{
	__super::LoadEvent(_spAnimModel, _load);
	_load.read((_char*)&m_AnimChangeDesc, sizeof(ANIMCHANGEDESC));
}

void UAnimChangeBetweenEvent::Free()
{
}

/*
=================================================
AnimChangeBetweenEvent
=================================================
AnimColliderEvent
=================================================
*/

UAnimColliderEvent::UAnimColliderEvent() : 
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER), m_AnimColliderDesc{}
{

}

UAnimColliderEvent::UAnimColliderEvent(const UAnimColliderEvent& _rhs) :
	UAnimSectionEvent(_rhs), m_AnimColliderDesc{ _rhs.m_AnimColliderDesc }
{

}

UAnimColliderEvent::UAnimColliderEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) :
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER), m_AnimColliderDesc{}
{
	LoadEvent(_spAnimModel, _load);
}

SHPTR<UAnimEvent> UAnimColliderEvent::Clone(UAnimModel* _pAnimModel)
{
	SHPTR<UAnimColliderEvent> spAnimEvent = CloneThis<UAnimColliderEvent>(*this);
	if (nullptr != _pAnimModel)
	{
		spAnimEvent->m_AnimColliderDesc.spCollider = _pAnimModel->BringAttackCollider(m_AnimColliderDesc.iColliderType);
		if (nullptr != m_AnimColliderDesc.spBoneNode)
		{
			spAnimEvent->m_AnimColliderDesc.spBoneNode = _pAnimModel->FindBoneNode(m_AnimColliderDesc.spBoneNode->GetName());
		}
	}
	return std::move(spAnimEvent);
}


_bool UAnimColliderEvent::EventCheck(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc, const _wstring& _wstrInputTrigger)
{
#ifdef _USE_DEBUGGING
	if (nullptr != m_AnimColliderDesc.spCollider)
	{
		SHPTR<UTransform> spTransform = _pPawn->GetTransform();

		m_AnimColliderDesc.spCollider->SetTranslate(m_AnimColliderDesc.vColliderTranslation);
		m_AnimColliderDesc.spCollider->SetScale(m_AnimColliderDesc.vColliderScale);

		if (nullptr != m_AnimColliderDesc.spBoneNode){
			m_AnimColliderDesc.spCollider->SetTransform(m_AnimColliderDesc.spBoneNode->GetCombineMatrix() * _pAnimModel->GetPivotMatirx() *  spTransform->GetWorldMatrix());
		}
		else
		{
			m_AnimColliderDesc.spCollider->SetTransform(_pAnimModel->GetPivotMatirx() * spTransform->GetWorldMatrix());
		}	
	}
#endif
	if (GetAnimSectionDesc().IsAnimEventActive(_dTimeAcc))
	{
		EventSituation(_pPawn, _pAnimModel, _dTimeDelta, _dTimeAcc);
		return true;
	}
	_pAnimModel->UpdateAttackData( false, m_AnimColliderDesc.spCollider);
	return false;
}

const ANIMOTHEREVENTDESC*  UAnimColliderEvent::OutOtherEventDesc()
{
	return &m_AnimColliderDesc;
}

void UAnimColliderEvent::EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc)
{
	_pAnimModel->UpdateAttackData(true, m_AnimColliderDesc.spCollider);
}

void UAnimColliderEvent::SaveEvent(std::ofstream& _save)
{
	RETURN_CHECK(nullptr == m_AnimColliderDesc.spCollider, ;);

	__super::SaveEvent(_save);
	if (nullptr == m_AnimColliderDesc.spBoneNode)
	{
		UMethod::SaveString(_save, L"");
	}
	else
	{
		UMethod::SaveString(_save, m_AnimColliderDesc.spBoneNode->GetName());
	}
	_save.write((_char*)&m_AnimColliderDesc.iColliderType, sizeof(_int));

	UCollider::COLLIDERDESC Desc(m_AnimColliderDesc.spCollider->GetScale(), m_AnimColliderDesc.spCollider->GetTranslate());
	_save.write((_char*)&Desc, sizeof(UCollider::COLLIDERDESC));
}

void UAnimColliderEvent::LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load)
{
	__super::LoadEvent(_spAnimModel, _load);
	_wstring wstrBoneName;
	UMethod::ReadString(_load, wstrBoneName);
	_load.read((_char*)&m_AnimColliderDesc.iColliderType, sizeof(_int));

	UCollider::COLLIDERDESC Desc;
	_load.read((_char*)&Desc, sizeof(UCollider::COLLIDERDESC));

	m_AnimColliderDesc.vColliderScale = Desc.vScale;
	m_AnimColliderDesc.vColliderTranslation = Desc.vTranslation;
	m_AnimColliderDesc.spBoneNode = _spAnimModel->FindBoneNode(wstrBoneName);
}

void UAnimColliderEvent::Free()
{
	m_AnimColliderDesc.spCollider.reset();
}

/*
=================================================
AnimColliderEvent
=================================================
AnimSoundEvent
=================================================
*/

UAnimSoundEvent::UAnimSoundEvent() :
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_SOUND), m_pSound{ nullptr }, m_isPlayOnce{ false }
{
}

UAnimSoundEvent::UAnimSoundEvent(const UAnimSoundEvent& _rhs) :
	UAnimSectionEvent(_rhs), m_AnimSoundDesc{ _rhs.m_AnimSoundDesc }, m_pSound{ nullptr }, m_isPlayOnce{ false }
{
}

UAnimSoundEvent::UAnimSoundEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) :
	UAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_SOUND), m_pSound{ nullptr }, m_isPlayOnce{ false }
{
	LoadEvent(_spAnimModel, _load);
}

SHPTR<UAnimEvent> UAnimSoundEvent::Clone(UAnimModel* _pAnimModel)
{
	return 	CloneThis<UAnimSoundEvent>(*this);
}

const ANIMOTHEREVENTDESC* UAnimSoundEvent::OutOtherEventDesc()
{
	return &m_AnimSoundDesc;
}

_bool UAnimSoundEvent::EventCheck(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc, const _wstring& _wstrInputTrigger)
{
	if (GetAnimSectionDesc().IsAnimEventActive(_dTimeAcc))
	{
		EventSituation(_pPawn, _pAnimModel, _dTimeDelta, _dTimeAcc);
		return true;
	}
	else
	{
		if (nullptr != m_pSound)
		{
			m_pSound->Stop();
		}
		m_isPlayOnce = false;
	}
	return false;
}

void UAnimSoundEvent::EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc)
{
	RETURN_CHECK(true == m_AnimSoundDesc.wstrSoundName.empty(), ;);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	USound* pSound = spGameInstance->BringSound(m_AnimSoundDesc.wstrSoundName).get();
	{
		if (nullptr != m_pSound && pSound != m_pSound)
		{
			m_pSound->Stop();
			m_pSound = pSound;
		}
		m_pSound = pSound;
		m_pSound->SetVolume(m_AnimSoundDesc.fSoundVolume);
		m_pSound->ChangeMinMaxDistance3D(m_AnimSoundDesc.fMinSoundDistance, m_AnimSoundDesc.fMaxSoundDistance);

		if (nullptr == m_wpPlayerCharacter.lock())
		{
			m_wpPlayerCharacter = spGameInstance->GetCurrPlayer();
		}
	}
	SHPTR<UActor> spPlayer = m_wpPlayerCharacter.lock();

	m_pSound->Tick();
	if (false == m_pSound->IsSoundPlay() || false == m_isPlayOnce) {
		m_pSound->Play();
		m_isPlayOnce = true;
	}
	if (nullptr == spPlayer)
		m_pSound->UpdateSound3D(_pPawn->GetTransform(), m_AnimSoundDesc.vSoundVelocity);
	else
		m_pSound->UpdateSound3D(_pPawn->GetTransform(), m_AnimSoundDesc.vSoundVelocity, spPlayer->GetTransform());
}

void UAnimSoundEvent::SaveEvent(std::ofstream& _save)
{
	__super::SaveEvent(_save);
	UMethod::SaveString(_save, m_AnimSoundDesc.wstrSoundName);
	UMethod::SaveOther(_save, &m_AnimSoundDesc.vSoundVelocity);
	UMethod::SaveOther(_save, &m_AnimSoundDesc.fMaxSoundDistance);
	UMethod::SaveOther(_save, &m_AnimSoundDesc.fMinSoundDistance);
}

void UAnimSoundEvent::LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load)
{
	__super::LoadEvent(_spAnimModel, _load);
	UMethod::ReadString(_load, m_AnimSoundDesc.wstrSoundName);
	UMethod::ReadOther(_load, &m_AnimSoundDesc.vSoundVelocity);
	UMethod::ReadOther(_load, &m_AnimSoundDesc.fMaxSoundDistance);
	UMethod::ReadOther(_load, &m_AnimSoundDesc.fMinSoundDistance);
}

void UAnimSoundEvent::Free()
{
}
