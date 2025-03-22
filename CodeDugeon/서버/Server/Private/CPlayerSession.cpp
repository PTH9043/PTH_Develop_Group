#include "ServerDefines.h"
#include "CPlayerSession.h"
#include "ACoreInstance.h"
#include "AServerService.h"
#include "ATransform.h"
#include "ACell.h"
#include "ANavigation.h"
#include "AMonster.h"
#include "AAnimController.h"

namespace Server {

	CPlayerSession::CPlayerSession(SESSION_CONSTRUCTOR)
		: Core::ASession(SESSION_CONDATA(Core::SESSIONTYPE::PLAYER)), m_iStartCellIndex{ 349 }
	{
	}

	_bool CPlayerSession::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start(_ReceiveDatas);

		SC_CONNECTSUCCESS scConnectSuccess;
		{
			PROTOFUNC::MakeScConnectSuccess(OUT & scConnectSuccess, GetSessionID(), m_iStartCellIndex, TAG_MAINPLAYER);
		}
		CombineProto(REF_OUT GetCopyBuffer(), REF_OUT GetPacketHead(), scConnectSuccess, TAG_SC::TAG_SC_CONNECTSUCCESS);

		SendData(GetCopyBufferPointer(), GetPacketHead());
		SetGameObjectType(TAG_CHAR::TAG_MAINPLAYER);
		SetMoveSpeed(10.f);
		SetRunSpeed(30.f);
		SetCurOnCellIndex(m_iStartCellIndex);
		SetCharStatus(CHARSTATUS{ 100, 0, 2500 });

		SHPTR<ANavigation> spNavigation = GetNavigation();

		SHPTR<ACell> spCell = spNavigation->FindCell(m_iStartCellIndex);
		GetTransform()->SetPos(spCell->GetCenterPos());
		return true;
	}

	void CPlayerSession::RecvData()
	{
		__super::RecvData();
	}

	_bool CPlayerSession::SendData(_char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		RETURN_CHECK(true == IsPermanentDisable() , false);

		CombineSendBuffer( _pPacket, _PacketHead);
		_bool result{ true };

		TCPSOCKET& Socket = GetTcpSocket(REF_RETURN);

		Socket.async_write_some(Asio::buffer(GetSendBuff(), _PacketHead.PacketSize + PACKETHEAD_SIZE),
			[this](const boost::system::error_code& _error, std::size_t _Size) {
			SESSIONID SessionID = GetSessionID();
			Core::SHPTR<Core::ACoreInstance> spCoreInstance{ GetCoreInstance() };
				// Packet
				if (_error)
				{
					if (nullptr != spCoreInstance)
						spCoreInstance->LeaveService(SessionID);
				}
			});
		return result;
	}

	void CPlayerSession::Disconnect()
	{
		__super::Disconnect();
	}

	void CPlayerSession::ConnectTcpSocket(){}

	bool CPlayerSession::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void CPlayerSession::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	_bool CPlayerSession::ProcessPacket(_char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		_int SessionID = GetSessionID();
		::memset(GetCopyBufferPointer(), 0, MAX_BUFFER_LENGTH);

		switch (_PacketHead.PacketType)
		{
			case TAG_CS::TAG_CS_LOGIN:
			{
				LoginState(spCoreInstance, SessionID, _pPacket, _PacketHead);
			}
			break;
			case TAG_CS::TAG_CS_PLAYERSTATE:
			{
				PlayerState(spCoreInstance, SessionID, _pPacket, _PacketHead);
			}
			break;
			case TAG_CS::TAG_CS_DAMAGED:
			{
				PlayerCollisionState(spCoreInstance, SessionID, _pPacket, _PacketHead);
			}
			break;
			case TAG_CS::TAG_CS_MONSTERCOLIISION:
			{
				MonsterCollisionState(spCoreInstance, SessionID, _pPacket, _PacketHead);
			}
			break;
			case TAG_CS::TAG_CS_MONSTERSTATE:
			{
				MonsterState(spCoreInstance, SessionID, _pPacket, _PacketHead);
			}
			break;
		}
		return true;
	}

	void CPlayerSession::LoginState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		// 현재 받아온 패킷을 재해석 한다. 
		CS_LOGIN Login;
		Login.ParseFromArray(_pPacket, _PacketHead.PacketSize);
