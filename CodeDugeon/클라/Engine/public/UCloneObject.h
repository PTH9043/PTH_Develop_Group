#pragma once

#include "UObject.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;

/*
Actor, Resource, Component 등의 부모 클래스
복제가 필요한 클래스는 반드시 이 클래스를 상속 받아야 한다.
*/

class UCloneObject  abstract : public UObject {
public:
	UCloneObject(CSHPTRREF<UDevice> _spDevice);
	UCloneObject(const UCloneObject& _rhs);
	virtual ~UCloneObject() = default;
public:
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _stDatas) PURE;
	virtual void Free() override PURE;
	virtual HRESULT NativeConstruct() override PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _stDatas) PURE;
protected:
	const _bool IsClone() const { return m_isClone; }
private:
	_bool											m_isClone;
};


END
