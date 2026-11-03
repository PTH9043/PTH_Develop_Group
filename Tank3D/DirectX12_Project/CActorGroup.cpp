#include "CActorGroup.h"
#include "CActor.h"

shared_ptr<CActorGroup> CActorGroup::Create()
{
	shared_ptr<CActorGroup> pInstance = CSmartPointerFactory<CActorGroup>::CreateShared();
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Free(pInstance);
		DEBUG_MSG("CActorGroup::Create To Failed");
	}
	return pInstance;
}

void CActorGroup::Free()
{
	for (auto& iter : m_ActiveActorList)
		Safe_Free(iter);

	m_ActiveActorList.clear();
	m_DeleteActorList.clear();
}

HRESULT CActorGroup::NativeConstruct()
{
	return S_OK;
}

void CActorGroup::Tick(const _double& _dTimeDelta)
{
	if (m_DeleteActorList.size() > 0)
	{
		for (auto& iter : m_DeleteActorList)
		{
			for (list<shared_ptr<CActor>>::iterator it = m_ActiveActorList.begin(); it != m_ActiveActorList.end();)
			{
				if ((*it) == iter)
				{
					(*it)->Free();
					it = m_ActiveActorList.erase(it);
				}
				else
					++it;
			}
		}
		m_DeleteActorList.clear();
	}

	for (auto& iter : m_ActiveActorList)
		iter->Tick_Awake(_dTimeDelta);

	for (auto& iter : m_ActiveActorList)
		iter->Tick(_dTimeDelta);
}

void CActorGroup::Late_Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_ActiveActorList)
		iter->Late_Tick(_dTimeDelta);
}

void CActorGroup::Add_ActiveActor(const shared_ptr<CActor>& _pActor)
{
	m_ActiveActorList.push_back(_pActor);
}

void CActorGroup::Add_DeleteActor(const shared_ptr<CActor>& _pActor)
{
	m_DeleteActorList.push_back(_pActor);
}