#ifdef USE_DEBUG
		std::cout << "Session ID Login Success Login [" << Login.id() << "]\nCurSession [" << _SessionID << "]\n";
#endif
		// 로그인 된 플레이어의 패킷을 보낸다. 
		{
			// Make Sc Login 
			SC_OTHERCLIENTLOGIN scOtherLogin;
			// 다른 클라이언트들에게 해당 플레이어가 접속했음을 알림
			PROTOFUNC::MakeScOtherClientLogin(OUT & scOtherLogin, _SessionID, m_iStartCellIndex, TAG_CHAR::TAG_OTHERPLAYER);
			CombineProto(REF_OUT GetCopyBuffer(), REF_OUT GetPacketHead(), scOtherLogin, TAG_SC::TAG_SC_OTHERCLIENTLOGIN);
			_spCoreInstance->BroadCastMessageExcludingSession(_SessionID, GetCopyBufferPointer(), GetPacketHead());

			for (auto& iter : _spCoreInstance->GetSessionContainer())
			{
				if (iter.first == _SessionID)
					continue;

				if (true == iter.second->IsPermanentDisable())
					continue;

				PROTOFUNC::MakeScOtherClientLogin(OUT & scOtherLogin, iter.first, m_iStartCellIndex, TAG_CHAR::TAG_OTHERPLAYER);
				CombineProto(REF_OUT GetCopyBuffer(), REF_OUT GetPacketHead(), scOtherLogin, TAG_SC::TAG_SC_OTHERCLIENTLOGIN);
				SendData(GetCopyBufferPointer(), GetPacketHead());
			}
			const MOBOBJCONTAINER& MobObjectContainer = _spCoreInstance->GetMobObjContainer();
			for (auto& iter : MobObjectContainer)
			{
				if (true == iter.second->IsPermanentDisable())
				{
					iter.second->RunningPermanentDisableSituation();
				}
			}
		}
	}

	void CPlayerSession::PlayerState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		SHPTR<ANavigation> spNavigation = GetNavigation();

		Vector3				vPosition;

		CHARSTATE csMove;
		csMove.ParseFromArray(_pPacket, _PacketHead.PacketSize);
		{
			vPosition = Vector3(csMove.posx(), csMove.posy(), csMove.posz());
			GetTransform()->SetPos(vPosition);
		}
		//  다른 모든 클라이언트에 메시지 보낸다. 
		{
		//	PROTOFUNC::MakeCharState(OUT & csMove, _SessionID, vSendPosition, vSendRotate);
			CombineProto(REF_OUT GetCopyBuffer(), REF_OUT GetPacketHead(), csMove, TAG_SC::TAG_SC_PLAYERSTATE);
	//		_spCoreInstance->BroadCastMessage(GetCopyBufferPointer(), GetPacketHead());
			_spCoreInstance->BroadCastMessageExcludingSession(_SessionID, GetCopyBufferPointer(), GetPacketHead());
		}

		{
			const MOBOBJCONTAINER& MobObjectContainer = _spCoreInstance->GetMobObjContainer();
			for (auto& iter : MobObjectContainer)
			{
				// 영구적 비활성화
				if (true == iter.second->IsPermanentDisable())
					continue;

				// 거리를 받아옴
				_float fDistanace = iter.second->GetTransform()->ComputeDistanceSq(vPosition);
				if (true == iter.second->IsCanSee(iter.second->GetTransform()))
				{
					iter.second->InsertMobJobTimer(GetSessionID());
				}
				else
				{
					iter.second->SetActive(false);
				}
			}
		}
	}

	void CPlayerSession::PlayerCollisionState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		CS_DAMAGED CollisionData;
		CollisionData.ParseFromArray(_pPacket, _PacketHead.PacketSize);
		SHPTR<ASession> spSession = _spCoreInstance->FindSession(CollisionData.id());
		spSession->SetDamaged(true);
		spSession->DamageToEnemy(CollisionData.damage());

		if (spSession->IsDead())
		{

		}
	}

	void CPlayerSession::MonsterCollisionState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		COLLISIONDATA CollisionData;
		CollisionData.ParseFromArray(_pPacket, _PacketHead.PacketSize);
		SHPTR<AMonster> spMonster = _spCoreInstance->FindMobObject(CollisionData.id());
		spMonster->SetDamaged(true);
		SHPTR<ASession> spSession = _spCoreInstance->FindSession(CollisionData.enemyid());
		spMonster->DamageToEnemy(spSession->GetCharStatus().fPower);

		if (spMonster->IsDead())
		{
			SC_DEAD scDead;
			scDead.set_id(spSession->GetSessionID());
			CombineProto<SC_DEAD>(GetCopyBuffer(), GetPacketHead(), scDead, TAG_SC_DEAD);
			_spCoreInstance->BroadCastMessage(GetCopyBufferPointer(), GetPacketHead());
		}
	}

	void CPlayerSession::MonsterState(SHPTR<ACoreInstance> _spCoreInstance, SESSIONID _SessionID, _char* _pPacket, const Core::PACKETHEAD& _PacketHead)
	{
		MOBSTATE monsterState;
		monsterState.ParseFromArray(_pPacket, _PacketHead.PacketSize);
		CombineProto<MOBSTATE>(GetCopyBuffer(), GetPacketHead(), monsterState, TAG_SC_MONSTERSTATE);
		_int MonsterID = monsterState.id();
		SHPTR<AMonster> spMonster = _spCoreInstance->FindMobObject(MonsterID);
		if (nullptr != spMonster)
		{
			spMonster->ProcessPacket(TAG_CS_MONSTERSTATE, &monsterState);
		}
		//_spCoreInstance->BroadCastMessage(GetCopyBufferPointer(), GetPacketHead());
		 _spCoreInstance->BroadCastMessageExcludingSession(_SessionID, GetCopyBufferPointer(), GetPacketHead());
	}

	void CPlayerSession::Free()
	{
	}
}


