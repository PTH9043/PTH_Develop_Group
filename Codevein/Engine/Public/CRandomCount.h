#pragma once
#include "CBase.h"

BEGIN(Engine)

class CRandomCount : public CBase
{
	DECLARE_SINGLETON(CRandomCount)
private:
	explicit CRandomCount();
	virtual ~CRandomCount() = default;

public:
	_uint Get_RandomCount(const _uint& _iCount = 10000);

protected:
	void Suffile(_uint* a, _uint* b);
private:
	vector<_uint> m_RanList;

public:
	virtual void Free() override;
};

END