#pragma once
#include "Include.h"

class CObj;

class CLine
{
public:
	explicit CLine();
	explicit CLine(const VECTOR2& v2LeftPos, const VECTOR2& v2RightPos);
	virtual ~CLine();
protected:
	LINEINFO m_tLineInfo;
	const int ZERO_INCLINATION;
public:
	CLine* Check_Line_In_Character(CObj*& _pActor);
	// Render
	void Render(HDC _hdc, const VECTOR2& v2ScreenPos);
	// Check Y Pos
	float Ground_Y_Pos(CObj*& _pActor);
public:
	const LINEINFO& Get_Info() const { return m_tLineInfo; }
	// Set Line Info
	void Set_Info(const LINEINFO& _tLineInfo) { this->m_tLineInfo = _tLineInfo; }
};

