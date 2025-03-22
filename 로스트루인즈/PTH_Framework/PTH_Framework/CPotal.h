#pragma once
#include "CSceneActive.h"

class CPotal : public CSceneActive
{
public:
	explicit CPotal();
	explicit CPotal(const ESCAID& eScaID, const RECT& tRect);
	virtual ~CPotal();
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
	bool Is_Check_Equal_YPos(RECT tRect);
};

