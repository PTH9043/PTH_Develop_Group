#pragma once
#include "DebugActor.h"

BEGIN(Engine)

class ENGINE_DLL CConsolDebugActor : public CDebugActor
{

private:
	explicit CConsolDebugActor();
	virtual ~CConsolDebugActor() = default;
public:
	// √ ±‚»≠
	virtual HRESULT NativeConstruct() override;

public:
	static CConsolDebugActor* Create();
	virtual void Free() override;
};

END