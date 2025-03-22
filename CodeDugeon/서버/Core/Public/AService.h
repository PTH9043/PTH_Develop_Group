#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ASERVICE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ASERVICE_H

#include "ACoreObject.h"

BEGIN(Core)
class AMonster;
class ASession;
/*
@ Date: 2024-01-06, Writer: 박태현
@ Explain
- PlayerSession 생성, 괸리 및 AWS 연결 관리
*/
class CORE_DLL AService abstract : public ACoreObject {
public:
	using SESSIONCONTAINER = CONUNORMAP<SESSIONID, SHPTR<ASession>>;
	using MOBOBJCONTAINER = CONUNORMAP<SESSIONID, SHPTR<AMonster>>;
public:
	AService(OBJCON_CONSTRUCTOR, SERVICETYPE _Type);
	DESTRUCTOR(AService)
public:
	virtual _bool NativeConstruct() PURE;
	// 해당 함수가 
	virtual _bool Start() PURE;
public:
	// ID를 통해서 SessionID
	SHPTR<ASession> FindSession(const SESSIONID _SessionID) ;
	// ID를 통해서 GameObject를 찾아온다. 
	SHPTR<AMonster> FindMobObject(const SESSIONID _SessionID);
	// 전체 서버 참여자에게 메시지를 보내는 함수이다. 
	void BroadCastMessage(_char* _pPacket, const PACKETHEAD& _PacketHead) ;
	// 해당 Session ID를 제외한 전체 서버 참여자에게 메시지를 보내는 함수
	void BroadCastMessageExcludingSession(const SESSIONID _SessionID, _char* _pPacket, const PACKETHEAD& _PacketHead) ;
	// Direct로 메시지 보내는 함수
	void DirectSendMessage(const SESSIONID _SessionID, _char* _pPacket, const PACKETHEAD& _PacketHead) ;
	// Session의 TCP Socket 연결만 끊고 Insert나 기타 다른 함수들을 정의해서 Socket들 제거
	void LeaveService(const SESSIONID _SessionID) ;
	// Session을 Container에 저장하는 함수이다. 
	void InsertSession(SESSIONID _SessionID, SHPTR<ASession> _spSession) ;
	// GameObject를 집어넣는 함수
	void InsertMobObject(SESSIONID _SessionID, SHPTR<AMonster> _spMobObject);
public: /* get set */
	const SESSIONCONTAINER& GetSessionContainer() const { return m_SessionContainer; }
	const MOBOBJCONTAINER& GetMobObjContainer() const { return m_MobObjContainer; }
	IOContext& GetIOContext(REF_RETURN) { return m_IOContext; }

	void IncreaseCurrentSessionCount(_llong _Count);
protected:
	virtual void Connect() PURE;
	virtual SESSIONID GiveID();
protected: /* Get Set */
	TCPSOCKET& GetTcpSocket(REF_RETURN) { return m_TcpSocket; }
	MUTEX& GetLock(REF_RETURN) { return m_Lock; }
	IOContext* GetIOConectPointer() { return &m_IOContext; }
private:
	virtual void Free() override;
private:
	SERVICETYPE						m_ServiceType;
	ATOMIC<SESSIONID>			m_IDIssuance;
	ATOMIC<_llong>					m_CurrentSessionCount;
	// Boost/Asio Context
	IOContext								m_IOContext;
	TCPSOCKET							m_TcpSocket;
	// Lock
	MUTEX									m_Lock;
	// Session Conatiner
	SESSIONCONTAINER			m_SessionContainer;
	MOBOBJCONTAINER			m_MobObjContainer;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ASERVICE_H