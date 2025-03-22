#pragma once
#include "CWorldObj.h"
#include "CInventory.h"


class CShop : public CWorldObj
{
public:
	explicit CShop();
	virtual ~CShop();
protected:
	// 인벤토리 슬롯
	vector<CInvenSlot*>		m_vecInvenSlot[3];
	// Select Slot
	CInvenSlot*						m_pSelectSlot;
	// Owner 
	CPlayer*							m_pCustomer;
	// ============= IMG =============
	// Shop Bmp
	CMyBmp*						m_pShopBmp;
	// Img
	CMyBmp*						m_pImgBmp;
	// Img
	CMyBmp*						m_pBackBmp;
	// Img
	CMyBmp*						m_pItemBackBmp;
	// Equip Img
	CMyBmp*						m_pEquipBmp;
	CMyBmp*						m_pItemBmp;
	CMyBmp*						m_pThingsBmp;
	// ============= IMG =============
	// ============= Gold Text =============
	CText*								m_pGoldText;
	CText*								m_pExplainText;
	// RGB Info 
	RGBINFO							m_tTranspercyRGB;
	// Slot Rect
	RECT								m_tSlotRect;
	// slot 위치를 잡기 위한 벡터 변수들 
	VECTOR2							m_v2StartSlotPos;
	VECTOR2							m_v2SpawnSlotPos;
	VECTOR2							m_v2ShopImgPos;
	VECTOR2							m_v2ShopDrawSize;
	VECTOR2							m_v2ShopSize;
	// 선택 위치를 잡기 위한 위치 변수.. 
	VECTOR2							m_v2SelectSlotPos;
	// 인벤토리 인덱스
	int									m_iInvenIndex;
	// 슬롯 길이를 저장해 놓은 변수
	int									m_iSaveSlotLength;
	int									m_iSelectIndex;
	// Under Slot 
	int									m_iCheckUnderSlot;
	int									m_iShowStart;
	int									m_iShowEnd;

	bool									m_bUse;
	bool									m_bCollision;
public:
	// 
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) override;
	virtual void Collision_Actor(CObj*& pObj) override;
	virtual void Release() override;

	// 아이템 추가
	void Add_Item(CObj* pItem);
	// Judge Slot List
	void Judge_SlotList();
	// Slot이 선택되었는지 판단하는 함수
	void Judge_Select_Slot();
	// 켜지면서 인벤토리 정보들 초기화
	void Reset_Info();
	// Buy Item;
	void Buy_Item();
protected:
	void Render_Shop(HDC _hdc, const VECTOR2& v2ScrollPos);
	// 슬롯에서 아이템이 쓰여서 없어진건 아닌지 확인.. 
	void Check_SlotList();
	// Check in Rect
	bool Check_Pos_IsInRect(const VECTOR2& v2Pos, const RECT& tRect);
	// Update Rect
	virtual void Update_Rect() override;
public:
};

