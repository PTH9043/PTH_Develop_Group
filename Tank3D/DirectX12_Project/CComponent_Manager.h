#pragma once
#include "CBase.h"

class CComponent;

class CComponent_Manager : public CBase
{
public:
	typedef unordered_map<_wstring, shared_ptr<CComponent>>	PROTOTYPES;
protected:
	explicit CComponent_Manager() = default;
	virtual ~CComponent_Manager() = default;

public:
	virtual void Free() override;
public:

	HRESULT NativeConstruct() { return S_OK; }
private:
	shared_ptr<CComponent> Find_Comp(const _wstring& _strPrototypes);
public:
	void Add_Prototypes(const _wstring& _strPrototypes, const shared_ptr<CComponent>& _pComponent);
	shared_ptr<CComponent> Clone(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas = {});
private:
	PROTOTYPES m_Prototypes{};
};

