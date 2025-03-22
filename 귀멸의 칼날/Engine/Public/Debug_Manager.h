#pragma once
#include "Base.h"
#include "RefDebugActor.h"
#include "ConsolDebugActor.h"

BEGIN(Engine)
class CDebugActor;

class ENGINE_DLL CDebug_Manager final : public CDebugActor
{
		DECLARE_SINGLETON(CDebug_Manager);
public:
	enum DEBUG_TAG
	{
		DEBUG_REFACTOR, DEBUG_CONSOLACTOR, DEBUG_END
	};
private:
	explicit CDebug_Manager();
	virtual ~CDebug_Manager() = default;
public:
	// Create Debug 
	HRESULT NativeConstruct();
	// Release();
	HRESULT Clear();
	// Add Debug Actor
	void Add_DebugActor(const DEBUG_TAG& _eDebugTag, CDebugActor* _pDebugActor);
	// Get DebugActor
	CDebugActor* Get_DebugActor(const DEBUG_TAG& _eDebugTag);
public:
	// Debug Controller;
	map<DEBUG_TAG, CDebugActor*> m_DebugController;
public:
	// Free
	virtual void Free() override;
};

END