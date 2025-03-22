#pragma once
#include "CStateMachine.h"

BEGIN(Client)
class CPlayer;
class CCamera_Free;
class CWeapon;

class CPlayerStateMachine final : public CStateMachine
{
public:
	enum STATE
	{
		STATE_IDLE = 1 , 
		STATE_WALK ,
		STATE_RUN ,
		STATE_ROLL,
		STATE_ATTACK,
		STATE_SPATTACK,
		STATE_HEALING,
		STATE_HIT ,
		STATE_SKILL,
		STATE_OPENSIGHT,
		STATE_ECT_BEHABIOR_FINDCHECKPOINTS,
		STATE_DIE ,
		STATE_END = 0
	};
public:
	const _uint& Get_State() const { return m_iState; }
	void Set_State(const _uint& _iState) { this->m_iState = _iState; }

	// Setting Weapon
	void Set_Weapon(CWeapon* _pWeapon);

	const _bool& Is_DoubleRoll() const { return m_bDoubleRoll; }
	void Set_DoubleRoll(const _bool& _bIsDoubleRoll) { this->m_bDoubleRoll = _bIsDoubleRoll; }

	const _bool& Is_StartRunAndWalk() const { return m_bStartRunAndWalk; }
	void Set_StartRunAndWalk(const _bool& _bIsRunAndWalk) { this->m_bStartRunAndWalk = _bIsRunAndWalk; }

	const _bool& Is_NextAttackMotion() const { return m_bNextAttackMotion; }
	void Set_NextAttackMotion(const _bool& _bNextAttackMotion) { this->m_bNextAttackMotion = _bNextAttackMotion; }

	const _uint& Get_AttackCount() const { return m_iAttackCount; }
	void Set_AttackCount(const _uint& _iAttackCount) { this->m_iAttackCount = _iAttackCount; }

	const _uint& Get_RandomAttackMotion() const { return m_iRandomAttack; }

	const _float& Get_AttackAnimControlValue() const { return m_fAttackAnimationControlValue; }

	const _uint& Get_Activation_CheckPoints() const { return m_iActivation_CheckPoints; }
	void Reset_Activation_CheckPoints() { m_iActivation_CheckPoints = 0; }
protected:
	explicit CPlayerStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CPlayerStateMachine() = default;
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Reset Roll state
	void Reset_RollState() { if (m_iPrevState == STATE_ROLL) m_bDoubleRoll = false; }
	// Change State
	void Change_State(const _uint& _iState, CPlayer* _pOwnerObject);
protected:
	//  Idle State
	HRESULT Idle_State(CPlayer* _pOwnerObject);
	// Walk State
	HRESULT Walk_State(CPlayer* _pOwnerObject);
	// Run State
	HRESULT Run_State(CPlayer* _pOwnerObject);
	// Roll State
	HRESULT Roll_State(CPlayer* _pOwnerObject);
	// Attack State
	HRESULT Attack_State(CPlayer* _pOwnerObject);
	// Specail State
	HRESULT AttackSp_State(CPlayer* _pOwnerObject);
	// Healing State
	HRESULT Healing_State(CPlayer* _pOwnerObject);
	// Hit State 
	HRESULT Hit_State(CPlayer* _pOwnerObject);
	// Skill State 
	HRESULT Skill_State(CPlayer* _pOwnerObject);
	// Open Sight State
	HRESULT OpenSight_State(CPlayer* _pOwnerObject);
	// Ect Behavior State
	HRESULT Find_CheckPoints_Ect_Behavior(CPlayer* _pOwnerObject);
	// Dead State
	HRESULT Dead_State(CPlayer* _pOwnerObject);
	// Prev Dot
	_float m_fPrevDot = 0.f;
	// Double Roll
	_bool		m_bDoubleRoll = false;
	// Next Attack Motin
	_bool		m_bNextAttackMotion = false;
	// Attack Count;
	_uint		m_iAttackCount = 0;
	// One Hand Max Count
	_uint		m_iOneHandMaxCount = 0;
private:
	_bool		m_bStartRunAndWalk = false;
	// Random Attack
	_int			m_iRandomAttack = 0;
	// Animation Control Value
	_float		m_fAttackAnimationControlValue = 2.5f;
	// Activation CheckPoints
	_uint		m_iActivation_CheckPoints = 0;
private:
	CHierarchyNode* m_pLeftLeg = nullptr;
	CHierarchyNode* m_pRightLeg = nullptr;
	_string   m_strSaveRollAnim = "";
public:
	static CPlayerStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END