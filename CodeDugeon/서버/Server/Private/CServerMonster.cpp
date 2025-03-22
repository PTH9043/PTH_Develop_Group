#include "ServerDefines.h"
#include "CServerMonster.h"
#include "ATransform.h"
#include "AAnimController.h"

namespace Server {

	CServerMonster::CServerMonster(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer)
		: AMonster(OBJCON_CONDATA, _ID, _spMonsterJobTimer)
	{
	}

	_bool CServerMonster::Start(const VOIDDATAS& _ReceiveDatas)
	{
		return __super::Start(_ReceiveDatas);
	}

	void CServerMonster::Tick(const _double& _dTimeDelta)
	{
	}

	void CServerMonster::State(SHPTR<ASession> _spSession, _int _MonsterState)
	{
		FindPlayer(_spSession);
	}

	void CServerMonster::ProcessPacket(_int _type, void* _pData)
	{
		switch (_type)
		{
		case TAG_CS_MONSTERSTATE:
		{
			MOBSTATE* MonsterStateData = static_cast<MOBSTATE*>(_pData);
			SHPTR<ATransform> spTransform = GetTransform();
			SHPTR<AAnimController> spAnimController = GetAnimController();
			{
				spTransform->SetPos({ MonsterStateData->posx(), MonsterStateData->posy(), MonsterStateData->posz() });
				spTransform->RotateFix({ MonsterStateData->rotatex(), MonsterStateData->rotatey(), MonsterStateData->rotatez() });
				spAnimController->SetAnimation(MonsterStateData->animationindex());
			}
		}
		break;
		}
	}

	bool CServerMonster::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void CServerMonster::Collision(APawn* _pPawn, const _double& _dTimeDelta)
	{
	}

	void CServerMonster::ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID)
	{
	}

	void CServerMonster::Free()
	{
	}

}