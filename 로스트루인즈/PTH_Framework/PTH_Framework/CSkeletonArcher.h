#pragma once
#include "CMonster.h"

class CSkeletonArcher : public CMonster
{
public:
	explicit CSkeletonArcher();
	explicit CSkeletonArcher(CPlayer* pTarget);
	virtual~CSkeletonArcher();
protected:
	bool m_bShoot;
public:
	// CCharacter을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	virtual void Damaged(const float & fDamage) override;
	virtual void Change_State(const ECHARSTATE & eCharState) override;
	// Copy Monster
	virtual CMonster* Copy_Monster() override;
	virtual void Obj_Active(const bool& IsActive) override;
protected:
	virtual void Update_Rect() override;
	virtual void Update_Attack_Rect() override;
	virtual void Update_Text() override;
	virtual void Dead() override;
	virtual void Attack() override;
	virtual void Move() override;
	virtual void Jump() override;
	virtual void Anim_Setting() override;

	// Animation IDLE
	void ANIM_IDLE();
	void ANIM_RUN();
	void ANIM_ATTACK();
	void ANIM_HIT();
	void ANIM_DEAD();
};
