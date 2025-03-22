#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CMonkeyDevil;

class CMonkeyDevilStateMachine  final : public CMonsterState
{
public:
protected:
	explicit CMonkeyDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CMonkeyDevilStateMachine() = default;
public:
	const _uint& Get_AtkCount() const { return m_iAttackCount; }
	void Set_AttackCount(const _uint& _iAtkCount) { this->m_iAttackCount = _iAtkCount; }
	const _bool& Is_InitAttack() const { return m_bInitAttack; }
	void Set_InitAttack(const _bool& _bInitAttack) { this->m_bInitAttack = _bInitAttack; }
	const _uint& Get_DownCount() const { return m_iDownCount; }
	const _uint& Get_AtkHammerSpCount() const { return m_iAttackHammerSPCount; }
	void Set_AtkHammerSpCount(const _uint& _iAtkHammerCount) { this->m_iAttackHammerSPCount = _iAtkHammerCount; }

public:
	const CUSTIMER& Get_Pattern1Timer() const { return m_tPattern1Timer; }
	void Set_Pattern1Timer(const CUSTIMER& _tCusTimer) { this->m_tPattern1Timer = _tCusTimer; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CMonkeyDevil* _pOwnerObject);
protected:
	void Idle_State(CMonkeyDevil* _pOwnerObject);
	void Find_State(CMonkeyDevil* _pOwnerObject);
	void Down_State(CMonkeyDevil* _pOwnerObject);
	void Dodge_State(CMonkeyDevil* _pOwnerObject);
	void Retarget_State(CMonkeyDevil* _pOwnerObject);
	void Walk_State(CMonkeyDevil* _pOwnerObject);
	void Run_State(CMonkeyDevil* _pOwnerObject);
	void Attack_State(CMonkeyDevil* _pOwnerObject);
	void Hit_State(CMonkeyDevil* _pOwnerObject);
	void Die_State(CMonkeyDevil* _pOwnerObject);
private:
	_uint				m_iAttackCount = 0;
	_bool				m_bInitAttack = true;
	_uint				m_iDownCount = 0;
	_uint				m_iAttackHammerSPCount = 0;
	// Patter1 Timer
	CUSTIMER		m_tPattern1Timer;
public:
	static CMonkeyDevilStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END