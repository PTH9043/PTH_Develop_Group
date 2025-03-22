#pragma once
#include "CSceneActive.h"
#include "CMyBmp.h"

class CLine;

class CFloor final : public CSceneActive
{
public:
	explicit CFloor();
	explicit CFloor(const ESCAID& eScaID, const RECT& tRect, const wstring& strFileLoad);
	virtual ~CFloor();
protected:
	CLine*			m_pLine;
	CMyBmp*	m_pMyBmp;
public:
	// Init
	virtual void Initialize() override;
	// Collision Obj
	virtual bool Collision_Obj(CObj*& pObj, const VECTOR2& v2ScrollPos) override;
	// Render
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos, const bool& IsRender) override;
	// Release
	virtual void Release() override;
protected:
	bool	 Collision_Line(VECTOR2& v2Pos, float* pY);
public:
	CLine* Get_Line() { return m_pLine; }
};

