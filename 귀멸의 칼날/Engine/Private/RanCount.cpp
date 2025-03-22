#include "RanCount.h"

IMPLEMENT_SINGLETON(CRanCount)

CRanCount::CRanCount()
{
	m_RanList.reserve(10000);
	for (_uint i = 0; i < 10000; ++i)
		m_RanList.push_back(i);
}

_uint CRanCount::Get_RandomCount(const _uint& _iCount)
{
	for (_uint i = 0; i < _iCount; ++i)
	{
		int ran1 = rand() % _iCount, ran2 = rand() % _iCount;
		Suffile(&m_RanList[ran1], &m_RanList[ran2]);
	}
	_int iRan = rand() % +_iCount;
	_uint ran = m_RanList[iRan];

	return ran;
}

void CRanCount::Suffile(_uint* a, _uint* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void CRanCount::Free()
{
	m_RanList.clear();
}
