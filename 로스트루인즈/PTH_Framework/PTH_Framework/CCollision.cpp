
#include "pch.h"
#include "CCollision.h"
#include "CObj.h"
#include "CActor.h"

bool CCollision::Is_Collision_Rect_PoinArr(const RECT & tRect, vector<VECTOR2>& ptArr)
{
	for (int i = 0; i < static_cast<int>(ptArr.size()); ++i)
	{
		if (Check_Pos_IsInRect_V2(ptArr[i], tRect))
			return true;
	}

	return false;
}

bool CCollision::Check_Pos_IsInRect_V2(const VECTOR2 & v2Pos, const RECT & tRect)
{
	if (tRect.left >= v2Pos.x)
		return false;
	else if (tRect.top >= v2Pos.y)
		return false;
	else if (tRect.right <= v2Pos.x)
		return false;
	else if (tRect.bottom <= v2Pos.y)
		return false;

	return true;
}

bool CCollision::Check_Pos_IsInRect_PT(const POINT & pt, const RECT & tRect)
{
	if (tRect.left >= pt.x)
		return false;
	else if (tRect.top >= pt.y)
		return false;
	else if (tRect.right <= pt.x)
		return false;
	else if (tRect.bottom <= pt.y)
		return false;

	return true;
}

bool CCollision::Is_OBBColl(CObj * p1, CObj * p2)
{
	if (nullptr == p1)
		return false;
	if (nullptr == p2)
		return false;

	VECTOR2 pos1 = p1->Get_CenterPos();
	VECTOR2 pos2 = p2->Get_CenterPos();
	VECTOR2 size1 = p1->Get_Size();
	VECTOR2 size2 = p2->Get_Size();

	size1 = size1 / 2.f;
	size2 = size2 / 2.f;
	// A
	float leftA = pos1.x - size1.x;
	float rightA = pos1.x + size1.x;
	float topA = pos1.y - size1.y;
	float bottomA = pos1.y + size1.y;
	// B
	float leftB = pos2.x - size2.x;
	float rightB = pos2.x + size2.x;
	float topB = pos2.y - size2.y;
	float bottomB = pos2.y + size2.y;

	float fRadianA = p1->Get_RotValue();
	float fRadianB = p2->Get_RotValue();

	VECTOR2 dir = pos1 - pos2;
	// P1의 X 축 
	VECTOR2 dir1A = VECTOR2(cosf(fRadianA), sinf(fRadianA));
	VECTOR2 dir2A = VECTOR2(cosf(fRadianA - PI * 0.5f), sinf(fRadianA - PI * 0.5f));
	// P2의 X 축 
	VECTOR2 dir1B = VECTOR2(cosf(fRadianB), sinf(fRadianB));
	VECTOR2 dir2B = VECTOR2(cosf(fRadianB - PI * 0.5f), sinf(fRadianB - PI * 0.5f));
	// len1 
	VECTOR2 len1A = dir1A * (rightA - leftA) * 0.5f;
	VECTOR2 len2A = dir2A * (bottomA -  topA) * 0.5f;
	// len2
	VECTOR2 len1B = dir1B * (rightB - leftB) * 0.5f;
	VECTOR2 len2B = dir2B * (bottomB - topB) * 0.5f;

	// 1 축 검사
	float lengthA = len1A.Get_Length_Sqrt();
	float lengthB = fabsf(Vector2_Dot(dir1A, len1B)) + fabsf(Vector2_Dot(dir1A, len2B));
	float length = fabsf(Vector2_Dot(dir1A, dir));
	if (length > lengthA + lengthB)
		return false;
	// 2 축 검사
	lengthA = len2A.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir2A, len1B)) + fabsf(Vector2_Dot(dir2A, len2B));
	length = fabsf(Vector2_Dot(dir2A, dir));
	if (length > lengthA + lengthB)
		return false;
	// 3 축 검사
	lengthA = len1B.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir1B, len1A)) + fabsf(Vector2_Dot(dir1B, len2A));
	length = fabsf(Vector2_Dot(dir1B, dir));
	if (length > lengthA + lengthB)
		return false;
	// 4 축 검사
	lengthA = len2B.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir2B, len1A)) + fabsf(Vector2_Dot(dir2B, len2A));
	length = fabsf(Vector2_Dot(dir2B, dir));
	if (length > lengthA + lengthB)
		return false;

	return true;
}

