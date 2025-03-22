#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CHARLEQUIN_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CHARLEQUIN_H

#include "CServerMonster.h"

BEGIN(Server)

class CHarlequin final : public CServerMonster {
public:
	CHarlequin(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer);
	DESTRUCTOR(CHarlequin)
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

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CHARLEQUIN_H