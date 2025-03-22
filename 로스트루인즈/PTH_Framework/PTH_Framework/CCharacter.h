#pragma once
#include "CActor.h"

class CWall;

class CCharacter abstract : public CActor
{
public:
	explicit CCharacter();
	virtual ~CCharacter();
protected:
	// Collision Wall 
	CWall*						m_pCollWall;
	// 캐릭터 Stat
	ECHARSTATE					m_eCharState;
	// Damaged Dir
	VECTOR2						m_v2DamagedDir;
	// Status
	STATUS						m_tStatus;
	// Jump
	bool						m_bJump;
	// Ground
	bool						m_bIsGround;
	// Left, Right Check
	VECTOR2						m_v2LeftPos;
	VECTOR2						m_v2RightPos;

	// 공격 
	RECT						m_tAttackRect;
	// Collision Direction
	EDIR						m_eCollDirection;
	// Save Direction	
	EDIR						m_eSaveDirection;
	bool						m_bDamaged;

	bool						m_bActiveUI;
	bool						m_bCollisionWall;
	bool						m_bCollisionDoor;

public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override PURE;
	virtual int Update() override PURE;
	virtual void Late_Update() override PURE;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override PURE;
	virtual void Collision_Actor(CObj*& pObj) override PURE;
	virtual void Release() override PURE;
	// Damage
	virtual void Damaged(const float& fDamage) PURE;
	// Character state
	virtual void Change_State(const ECHARSTATE& eCharState) PURE;
protected:
	// Update Rect
	virtual void Update_Rect() override;
	virtual void Update_Attack_Rect(){}
	// Attack
	virtual void Attack(){}
	// Move 
	virtual void Move() PURE;
	// Jump
	virtual void Jump() PURE;
	// Animation Setting
	virtual void Anim_Setting()  PURE;
public:
	// Get
	CWall* Get_CollWall() { return m_pCollWall; }
	const ECHARSTATE& Get_CharState() const { return m_eCharState; }
	const STATUS& Get_Status() const { return m_tStatus; }
	const bool& Is_Jump() const { return m_bJump; }
	const RECT& Get_AttackRect() const { return m_tAttackRect; }
	const VECTOR2& Get_LeftPos() const { return m_v2LeftPos; }
	const VECTOR2& Get_RightPos() const { return m_v2RightPos; }
	const VECTOR2& Get_DamagedDir() const { return m_v2DamagedDir; }
	const bool& Is_CollDoor() const { return m_bCollisionDoor; }
	// Set 
	void Set_CollWall(CWall*& pWall) { this->m_pCollWall = pWall; }
	void Set_CharState(const ECHARSTATE& eCharState) { this->m_eCharState = eCharState; }
	void Set_Status(const STATUS& tStatus) { this->m_tStatus = tStatus; }
	void Set_Jump(const bool& IsJump) { this->m_bJump = IsJump; }
	void Set_AttackRect(const RECT& tRect) { this->m_tAttackRect = tRect; }
	void Set_DamagedDir(const VECTOR2& v2Damaged) { this->m_v2DamagedDir = v2Damaged; }
	void Set_CollDoor(const bool& bCollDoor) { this->m_bCollisionDoor = bCollDoor; }
};

