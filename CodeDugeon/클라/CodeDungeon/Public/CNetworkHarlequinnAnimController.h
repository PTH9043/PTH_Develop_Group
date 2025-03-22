#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CHarlequinn;
/*
@ Date: 2024-07-05, Writer: 이성현
@ Explain
-  Minotaur를 조종하는 클래스
*/
class CNetworkHarlequinnAnimController final : public CMonsterAnimController {
public:
	CNetworkHarlequinnAnimController(CSHPTRREF<UDevice> _spDevice);
	CNetworkHarlequinnAnimController(const CNetworkHarlequinnAnimController& _rhs);
	virtual ~CNetworkHarlequinnAnimController() = default;

	enum HARLEQUINNSTATE
	{
		ANIM_TAUNT = 6
	};

public:
	virtual void Free() override;
	CLONE_MACRO(CNetworkHarlequinnAnimController, "CNetworkHarlequinnAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;

private:
	WKPTR< CHarlequinn>			m_wpHarlequinnMob;

	_bool					m_bAttackMode;
	_double					m_dlastHitTime;
	_double					m_dlastAttackTime;
	_bool					m_bstartlastHitTime;
	_bool					m_blastAttackWasFirst;
	_double					m_dIdleTimer;
	_bool					m_bTauntMode;

	_bool					m_bAttack1FirstTime;
	_float3					m_f3ThrowingPos;

	ARRAY<_float3, 6>		m_arrThrowingDir;

	_double					m_didleRandomValueChoosingTimerforPatrol;

	_int					m_iRandomValueforPatrol;
	_int					m_iRandomValueforDodge;

	_bool					m_bDodge;
	_bool					m_bWillWalkUntilCloseRange;
	_bool					m_bWillJumpToCloseRange;

};

END
