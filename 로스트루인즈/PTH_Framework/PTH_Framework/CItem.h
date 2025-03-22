#pragma once
#include "CActor.h"
#include "CMyBmp.h"

class CItem abstract : public CActor
{
public:
	explicit CItem();
	explicit CItem(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName);
	virtual ~CItem();
protected:
	CObj*						m_pOwner;
	CMyBmp*				m_pImgBmp;
	EITEMID					m_eItemID;
	bool							m_bItemPicked;
	bool							m_bUse;
	bool							m_bCollision;
	POINT						m_tPoint[3];
	float							m_fDiagonal;
	// 개발자용, 충돌이 제대로 되고 있는지 판단하는 것
	vector<VECTOR2>	m_vecCollisionPoint;
	const TCHAR*			m_szImgPath;

	wstring					m_strExplainText;

public:
	// 
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj*& pObj) PURE;
	virtual void Release() PURE;

	virtual CItem* Copy_Item() { return nullptr; }
protected:
	virtual  void Fall();
	virtual void Rotate(){}
	virtual bool Check_Pos_IsInRect(const VECTOR2& v2ScrollPos, const RECT& tRect);
public:
	// get 
	CObj* Get_Owner() { return m_pOwner; }
	CMyBmp* Get_Img() { return m_pImgBmp; }
	const EITEMID& Get_ItemID() const { return m_eItemID; }
	const bool& Is_ItemPicked() const { return m_bItemPicked; }
	const bool& Is_Use() const { return m_bUse; }
	// set 
	void Set_Owner(CObj* pOwner) { this->m_pOwner = pOwner; }
	void Set_ItemID(const EITEMID& eItemType) { this->m_eItemID = eItemType; }
	void Set_ItemPicked(const bool& IsItemPicked) { this->m_bItemPicked = IsItemPicked; }
	void Set_Use(const bool& IsUse) { this->m_bUse = IsUse; }
};

