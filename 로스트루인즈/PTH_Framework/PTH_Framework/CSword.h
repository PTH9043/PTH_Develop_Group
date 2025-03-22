#pragma once
#include "CWeapon.h"
#include "CMyBmp.h"

class CSword : public CWeapon
{
public:
	explicit CSword();
	explicit CSword(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName, const ESWORDID& eSwordID);
	explicit CSword(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName, const ESWORDID& eSwordID, const int& iPower, const int& iGold);
	virtual ~CSword();
protected:
	ESWORDID			m_eSwordID;
	CMyBmp*			m_pRotate;
	CMyBmp*			m_pReset;
	bool						m_bRotate;
	float						m_fSpinAngle;
	TIMER					m_tSpinTimer;
	int						m_iMultipleValue;
	VECTOR2				m_v2AttackPos;
public:
	// CEquip을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	virtual CItem* Copy_Item() override;
protected:
	virtual void Update_Rect() override;
	// Rotate 
	virtual void Rotate();
	// override
	virtual void Update_OnwerInfo() override;
	virtual  void Fall() override;
public:
	const ESWORDID& Get_SwordID() const { return m_eSwordID; }
	const bool& Is_Rotate() const { return m_bRotate; }
	const VECTOR2& Get_AttackPos() const { return m_v2AttackPos; }
	
	void Set_SwordID(const ESWORDID& eSwordID) { this->m_eSwordID = eSwordID; }
	void Set_Rotate(const bool& IsRotate) { this->m_bRotate = IsRotate; }
	void Set_SpinValue(const float& fValue) { this->m_fSpinAngle = fValue; }
	void Set_MultipleValue(const int& multi) { this->m_iMultipleValue = multi; }
	void Set_AttackPos(const VECTOR2& v2AttackPos) { this->m_v2AttackPos = v2AttackPos; }
};

