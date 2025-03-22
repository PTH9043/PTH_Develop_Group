#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CMinotaur;
/*
@ Date: 2024-07-05, Writer: 이성현
@ Explain
-  Minotaur를 조종하는 클래스
*/
class CNetworkMinotaurAnimController final : public CMonsterAnimController {
public:
	CNetworkMinotaurAnimController(CSHPTRREF<UDevice> _spDevice);
	CNetworkMinotaurAnimController(const CNetworkMinotaurAnimController& _rhs);
	virtual ~CNetworkMinotaurAnimController() = default;

public:
	virtual void Free() override;
	CLONE_MACRO(CNetworkMinotaurAnimController, "CNetworkMinotaurAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;

	const _bool& GetRushMode() { return m_bRushMode; }
	const _float3& GetTargetPos() { return m_f3RushTargetPos; }
private:
	WKPTR< CMinotaur>			m_wpMinotaurMob;

	_bool					m_bAttackMode;
	_double					m_dlastHitTime;
	_double					m_dlastAttackTime;
	_bool					m_bstartlastHitTime;
	_bool					m_blastAttackWasFirst;
	_double					m_dIdleTimer;

	_bool					m_bRushMode;
	_float3					m_f3RushTargetPos;
	_double					m_dRushModeTimer;

	_double					m_dRushAttackTimer;
	_double					m_didleRandomValueChoosingTimer;
	_int					m_iRandomValue;
};

END
