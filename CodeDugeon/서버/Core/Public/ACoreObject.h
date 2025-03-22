#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ACOREOBJECT_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ACOREOBJECT_H

#include "ACoreBase.h"

BEGIN(Core)
class ACoreInstance;

/*
@ Date: 2023-12-26
@ Writer: 박태현
@ Explain
- ACoreInstance 객체가 필요한 모든 클래스
*/
class CORE_DLL  ACoreObject abstract : public ACoreBase {
public:
	ACoreObject(SHPTR<ACoreInstance> _spCoreInstance);
	ACoreObject(const ACoreObject& _rhs);
	DESTRUCTOR(ACoreObject)

protected:
	SHPTR<ACoreInstance> GetCoreInstance() const { return m_wpCoreInstance.lock(); }

private:
	virtual void Free() override;

private:
	WKPTR<ACoreInstance>		m_wpCoreInstance;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ACOREOBJECT_H