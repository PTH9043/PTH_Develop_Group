#include "pch.h"
#include "CLine.h"
#include "CObj.h"

CLine::CLine() : ZERO_INCLINATION(0), m_tLineInfo(LINEINFO())
{
}

CLine::CLine(const VECTOR2& v2LeftPos, const VECTOR2& v2RightPos)
	: ZERO_INCLINATION(0), m_tLineInfo(LINEINFO())
{
	m_tLineInfo.v2LeftPos = v2LeftPos;
	m_tLineInfo.v2RightPos = v2RightPos;
	VECTOR2 v2Dir = m_tLineInfo.v2RightPos - m_tLineInfo.v2LeftPos;
	if (v2Dir.x == 0 || v2Dir.y == 0)
		m_tLineInfo.fInclination = 0.f;
	else
		m_tLineInfo.fInclination = v2Dir.y / v2Dir.x;
}

CLine::~CLine()
{
}

CLine * CLine::Check_Line_In_Character(CObj *& _pActor)
{
	if (nullptr == _pActor)
		return nullptr;
	// 현재 위치
	VECTOR2 v2Pos = _pActor->Get_CurPos();

	// left pos
	if (m_tLineInfo.v2LeftPos.x <= v2Pos.x && m_tLineInfo.v2RightPos.x >= v2Pos.x)
	{
		return this;
	}
	return nullptr;
}

void CLine::Render(HDC _hdc, const VECTOR2& v2ScrollPos)
{
	MoveToEx(_hdc, static_cast<int>(m_tLineInfo.v2LeftPos.x), static_cast<int>(m_tLineInfo.v2LeftPos.y), NULL);
	LineTo(_hdc, static_cast<int>(m_tLineInfo.v2RightPos.x), static_cast<int>(m_tLineInfo.v2RightPos.y));
}

float CLine::Ground_Y_Pos(CObj *& _pActor)
{
	if (nullptr == _pActor)
		return 0.0f;
	// 현재 위치
	VECTOR2 v2Pos = _pActor->Get_CurPos();
	float yPos = 0.f;
	// 기울기 0 인지 확인
	if (m_tLineInfo.fInclination != 0.f)
	{
		VECTOR2 v2LeftDir = v2Pos - m_tLineInfo.v2LeftPos;
		yPos = m_tLineInfo.fInclination * v2LeftDir.x;
	}
	return m_tLineInfo.v2LeftPos.y + yPos;
}
