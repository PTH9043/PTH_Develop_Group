




#pragma once
#include "Include.h"
#include "CObj.h"
#include "CSword.h"

class CCollision
{
public:
	static bool Is_Collision_Rect_PoinArr(const RECT& tRect, vector<VECTOR2>& ptArr);

	static bool Check_Pos_IsInRect_V2(const VECTOR2& v2Pos, const RECT& tRect);

	static bool Check_Pos_IsInRect_PT(const POINT& pt, const RECT& tRect);

	static bool Is_OBBColl(CObj* p1, CObj* p2);

	static bool Is_ObbColl_Equip(CObj* p1, CSword* pSword);

	static float Vector2_Dot(const VECTOR2& v1, const VECTOR2& v2);
};

