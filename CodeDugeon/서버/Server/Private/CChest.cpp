#include "ServerDefines.h"
#include "CChest.h"
#include "ATransform.h"
#include "AAnimController.h"
#include "CChestAnimController.h"

namespace Server {
	CChest::CChest(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer) : 
		CServerMonster(OBJCON_CONDATA, _ID, _spMonsterJobTimer)
	{
		SetMonsterType(TAG_CHAR::TAG_CHEST);
		UpdateFindRange(40.f, 90.f);
		SetMoveSpeed(5);
		SetAttackRange(9.f);
	}

	_bool CChest::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start(_ReceiveDatas);
		SetAnimController(Create<CChestAnimController>(GetCoreInstance(), ThisShared<CChest>(),
			"..\\..\\Resource\\Anim\\Chest\\", "Chest 1_FBX.bin"));
#ifndef CREATED_SERVERMOBDATA
		MOBDATA* pMobData = static_cast<MOBDATA*>(_ReceiveDatas[0]);
		// Setting Animation 
		GetAnimController()->SetAnimation(pMobData->strAnimName);
		GetTransform()->SetPos(pMobData->mWorldMatrix.Get_Pos());
		GetTransform()->SetDirection(pMobData->mWorldMatrix.Get_Look());
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
		return true;
	}

	void CChest::Tick(const _double& _dTimeDelta)
	{
	}

	void CChest::State(SHPTR<ASession> _spSession, _int _MonsterState)
	{
		__super::State(_spSession, _MonsterState);
	}

	void CChest::ProcessPacket(_int _type, void* _pData)
	{
		__super::ProcessPacket(_type, _pData);
	}

	bool CChest::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void CChest::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	void CChest::ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID)
	{
	}

	void CChest::Free()
	{
	}

}