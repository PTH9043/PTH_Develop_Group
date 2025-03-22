#pragma once
#include "UBase.h"

BEGIN(Engine)

class UResourceLoader : public UBase {
public:
	UResourceLoader(const UDelegate<void, _bool&>& _cfunction);
	NO_COPY(UResourceLoader)
		DESTRUCTOR(UResourceLoader)
public:
	const _bool IsFunctional() const { return m_isFunctional; }
public:
	virtual void Free() override;
public:
	static void Run(CSHPTRREF<UResourceLoader> _spResourceLoader);
private:
	// Running
	void Running();
private:
	// Delegate
	UDelegate<void, _bool&>		m_cFunction;
	std::atomic<_bool>					m_isFunctional;
};

END
