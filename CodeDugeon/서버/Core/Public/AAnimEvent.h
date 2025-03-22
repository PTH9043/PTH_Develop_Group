#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AANIMEVENT_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AANIMEVENT_H

#include "ACoreBase.h"

BEGIN(Core)
class AAnimator;
class APawn;
/*
@ Date: 2024-02-10, Writer: 박태현
@ Explain
- Animation Event를 정의할 부모 클래스
*/
class AAnimEvent abstract : public ACoreBase {
public:
	AAnimEvent(ANIMEVENTTYPE _AnimEventType, ANIMEVENTCATEGORY _AnimEventCategory);
	AAnimEvent(const AAnimEvent& _rhs);
	DESTRUCTOR(AAnimEvent)
public:
	virtual SHPTR<AAnimEvent> Clone(AAnimator* _pAnimator = nullptr) PURE;
public:
	virtual _bool EventCheck(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc,	const _string& _strInputTrigger) PURE;
	// AnimEventDesc를 상속받는 구조체를 리턴하는 함수
	virtual const  ANIMEVENTDESC* OutAnimEventDesc() PURE;
	// AnimEvent에 다른 구조체가 필요할 경우 해당 구조체의 상속을 받는 녀석들만 내보낼 수 있도록 한다.
	virtual const ANIMOTHEREVENTDESC* OutOtherEventDesc() PURE;
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc) PURE;
	virtual void LoadEvent(AAnimator* _pAnimator, std::ifstream& _load) PURE;
private:
	virtual void Free() PURE;
private:
	ANIMEVENTTYPE					m_AnimEventType;
	ANIMEVENTCATEGORY		m_AnimEventCategory;
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
class AAnimSectionEvent abstract : public AAnimEvent {
public:
	AAnimSectionEvent(ANIMEVENTTYPE _AnimEventType);
	AAnimSectionEvent(const AAnimSectionEvent& _rhs);
	AAnimSectionEvent(const ANIMEVENTSECTIONDESC& _AnimEventDesc, ANIMEVENTTYPE _AnimEventType);
	DESTRUCTOR(AAnimSectionEvent)
public:
	virtual SHPTR<AAnimEvent> Clone(AAnimator* _pAnimator = nullptr) PURE;
public:
	virtual _bool EventCheck(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc, const _string& _strInputTrigger) PURE;
public: /* get set*/
	virtual const ANIMEVENTDESC* OutAnimEventDesc() override { return &m_AnimSectionDesc; }
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc) PURE;
	virtual void LoadEvent(AAnimator* _pAnimator, std::ifstream& _load) PURE;
	void ResetEventData(AAnimator* _pAnimator);
protected: /* get set*/
	const ANIMEVENTSECTIONDESC& GetAnimSectionDesc() const { return m_AnimSectionDesc; }
private:
	virtual void Free() PURE;
private:
	ANIMEVENTSECTIONDESC			m_AnimSectionDesc;
};

/*
=====================================
AnimSectionEvent
=====================================
AnimChangeBetweenEvent
=====================================
*/

/*
@ Date: 2024-02-17, Writer: 박태현
@ Explain
- 구간 사이에서 특정 이벤트가 발생할 때 다음 애니메이션으로 넘어가게 만드는 이벤트이다.
*/
class AAnimChangeBetweenEvent final : public AAnimSectionEvent {
public:
	AAnimChangeBetweenEvent();
	AAnimChangeBetweenEvent(const AAnimChangeBetweenEvent& _rhs);
	AAnimChangeBetweenEvent(AAnimator* _pAnimator, std::ifstream& _load);
	DESTRUCTOR(AAnimChangeBetweenEvent)
public:
	virtual SHPTR<AAnimEvent> Clone(AAnimator* _pAnimator = nullptr) override;
public:
	virtual _bool EventCheck(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc, const _string& _strInputTrigger) override;
	// UAnimSectionEvent을(를) 통해 상속됨
	virtual const ANIMOTHEREVENTDESC* OutOtherEventDesc() override;
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc) override;
	virtual void LoadEvent(AAnimator* _pAnimator, std::ifstream& _load) override;
private:
	virtual void Free() override;
private:
	// 애니메이션과 애니메이션 사이를 변경하는 이벤트
	ANIMCHANGEDESC		m_AnimChangeDesc;
};

/*
=================================================
AnimChangeBetweenEvent
=================================================
AnimColliderEvent
=================================================
*/

/*
@ Date: 2024-02-22, Writer: 박태현
@ Explain
- 구간 사이에서 특정 이벤트가 발생할 때 다음 애니메이션으로 넘어가게 만드는 이벤트이다.
*/
class AAnimColliderEvent final : public AAnimSectionEvent {
public:
	AAnimColliderEvent();
	AAnimColliderEvent(const AAnimColliderEvent& _rhs);
	AAnimColliderEvent(AAnimator* _pAnimator, std::ifstream& _load);
	DESTRUCTOR(AAnimColliderEvent)
public:
	virtual SHPTR<AAnimEvent> Clone(AAnimator* _pAnimator = nullptr) override;
public:
	virtual _bool EventCheck(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc, const _string& _strInputTrigger) override;
	// UAnimSectionEvent을(를) 통해 상속됨
	virtual const ANIMOTHEREVENTDESC* OutOtherEventDesc() override;
protected:
	// Event 상황일 때를 정의
	virtual void EventSituation(APawn* _pPawn, AAnimator* _pAnimator,
		const _double& _dTimeDelta, const _double& _dTimeAcc) override;
	virtual void LoadEvent(AAnimator* _pAnimator, std::ifstream& _load) override;
private:
	virtual void Free() override;
private:
	ANIMCOLLIDERDESC	m_AnimColliderDesc;
};

END

#endif