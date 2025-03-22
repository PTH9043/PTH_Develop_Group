#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CPLAYERSESSION_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CPLAYERSESSION_H

#include "ASession.h"

BEGIN(Core)
class AService;
class ACoreInstance;
END

BEGIN(Server)
/*
@ Date: 2023-01-02, Writer: 박태현
@ Explain
- Client에 존재하는 플레이어들과 통신하기 위한 클래스
*/
class  CPlayerSession final : public Core::ASession {
public:
	CPlayerSession(SESSION_CONSTRUCTOR);
	NO_COPY(CPlayerSession)
	DESTRUCTOR(CPlayerSession)
public:
	virtual _bool Start(const VOIDDATAS& _ReceiveDatas = {}) override;
	// 클라이언트에서 전송된 버퍼를 읽는 함수
	virtual void RecvData() override;
	// 클라이언트에게 버퍼를 조합하여 전송하는 함수
	virtual _bool SendData(_char* _pPacket, const Core::PACKETHEAD& _PacketHead) override;
	virtual void Disconnect() override;
	virtual void ConnectTcpSocket() override;
	virtual bool IsHit(APawn* _pPawn, const _double& _dTimeDelta) override;
protected:
	// Damaged
	virtual void Collision(APawn* _pPawn, const _double& _dTimeDelta) override;
	virtual _bool ProcessPacket(_char* _pPacket, const Core::PACKETHEAD& _PacketHead) override;
private: 
	void LoginState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead);
	void PlayerState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead);
	void PlayerCollisionState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead);
	void MonsterCollisionState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead);
	void MonsterState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead);
private:
	virtual void Free() override;
private:
	_int						m_iStartCellIndex;
};


END
#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CPLAYERSESSION_H