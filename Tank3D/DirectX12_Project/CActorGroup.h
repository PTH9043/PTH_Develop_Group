#pragma once
#include "CBase.h"

class CActorGroup : public CBase
{
public:
	typedef list<shared_ptr<class CActor>> ACTORLIST;
protected:
	explicit CActorGroup() = default;
	explicit CActorGroup(const CActorGroup& _rhs) : CBase(_rhs) {}
	virtual ~CActorGroup() = default;

public:
	static shared_ptr<CActorGroup> Create();
	virtual void Free() override;

protected:
	HRESULT NativeConstruct();

public:
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);
	void Add_ActiveActor(const shared_ptr<CActor>& _pActor);
	void Add_DeleteActor(const shared_ptr<CActor>& _pActor);
public:
	const ACTORLIST& Get_ActiveActorList() const { return m_ActiveActorList; }
private:
	ACTORLIST m_DeleteActorList{};
	ACTORLIST m_ActiveActorList{};
};

