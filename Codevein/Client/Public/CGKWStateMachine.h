#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CGateKeeperWoman;

class CGKWStateMachine final : public CMonsterState
{
private:
	explicit CGKWStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CGKWStateMachine() = default;

public:
	const _uint& Get_DogdeCount() const { return m_iDogdeCount; }
	void Set_DogdeCount(const _uint& _iDogdeCount) { this->m_iDogdeCount = _iDogdeCount; }

	const _uint& Get_TiredCount() const { return m_iTiredCount; }
	void Set_TiredCount(const _uint& _iTiredCount) { this->m_iTiredCount = _iTiredCount; }

	const _bool& Is_TiredState() const { return m_bTiredState; }
	void Set_TiredState(const _bool& _bIsTiredState) { this->m_bTiredState = _bIsTiredState; }

	const _bool& Is_NextPage() const { return m_bNextPage; }

	void Set_Damaged(const _bool& _bIsDamaged) { this->m_bDamaged = _bIsDamaged; }
	const _bool& Is_Damaged() const { return m_bDamaged; }

	const _bool& Is_Turn180() const { return m_bTurn180; }
	void Set_Turn180(const _bool& _bIsTurn180) { this->m_bTurn180 = _bIsTurn180; }

	const _bool& Is_TurnLeft90() const { return m_bTurnLeft90; }
	void Set_TurnLeft90(const _bool& _bTurnLeft90) { this->m_bTurnLeft90 = _bTurnLeft90; }

	const _bool& Is_TurnRight90() const { return m_bTurnRight90; }
	void Set_TurnRight90(const _bool& _bTurnRight90) { this->m_bTurnRight90 = _bTurnRight90; }

	const _bool& Is_Arrive() const { return m_bArrive; }
	void Set_Arrive(const _bool& _bIsArrive) { this->m_bArrive = _bIsArrive; }

	const _bool& Is_ChangeMoveState() const {	return m_bChangeMoveState;	}
	void Set_ChangeMoveState(const _bool& _bIsChangeMoveState) { this->m_bChangeMoveState = _bIsChangeMoveState; }

	const _bool& Is_RandomMove() const { return m_bRadomMove; }
	void Set_RandomMove(const _bool& _bIsRandomMove) { this->m_bRadomMove = _bIsRandomMove; }

	const _float& Get_NextAnimationCaluclateValue() const {return m_fNextAnimationCalculationTime;}
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CGateKeeperWoman* _pOwnerObject);
protected:
	void Idle_State(CGateKeeperWoman* _pOwnerObject);
	void Find_State(CGateKeeperWoman* _pOwnerObject);
	void Retarget_State(CGateKeeperWoman* _pOwnerObject);
	void Dodge_State(CGateKeeperWoman* _pOwnerObject);
	void Move_State(CGateKeeperWoman* _pOwnerObject);
	void Run_State(CGateKeeperWoman* _pOwnerObject);
	void Attack_State(CGateKeeperWoman* _pOwnerObject);
	void Hit_State(CGateKeeperWoman* _pOwnerObject);
	void Die_State(CGateKeeperWoman* _pOwnerObject);
private:
	_uint		m_iDogdeCount = 0;
	_uint		m_iTiredCount = 0;
	_bool		m_bNextPage = false;
	_bool		m_bTiredState = false;
	_bool		m_bDamaged = false;
	_bool		m_bTurn180 = false;
	_bool		m_bTurnLeft90 = false, m_bTurnRight90 = false;
	_bool		m_bArrive = false;
	_bool		m_bChangeMoveState = false;
	_bool		m_bRadomMove = false;
	_float		m_fNextAnimationCalculationTime = 2.5f;
public: 
	static CGKWStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END