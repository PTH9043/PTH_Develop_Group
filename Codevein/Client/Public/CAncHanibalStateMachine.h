#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CAncientHannibal;

class CAncHanibalStateMachine : public CMonsterState
{
private:
	explicit CAncHanibalStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CAncHanibalStateMachine() = default;
public:
	const _bool& Is_PlayerNotForward_MoveStation() const { return m_bPlayerNotForward_MoveStation; }
	void Set_PlayerNotForward_MoveStation(const _bool& _bIsPlayerNotForwardMove) { this->m_bPlayerNotForward_MoveStation = _bIsPlayerNotForwardMove; }

	const _bool& Is_PlayerFindIdleState() const { return m_bPlayerFindIdleState; }
	void Set_PlayerFindIdleState(const _bool& _bPlayerFindIdleState) { this->m_bPlayerFindIdleState = _bPlayerFindIdleState; }

	void Set_TierdState(const _bool& _bTiredState) { this->m_bTiredState = _bTiredState; }
	const _bool& Is_TiredState() const { return m_bTiredState; }

	const _bool& Is_NextPage() const { return m_bNextPage; }

	void Set_DoubleDogde(const _bool& _bIsDoubleDogde) { this->m_bDoubleDogde = _bIsDoubleDogde; }
	const _bool& Is_DoubleDogde() const { return m_bDoubleDogde; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CAncientHannibal* _pOwnerObject);
protected:
	void Idle_State(CAncientHannibal* _pOwnerObject);
	void Find_State(CAncientHannibal* _pOwnerObject);
	void Retarget_State(CAncientHannibal* _pOwnerObject);
	void Dodge_State(CAncientHannibal* _pOwnerObject);
	void Move_State(CAncientHannibal* _pOwnerObject);
	void Run_State(CAncientHannibal* _pOwnerObject);
	void Attack_State(CAncientHannibal* _pOwnerObject);
	void Hit_State(CAncientHannibal* _pOwnerObject);
	void Die_State(CAncientHannibal* _pOwnerObject);
protected:
	_bool	m_bPlayerNotForward_MoveStation = false;
	_bool	m_bPlayerFindIdleState = false;
	_bool	m_bTiredState = false;
	_bool m_bNextPage = false;
	_bool m_bDoubleDogde = false;
public:
	static CAncHanibalStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END