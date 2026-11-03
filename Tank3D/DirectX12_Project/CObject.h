#pragma once
#include "CBase.h"

class CObject abstract : public CBase {
protected:
	explicit CObject() = default;
	explicit CObject(const CObject& _rhs) :CBase(_rhs) {}
	virtual ~CObject() = default;
public:
	virtual void Free() = 0;
protected:
	virtual HRESULT NativeConstruct() = 0;
private:
};

