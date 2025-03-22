#pragma once
#include "CStateMachine.h"

BEGIN(Engine)
class CGameObject;
class CModel;
class CTransform;
class CGameObject;
class CHierarchyNode;
END

BEGIN(Client)
 
class CMonsterState : public CStateMachine
{
public:
	enum STATE : _uint
	{
		STATE_IDLE,
		STATE_DOWN,
		STATE_FIND,
		STATE_RETARGET,
		STATE_DODGE,
		STATE_MOVE,
		STATE_RUN,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};
protected:
	explicit CMonsterState(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CMonsterState() = default;
public:
	const _uint& Get_AttackCount() const { return m_iAttackCount; }
	void Set_AttackCount(const _uint& _iAtkCout) { this->m_iAttackCount = _iAtkCout; }
public:
	const _bool& Is_FindAttackState() const { return m_bFindAttackState; }
	const _bool& Is_NextAttack()  const { return m_bNextAttack; }
	void Set_NextAttack(const _bool& _bIsAttack) { this->m_bNextAttack = _bIsAttack; }
public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject) override;
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) = 0;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) = 0;
protected:
	void Reset_FindAttackState(const _double& _dDeltaTime);

protected:
	_bool		m_bFindAttackState = false;
	_float		m_fFindAttackStateResetTime = 0.f;
	_bool		m_bNextAttack = false;
	_uint		m_iAttackCount = 0;
public:
	virtual void Free() override;
};

END