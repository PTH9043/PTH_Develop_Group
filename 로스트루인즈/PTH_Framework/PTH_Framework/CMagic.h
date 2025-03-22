#pragma once
#include "CEquip.h"

class CMagic : public CEquip
{
public:
	explicit CMagic();
	explicit CMagic(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName, const int& iGold);
	virtual ~CMagic();
protected:
	int			m_iShootSpeed;
	bool			m_bShoot;
public:
	// 
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) override;
	virtual void Collision_Actor(CObj*& pObj) override;
	virtual void Release() override;
	// Shoot Arrow
	virtual void Shoot_Magic() {}

	virtual CItem* Copy_Item() override;
protected:
public:
	// get
	const int& Get_ShootSpeed() const { return m_iShootSpeed; }
	const bool& Is_Shoot() const { return m_bShoot; }
	// set
	void Set_ShootSpeed(const int& iShootSpeed) { this->m_iShootSpeed = iShootSpeed; }
	void Set_Shoot(const bool& IsShoot) { this->m_bShoot = IsShoot; }
};


