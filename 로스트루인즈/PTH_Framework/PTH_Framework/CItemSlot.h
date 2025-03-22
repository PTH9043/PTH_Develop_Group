#pragma once
#include "CSlot.h"

class CItemSlot final : public CSlot
{
public:
	explicit CItemSlot();
	explicit CItemSlot(CObj* pOwner, const TCHAR* szSlotPath, const TCHAR* szBackImg, const RGBINFO& tInfo);
	virtual ~CItemSlot();
protected:
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
};

