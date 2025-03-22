#pragma once
#include "Include.h"

class CObj;

class CSceneActive abstract
{
public:
	explicit CSceneActive();
	explicit CSceneActive(const ESCAID& eScaID, const RECT& tRect);
	virtual ~CSceneActive();
protected:
	// Dpy ID 
	ESCAID m_eSceneActiveID;
	// Rect
	RECT m_tRect;
	RECT m_tDrawRect;
	// CenterPos
	VECTOR2 m_v2CenterPos;
	// 4개의 점을 구하기 위한 변수
	vector<VECTOR2> m_vecPosList;
	// 충돌을 위한 변수
	bool		m_bCollision;
	// 보기 위한 것들
	int m_iR;
	int m_iG;
	int m_iB;
public:
	// Init
	virtual void Initialize();
	// Collision Obj
	virtual bool Collision_Obj(CObj*& pObj, const VECTOR2& v2ScrollPos) PURE;
	// Render
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos, const bool& IsRender);
	// Release
	virtual void Release() PURE;
protected:
	bool Is_Not_Render(const VECTOR2 & v2ScrollPos);

	bool Check_Pos_IsInRect(const VECTOR2 & v2Pos, const RECT & tRect);

public:
	const ESCAID& Get_SCID() const { return m_eSceneActiveID; }
	const RECT& Get_Rect() const { return m_tRect; }
	const VECTOR2& Get_CenterPos() const { return m_v2CenterPos; }
};

