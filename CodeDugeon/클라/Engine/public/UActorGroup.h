#pragma once
#include "UBase.h"

BEGIN(Engine)
class UActor;

class UActorGroup : public UBase {
public:
	UActorGroup();
	NO_COPY(UActorGroup)
		DESTRUCTOR(UActorGroup)
public:
	using ACTORLIST = SET<SHPTR<UActor>>;
	const ACTORLIST& GetActiveActorList() const { return m_lsActiveActors; }
public:
	virtual void Free() override;
public:
	void Tick(const _double& _dTimeDelta);
	void LateTick(const _double& _dTimeDelta);
	void NetworkTick(const _double& _dTimeDelta);
	void AddActive(CSHPTRREF<UActor> _pActor);
	void AddDelete(CSHPTRREF<UActor> _pActor);

	void ActorsTickDisable();
	void ActorsTickEnable();

	void ActorsDisable();
	void ActorsEnable();
private:
	ACTORLIST	m_lsActiveActors;
	ACTORLIST	m_lsDeleteActors;
};

END