#pragma once
#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL CDebugActor abstract : public CSuperBase
{
protected:
	explicit CDebugActor();
	virtual ~CDebugActor() = default;
public:
	// 초기화
	virtual HRESULT NativeConstruct();
	virtual HRESULT Clear();
private:
	/* 참조 개수를 세어서 보관한다. */
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free() override;
};

END