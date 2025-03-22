#pragma once
#include "UAnimationController.h"

BEGIN(Client)
class CWarriorPlayer;
class UAnimation;
/*
@ Date: 2024-04-30, Writer: 박태현
@ Explain
-  Warrior를 조종하는 클래스
*/
class CUserWarriorAnimController final : public UAnimationController {
public:
	enum WARRIOR_STATE {
		ANIM_ROLL = 6, ANIM_RUN = 7, ANIM_JUMP = 8
	};
public:
	CUserWarriorAnimController(CSHPTRREF<UDevice> _spDevice);
	CUserWarriorAnimController(const CUserWarriorAnimController& _rhs);
	virtual ~CUserWarriorAnimController() = default;
public:
	virtual void Free() override;
	CLONE_MACRO(CUserWarriorAnimController, "CUserWarriorAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
	// 네트워크 정보를 받아오는 함수
	virtual void ReceiveNetworkProcessData(void* _pData) override;

	_float GetUltAttackOneCoolTime() {return UltimateAttackOneCoolTime;}
	_float GetUltAttackTwoCoolTime() {return UltimateAttackTwoCoolTime;}
	_float GetShortAttackCoolTime() {return ShortAttackCoolTime;}
	_bool GetBlindEffectBool() { return BlindEffectOn; }
	const _bool& GetDieEffectBool() { return m_bDieEffectTurnedOn; }

private:
	WKPTR< CWarriorPlayer>			m_wpWarriorPlayer;
	_int													m_iWComboStack;
	_int													m_iSComboStack;

	_float			UltimateAttackOneCoolTime = 0;
	_float			UltimateAttackTwoCoolTime = 0;
	_float			ShortAttackCoolTime = 0;

	_float			UltimateAttackOneDurationTime = 0;
	_float			UltimateAttackTwoDurationTime = 0;
	_float			ShortAttackDurationTime = 0;

	_bool UltAttackOneisAvailable = true;
	_bool UltAttackTwoisAvailable = true;
	_bool ShortAttackisAvailable = true;

	_bool m_bDieEffectTurnedOn = false;

	_float HitStack = 0;
	_bool BlindEffectOn = false;

	_bool ifStartedGame = false;

	//=====================SOUND BOOL================
	_bool m_bReviveSoundPlay = false;
	_bool m_bDieSoundPlay = false;

};

END