bool CCollision::Is_ObbColl_Equip(CObj * p1, CSword * pSword)
{
	if (nullptr == p1)
		return false;
	if (nullptr == pSword)
		return false;

	VECTOR2 pos1 = p1->Get_CenterPos();
	VECTOR2 pos2 = pSword->Get_AttackPos();
	VECTOR2 size1 = p1->Get_Size();
	VECTOR2 size2 = pSword->Get_Size();

	size1 = size1 / 2.f;
	size2 = size2 / 2.f;
	// A
	float leftA = pos1.x - size1.x;
	float rightA = pos1.x + size1.x;
	float topA = pos1.y - size1.y;
	float bottomA = pos1.y + size1.y;
	// B
	float leftB = pos2.x - size2.x;
	float rightB = pos2.x + size2.x;
	float topB = pos2.y - size2.y;
	float bottomB = pos2.y + size2.y;

	float fRadianA = p1->Get_RotValue();
	float fRadianB = pSword->Get_RotValue();

	VECTOR2 dir = pos1 - pos2;
	// P1의 X 축 
	VECTOR2 dir1A = VECTOR2(cosf(fRadianA), sinf(fRadianA));
	VECTOR2 dir2A = VECTOR2(cosf(fRadianA - PI * 0.5f), sinf(fRadianA - PI * 0.5f));
	// P2의 X 축 
	VECTOR2 dir1B = VECTOR2(cosf(fRadianB), sinf(fRadianB));
	VECTOR2 dir2B = VECTOR2(cosf(fRadianB - PI * 0.5f), sinf(fRadianB - PI * 0.5f));
	// len1 
	VECTOR2 len1A = dir1A * (rightA - leftA) * 0.5f;
	VECTOR2 len2A = dir2A * (bottomA - topA) * 0.5f;
	// len2
	VECTOR2 len1B = dir1B * (rightB - leftB) * 0.5f;
	VECTOR2 len2B = dir2B * (bottomB - topB) * 0.5f;

	// 1 축 검사
	float lengthA = len1A.Get_Length_Sqrt();
	float lengthB = fabsf(Vector2_Dot(dir1A, len1B)) + fabsf(Vector2_Dot(dir1A, len2B));
	float length = fabsf(Vector2_Dot(dir1A, dir));
	if (length > lengthA + lengthB)
		return false;
	// 2 축 검사
	lengthA = len2A.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir2A, len1B)) + fabsf(Vector2_Dot(dir2A, len2B));
	length = fabsf(Vector2_Dot(dir2A, dir));
	if (length > lengthA + lengthB)
		return false;
	// 3 축 검사
	lengthA = len1B.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir1B, len1A)) + fabsf(Vector2_Dot(dir1B, len2A));
	length = fabsf(Vector2_Dot(dir1B, dir));
	if (length > lengthA + lengthB)
		return false;
	// 4 축 검사
	lengthA = len2B.Get_Length_Sqrt();
	lengthB = fabsf(Vector2_Dot(dir2B, len1A)) + fabsf(Vector2_Dot(dir2B, len2A));
	length = fabsf(Vector2_Dot(dir2B, dir));
	if (length > lengthA + lengthB)
		return false;

	return true;
}

float CCollision::Vector2_Dot(const VECTOR2 & v1, const VECTOR2 & v2)
{
	float r;
	r = (v1.x * v2.x) + (v1.y * v2.y);
	return r;
}
