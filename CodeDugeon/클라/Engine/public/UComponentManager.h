#pragma once
#include "UBase.h"


BEGIN(Engine)
class UComponent;

class UComponentManager : public UBase {
public:
	UComponentManager();
	NO_COPY(UComponentManager)
		DESTRUCTOR(UComponentManager)
public:
	virtual void Free() override;

	HRESULT AddPrototype(const _wstring& _wstrPrototypes, const SHPTR<UComponent>& _spComponent);
	SHPTR<UComponent> CloneComp(const _wstring& _wstrPrototypes, const VOIDDATAS& _stDatas = VOIDDATAS{});
private:
	SHPTR<UComponent> FindComp(const _wstring& _wstrPrototypes);
private:
	using PROTOTYPES = UNORMAP<_wstring, SHPTR<UComponent>>;
	PROTOTYPES m_Prototypes{};
};

END
