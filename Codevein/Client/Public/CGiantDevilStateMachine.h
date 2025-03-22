#pragma once
#include "CMonsterState.h"

BEGIN(Client)
class CGiantDevil;
class CCamera_Free;
class CWeapon;

class CGiantDevilStateMachine final : public CMonsterState
{
protected:
	explicit CGiantDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CGiantDevilStateMachine() = default;
public:
	const _uint& Get_AtkCount() const { return m_iAttackCount; }
	void Set_AttackCount(const _uint& _iAtkCount) { this->m_iAttackCount = _iAtkCount; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) override;
	// Change State
	void Change_State(const _uint& _iState, CGiantDevil* _pOwnerObject);
protected:
	void Idle_State(CGiantDevil* _pOwnerObject);
	void GetUp_State(CGiantDevil* _pOwnerObject);
	void Find_State(CGiantDevil* _pOwnerObject);
	void Threat_State(CGiantDevil* _pOwnerObject);
	void Attack_State(CGiantDevil* _pOwnerObject);
private:
	_uint m_iAttackCount = 0;
public:
	static CGiantDevilStateMachine* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END