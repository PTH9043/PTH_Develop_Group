#pragma once
#include "CMonsterAnimController.h"
#include "USound.h"
#include "UResource.h"
BEGIN(Client)
class CMummy;
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  Mummy를 조종하는 클래스
*/
class CMummyAnimController final : public CMonsterAnimController {
public:
	CMummyAnimController(CSHPTRREF<UDevice> _spDevice);
	CMummyAnimController(const CMummyAnimController& _rhs);
	virtual ~CMummyAnimController() = default;

	enum MUMMYSTATE
	{
		ANIM_SLEEP = 6, ANIM_AWAKE = 7, ANIM_TAUNT = 8
	};
public:
	virtual void Free() override;
	CLONE_MACRO(CMummyAnimController, "CMummyAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	WKPTR< CMummy>			m_wpMummyMob;

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

	//=====================SOUND CHANNEL================
	FMOD::Channel* m_pAttack1Channel;
	FMOD::Channel* m_pAttack2Channel;
	FMOD::Channel* m_pSwhoosh1Channel;
	FMOD::Channel* m_pSwhoosh2Channel;
	FMOD::Channel* m_pTauntChannel;

	_int	m_irandomNumforhit = 0;
	FMOD::Channel* m_pHitChannel;

	_bool		m_bPlayAttackSound1;
	_bool		m_bPlayAttackSound2;
	_bool		m_bPlayHitSound;
	 

};

END
