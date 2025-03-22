#include "pch.h"
#include "CObj.h"

CObj::CObj() : m_iID(0),
	m_v2CurPos(VECTOR2()), m_v2CenterPos(VECTOR2()),m_v2Dir(VECTOR2()),m_v2Size(VECTOR2()), m_v2DrawSize(VECTOR2()),
m_eObjID(OBJ_END), m_bEnable(false), m_strName(L""),
m_bDead(false), m_fRotValue(0.f)
{
	ZeroMemory(&m_tRect, sizeof(RECT));
	ZeroMemory(&m_tDrawRect, sizeof(RECT));
}

CObj::~CObj()
{
}

void CObj::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}


bool CObj::Check_Pos_IsInRect(const VECTOR2& v2ScrollPos, const RECT & tRect)
{
	RECT rc;
	rc = m_tDrawRect;
	rc.left += static_cast<int>(v2ScrollPos.x);
	rc.right += static_cast<int>(v2ScrollPos.x);
	rc.top += static_cast<int>(v2ScrollPos.y);
	rc.bottom += static_cast<int>(v2ScrollPos.y);

	if (tRect.left <= rc.left && tRect.right >= rc.right)
	{
		if (tRect.top <= rc.top && tRect.bottom >= rc.bottom)
		{
			return true;
		}
	}
	return false;
}

void CObj::Draw_Line(HDC _hdc, const VECTOR2 & v2ScrollPos, POINT & pt1, POINT & pt2)
{
	MoveToEx(_hdc, static_cast<int>(v2ScrollPos.x + pt1.x), static_cast<int>(v2ScrollPos.y + pt1.y), NULL);
	LineTo(_hdc, static_cast<int>(v2ScrollPos.x + pt2.x), static_cast<int>(v2ScrollPos.y + pt2.y));
}

void CObj::Draw_Line_V2(HDC _hdc, const VECTOR2 & v2ScrollPos, VECTOR2 & pt1, VECTOR2 & pt2)
{
	MoveToEx(_hdc, static_cast<int>(v2ScrollPos.x + pt1.x), static_cast<int>(v2ScrollPos.y + pt1.y), NULL);
	LineTo(_hdc, static_cast<int>(v2ScrollPos.x + pt2.x), static_cast<int>(v2ScrollPos.y + pt2.y));
}

void CObj::Rotation_Point(POINT & pt, const VECTOR2& v2Size, const float& fAngle, const float & fRotateValue, const float & fX, const float& fY)
{
	//ÁÂ »ó´Ü
	pt.x = long((v2Size.x) + cosf((fRotateValue + fAngle) * (PI / 180.f))* fX);
	pt.y = long((v2Size.y) - sinf((fRotateValue + fAngle) * (PI / 180.f))* fY);
}

void CObj::Rotation_V2(VECTOR2 & v2Point,const VECTOR2 & v2CenterPos, const float& fX, const float& fY, const float& fAngle)
{
	float r = atan2f(v2CenterPos.y - v2Point.y, v2CenterPos.x - v2Point.x) * RAD2DEG;
	v2Point.x = v2CenterPos.x + fY * cosf(DEG2RAD * (r + fAngle));
	v2Point.y = v2CenterPos.y + fY * sinf(DEG2RAD * (r + fAngle));
}