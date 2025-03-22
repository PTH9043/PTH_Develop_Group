#pragma once
#include "CItem.h"

class CArrow : public CItem
{
public:
	explicit CArrow();
	explicit CArrow(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName);
	virtual ~CArrow();
protected:
	TIMER		m_tDeadTimer;
	int			m_iAttackPower;
	int			m_iSpeed;
	bool		m_bFloor;
public:
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) override;
	virtual void Collision_Actor(CObj*& pObj) override;
	virtual void Release() override;

	virtual CItem* Copy_Item() override;

	virtual void Obj_Active(const bool& IsActive) override;

protected:
	virtual void Update_Rect() override;
	// Rotate 
	virtual void Rotate() override;

	virtual  void Fall() override;

public:
	const int Get_AttackPower() const { return m_iAttackPower; }
	const int Get_Speed() const { return m_iSpeed; }

	void Set_AttackPower(const int& iAttackPower) { this->m_iAttackPower = iAttackPower; }
	void Set_Speed(const int& iSpeed) { this->m_iSpeed = iSpeed; }
};

