#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CWolfDevil;

class CWolfDevilStateMachine : public CMonsterState
{
private:
	explicit CWolfDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CWolfDevilStateMachine() = default;
public:
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CWolfDevil* _pOwnerObject);
protected:
	void Idle_State(CWolfDevil* _pOwnerObject);
	void Find_State(CWolfDevil* _pOwnerObject);
	void Retarget_State(CWolfDevil* _pOwnerObject);
	void Dodge_State(CWolfDevil* _pOwnerObject);
	void Run_State(CWolfDevil* _pOwnerObject);
	void Attack_State(CWolfDevil* _pOwnerObject);
	void Hit_State(CWolfDevil* _pOwnerObject);
	void Die_State(CWolfDevil* _pOwnerObject);
protected:
	_uint	m_iDownCount = 0;
public:
	static CWolfDevilStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END