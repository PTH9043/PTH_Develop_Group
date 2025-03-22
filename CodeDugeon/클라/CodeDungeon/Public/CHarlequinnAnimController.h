
#pragma once
#include "CMonsterAnimController.h"
#include "USound.h"

BEGIN(Client)
class CHarlequinn;
/*
@ Date: 2024-07-05, Writer: 이성현
@ Explain
-  Minotaur를 조종하는 클래스
*/
class CHarlequinnAnimController final : public CMonsterAnimController {
public:
	CHarlequinnAnimController(CSHPTRREF<UDevice> _spDevice);
	CHarlequinnAnimController(const CHarlequinnAnimController& _rhs);
	virtual ~CHarlequinnAnimController() = default;

	enum HARLEQUINNSTATE
	{
		ANIM_TAUNT = 6
	};

public:
	virtual void Free() override;
	CLONE_MACRO(CHarlequinnAnimController, "CHarlequinnAnimController::Clone To Failed")
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

	_int m_irandomNumforhit = 0;
	_int m_iHitCount = 0;
	_bool	m_bisHitCooldown = false;
	_double m_dHitCooldownTime = 0;
	const _double HIT_COOLDOWN_DURATION = 5.0;

	FMOD::Channel* m_pHitChannel;
	
};

END
