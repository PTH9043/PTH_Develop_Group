#pragma once
#include "CSlot.h"
#include "CItem.h"
#include "CText.h"

class CInvenSlot final : public CSlot
{
public:
	explicit CInvenSlot();
	explicit CInvenSlot(CObj* pOwner, const TCHAR* szSlotPath,  const RGBINFO& tInfo);
	virtual ~CInvenSlot();
protected:
	// 아이템을 저장할 리스트
	list<CItem*>		m_lsItemList;
	// Text
	CText*				m_pText;
	// Selected
	bool				m_bSelected;
	// Restrict bool
	bool				m_bOverRestrict;
	bool				m_bUse;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	// Insert Item
	void Insert_Item(CObj*& pItem);
	// Item Num Minus
	void ItemNum_Minus();
	// Delete Item
	void Delete_Item();
protected:
	void Update_Text();
public:
	// get 
	CItem* Get_Item() { if (m_lsItemList.size() > 0) return m_lsItemList.front(); else return nullptr; }
	const bool& Is_Selected() const { return m_bSelected; }
	const int Get_Size() { return static_cast<int>(m_lsItemList.size()); }
	//set 
	void Set_Selected(const bool& IsSelected) { this->m_bSelected = IsSelected; }
	void Set_RestrictRect(const RECT& tRestrict) { this->m_tRestrictDraw = tRestrict; }
};
