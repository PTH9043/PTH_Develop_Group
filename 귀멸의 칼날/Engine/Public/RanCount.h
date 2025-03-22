#pragma once
#include "Base.h"

BEGIN(Engine)

class CRanCount : public CBase
{
	DECLARE_SINGLETON(CRanCount)
private:
	explicit CRanCount();
	virtual ~CRanCount() = default;

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