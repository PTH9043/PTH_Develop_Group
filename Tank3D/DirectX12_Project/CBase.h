#pragma once
#include "Defines.h"


class CBase abstract : public enable_shared_from_this<CBase>
{
protected:
	explicit CBase() = default;
	explicit CBase(const CBase& _rhs) : enable_shared_from_this<CBase>(_rhs){}
	virtual ~CBase() = default;

public:
	virtual void Free() = 0;

protected:
	template<class T>
	shared_ptr<T> ThisShared() { return static_pointer_cast<T>(shared_from_this()); }
	template<class T>
	shared_ptr<T> ThisWeak() { return  static_pointer_cast<T>(weak_from_this()); }
};

