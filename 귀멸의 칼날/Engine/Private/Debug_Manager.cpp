#include "Debug_Manager.h"
#include "RefDebugActor.h"
#include "ConsolDebugActor.h"

IMPLEMENT_SINGLETON(CDebug_Manager)

CDebug_Manager::CDebug_Manager()
{

}

HRESULT CDebug_Manager::NativeConstruct()
{
	{
		CDebugActor* pInstance = CRefDebugActor::Create();
		if (nullptr == pInstance)
			return E_FAIL;

		m_DebugController.emplace(pair<DEBUG_TAG, CDebugActor*>(DEBUG_REFACTOR , pInstance));
	}

	{
		CDebugActor* pInstance = CConsolDebugActor::Create();
		if (nullptr == pInstance)
			return E_FAIL;

		m_DebugController.emplace(pair<DEBUG_TAG, CDebugActor*>(DEBUG_CONSOLACTOR, pInstance));
	}

	return S_OK;
}

HRESULT CDebug_Manager::Clear()
{
	for (auto& iter : m_DebugController)
		if (FAILED(iter.second->Clear()))
			return E_FAIL;

	return S_OK;
}

void CDebug_Manager::Add_DebugActor(const DEBUG_TAG& _eDebugTag, CDebugActor* _pDebugActor)
{
	if (nullptr == _pDebugActor)
		return;

	m_DebugController.emplace(pair<DEBUG_TAG, CDebugActor*>(_eDebugTag, _pDebugActor));
}

CDebugActor* CDebug_Manager::Get_DebugActor(const DEBUG_TAG& _eDebugTag)
{
	const auto& iter = find_if(m_DebugController.begin(), m_DebugController.end(), [&](auto& Pair)->bool {

		if (_eDebugTag == Pair.first)
			return true;

		return false;
	});

	if (iter == m_DebugController.end())
		return nullptr;

	return m_DebugController[_eDebugTag];
}


void CDebug_Manager::Free()
{
	for (auto& iter : m_DebugController)
		Safe_Release(iter.second);
}
