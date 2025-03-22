#pragma once
#include "CCharacter.h"
#include "CDamageText.h"

class CPlayer;
class CMonsterAI;
class CFollowText;

class CMonster abstract : public CCharacter
{
public:
	explicit CMonster();
	explicit CMonster(CPlayer* pTarget);
	virtual ~CMonster();
protected:
	CMonsterAI*						m_pMonsterAI;
	CPlayer*								m_pTarget;
	CFollowText*						m_pNameText;
	vector <CDamageText*>	m_vecDamageText;
	int										m_iDeadCount;
	int										m_iDamgeTextIndex;
	const int								MAX_DAMGE_TEXT;
	bool										m_bDamaged;
	bool										m_bCollision;
public:
	// CCharacter을(를) 통해 상속됨
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	virtual void Damaged(const float & fDamage) override;
	virtual void Change_State(const ECHARSTATE & eCharState) PURE;

	virtual void Obj_Active(const bool& IsActive) override;
	// Copy Monster
	virtual CMonster* Copy_Monster() { return nullptr; }

	void Active_DamageText(const float& fDamage);

	void Blood_Effect_To_Player(CObj* pObj, const int& iBloodIndex);
protected:
	virtual void Collision_Wall();
	virtual void Update_Text();
	// Dead
	virtual void Dead();
	virtual void Move();
	virtual void Jump();
	virtual void Anim_Setting() PURE;

public:
	void Set_DeadCount(const int& iDeadCount) { this->m_iDeadCount = iDeadCount; }
};

