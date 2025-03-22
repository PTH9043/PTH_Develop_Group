#pragma once
#include "Include.h"

class CAnimator;

class CObj abstract
{
protected:
	explicit CObj();
public:
	virtual ~CObj();
protected:
	UINT							m_iID;
	// Name 
	wstring						m_strName;
	// 위치, 방향, 크기
	VECTOR2						m_v2CurPos;
	VECTOR2						m_v2CenterPos;
	VECTOR2						m_v2Dir;
	VECTOR2						m_v2Size;
	VECTOR2						m_v2DrawSize;
	// RECT
	RECT							m_tRect;
	RECT							m_tDrawRect;
	RECT							m_tRestrictDraw;
	// Object ID
	EOBJID							m_eObjID;
	// 활성화
	bool								m_bEnable;
	// 죽는 거 판단
	bool								m_bDead;
	// 회전 값
	float								m_fRotValue;
public:
	// 
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj*& pObj) PURE;
	virtual void Release() PURE;

	virtual void Obj_Active(const bool& IsActive) { m_bEnable = IsActive; }
protected:
	// Update Rect
	virtual void Update_Rect();
	// Check in Rect
	bool Check_Pos_IsInRect(const VECTOR2& v2Pos, const RECT& tRect);

	// 개발자용
	void Draw_Line(HDC _hdc, const VECTOR2& v2ScrollPos, POINT& pt1, POINT& pt2);
	void Draw_Line_V2(HDC _hdc, const VECTOR2& v2ScrollPos, VECTOR2& pt1, VECTOR2& pt2);

	virtual void Rotation_Point(POINT& pt, const VECTOR2& v2Size, const float& fAngle, const float& fRotateValue, const float & fX, const float& fYl);

	virtual void Rotation_V2(VECTOR2 & v2Point, const VECTOR2 & v2CenterPos, const float& fX, const float& fY, const float& fAngle);
public:
	const UINT& Get_ID() const { return m_iID; }
	const wstring& Get_Name() const { return m_strName; }
	const VECTOR2& Get_CurPos() const { return m_v2CurPos; }
	const VECTOR2& Get_CenterPos() const { return m_v2CenterPos; }
	const VECTOR2& Get_Dir() const { return m_v2Dir; }
	const VECTOR2& Get_Size() const { return m_v2Size; }
	const VECTOR2& Get_DrawSize() const { return m_v2DrawSize; }
	const RECT& Get_Rect() const { return m_tRect; }
	const RECT& Get_DrawRect() const { return m_tDrawRect; }
	const EOBJID& Get_ObjID() const { return m_eObjID; }
	const bool& Is_Enable() const { return m_bEnable; }
	const bool& Is_Dead() const { return m_bDead; }
	const float& Get_RotValue() const { return m_fRotValue; }
	// set
	void Set_ID(const UINT& iID) { this->m_iID = iID; }
	void Set_Name(const wstring& strName) { this->m_strName = strName; }
	void Set_CurPos(const VECTOR2& v2CurPos) { this->m_v2CurPos = v2CurPos; }
	void Set_CenterPos(const VECTOR2& v2CenterPos) { this->m_v2CenterPos = v2CenterPos; }
	void Set_Dir(const VECTOR2& v2Dir) { this->m_v2Dir = v2Dir; }
	void Set_Size(const VECTOR2& v2Size) { this->m_v2Size = v2Size; }
	void Set_DrawSize(const VECTOR2& v2DrawSize) { this->m_v2DrawSize = v2DrawSize; }
	void Set_OBJID(const EOBJID& eObjID) { this->m_eObjID = eObjID; }
	void Set_Enable(const bool& bEnable) { this->m_bEnable = bEnable; }
	void Set_Dead(const bool& bDead) { this->m_bDead = bDead; }
	void Set_RotValue(const float& fRotValue) { this->m_fRotValue = fRotValue; }
};

