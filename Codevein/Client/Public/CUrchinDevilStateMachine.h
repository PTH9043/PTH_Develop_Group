#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CUrchinDevil;

class CUrchinDevilStateMachine : public CMonsterState
{
private:
	explicit CUrchinDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CUrchinDevilStateMachine() = default;
public:
	const _uint& Get_AttackMandibleIndex() const { return m_iAttackMandibleIndex; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CUrchinDevil* _pOwnerObject);
protected:
	void Idle_State(CUrchinDevil* _pOwnerObject);
	void Find_State(CUrchinDevil* _pOwnerObject);
	void Retarget_State(CUrchinDevil* _pOwnerObject);
	void Run_State(CUrchinDevil* _pOwnerObject);
	void Attack_State(CUrchinDevil* _pOwnerObject);
	void Hit_State(CUrchinDevil* _pOwnerObject);
	void Die_State(CUrchinDevil* _pOwnerObject);
protected:
	_uint m_iAttackMandibleIndex = 0;
public:
	static CUrchinDevilStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END