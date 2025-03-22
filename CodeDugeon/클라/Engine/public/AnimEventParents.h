#pragma once

#include "UBase.h"

BEGIN(Engine)
class UAnimModel;
class UAnimator;
class UPawn;
/*
@ Date: 2024-02-10, Writer: 박태현
@ Explain
- Animation Event를 정의할 부모 클래스
*/
class UAnimEvent abstract : public UBase {
protected:
	UAnimEvent(ANIMEVENTTYPE _AnimEventType, ANIMEVENTCATEGORY _AnimEventCategory);
	UAnimEvent(const UAnimEvent& _rhs);
	DESTRUCTOR(UAnimEvent)
public:
	const ANIMEVENTTYPE GetAnimEventType() const { return m_AnimEventType; }
	const ANIMEVENTCATEGORY GetAnimEventCategory() const { return m_AnimEventCategory; }
public:
	virtual SHPTR<UAnimEvent> Clone(UAnimModel* _pAnimModel = nullptr) PURE;
public:
	virtual _bool EventCheck(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc,
		const _wstring& _wstrInputTrigger) PURE;
	// AnimEventDesc를 상속받는 구조체를 리턴하는 함수
	virtual const  ANIMEVENTDESC*  OutAnimEventDesc() PURE;
	// AnimEvent에 다른 구조체가 필요할 경우 해당 구조체의 상속을 받는 녀석들만 내보낼 수 있도록 한다.
	virtual const ANIMOTHEREVENTDESC*  OutOtherEventDesc() PURE;

	virtual void SaveEvent(std::ofstream& _save) PURE;
	virtual void LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) PURE;
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc) PURE;
private:
	virtual void Free() PURE;
private:
	ANIMEVENTTYPE				m_AnimEventType;
	ANIMEVENTCATEGORY	m_AnimEventCategory;
};
/* 
=====================================
AnimEvent
=====================================
AnimSectionEvent
=====================================
*/
/*
@ Date: 2024-02-08, Writer: 박태현
@ Explain
- 구간에만 이벤트를 발생시키는 클래스
*/
class UAnimSectionEvent abstract : public UAnimEvent {
protected:
	UAnimSectionEvent(ANIMEVENTTYPE _AnimEventType);
	UAnimSectionEvent(const UAnimSectionEvent& _rhs);
	UAnimSectionEvent(const ANIMEVENTSECTIONDESC& _AnimEventDesc, ANIMEVENTTYPE _AnimEventType);
	DESTRUCTOR(UAnimSectionEvent)
public:
	virtual SHPTR<UAnimEvent> Clone(UAnimModel* _pAnimModel = nullptr) PURE;
public:
	virtual _bool EventCheck(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc,
		const _wstring& _wstrInputTrigger) override;
	virtual const ANIMEVENTDESC*  OutAnimEventDesc() override { return &m_AnimSectionDesc; }
protected: /* get set*/
	const ANIMEVENTSECTIONDESC& GetAnimSectionDesc() const { return m_AnimSectionDesc; }
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc) PURE;
	virtual void SaveEvent( std::ofstream& _save) PURE;
	virtual void LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) PURE;
protected:
	void ResetEventData();
private:
	virtual void Free() PURE;
private:
	ANIMEVENTSECTIONDESC			m_AnimSectionDesc;
};
/*
=====================================
AnimSectionEvent
=====================================
AnimOccurEvent
=====================================
*/
/*
@ Date: 2024-02-08, Writer: 박태현
@ Explain
- 구간을 지나면 이벤트를 발생시키는 클래스
*/
class UAnimOccurEvent abstract : public UAnimEvent {
protected:
	UAnimOccurEvent(ANIMEVENTTYPE _AnimEventType);
	UAnimOccurEvent(const UAnimOccurEvent& _rhs);
	UAnimOccurEvent(const ANIMOCURRESDESC& _AnimEventDesc,  ANIMEVENTTYPE _AnimEvent);
	DESTRUCTOR(UAnimOccurEvent)
public:
	virtual SHPTR<UAnimEvent> Clone(UAnimModel* _pAnimModel = nullptr) PURE;
	virtual _bool EventCheck(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc,
		const _wstring& _wstrInputTrigger) override;
	virtual const ANIMEVENTDESC*  OutAnimEventDesc() override { return &m_AnimOccurDesc; }
protected: /* get set*/
	const ANIMOCURRESDESC& GetAnimOccursDesc() const { return m_AnimOccurDesc; }
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _dTimeDelta, const _double& _dTimeAcc) PURE;
	virtual void SaveEvent(std::ofstream& _save) PURE;
	virtual void LoadEvent(CSHPTRREF<UAnimModel> _spAnimModel, std::ifstream& _load) PURE;
private:
	virtual void Free() PURE;
private:
	ANIMOCURRESDESC		m_AnimOccurDesc;
};

END