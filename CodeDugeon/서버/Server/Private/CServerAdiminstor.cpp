#include "ServerDefines.h"
#include "CServerAdiminstor.h"
#include "CPlayerSession.h"
#include "ACoreInstance.h"
#include "CMonsterJobTimer.h"
#include "CMummy.h"
#include "CMobLayoutLoader.h"
#include "CSarcophagus.h"
#include "CMobLayOutSaver.h"
#include "CChest.h"
#include "ATransform.h"
#include "AAnimController.h"
#include "CAnubis.h"
#include "CMimic.h"
#include "CHarlequin.h"
#include "CMinotaur.h"

//#define CREATED_SERVERMOBDATA

namespace Server
{
	CServerAdiminstor::CServerAdiminstor(OBJCON_CONSTRUCTOR, const _string& _strNavigationPath) :
		Core::AServerService(OBJCON_CONDATA, _strNavigationPath)
	{
	}

	bool CServerAdiminstor::NativeConstruct()
	{
		return __super::NativeConstruct();
	}

	bool CServerAdiminstor::Start()
	{
#ifndef CREATED_SERVERMOBDATA
		CreateServerMobResourceData();
#else
		CreateServerMob();
#endif
		ThreadMiliRelax(100);
		Connect();
		return __super::Start();
	}

	void CServerAdiminstor::Connect()
	{
		TCPACCEPTOR& TcpAcceptor = GetTcpAccepctor();
		TCPSOCKET& TcpSocket = GetTcpSocket();
		TcpAcceptor.async_accept(TcpSocket, [&](const boost::system::error_code& _error) {
			if (!_error)
			{
				SESSIONID id = GiveID();
				Core::SHPTR<Core::ACoreInstance> spCoreInstance = GetCoreInstance();
				Core::SHPTR<Core::ASession> spSession = Core::Create<CPlayerSession>(spCoreInstance, std::move(TcpSocket), id);
				// Insert 
				InsertSession(id, spSession);
				spSession->Start();
			}
			Connect();
			});
	}

