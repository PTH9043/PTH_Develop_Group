#include "SuperBase.h"

CSuperBase::CSuperBase()
{
}

CSuperBase::~CSuperBase()
{
}

#ifdef _DEBUG
unsigned long CSuperBase::AddRef(const _char* FILE, const _uint& LINE, const _char* TIME)
{
	return ++m_dwRefCnt;
}

unsigned long CSuperBase::Release(const _char* FILE, const _uint& LINE, const _char* TIME)
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;
}
#else
unsigned long CSuperBase::AddRef()
{
	return ++m_dwRefCnt;
}
	
unsigned long CSuperBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;
		
		return 0;
	}
	else
		return m_dwRefCnt--;
}
#endif

void CSuperBase::Free()
{
}
