#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CMimic;
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  Mimic를 조종하는 클래스
*/
class CNetworkMimicAnimController final : public CMonsterAnimController {
public:
	CNetworkMimicAnimController(CSHPTRREF<UDevice> _spDevice);
	CNetworkMimicAnimController(const CNetworkMimicAnimController& _rhs);
	virtual ~CNetworkMimicAnimController() = default;

	enum MimicSTATE
	{
		ANIM_SLEEP = 6, ANIM_AWAKE = 7, ANIM_TAUNT = 8
	};
public:
	virtual void Free() override;
	CLONE_MACRO(CNetworkMimicAnimController, "CNetworkMimicAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	WKPTR< CMimic>			m_wpMimicMob;

	_bool					m_bAttackMode;
	_bool					m_bTauntMode;
	_double					m_dlastHitTime;
	_double					m_dlastAttackTime;
	_bool					m_bstartlastHitTime;
	_bool					m_blastAttackWasFirst;
	_double					m_dIdleTimer;
	_bool					m_bFoundPlayerFirsttime;
	_double					m_didleRandomValueChoosingTimer;
	_int					m_iRandomValue;
	_double				m_dRecvAnimDuration;

};

END
