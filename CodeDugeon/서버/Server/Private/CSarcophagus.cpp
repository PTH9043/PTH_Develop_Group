#include "ServerDefines.h"
#include "CSarcophagus.h"
#include "CMummy.h"
#include "ATransform.h"
#include "AAnimator.h"
#include "CSacrophagusAnimController.h"
#include "ACoreInstance.h"
#include "AAnimation.h"
#include "CMobLayoutLoader.h"
#include "CMobLayOutSaver.h"

namespace Server
{
	CSarcophagus::CSarcophagus(OBJCON_CONSTRUCTOR, SESSIONID _ID, SARCOPHAGUSTYPE _eSarcophagusType, 
		SHPTR<AJobTimer> _spMonsterJobTimer) :
		CServerMonster(OBJCON_CONDATA, _ID, _spMonsterJobTimer), m_eSarcophagusType{_eSarcophagusType}, m_isInitStart{false}
	{
		if (SARCO_LAYING == m_eSarcophagusType)
		{
			SetMonsterType(TAG_CHAR::TAG_SARCOPHAGUS_LAYING);
		}
		else
		{
			SetMonsterType(TAG_CHAR::TAG_SARCOPHAGUS_STANDING);
		}

		UpdateFindRange(40.f, 80.f);
	}

	_bool CSarcophagus::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start(_ReceiveDatas);
		ASSERT_CRASH(_ReceiveDatas.size() >= 0);
		_float4x4 Matrix = _float4x4::CreateScale(0.1f) * _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180));
#ifndef CREATED_SERVERMOBDATA
		MOBDATA* pMoveData = static_cast<MOBDATA*>(_ReceiveDatas[0]);
		SESSIONID* pSessionID = static_cast<SESSIONID*>(_ReceiveDatas[1]);

		SHPTR<CSacrophagusAnimController> spSacrophagusAnimController = nullptr;

		SHPTR<CMummy> spMummy;
		switch (m_eSarcophagusType)
		{
		case SARCO_LAYING:
		{
			spSacrophagusAnimController =	Create<CSacrophagusAnimController>(GetCoreInstance(), ThisShared<CSarcophagus>(), 
				"..\\..\\Resource\\Anim\\Sarcophagus\\", "SarcophagusLaying_FBX.bin", Matrix);
			spMummy = Create<CMummy>(GetCoreInstance(), *pSessionID, MUMMYTYPE::MUMMY_LAYING, GetMonsterJobTimer());
		}
		break;
		case SARCO_STANDING:
		{
			spSacrophagusAnimController = Create<CSacrophagusAnimController>(GetCoreInstance(), ThisShared<CSarcophagus>(),
				"..\\..\\Resource\\Anim\\Sarcophagus\\", "SarcophagusStanding_FBX.bin", Matrix);
			spMummy = Create<CMummy>(GetCoreInstance(), *pSessionID, MUMMYTYPE::MUMMY_STANDING, GetMonsterJobTimer());
		}
		break;
		}
		SetAnimController(spSacrophagusAnimController);
		spMummy->Start({ _ReceiveDatas[0] });
		SetOwnerMonsterSessionID(spMummy->GetSessionID());
		GetTransform()->SetNewWorldMtx(pMoveData->mWorldMatrix);
		GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
		BringCellIndextoPosition();
		GetCoreInstance()->InsertMobObject(spMummy->GetSessionID(), spMummy);

		if (SARCO_LAYING == m_eSarcophagusType)
			spMummy->GetTransform()->TranslateDir((spMummy->GetTransform()->GetLook() * -1), 1, 10);
#else
		SHPTR<CSacrophagusAnimController> spSacrophagusAnimController = Create<CSacrophagusAnimController>(GetCoreInstance(), ThisShared<CSarcophagus>(),
			"..\\..\\Resource\\Anim\\Sarcophagus\\", "SarcophagusLaying_FBX.bin", Matrix);
		SetAnimController(spSacrophagusAnimController);

		MOBSERVERDATA* pMobData = static_cast<MOBSERVERDATA*>(_ReceiveDatas[0]);
		GetAnimController()->SetAnimation(pMobData->iStartAnimIndex);
		GetTransform()->SetNewWorldMtx(pMobData->mWorldMatrix);
		GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
		BringCellIndextoPosition();
		SetSessionID(pMobData->iMobID);
#endif
		return S_OK;
	}

	void CSarcophagus::RunningPermanentDisableSituation()
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		SC_MONSTERFIND scMonsterFind;
		PROTOFUNC::MakeScMonsterFind(&scMonsterFind, GetSessionID(), TAG_FIND_ACTIVE, GetCurrentTargetPlayerID());
		CombineProto<SC_MONSTERFIND>(GetCopyBuffer(), GetPacketHead(), scMonsterFind, TAG_SC_MONSTERFIND);
		spCoreInstance->BroadCastMessage(GetCopyBufferPointer(), GetPacketHead());
	}

	void CSarcophagus::Tick(const _double& _dTimeDelta)
	{
		__super::Tick(_dTimeDelta);
	}

	void CSarcophagus::State(SHPTR<ASession> _spSession, _int _MonsterState)
	{
		FindPlayer(_spSession);
	}

	void CSarcophagus::ProcessPacket(_int _type, void* _pData)
	{
		__super::ProcessPacket(_type, _pData);
	}

	bool CSarcophagus::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void CSarcophagus::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	void CSarcophagus::ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID)
	{
	}

	void CSarcophagus::LastBehavior()
	{
	}

	void CSarcophagus::Free()
	{
	}
}