#pragma once
#include "UBase.h"
#include "UResourceLoader.h"
#include <thread>

BEGIN(Engine)

class ULoader : public UBase {
public:
	ULoader();
	NO_COPY(ULoader)
		DESTRUCTOR(ULoader)
public:
	const VECTOR<SHPTR<UResourceLoader>>& GetResourceLoaders() const { return m_vecResourcLoaders; }
public:
	// Free
	virtual void Free() override;
	HRESULT NativeConstruct(const VECTOR< UDelegate<void, _bool&>>& _vecFunctions, const _uint& _iMethodCount);
	_bool IsLoadEnd();
private:
	VECTOR<std::thread>									m_vecThreads;
	VECTOR<SHPTR<UResourceLoader>>		m_vecResourcLoaders;
};

END



