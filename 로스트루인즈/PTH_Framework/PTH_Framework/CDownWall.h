#pragma once
#include "CWall.h"
#include "CMyBmp.h"

class CDownWall : public CWall
{
public:
	explicit CDownWall();
	explicit CDownWall(const ESCAID& eScaID, const RECT& tRect, const wstring& strFileLoad);
	virtual ~CDownWall();
protected:
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

};
