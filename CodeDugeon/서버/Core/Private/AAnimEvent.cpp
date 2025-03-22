#include "CoreDefines.h"
#include "AAnimEvent.h"
#include "AAnimator.h"
#include "AAnimation.h"
#include "ABoneNode.h"
#include "ACollider.h"

namespace Core {
	AAnimEvent::AAnimEvent(ANIMEVENTTYPE _AnimEventType, ANIMEVENTCATEGORY _AnimEventCategory) : 
		m_AnimEventType{_AnimEventType}, m_AnimEventCategory{_AnimEventCategory}
	{
	}

	AAnimEvent::AAnimEvent(const AAnimEvent& _rhs) : 
		m_AnimEventType{_rhs.m_AnimEventType}, m_AnimEventCategory{_rhs.m_AnimEventCategory}
	{
	}

	void AAnimEvent::LoadEvent(AAnimator* _pAnimator, std::ifstream& _load)
	{
		_load.read((_char*)&m_AnimEventType, sizeof(ANIMEVENTTYPE));
	}

	void AAnimEvent::Free()
	{
	}

	/*
	=====================================
	AnimEvent
	=====================================
	AnimSectionEvent
	=====================================
	*/

	AAnimSectionEvent::AAnimSectionEvent(ANIMEVENTTYPE _AnimEventType) :
		AAnimEvent(_AnimEventType, ANIMEVENTCATEGORY::CATEGROY_SECTION), m_AnimSectionDesc{}
	{
	}

	AAnimSectionEvent::AAnimSectionEvent(const AAnimSectionEvent& _rhs) :
		AAnimEvent(_rhs), m_AnimSectionDesc{ _rhs.m_AnimSectionDesc }
	{
	}

	AAnimSectionEvent::AAnimSectionEvent(const ANIMEVENTSECTIONDESC& _AnimEventDesc, ANIMEVENTTYPE _AnimEventType) :
		m_AnimSectionDesc{ _AnimEventDesc }, AAnimEvent(_AnimEventType, ANIMEVENTCATEGORY::CATEGROY_SECTION)
	{
	}

	_bool AAnimSectionEvent::EventCheck(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta, 
		const _double& _dTimeAcc, const _string& _strInputTrigger)
	{
		if (false == m_AnimSectionDesc.isAnimChangeActive)
		{
			if (m_AnimSectionDesc.IsAnimEventActive(_dTimeAcc))
			{
				if ((!strcmp(m_AnimSectionDesc.strEventTrigger.c_str(), _strInputTrigger.c_str())
					|| !strcmp(m_AnimSectionDesc.strEventTrigger.c_str(), "")))
				{
					m_AnimSectionDesc.isAnimChangeActive = true;
					EventSituation(_pPawn, _pAnimator, _dTimeDelta, _dTimeAcc);
					return true;
				}
			}
		}
		else
		{
			EventSituation(_pPawn, _pAnimator, _dTimeDelta, _dTimeAcc);
			return false;
		}
		return false;
	}

	void AAnimSectionEvent::EventSituation(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta, const _double& _dTimeAcc)
	{
	}

	void AAnimSectionEvent::LoadEvent(AAnimator* _pAnimator, std::ifstream& _load)
	{
		__super::LoadEvent(_pAnimator, _load);
		_wstring wstrTrigger{ L"" };
		ReadString(_load, wstrTrigger);
		m_AnimSectionDesc.strEventTrigger = ConvertWToS(wstrTrigger);

		_load.read((_char*)&m_AnimSectionDesc.dStartTime, sizeof(_double));
		_load.read((_char*)&m_AnimSectionDesc.dEndTime, sizeof(_double));
		_load.read((_char*)&m_AnimSectionDesc.dStopTime, sizeof(_double));
	}

	void AAnimSectionEvent::ResetEventData(AAnimator* _pAnimator)
	{
		m_AnimSectionDesc.isActiveEvent = false;
		m_AnimSectionDesc.isAnimChangeActive = false;
	}

	void AAnimSectionEvent::Free()
	{
	}

	/*
	=====================================
	AnimSectionEvent
	=====================================
	AnimChangeBetweenEvent
	=====================================
	*/

