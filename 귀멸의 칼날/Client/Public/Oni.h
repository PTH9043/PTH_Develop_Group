#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
END

BEGIN(Client)
class COni : public CCharacter
{
public:
	enum ONISTATE : _ulong
	{
		ONISTATE_IDLE = 1,
		ONISTATE_RECOGNIZE = 2,
		ONISTATE_BATTLE = 3,
		ONISTATE_DEAD = 4,

		ONISTATE_RECOGNIZE_APPEARANCE = 100,
		ONISTATE_RECOGNIZE_RUN = 200,
		ONISTATE_RECOGNIZE_STEP = 300,
		ONISTATE_RECOGNIZE_CHASE = 400,

		ONISTATE_BATTLE_NUT = 1000,
		ONISTATE_BATTLE_MOVELOOK_R = 1100,
		ONISTATE_BATTLE_MOVELOOK_L = 1200,
		ONISTATE_BATTLE_BURST = 1300,
		ONISTATE_BATTLE_LOOK = 1400,

		ONISTATE_BATTLE_RUN = 1500,
		ONISTATE_BATTLE_BACK = 1600,

		ONISTATE_BATTLE_STEP_R = 1700,
		ONISTATE_BATTLE_STEP_L = 1800,
		ONISTATE_BATTLE_STEP_B = 1900,

		ONISTATE_BATTLE_ATTACK1 = 2000,
		ONISTATE_BATTLE_ATTACK2 = 2100,
		ONISTATE_BATTLE_SKILL1 = 2200,
		ONISTATE_BATTLE_SKILL1_END = 2300,
		ONISTATE_BATTLE_SKILL2 = 2400,
		ONISTATE_BATTLE_SKILL2_END = 2500,
		ONISTATE_BATTLE_HIT = 2600,

		ONISTATE_BATTLE_STEP_F = 2700,
		ONISTATE_BATTLE_ATTACK3 = 2800,
		ONISTATE_BATTLE_TURN = 2900,

		ONISTATE_JUDGE = 100,
		ONISTATE_END = 0
	};
protected:
	explicit COni(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit COni(const COni& _rhs);
	virtual ~COni(void) = default;
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

protected:
	// 피격 당했을때 처리
	virtual void Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
protected:
	// 상태 지정해주기 
	virtual void Give_State(const _double& _dTimeDelta);

	virtual void Idle_Oni(const _double& _dTimeDelta) = 0;
	virtual void Recognize_Oni(const _double& _dTimeDelta) = 0;
	virtual void Battle_Oni(const _double& _dTimeDelta) = 0;
	// 움직이기 
	virtual void Animation_Control_State(const _double& _dTimeDelta);
	virtual void Idle_State(const _double& _dTimeDelta) = 0;
	virtual void Recognize_State(const _double& _dTimeDelta) = 0;
	virtual void Battle_State(const _double& _dTimeDelta) = 0;

	virtual void Behavior_State(const _double& _dTimeDelta);
protected:
	virtual void Restrict_Nav(const _double& _dTimeDelta) override;
protected:
	// 몬스터들끼리의 충돌처리 
	virtual void Collision_Oni(const _double& _dTimeDelta);
	// 죽을때 호출할 삭제 함수 
	virtual HRESULT Delete_Self(void);
	// 플레이어와 충돌처리
	void Collision_Player(const _double& _dTimeDelta);
	//virtual _bool	Hit_SuperArmor(void);
public:
	// 내 공격이 상대방에게 맞았을 때 상태 처리 
	virtual void Check_AttackCollision(CCharacter* _pOther);
	// 공격받고 hp깎을때 외부에서 호출할 함수 
	void Minus_OniHp(void) { m_tBattleData.fCurHp--;/*m_iHp--;*/ }
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free(void) override;
protected:
	// 배틀매니저, 플레이어가 가져가서 쓰는 콜라이더 
	CCollider* m_pHitColliderCom = nullptr;
	// 오니가 상대하는 플레이어 
	class CPlayer* m_pTargetObject = nullptr;
	// 플레이어 인식 처음 들어왔을때를 판단하는 불변수 
	_bool	m_bFirstCheck = false;
	// 오니 상태값
	_uint	m_iOniState = 0;
	// 시간계산에 쓰이는 변수
	_double m_dBattleTime = 0.0;
	// 패턴 흐름 순서를 지정할 카운트 
	_uint m_iPatternCount = 0;
	// 오니 hp 
	//_int m_iHp = 0;
	// 자식오니에서 다른 레이어 오니들이 죽었는지 판단하고 틱, 랜더 돌리기위한 불변수 
	_bool m_bTickCheck = false;
	// 스킬이 끝났는지를 체크하는 불변수 
	_bool m_bSkillEndCheck = false;
	// 오리지널 위치값
	_float3 m_vOriginalPos = {};
	// 네비에 닿았을 때 체크할 불변수 
	_bool m_bNaviCheck = true;
	// 공격이 성공했는지 체크하는 불변수
	_bool m_bIsAttackSuccess = false;
	_uint m_iOniLayerNum = 1;
	// 외곽선 변수 
	_float m_fEdge = 0.f;
	// 스킬 체크 변수
	_bool m_bMSkillCheck = false;
	// 사망시 Delete 타이머
	CUSTIMER m_tDeleteTimer = CUSTIMER(0.5f);
};
END