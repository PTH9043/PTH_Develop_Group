#include "CRandomCount.h"


IMPLEMENT_SINGLETON(CRandomCount)

CRandomCount::CRandomCount()
{
	m_RanList.reserve(10000);
	for (_uint i = 0; i < 10000; ++i)
		m_RanList.push_back(i);
}

_uint CRandomCount::Get_RandomCount(const _uint& _iCount)
{
	for (_uint i = 0; i < _iCount; ++i)
	{
		int ran1 = rand() % 10000, ran2 = rand() % 10000;
		Suffile(&m_RanList[ran1], &m_RanList[ran2]);
	}

	_uint ran = m_RanList[0];

	return ran;
}

void CRandomCount::Suffile(_uint* a, _uint* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void CRandomCount::Free()
{
	m_RanList.clear();
}