	AAnimChangeBetweenEvent::AAnimChangeBetweenEvent() :
		AAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN),
		m_AnimChangeDesc{}
	{
	}

	AAnimChangeBetweenEvent::AAnimChangeBetweenEvent(const AAnimChangeBetweenEvent& _rhs) :
		AAnimSectionEvent(_rhs), m_AnimChangeDesc{ _rhs.m_AnimChangeDesc }
	{
	}

	AAnimChangeBetweenEvent::AAnimChangeBetweenEvent(AAnimator* _pAnimator, std::ifstream& _load) : 
		AAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN)
	{
		LoadEvent(_pAnimator, _load);
	}

	SHPTR<AAnimEvent> AAnimChangeBetweenEvent::Clone(AAnimator* _pAnimator)
	{
		return std::move(CloneThis<AAnimChangeBetweenEvent>(*this));
	}

	_bool AAnimChangeBetweenEvent::EventCheck(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta, const _double& _dTimeAcc, const _string& _strInputTrigger)
	{
		return __super::EventCheck(_pPawn, _pAnimator, _dTimeDelta, _dTimeAcc, _strInputTrigger);
	}

	const ANIMOTHEREVENTDESC* AAnimChangeBetweenEvent::OutOtherEventDesc()
	{
		return &m_AnimChangeDesc;
	}

	void AAnimChangeBetweenEvent::EventSituation(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta,
		const _double& _dTimeAcc)
	{
		RETURN_CHECK(_pAnimator->GetCurAnimIndex() == m_AnimChangeDesc.iNextAnimIndex, ;);

		// 만약 EnableLastLerp가 켜져 있지 않다면
		if (false == m_AnimChangeDesc.isEnableLastSettingAnim)
		{
			// 바꿔야 하는 구간에 바꾼다. 
			if (m_AnimChangeDesc.fAnimChangeTime <= _dTimeAcc)
			{
				ResetEventData(_pAnimator);
				// 현재 event 비활성화
				_pAnimator->SetSupplyLerpValue(m_AnimChangeDesc.fSupplyAnimValue);
				_pAnimator->ChangeAnimation(m_AnimChangeDesc.iNextAnimIndex, m_AnimChangeDesc.dNextAnimTimeAcc);
			}
		}
		// 켜저 있다면
		else
		{
			// 만약
			if (_pAnimator->GetCurAnimation()->GetAnimationProgressRate() >= m_AnimChangeDesc.fLastProgressValue)
			{
				ResetEventData(_pAnimator);
				_pAnimator->SetAnimation(m_AnimChangeDesc.iNextAnimIndex, m_AnimChangeDesc.dNextAnimTimeAcc);
			}
		}
	}

	void AAnimChangeBetweenEvent::LoadEvent(AAnimator* _pAnimator, std::ifstream& _load)
	{
		__super::LoadEvent(_pAnimator, _load);
		_load.read((_char*)&m_AnimChangeDesc, sizeof(ANIMCHANGEDESC));
	}

	void AAnimChangeBetweenEvent::Free()
	{
	}

	/*
	=================================================
	AnimChangeBetweenEvent
	=================================================
	AnimColliderEvent
	=================================================
	*/

	AAnimColliderEvent::AAnimColliderEvent() : 
		AAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER), m_AnimColliderDesc{}
	{
	}

	AAnimColliderEvent::AAnimColliderEvent(const AAnimColliderEvent& _rhs) :
		AAnimSectionEvent(_rhs), m_AnimColliderDesc{ _rhs.m_AnimColliderDesc }
	{
	}

	AAnimColliderEvent::AAnimColliderEvent(AAnimator* _pAnimator, std::ifstream& _load) :
		AAnimSectionEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER), m_AnimColliderDesc{}
	{
		LoadEvent(_pAnimator, _load);
	}

	SHPTR<AAnimEvent> AAnimColliderEvent::Clone(AAnimator* _pAnimator)
	{
		SHPTR<AAnimColliderEvent> spAnimEvent = CloneThis<AAnimColliderEvent>(*this);
		if (nullptr != _pAnimator)
		{
			spAnimEvent->m_AnimColliderDesc.spCollider = _pAnimator->BringAttackCollider(m_AnimColliderDesc.iColliderType);
			if (nullptr != m_AnimColliderDesc.spBoneNode)
			{
				spAnimEvent->m_AnimColliderDesc.spBoneNode = _pAnimator->FindBoneNode(m_AnimColliderDesc.spBoneNode->GetName());
			}
		}
		return std::move(spAnimEvent);
	}

	_bool AAnimColliderEvent::EventCheck(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta, const _double& _dTimeAcc, const _string& _strInputTrigger)
	{
		if (GetAnimSectionDesc().IsAnimEventActive(_dTimeAcc))
		{
			EventSituation(_pPawn, _pAnimator, _dTimeDelta, _dTimeAcc);
			return true;
		}
		_pAnimator->UpdateAttackData(false, m_AnimColliderDesc.spCollider);
		return false;
	}

	const ANIMOTHEREVENTDESC* AAnimColliderEvent::OutOtherEventDesc()
	{
		return &m_AnimColliderDesc;
	}

	void AAnimColliderEvent::EventSituation(APawn* _pPawn, AAnimator* _pAnimator, const _double& _dTimeDelta, const _double& _dTimeAcc)
	{
		_pAnimator->UpdateAttackData(true, m_AnimColliderDesc.spCollider);
	}

	void AAnimColliderEvent::LoadEvent(AAnimator* _pAnimator, std::ifstream& _load)
	{
		__super::LoadEvent(_pAnimator, _load);
		_wstring wstrBoneName;
		ReadString(_load, wstrBoneName);
		_load.read((_char*)&m_AnimColliderDesc.iColliderType, sizeof(_int));
		_string strBoneName = ConvertWToS(wstrBoneName);

		ACollider::COLLIDERDESC Desc;
		_load.read((_char*)&Desc, sizeof(ACollider::COLLIDERDESC));

		m_AnimColliderDesc.vColliderScale = Desc.vScale;
		m_AnimColliderDesc.vColliderTranslation = Desc.vTranslation;
		m_AnimColliderDesc.spBoneNode = _pAnimator->FindBoneNode(strBoneName);
	}

	void AAnimColliderEvent::Free()
	{
	}

}