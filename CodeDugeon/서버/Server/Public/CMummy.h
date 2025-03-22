#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CMUMMY_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CMUMMY_H

#include "CServerMonster.h"

BEGIN(Server)

class CMummy final : public CServerMonster {
public:
	CMummy(OBJCON_CONSTRUCTOR, SESSIONID _ID, MUMMYTYPE _MummyType, SHPTR<AJobTimer> _spMonsterJobTimer);
	DESTRUCTOR(CMummy)
public:
	virtual _bool Start(const VOIDDATAS& _ReceiveDatas = {}) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void State(SHPTR<ASession> _spSession, _int _MonsterState = 0) override;
	virtual void ProcessPacket(_int _type, void* _pData) override;
	virtual bool IsHit(APawn* _pPawn, const _double& _dTimeDelta) override;
	virtual void CallActiveEnable() override;
	virtual void CallActiveDisable() override;
protected:
	// Damaged
	virtual void Collision(APawn* _pPawn, const _double& _dTimeDelta) override;
	virtual void ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID) override;
private:
	virtual void Free() override;
private:
	MUMMYTYPE					m_eMumyType;
	_bool									m_isAttackMode;
	_bool									m_isLastAttackWasFirst;

	CUSTIMER							m_IdleTimer;
	CUSTIMER							m_IdleRandomValueChooseTimer;
	CUSTIMER							m_AttackTimer;
	_int										m_iRandomValue;
	_bool									m_isSendMonsterFindPacket;
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CMUMMY_H