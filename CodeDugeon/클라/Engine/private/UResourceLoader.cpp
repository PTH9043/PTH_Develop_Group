#include "EngineDefine.h"
#include <atomic>
#include "UResourceLoader.h"

UResourceLoader::UResourceLoader(const UDelegate<void, _bool&>& _cfunction)
	: m_isFunctional(false)
{
}

void UResourceLoader::Free()
{
}

void UResourceLoader::Run(CSHPTRREF<UResourceLoader> _spResourceLoader)
{
	if (nullptr != _spResourceLoader)
		_spResourceLoader->Running();
}

void UResourceLoader::Running()
{
	_bool isLoading{ false };
	m_cFunction.Invoke(isLoading);
	m_isFunctional.store(isLoading);
}
