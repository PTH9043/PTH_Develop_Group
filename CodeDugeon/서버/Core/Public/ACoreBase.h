#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ACOREBASE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ACOREBASE_H

BEGIN(Core)
/*
@ Date: 2023-12-26
@ Writer: 박태현
@ Explain
- 최상위 부모 클래스, 서버에서 객체의 역활을 하는 모든 클래스는 모두 이 클래스를 무조건 상속 받는다.
*/
class  CORE_DLL  ACoreBase abstract : public Core::AEnableSharedFromThis<ACoreBase> {
protected:
	ACoreBase();
	ACoreBase(const ACoreBase& _rhs);
	virtual ~ACoreBase();

public:
	/*
	@ Explain
	- 자기 자신의 WEAKPTR을 캐스팅 하기 위한 함수들이다. 
	*/
	template<class T>
	SHPTR<T> ThisShared() { return Core::static_shared_cast<T>(shared_from_this()); }
	template<class T>
	WKPTR<T> ThisWeak() { return Core::static_shared_cast<T>(weak_from_this()); }

	/*
	@ Explain
	- 메모리를 해제해야 할 것이 있을 때 FREE에 정의하여 해제한다. 
	*/
protected:
	virtual void Free() PURE;

protected:
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ACOREBASE_H