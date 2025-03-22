#pragma once
#include "CEquip.h"

class CBow : public CEquip
{
public:
	explicit CBow();
	explicit CBow(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName);
	virtual ~CBow();
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
	void Shoot_Arrow();
	// Update Shoot Speed
	void Update_Shoot_Speed();

	virtual CItem* Copy_Item() override;
protected:
	virtual void Update_OnwerInfo() {}

public:
	// get
	const int& Get_ShootSpeed() const { return m_iShootSpeed; }
	const bool& Is_Shoot() const { return m_bShoot; }
	// set
	void Set_ShootSpeed(const int& iShootSpeed) { this->m_iShootSpeed = iShootSpeed; }
	void Set_Shoot(const bool& IsShoot) { this->m_bShoot = IsShoot; }
};

