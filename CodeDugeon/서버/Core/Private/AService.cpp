#include "CoreDefines.h"
#include "AService.h"
#include "ASession.h"
#include "ACoreInstance.h"
#include "AMonster.h"

namespace Core {

	AService::AService(OBJCON_CONSTRUCTOR, SERVICETYPE _Type) :
		ACoreObject(OBJCON_CONDATA), m_ServiceType{_Type}, 
		m_IOContext{TLS::MAX_THREAD},
		m_TcpSocket{ m_IOContext },
		m_CurrentSessionCount{0},
		m_IDIssuance{1}
	{
	}

	_bool AService::Start()
	{
		return true;
	}

	SHPTR<ASession> AService::FindSession(const SESSIONID _SessionID)
	{
		const auto& iter = m_SessionContainer.find(_SessionID);
		RETURN_CHECK(m_SessionContainer.end() == iter, nullptr);
		return iter->second;
	}

	SHPTR<AMonster> AService::FindMobObject(const SESSIONID _SessionID)
	{
		const auto& iter = m_MobObjContainer.find(_SessionID);
		RETURN_CHECK(m_MobObjContainer.end() == iter, nullptr);
		return iter->second;
	}

	void AService::BroadCastMessage(_char* _pPacket, const PACKETHEAD& _PacketHead)
	{
		RETURN_CHECK(0 >= m_SessionContainer.size(), ;);
		for (auto& iter : m_SessionContainer)
		{
			if (true == iter.second->IsPermanentDisable())
				continue;

			iter.second->SendData(_pPacket, _PacketHead);
		}
	}

	void AService::BroadCastMessageExcludingSession(const SESSIONID _SessionID, _char* _pPacket, const PACKETHEAD& _PacketHead)
	{
		RETURN_CHECK(0 >= m_SessionContainer.size(), ;);
		for (auto& iter : m_SessionContainer)
		{
			if (iter.first == _SessionID)
				continue;

			if (true == iter.second->IsPermanentDisable())
				continue;


			iter.second->SendData(_pPacket, _PacketHead);
		}
	}

	void AService::DirectSendMessage(const SESSIONID _SessionID, _char* _pPacket, const PACKETHEAD& _PacketHead)
	{
		SHPTR<ASession> spSession = FindSession(_SessionID);
		RETURN_CHECK(nullptr == spSession, ;);
		spSession->SendData(_pPacket, _PacketHead);
	}

	void AService::LeaveService(const SESSIONID _SessionID)
	{
		const auto& iter = m_SessionContainer.find(_SessionID);
		RETURN_CHECK(iter == m_SessionContainer.end(), ;);
		RETURN_CHECK(nullptr == iter->second, ;);
		// Disconnect
		{
			std::atomic_thread_fence(std::memory_order_seq_cst);
			if (iter->second)
			{
				// Session »èÁ¦
				iter->second->Disconnect();
			}
		}
	}

	void AService::InsertSession(SESSIONID _SessionID, SHPTR<ASession> _spSession)
	{
		SHPTR<ASession> spSession = FindSession(_SessionID);
		RETURN_CHECK(nullptr == _spSession && nullptr != spSession, ;);
		m_SessionContainer.insert(MakePair(_SessionID, _spSession));
	}

	void AService::InsertMobObject(SESSIONID _SessionID, SHPTR<AMonster> _spMobObject)
	{
		SHPTR<AMonster> spObject = FindMobObject(_SessionID);
		RETURN_CHECK(nullptr == spObject && nullptr != spObject, ;);
		m_MobObjContainer.insert(MakePair(_SessionID, _spMobObject));
	}

	void AService::IncreaseCurrentSessionCount(_llong _Count)
	{
		this->m_IDIssuance += _Count;
		this->m_CurrentSessionCount += _Count;
	}

	SESSIONID AService::GiveID()
	{
		return m_IDIssuance.fetch_add(1);
	}

	void AService::Free()
	{
		for (auto& iter : m_SessionContainer)
		{
			iter.second->Disconnect();
		}
		m_TcpSocket.close();
		m_IOContext.stop();
	}
}
