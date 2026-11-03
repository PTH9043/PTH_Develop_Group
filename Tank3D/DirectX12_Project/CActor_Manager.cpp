#include "CActor_Manager.h"
#include "CActor.h"
#include "CActorGroup.h"

void CActor_Manager::Free()
{
	for (auto& iter : m_Clones)
		Safe_Free(iter.second);

	m_Clones.clear();

	for (auto& iter : m_Prototypes)
		Safe_Free(iter.second);

	m_Prototypes.clear();
}

shared_ptr<CActor> CActor_Manager::Find_Actor(const _wstring& _strPrototypeNames)
{
	const auto& iter = m_Prototypes.find(_strPrototypeNames);
	if (iter == m_Prototypes.end())
		return nullptr;

	return (*iter).second;
}

void CActor_Manager::Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_Clones)
		iter.second->Tick(_dTimeDelta);

	for (auto& iter : m_Clones)
		iter.second->Tick(_dTimeDelta);
}

void CActor_Manager::Late_Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_Clones)
		iter.second->Late_Tick(_dTimeDelta);
}

void CActor_Manager::Add_Prototypes(const _wstring& _strName, const shared_ptr<CActor>& _pActor)
{
	if (nullptr != Find_Actor(_strName))
		return;

	m_Prototypes.emplace(pair<_wstring, shared_ptr<CActor>>(_strName, _pActor));
}

void CActor_Manager::Clone_Actor(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas )
{
	Clone_Actor_Add(_strProtoNames, _strCloneLayer, _tDatas);
}

shared_ptr<CActor> CActor_Manager::Clone_Actor_Add(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas)
{
	shared_ptr<CActor> pInstnace = Find_Actor(_strProtoNames);
	if (nullptr == pInstnace)
		return nullptr;

	shared_ptr<CActor> pClones = pInstnace->Clone(_tDatas);
	if (nullptr == pClones)
		return nullptr;


	const auto& ActorList = m_Clones.find(_strCloneLayer);
	if (ActorList == m_Clones.end())
	{
		shared_ptr<CActorGroup> pGroup = CActorGroup::Create();
		pGroup->Add_ActiveActor(pClones);
		m_Clones.emplace(pair<_wstring, shared_ptr<CActorGroup>>(_strCloneLayer, pGroup));
	}
	else
	{
		ActorList->second->Add_ActiveActor(pClones);
	}

	return pClones;
}

void CActor_Manager::Add_DeleteActor(const _wstring& _strLayer, const shared_ptr<CActor>& _pActor)
{
	CLONES::iterator FindIter = m_Clones.find(_strLayer);
	if (m_Clones.end() == FindIter)
		return;

	(*FindIter).second->Add_DeleteActor(_pActor);
}

list<shared_ptr<CActor>> CActor_Manager::Get_ActorList(const _wstring& _strCloneLayer)
{
	const auto& ActorList = m_Clones.find(_strCloneLayer);
	if (m_Clones.end() == ActorList)
		return list<shared_ptr<CActor>>();

	return ActorList->second->Get_ActiveActorList();
}

void CActor_Manager::Clear(){
	m_Clones.clear();
}
