#pragma once
#include "CEquip.h"

class CWeapon : public CEquip
{
public:
	explicit CWeapon();
	explicit CWeapon(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName);
	virtual ~CWeapon();
protected:
	EWEAPONID m_eWeaponID;
public:
	// CEquip을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	// Rotate 
	virtual void Rotate();
public:
	//get 
	const EWEAPONID& Get_WeaponID() const { return m_eWeaponID; }
	const bool& Is_Collision() const { return m_bCollision; }
	// set 
	void Set_WeaponID(const EWEAPONID& eWeaponType) { this->m_eWeaponID = eWeaponType; }
	void Set_Collision(const bool& IsCollision) { this->m_bCollision = IsCollision; }
};

