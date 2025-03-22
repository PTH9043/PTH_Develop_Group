#pragma once
#include "CMonster.h"
#include "CBar.h"

class CBoss abstract : public CMonster
{
public:
	explicit CBoss();
	explicit CBoss(CPlayer* pTarget);
	virtual ~CBoss();
protected:
	vector<CBar*>	m_vecBarList;
	EBossState			m_eBossState;
	VECTOR2				m_v2BossBarPos;
	VECTOR2				m_v2TextPos;
	bool						m_bChange;
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
	virtual void Change_BossState(const EBOSSSTATE& eBossState) PURE;
protected:
	virtual void Update_Text() override {}
	virtual void Move() PURE;
	virtual void Jump() PURE;
	virtual void Anim_Setting() PURE;
public:
	const EBossState& Get_BossState() const { return m_eBossState; }
	const int& Get_DeadCount() const { return m_iDeadCount; }
	const bool& Is_Change() const { return m_bChange; }

	void Set_BossState(const EBossState& eBossState) { this->m_eBossState = eBossState; }
	void Set_DeadCount(const int& iDeadCount) { this->m_iDeadCount = iDeadCount; }
	void Set_Change(const bool& bChange) { this->m_bChange = bChange; }
};