/*
				// Login Packet을 조합하고 메시지를 보낸다.
				{
					const MOBOBJCONTAINER& GameObjectContainer = spCoreInstance->GetMobObjContainer();
					SC_VIEWINRANGE scViewInRange;
					// GameObject
					SET<AMonster*> GameObjectList;
					{
						// 시야처리
						for (auto& iter : GameObjectContainer)
						{
							GameObjectList.insert(iter.second.get());
						}
						SC_VIEWINRANGE scViewRange;
						PROTOALLOC(VECTOR3, position);
						// 해당 녀석들이 있다고 보낸다.
						for (auto& iter : GameObjectList)
						{
							if (Login.id() == iter->GetSessionID())
								continue;

							_int GameObjectID = GetGameObjectType();

							SHPTR<ATransform> spTransform = iter->GetTransform();
							Vector3 vPosition = spTransform->GetPos();
							PROTOFUNC::MakeVector3(OUT  position, vPosition.x, vPosition.y, vPosition.z);
							PROTOFUNC::MakeScViewInRange(OUT &scViewRange, iter->GetSessionID(), IN position, iter->GetCellIndex(), GameObjectID);
							CombineProto(REF_OUT m_CopyBuffer, REF_OUT m_CopyPacketHead, scViewInRange, TAG_SC::TAG_SC_VIEWINRANGE);
							spCoreInstance->DirectSendMessage(iter->GetSessionID(), &m_CopyBuffer[0], m_CopyPacketHead);
						}
					}
				}
*/