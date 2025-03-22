#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

/* 슈퍼 클래스 .*/
/* 레퍼런스 카운트를 관리한다*/
class ENGINE_DLL CBase abstract
{
protected:	
	CBase();
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* 참조 개수를 증가시킨다, 증가시킨 레퍼런스카운트를 리턴한다. */
	unsigned long Release(); /* 감소시킨다. or 삭제한다. 감소시키기 이전의 레퍼런스 카운트를 리턴한다. */

private:
	/* 참조 개수를 세어서 보관한다. */
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free();

};

END



