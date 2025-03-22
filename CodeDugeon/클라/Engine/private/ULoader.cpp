#include "EngineDefine.h"
#include "ULoader.h"

ULoader::ULoader()
{
}

void ULoader::Free()
{
	m_vecThreads.clear();
}

HRESULT ULoader::NativeConstruct(const VECTOR<UDelegate<void, _bool&>>& _vecFunctions, const _uint& _iMethodCount)
{
	m_vecThreads.reserve(_iMethodCount);
	m_vecResourcLoaders.reserve(_iMethodCount);
	for (_uint i = 0; i < _iMethodCount; ++i)
	{
		SHPTR<UResourceLoader> spResourceLoader{ Create<UResourceLoader>(_vecFunctions[i]) };
		m_vecThreads.emplace_back(UResourceLoader::Run, spResourceLoader);
	}

	for (auto& iter : m_vecThreads)
		iter.detach();
	return S_OK;
}

_bool ULoader::IsLoadEnd()
{
	_bool isLoadEnd{ true };

	for (auto& iter : m_vecResourcLoaders)
	{
		if (false == iter->IsFunctional())
		{
			isLoadEnd = false;
			break;
		}
	}

	return isLoadEnd;
}
