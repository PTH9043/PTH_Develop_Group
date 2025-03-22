#pragma once
#include "CSlot.h"

class CEquip;

class CActionSlot final : public CSlot
{
public:
	explicit CActionSlot();
	explicit CActionSlot(CObj* pOwner, const TCHAR* szSlotPath, const TCHAR* szBackImg, const RGBINFO& tInfo
	, const int& iIndex);
	virtual ~CActionSlot();
protected:
	// Owner가 착용하고 있는 Equip를 확인
	CMyBmp*	    m_pWearableEquipBmp;
	// 뒤 이미지
	CMyBmp*		m_pBackImg;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

protected:
	// Check Player Item
	void Check_Player_EquipItem();

public:
	void Set_WeableEquipBmp(CMyBmp* pBmp) { this->m_pWearableEquipBmp = pBmp; }
};

