#pragma once
#include "DebugActor.h"

BEGIN(Engine)

class CBase;

class ENGINE_DLL CRefDebugActor final : public CDebugActor
{
public:
		typedef list<DEBUGREMAINDESC> REF_LIST;

public:
	enum CHECK_ADD_EXPLAIN
	{
		EX_CREATE, EX_ADD, EX_RELEASE, EX_FREE, EX_END
	};
private:
	explicit CRefDebugActor();
	virtual ~CRefDebugActor() = default;
public:
	// √ ±‚»≠
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT Clear() override;
#ifdef _DEBUG
	// Create Add To Ref
	DEBUGREMAINDESC* Add_RefCount_BaseActor(CBase* _pInstance, _ulong* _pRefCount);
	void Add_DebugExplain(const _char* _pName, EXPLAINS _Explain, const CHECK_ADD_EXPLAIN& _eCheck, DEBUGREMAINDESC* _pDesc);
	void Clear_Explain();
protected:
	void Save_Text(const _string& _strName, const _string Log);
protected:
	_string Combine_Sentence(EXPLAINS _Explain);
private:
	_string						m_strExplains = "";
	REF_LIST					m_RefList;
	_ulong						m_iCount = 0;
	ofstream					m_Out;
#endif
public:
	static CRefDebugActor* Create(); 
	virtual void Free() override;
};

END