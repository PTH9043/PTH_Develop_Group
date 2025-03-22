#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ASESSION_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ASESSION_H

#include "APawn.h"


BEGIN(Core)
class AService;
class ATransform;
class ACollider;
class ASpace;

/*
@ Date: 2024-01-22, Writer: 박태현
@ Explain
- 클라이언트와 통신하기 위한 Session 클래스이고 가상함수로 Server 솔루션에서 해당 클래스를 부모로 자식 클래스를 
정의해주어야 한다.
*/
class CORE_DLL ASession abstract : public APawn {
public:
	using BUFFER = ARRAY<_char, MAX_BUFFER_LENGTH>;
	using TOTALBUFFER = ARRAY<_char, MAX_PROCESSBUF_LENGTH>;
public:
	ASession(OBJCON_CONSTRUCTOR, TCPSOCKET _TcpSocket, SESSIONID _ID, SESSIONTYPE _SessionType);
	DESTRUCTOR(ASession)
public: 
	virtual _bool Start(const VOIDDATAS& _ReceiveDatas = {}) PURE;
	// 클라이언트에서 전송된 버퍼를 읽는 함수
	virtual void RecvData() PURE;
	// 클라이언트에게 버퍼를 조합하여 전송하는 함수
	virtual _bool SendData(_char* _pPacket, const PACKETHEAD& _PacketHead) PURE;
	virtual void Disconnect() PURE;
	virtual void ConnectTcpSocket() PURE;
	/*
	@ Date: 2024-01-05, Writer: 박태현
	@ Explain
	- ProtocolBuffer를 TCP에 연결된 상대에게 보내기 위한 템플릿 함수이다. 
	*/
	template<class T>
	requires CheckProtoType<T>
	void SendProtoData(BUFFER& _buffer, const T& _data, short _tag)
	{
		Core::PACKETHEAD PacketHead;
		CombineProto<T>(REF_OUT _buffer, REF_OUT PacketHead, _data, _tag);
		SendData(&_buffer[0], PacketHead);
	}
public: /*Get Set */
	TCPSOCKET& GetTcpSocket(REF_RETURN) { return m_TcpSocket; }
protected:
	virtual _bool ProcessPacket(_char* _pPacket, const PACKETHEAD& _PacketHead) PURE;
	void PacketCombine(_char* _pPacket, _llong _Size);
	void CombineSendBuffer(_char* _pPacket, const PACKETHEAD& _PacketHead);
	void Leave();
protected: /* get set*/
	BUFFER& GetSendBuff(REF_RETURN) { return m_SendBuffer; }
private:
	virtual void Free() override;

private:
	TCPSOCKET					m_TcpSocket;
	_llong								m_CurBuffuerLocation;
	// Buffer 모음
	TOTALBUFFER				m_TotalBuffer;
	BUFFER							m_RecvBuffer;
	// SendBuffer
	BUFFER							m_SendBuffer;
};

END

#endif //  _SERVERFRAMEWORK_CORE_PUBLIC_ASESSION_H
