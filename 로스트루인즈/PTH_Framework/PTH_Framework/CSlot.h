#pragma once
#include "CUserInterface.h"
#include "CMyBmp.h"

class CSlot : public CUserInterface
{
public:
	explicit CSlot();
	explicit CSlot(CObj* pOwner, const TCHAR* szSlotPath, const RGBINFO& tInfo);
	virtual ~CSlot();
protected:
	// Owner 
	CObj*				m_pOwner;
	// Owner Obj
	CMyBmp*		m_pSlotImg;
	// RGB Info 
	RGBINFO			m_tTranspercyRGB;
	// Index
	int					m_iIndex;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

public:
	const int& Get_Index() const { return m_iIndex; }
	void Set_Index(const int& Index) { this->m_iIndex = Index; }
};

