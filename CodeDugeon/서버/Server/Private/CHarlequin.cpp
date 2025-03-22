#include "ServerDefines.h"
#include "CHarlequin.h"
#include "ATransform.h"
#include "CHarlequinAnimController.h"

namespace Server {

	CHarlequin::CHarlequin(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer) :
		CServerMonster(OBJCON_CONDATA, _ID, _spMonsterJobTimer)
	{
		SetMonsterType(TAG_CHAR::TAG_HARLEQUINN);
		UpdateFindRange(80, 150);
		SetMoveSpeed(5);
		SetAttackRange(9.f);
		SetCharStatus(CHARSTATUS{ 1, 0, 1500 });
	}

	_bool CHarlequin::Start(const VOIDDATAS& _ReceiveDatas)
	{
		__super::Start(_ReceiveDatas);
		SetAnimController(Create<CHarlequinAnimController>(GetCoreInstance(), ThisShared<CHarlequin>(),
			"..\\..\\Resource\\Anim\\Harlequin\\", "Harlequin1_FBX.bin"));
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

	void CHarlequin::Tick(const _double& _dTimeDelta)
	{
	}

	void CHarlequin::State(SHPTR<ASession> _spSession, _int _MonsterState)
	{
		__super::State(_spSession, _MonsterState);
	}

	void CHarlequin::ProcessPacket(_int _type, void* _pData)
	{
		__super::ProcessPacket(_type, _pData);
	}

	bool CHarlequin::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void CHarlequin::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	void CHarlequin::ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID)
	{
	}

	void CHarlequin::Free()
	{
	}

}