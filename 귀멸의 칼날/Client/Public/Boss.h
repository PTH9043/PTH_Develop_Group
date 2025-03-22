#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
class CGameInstance;
END

BEGIN(Client)
class CBoss : public CCharacter
{
protected:
	explicit CBoss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CBoss(const CBoss& _rhs);
	virtual ~CBoss(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render(void) override;
protected:
	virtual HRESULT	SetUp_ConstantTable(void) override;
	virtual HRESULT	SetUp_Components(void) override;
protected:
	virtual void Coll_Tick(_matrix _WorldMatrix);
#ifdef _DEBUG
	virtual void Coll_Render(void);
#endif

public:
	// 내 공격이 상대방에게 맞았을 때 상태 처리 
	virtual void Check_AttackCollision(CCharacter* _pOther);
	virtual void Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter) override;

protected:
	// 상태주기 / 포효 / 스텝 / 어텍
	virtual void Move_HitCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	virtual void Move_StepCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	virtual void Give_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	// 애니메이션 처리 
	virtual void Animation_Control_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	// 움직임 처리 
	virtual void Behavior_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) = 0;
	// 상태 초기화 
	virtual void Init_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) = 0;
protected:
	virtual void Restrict_Nav(const _double& _dTimeDelta) override;
	// 맞았을때 처리
	virtual void Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager) override;
	// 피격 당했을때 이펙트 처리
	virtual void Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter) override;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free(void) override;
protected:
	// 플레이어 
	CCharacter* m_pTargetObject = nullptr; 
	// 러쉬 공격할때 네비에 닿았는지 필요한 불변수 
	_bool m_bNaviCheck = true; 
	// 히트카운트 
	_uint m_iHitCount = 0;
	//// hp
	//_int m_iHP = 0;
	// 과거 상태 
	_uint m_iPreState = 0;
	_uint m_iPreAtkState = 0;
	_uint m_iPreIdleState = 0;
	// 페이즈 불변수 
	_bool m_bPhase2 = false;
	// 턴 카운트 
	_uint m_iTurnCount = 0;
	_uint m_iWalkCount = 0;
	// 외곽선 변수 
	_float m_fEdge = 0.f;
	// 가드상태 변수 
	_bool m_bGuard = false;
};
END