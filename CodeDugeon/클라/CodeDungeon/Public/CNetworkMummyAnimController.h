#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CMummy;
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  Mummy를 조종하는 클래스
*/
class CNetworkMummyAnimController final : public CMonsterAnimController {
public:
	CNetworkMummyAnimController(CSHPTRREF<UDevice> _spDevice);
	CNetworkMummyAnimController(const CNetworkMummyAnimController& _rhs);
	virtual ~CNetworkMummyAnimController() = default;

	enum MUMMYSTATE
	{
		ANIM_SLEEP = 6, ANIM_AWAKE = 7, ANIM_TAUNT = 8
	};
public:
	virtual void Free() override;
	CLONE_MACRO(CNetworkMummyAnimController, "CNetworkMummyAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	WKPTR< CMummy>			m_wpMummyMob;

	_bool										m_bAttackMode;
	_bool										m_bTauntMode;
	_double									m_dlastHitTime;
	_double									m_dlastAttackTime;
	_bool										m_bstartlastHitTime;
	_bool										m_blastAttackWasFirst;
	_double									m_dIdleTimer;
	_bool										m_bFoundPlayerFirsttime;
	_double									m_didleRandomValueChoosingTimer;
	_int											m_iRandomValue;
	_double									m_dRecvAnimDuration;

};

END
