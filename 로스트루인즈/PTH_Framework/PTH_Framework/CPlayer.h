#pragma once
#include "CCharacter.h"
#include "CEquip.h"

class CInventory;
class CBow;

class CPlayer final : public CCharacter
{
public:
	explicit CPlayer();
	virtual ~CPlayer();
protected:
	// Inven 
	CInventory*				m_pInventory;
	// Equip List
	vector<CEquip*>	m_vecEquipList;
	// Hit
	TIMER						m_tHitTimer;
	// Player State
	EPlayerState			m_ePlayerState;

	VECTOR2					m_v2EquipPos;

	int							m_iAttackIndex;
	int							m_iDamageSoundIndex;

	TIMER						m_tHurtTimer;
	bool							m_bRealDead;
	bool							m_bPressTab;
	bool							m_bOpenShop;

	TCHAR*					m_pMoveSound;
public:
	// CCharacter을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj*& pObj) override;
	virtual void Release() override;
	// Damage
	virtual void Damaged(const float& fDamage);
	// Character state
	virtual void Change_State(const ECHARSTATE& eCharState) override;
	// 플레이어 행동
	void Change_PlayerState(const EPlayerState& ePlayerState);

	void Reback();
protected:
	// Show Inven 
	void Show_Inven();
	// Change Dir
	void Change_Dir();
	// Update Rect
	virtual void Update_Rect() override;

	bool Dead();

	void Hit();

	void Item_Equip();

	virtual void Attack() override;
	// Move 
	virtual void Move() override;
	// Jump
	virtual void Jump() override;
	// Shoot Arrow
	void Shoot_Arrow(CBow* pBow, const int& iIndex, const char& chString);
	// Animation Setting
	virtual void Anim_Setting() override;
	// Path
	void Anim_Stand_IDLE();
	void Anim_Stand_RUN();
	void Anim_Stand_RISE();
	void Anim_Stand_FALL();
	void Anim_Stand_FALL_TO_IDLE();
	void Anim_Stand_ROLL_NORMAL();
	void Anim_Stand_ROLL_JP();
	void Anim_Stand_OneHand();
	void Anim_Stand_TwoHand();
	void Anim_Stand_Dagger();
	void Anim_Stand_SpellCast();
	void Anim_Stand_SpellFire();
	void Anim_Stand_Block();
	void Anim_Damaged();
	void Anim_Bow();
	void Anim_Die();
public:
	// get 
	CInventory* Get_Inventory() { return m_pInventory; }
	CEquip* Get_Wearable_Equip(const int& iIndex) { return m_vecEquipList[iIndex]; }
	const VECTOR2& Get_EquipPos() const { return m_v2EquipPos; }
	const bool& Is_OpenShop() const { return m_bOpenShop; }
	const bool& Is_RealDead() const { return m_bRealDead; }
	// set 
	void Set_Inven(CInventory* pInven) { this->m_pInventory = pInven; }
	void Set_Wearable_Equip(const int& iIndex, CEquip* pEquip) { m_vecEquipList[iIndex] = pEquip; }
	void Set_EquipPos(const VECTOR2& v2Pos) { this->m_v2EquipPos = v2Pos; }
	void Set_OpenShop(const bool& IsOpenShop) { this->m_bOpenShop = IsOpenShop; }
	void Set_MoveSound(TCHAR* pMoveSound) { this->m_pMoveSound = pMoveSound; }
	void Set_RealDead(const bool& IsRealDead) { this->m_bRealDead = IsRealDead; }
};

