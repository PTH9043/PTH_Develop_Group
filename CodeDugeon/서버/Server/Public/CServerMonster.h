#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERMONSTER_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERMONSTER_H

#include "AMonster.h"

BEGIN(Server)

class CServerMonster abstract : public AMonster {
public:
	CServerMonster(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer);
	DESTRUCTOR(CServerMonster)
public:
	virtual _bool Start(const VOIDDATAS& _ReceiveDatas = {}) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void State(SHPTR<ASession> _spSession, _int _MonsterState = 0) override;
	virtual void ProcessPacket(_int _type, void* _pData) override;
	virtual bool IsHit(APawn* _pPawn, const _double& _dTimeDelta) override;
protected:
	// Damaged
	virtual void Collision(APawn* _pPawn, const _double& _dTimeDelta) override;
	virtual void ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID) override;
private:
	virtual void Free() override;
};

END

#endif