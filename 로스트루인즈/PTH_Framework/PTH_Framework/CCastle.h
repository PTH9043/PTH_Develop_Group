#pragma once
#include "CBoss.h"
#include "CBossSword.h"

class CCastle : public CBoss
{
public:
	explicit CCastle();
	explicit CCastle(CPlayer* pTarget);
	virtual ~CCastle();
protected:
	// 대쉬 어택을 위한 거리
	int			m_iDashAttackDistance;
	int			m_iMagicAttackDistance;
	int			m_iMagicTwoDistance;
	int			m_iSpawnSwordIndex;
	bool			m_bDash;
	bool			m_bMagic_1;
	bool			m_bMagic_2;
	// 타이머
	TIMER	m_tDashTimer;
	TIMER	m_tChangeTimer;
	TIMER	m_tMagic1SpawnSwordTimer;
	TIMER m_tMagic1AttackTimer;
	TIMER m_tMagic2AttackTimer;
	TIMER m_tGiveDamageTimer;
	// Sword
	vector<CBossSword*> m_vecSwordList;

public:
	// CMonster을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	virtual void Damaged(const float & fDamage) override;
	virtual void Change_State(const ECHARSTATE & eCharState) override;
	virtual void Change_BossState(const EBOSSSTATE& eBossState) override;

	virtual void Obj_Active(const bool& IsActive) override;
protected:
	virtual void Update_Rect() override;
	virtual void Update_Attack_Rect() override;
	virtual void Update_Text() override;
	// Dead
	virtual void Dead() override;
	// 대쉬
	void Dash();
	// Pase Change
	void Phase_Change();
	// Magic 1
	void Magic_1();
	// Magic 2
	void Magic_2();

	virtual void Move() override;
	virtual void Jump() override;
	virtual void Anim_Setting() override;
	// Animation IDLE
	void ANIM_IDLE();
	void ANIM_RUN();
	void ANIM_ATTACK();
	void ANIM_DASHATTACK();
	void ANIM_DASH();
	void ANIM_RUNSTOP();
	void ANIM_HIT();
	void ANIM_DEAD();
	void ANIM_PHASE2();
	void ANIM_P2_IDLE();
	void ANIM_P2_CAST();
	void ANIM_P2_CAST_FRONT();
	void ANIM_P2_HIT();
	void ANIM_P2_DIE();
public:
	const bool Is_Magic1() const { return m_bMagic_1; }
	const bool Is_Magic2() const { return m_bMagic_2; }
};

