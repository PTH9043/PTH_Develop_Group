#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CGiantWhiteDevil;

class CGWhiteDevilStateMachine : public CMonsterState
{
public:
private:
	explicit CGWhiteDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CGWhiteDevilStateMachine() = default;
public:
	const _uint& Get_AtkIndex() const { return m_iAttackCount; }
	void Set_AtkIndex(const _uint& _iAtkIndex) { this->m_iAttackCount = _iAtkIndex; }

	const _bool& Is_PlayerNotForward_MoveStation() const { return m_bPlayerNotForward_MoveStation; }
	void Set_PlayerNotForward_MoveStation(const _bool& _bIsPlayerNotForwardMove) { this->m_bPlayerNotForward_MoveStation = _bIsPlayerNotForwardMove; }

	CUSTIMER* Get_PatternMoveTimer() { return &m_tPatternMoveTimer; }
	void Set_PatternMoveTimer(const CUSTIMER& _tCusMoveTimer) { this->m_tPatternMoveTimer = _tCusMoveTimer; }

	const _bool& Is_PlayerFindIdleState() const { return m_bPlayerFindIdleState; }
	void Set_PlayerFindIdleState(const _bool& _bPlayerFindIdleState) { this->m_bPlayerFindIdleState = _bPlayerFindIdleState; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CGiantWhiteDevil* _pOwnerObject);
protected:
	void Idle_State(CGiantWhiteDevil* _pOwnerObject);
	void Find_State(CGiantWhiteDevil* _pOwnerObject);
	void Retarget_State(CGiantWhiteDevil* _pOwnerObject);
	void Dodge_State(CGiantWhiteDevil* _pOwnerObject);
	void Move_State(CGiantWhiteDevil* _pOwnerObject);
	void Run_State(CGiantWhiteDevil* _pOwnerObject);
	void Attack_State(CGiantWhiteDevil* _pOwnerObject);
	void Hit_State(CGiantWhiteDevil* _pOwnerObject);
	void Die_State(CGiantWhiteDevil* _pOwnerObject);
protected:
	_uint	m_iDownCount = 0;
	_uint  m_iAttackCount = 0;

	_bool	m_bPlayerNotForward_MoveStation = false;
	_bool	m_bPlayerFindIdleState = false;
	// CusTimer
	CUSTIMER m_tPatternMoveTimer;
public:
	static CGWhiteDevilStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END