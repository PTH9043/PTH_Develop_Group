#pragma once
#include "CSceneActive.h"
#include "CMyBmp.h"

class CWall : public CSceneActive
{
public:
	explicit CWall();
	explicit CWall(const ESCAID& eScaID, const RECT& tRect, const wstring& strFileLoad);
	virtual ~CWall();
protected:
	CMyBmp*	m_pMyBmp;
	EWALLID		m_eWallID;
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
public:
	const EWALLID& Get_WallID() const { return m_eWallID; }
};

