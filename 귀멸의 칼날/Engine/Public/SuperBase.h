#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

/* 레퍼런스 카운트를 관리한다*/
class ENGINE_DLL CSuperBase abstract
{
protected:
	CSuperBase();
	virtual ~CSuperBase();
public:
#ifdef _DEBUG
	virtual unsigned long AddRef(const _char* FILE, const _uint& LINE, const _char* TIME); /* 참조 개수를 증가시킨다, 증가시킨 레퍼런스카운트를 리턴한다. */
	virtual unsigned long Release(const _char* FILE, const _uint& LINE, const _char* TIME); /* 감소시킨다. or 삭제한다. 감소시키기 이전의 레퍼런스 카운트를 리턴한다. */
#else
	unsigned long AddRef();
	unsigned long Release();
#endif

protected:
	/* 참조 개수를 세어서 보관한다. */
	unsigned long								m_dwRefCnt = 0;
public:
	virtual void Free();
};

END
