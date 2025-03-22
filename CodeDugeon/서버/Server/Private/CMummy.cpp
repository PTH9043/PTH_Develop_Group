#include "ServerDefines.h"
#include "CMummy.h"
#include "ATransform.h"
#include "CMummyAnimController.h"
#include "CMobLayoutLoader.h"
#include "ANavigation.h"
#include "ACoreInstance.h"
#include "ACell.h"
#include "AAnimator.h"
#include "AAnimController.h"
#include "AAnimation.h"
#include "ASession.h"

namespace Server
{
	CMummy::CMummy(OBJCON_CONSTRUCTOR,  SESSIONID _ID, MUMMYTYPE _MummyType, SHPTR<AJobTimer> _spMonsterJobTimer)
		: CServerMonster(OBJCON_CONDATA, _ID, _spMonsterJobTimer),
		m_eMumyType{ _MummyType }, m_isAttackMode{ false }, m_isLastAttackWasFirst{ false },
		m_IdleTimer{2}, m_IdleRandomValueChooseTimer{2}, m_AttackTimer{3},
		m_iRandomValue{0}, m_isSendMonsterFindPacket{true}
	{
		if (MUMMY_LAYING == m_eMumyType)
		{
			SetMonsterType(TAG_CHAR::TAG_MUMMY_LAYING);
		}
		else
		{
			SetMonsterType(TAG_CHAR::TAG_MUMMY_STANDING);
		}
		UpdateFindRange(40.f, 90.f);
		SetMoveSpeed(5);
		SetAttackRange(9.f);
		SetCharStatus(CHARSTATUS{ 1, 0, 500 });
	}
	_bool CMummy::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start(_ReceiveDatas);
		_float4x4 Matrix = _float4x4::CreateScale(0.1f) * _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180));
		SetAnimController(Create<CMummyAnimController>(GetCoreInstance(), ThisShared<CMummy>(),
			"..\\..\\Resource\\Anim\\Mummy\\", "Mummy_DEMO_1_FBX.bin", Matrix));
#ifndef CREATED_SERVERMOBDATA
		MOBDATA* pMobData = static_cast<MOBDATA*>(_ReceiveDatas[0]);
		// Setting Animation 
		GetAnimController()->SetAnimation(pMobData->strAnimName);
		GetTransform()->SetNewWorldMtx(pMobData->mWorldMatrix);
		GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
		BringCellIndextoPosition();
#else
		MOBSERVERDATA* pMobData = static_cast<MOBSERVERDATA*>(_ReceiveDatas[0]);
		GetAnimController()->SetAnimation(pMobData->iStartAnimIndex);
		GetTransform()->SetNewWorldMtx(pMobData->mWorldMatrix);
		GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
		BringCellIndextoPosition();
		SetSessionID(pMobData->iMobID);
#endif
		SetCharStatus(CHARSTATUS{ 1, 0, 5 });
		return true;
	}

	void CMummy::Tick(const _double& _dTimeDelta)
	{
		SESSIONID CurrentTargetPlayerID = GetCurrentTargetPlayerID();

		int type = TAG_FIND_ACTIVE;
		if (true == IsCurrentFindPlayer())
		{
			type = TAG_FIND_NEAR;
			m_isSendMonsterFindPacket = false;
		}
		else if (true == IsCurrentAtkPlayer())
		{
			type = TAG_FIND_ATK;
			m_isSendMonsterFindPacket = false;
		}

		if (false == m_isSendMonsterFindPacket)
		{
			SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
			SC_MONSTERFIND scMonsterFind;
			PROTOFUNC::MakeScMonsterFind(&scMonsterFind, GetSessionID(), type, CurrentTargetPlayerID);
			CombineProto<SC_MONSTERFIND>(GetCopyBuffer(), GetPacketHead(), scMonsterFind, TAG_SC_MONSTERFIND);
			spCoreInstance->DirectSendMessage(GetCurrentTargetPlayerID(), GetCopyBufferPointer(), GetPacketHead());
			m_isSendMonsterFindPacket = true;
		}
	}

	void CMummy::State(SHPTR<ASession> _spSession, _int _MonsterState)
	{
		__super::State(_spSession, _MonsterState);
	}

	void CMummy::ProcessPacket(_int _type, void* _pData)
	{
		__super::ProcessPacket(_type, _pData);
	}

	bool CMummy::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return __super::IsHit(_pPawn, _dTimeDelta);
	}

	void CMummy::CallActiveEnable()
	{
		//SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		//SC_MONSTERFIND scMonsterFind;
		//PROTOFUNC::MakeScMonsterFind(&scMonsterFind, GetSessionID(), TAG_FIND_ACTIVE, GetCurrentTargetPlayerID());
		//CombineProto<SC_MONSTERFIND>(GetCopyBuffer(), GetPacketHead(), scMonsterFind, TAG_SC_MONSTERFIND);
		//spCoreInstance->DirectSendMessage(GetCurrentTargetPlayerID(), GetCopyBufferPointer(), GetPacketHead());
	}

	void CMummy::CallActiveDisable()
	{
	}

	void CMummy::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	void CMummy::ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID)
	{
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		SC_MONSTERFIND scMonsterFind;
		{
			PROTOFUNC::MakeScMonsterFind(&scMonsterFind, GetSessionID(), TAG_FIND_DISABLE, _CurPlayerSessionID);
			CombineProto<SC_MONSTERFIND>(GetCopyBuffer(), GetPacketHead(), scMonsterFind, TAG_SC_MONSTERFIND);
			spCoreInstance->DirectSendMessage(_CurPlayerSessionID, GetCopyBufferPointer(), GetPacketHead());
		}
	}

	void CMummy::Free()
	{
	}

}