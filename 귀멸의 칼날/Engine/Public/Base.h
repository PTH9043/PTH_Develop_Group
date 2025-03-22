#pragma once
#include "SuperBase.h"

BEGIN(Engine)
class CRefDebugActor;

/* 슈퍼 클래스 .*/
/* 레퍼런스 카운트를 관리한다*/
class ENGINE_DLL  CBase abstract  : public CSuperBase
{
protected:	
	CBase();
	virtual ~CBase();
#ifdef _DEBUG
public:
	virtual unsigned long AddRef(const _char* FILE,
		const _uint& LINE, const _char* FUNC) override;
	virtual unsigned long Release(const _char* FILE,
		const _uint& LINE, const _char* FUNC) override;
private:
	EXPLAINS Make_Explain(const _string& _strName );
	CRefDebugActor*						m_pRefActor = nullptr;
	DEBUGREMAINDESC*				m_pRemainDesc = nullptr;
#endif
public:
	virtual void Free();

};

END



