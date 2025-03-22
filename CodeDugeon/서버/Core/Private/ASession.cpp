#include "CoreDefines.h"
#include "ASession.h"
#include "ATransform.h"
#include "ASpace.h"
#include "ACoreInstance.h"

namespace Core
{
	ASession::ASession(OBJCON_CONSTRUCTOR, TCPSOCKET _TcpSocket, SESSIONID _ID, SESSIONTYPE _SessionType) :
		APawn(OBJCON_CONDATA, _ID, _SessionType),
		m_TcpSocket(std::move(_TcpSocket)), m_CurBuffuerLocation{0}
	{
		MemoryInitialization(m_SendBuffer.data(), MAX_BUFFER_LENGTH);
		MemoryInitialization(m_RecvBuffer.data(), MAX_BUFFER_LENGTH);
		MemoryInitialization(m_TotalBuffer.data(), MAX_PROCESSBUF_LENGTH);
	}

	_bool ASession::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start();
		RecvData();
		return true;
	}

	void ASession::RecvData()
	{
		RETURN_CHECK(true == IsPermanentDisable(), ;);
		m_TcpSocket.async_read_some(Asio::buffer(m_RecvBuffer),
			[this](const boost::system::error_code& _error, std::size_t _Size)
			{	
				if (!_error)
				{
					if (_Size == 0)
					{
						Leave();
						return;
					}
					PacketCombine(&m_RecvBuffer[0], _Size);
					RecvData();
				}
				else
				{
#ifdef USE_DEBUG
					if (_error.value() == boost::asio::error::operation_aborted) return;
					std::cout << "Receive Error on Session[" << GetSessionID() << "] EC[" << _error << "]\n";
#endif
					Leave();
				}
			});
	}

	_bool ASession::SendData(_char* _pPacket, const PACKETHEAD& _PacketHead)
	{
		RETURN_CHECK(true == IsPermanentDisable(), false);
		return true;
	}

	void ASession::Disconnect()
	{
#ifdef USE_DEBUG
		std::cout << "DisConnected" << std::endl;
#endif
		ActivePermanentDisable();
	}

	void ASession::ConnectTcpSocket(){ }

	/*
	@ Data: 2024-01-13, Writer : 박태현
	@ Explain
	- Buffer 클래스를 받아서 Combine 한 후 Packet을 처리하는 함수이다.
	*/
	void ASession::PacketCombine(_char* _pPacket, _llong _Size)
	{
		// 버퍼를 조합한다. 
		{
			::memcpy(&m_TotalBuffer[m_CurBuffuerLocation], _pPacket, _Size);
			m_CurBuffuerLocation +=_Size;
		}
		short moveBuffer{ 0 };
		char* pBufferMove = &m_TotalBuffer[0];
		// 만약 BufferLocation이 존재할 때
		while (m_CurBuffuerLocation > 0)
		{
			// PacketSize를 구한다. 
			PACKETHEAD PacketHead;
			memcpy(&PacketHead, pBufferMove, PACKETHEAD_SIZE);
			short CurrPacket = PacketHead.PacketSize + PACKETHEAD_SIZE;
			// 패킷의 현재 위치가 음수가 되는 경우면 
			if ((m_CurBuffuerLocation - CurrPacket) < 0)     
			{
#ifdef USE_DEBUG
		//		std::cout << m_CurBuffuerLocation << "\n";
#endif
				// 최종적인 버퍼에 PacketSize만큼 이동하여 앞쪽에 존재하는 데이터들을 지운다. 
				memmove(&m_TotalBuffer[0], pBufferMove, MAX_PROCESSBUF_LENGTH - moveBuffer);
				return;
			}
			ProcessPacket(&pBufferMove[PACKETHEAD_SIZE], PacketHead);
			// Buffer의 위치를 옮긴다. 
			m_CurBuffuerLocation -= CurrPacket;
			pBufferMove += CurrPacket;
			moveBuffer += CurrPacket;
		}
		m_CurBuffuerLocation = 0;
		MemoryInitialization(m_TotalBuffer.data(), MAX_PROCESSBUF_LENGTH);
	}

	/*
	@ Data: 2023-12-31,  Writer : 박태현
	@ Explain
	- Buffer를 조합하는 함수
	*/
	void ASession::CombineSendBuffer( _char* _pPacket, const PACKETHEAD& _PacketHead)
	{
		::memset(&GetSendBuff(REF_RETURN)[0], 0, MAX_BUFFER_LENGTH);
		memcpy(&GetSendBuff(REF_RETURN)[0], &_PacketHead, PACKETHEAD_SIZE);
		// [0 ~ 1] : PACKETSIZE		[2 ~ 3] : PACKETTYPE		[4 ~ ] Remain... 
		memcpy(&GetSendBuff(REF_RETURN)[PACKETHEAD_SIZE], _pPacket, _PacketHead.PacketSize);
	}

	void ASession::Leave()
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		spCoreInstance->LeaveService(GetSessionID());
	}

	void ASession::Free()
	{
		m_TcpSocket.close();
	}

}