	void CServerAdiminstor::CreateServerMobResourceData()
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		SHPTR<AJobTimer> spMonsterJobTimer = Create<CMonsterJobTimer>(GetCoreInstance(), GetIOContext());
		spCoreInstance->RegisterJob(TIMERTYPE::TIMER_MOB, spMonsterJobTimer);
		// Standing
		{
			VECTOR< MOBDATA> MobData;
			CMobLayoutLoader Chest("..\\..\\Resource\\MobsLayouts\\Chest.bin");
			CMobLayoutLoader Interior_Hallway_E("..\\..\\Resource\\MobsLayouts\\Interior_Hallway_E.bin");
			CMobLayoutLoader Interior_Room_C("..\\..\\Resource\\MobsLayouts\\Interior_Room_C.bin");
			CMobLayoutLoader Interior_Room_D("..\\..\\Resource\\MobsLayouts\\Interior_Room_D.bin");
			CMobLayoutLoader Interior_Room_E("..\\..\\Resource\\MobsLayouts\\Interior_Room_E.bin");
			CMobLayoutLoader Interior_Room_F("..\\..\\Resource\\MobsLayouts\\Interior_Room_F.bin");
			CMobLayoutLoader Interior_Room_G("..\\..\\Resource\\MobsLayouts\\Interior_Room_G.bin");
			CMobLayoutLoader Mimic("..\\..\\Resource\\MobsLayouts\\Mimics.bin");

			// Mob데이터를 받아온다. 
			for (auto& iter : Chest.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Hallway_E.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Room_C.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Room_D.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Room_E.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Room_F.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Interior_Room_G.GetMobData())
			{
				MobData.push_back(iter);
			}
			for (auto& iter : Mimic.GetMobData())
			{
				MobData.push_back(iter);
			}
			// Mob 데이터를 상자로 만든다. 
			for (auto& iter : MobData)
			{
				if (iter.strAnimModelName == "Chest 1_FBX.bin")
				{
					CreateMonster<CChest>(&iter, spMonsterJobTimer);
				}
				else if (iter.strAnimModelName == "Mummy_DEMO_1_FBX.bin")
				{
					if (iter.strAnimName == "staticLaying")
					{
						CreateMummyAndSarphagousMob(&iter, SARCO_LAYING, spMonsterJobTimer);
					}
					else
					{
						CreateMummyAndSarphagousMob(&iter, SARCO_STANDING, spMonsterJobTimer);
					}
				}
				else if (iter.strAnimModelName == "minotaur_FBX")
				{
					CreateMonster<CMinotaur>(&iter, spMonsterJobTimer);
				}
				else if (iter.strAnimModelName == "Harlequin1_FBX")
				{
					CreateMonster<CHarlequin>(&iter, spMonsterJobTimer);
				}
				else if (iter.strAnimModelName == "Anubis_FBX")
				{
					CreateMonster<CAnubis>(&iter, spMonsterJobTimer);
				}
				else if (iter.strAnimModelName == "Mimic_FBX")
				{
					CreateMonster<CMimic>(&iter, spMonsterJobTimer);
				}
			}
			// Monster 
			{
				const auto& GameObjectList = spCoreInstance->GetMobObjContainer();
				VECTOR<MOBSERVERDATA> MobData;
				MobData.reserve(GameObjectList.size());

				for (auto& iter : GameObjectList)
				{
					MOBSERVERDATA data;
					data.mWorldMatrix = iter.second->GetTransform()->GetWorldMatrix();
					data.iMobID = iter.second->GetSessionID();
					data.iMobType = iter.second->GetMonsterType();
					data.iStartAnimIndex = iter.second->GetAnimController()->GetCurAnimIndex();
					data.iMobParentsID = iter.second->GetOwnerMonsterSessionID();
					MobData.push_back(data);
				}
				CMobLayOutSaver saver("..\\..\\NetworkMobData.bin", MobData);
			}
		}
	}

	void CServerAdiminstor::CreateServerMob()
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		SHPTR<AJobTimer> spMonsterJobTimer = Create<CMonsterJobTimer>(GetCoreInstance(), GetIOContext());
		spCoreInstance->RegisterJob(TIMERTYPE::TIMER_MOB, spMonsterJobTimer);

		CMobServerLayoutLoader MobServerLayoutLoader("..\\..\\Resource\\NetworkMobData.bin");
		ThreadMiliRelax(100);
		for (auto& iter : MobServerLayoutLoader.GetMobData())
		{
			SHPTR<AMonster> spMonster = nullptr;

			switch (iter.iMobType)
			{
			case TAG_CHEST:
				spMonster = Create<CChest>(GetCoreInstance(), iter.iMobID, spMonsterJobTimer);
				break;
			case TAG_MUMMY_LAYING:
				spMonster = Create<CMummy>(GetCoreInstance(), iter.iMobID, MUMMYTYPE::MUMMY_LAYING, spMonsterJobTimer);
				break;
			case TAG_MUMMY_STANDING:
				spMonster = Create<CMummy>(GetCoreInstance(), iter.iMobID, MUMMYTYPE::MUMMY_STANDING, spMonsterJobTimer);
				break;
			case TAG_ANUBIS:
				spMonster = Create<CAnubis>(GetCoreInstance(), iter.iMobID, spMonsterJobTimer);
				break;
			case TAG_HARLEQUINN:
				spMonster = Create<CHarlequin>(GetCoreInstance(), iter.iMobID, spMonsterJobTimer);
				break;
			case TAG_MINOTAUR:
				spMonster = Create<CMinotaur>(GetCoreInstance(), iter.iMobID, spMonsterJobTimer);
				break;
			case TAG_MIMIC:
				spMonster = Create<CMimic>(GetCoreInstance(), iter.iMobID, spMonsterJobTimer);
				break;
			}

			if (nullptr == spMonster)
				continue;

			spCoreInstance->InsertMobObject(spMonster->GetSessionID(), spMonster);
			spMonster->Start({ &iter });
		}
		IncreaseCurrentSessionCount(MobServerLayoutLoader.GetMobData().size());
	}

	void CServerAdiminstor::CreateMummyAndSarphagousMob(void* _pData, SARCOPHAGUSTYPE _SarcophagusType, SHPTR<AJobTimer> _spMonsterJobTimer)
	{
		SHPTR<CSarcophagus> spSarcophagus = Create<CSarcophagus>(GetCoreInstance(), GiveID(), _SarcophagusType, _spMonsterJobTimer);
		SESSIONID MummyID = GiveID();
		spSarcophagus->Start(VOIDDATAS{ _pData, &MummyID });
		InsertMobObject(spSarcophagus->GetSessionID(), spSarcophagus);
	}

	void CServerAdiminstor::Free()
	{
	}
}