#pragma once
#include "CText.h"

class CDamageText : public CText
{
public:
	explicit CDamageText();
	explicit CDamageText(CObj* pObj, const TCHAR* szFontName, const RGBINFO& tInfo);
	virtual ~CDamageText();
protected:
	TIMER m_tTimer;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	virtual void Obj_Active(const bool& IsActive) override;
public:
	void Set_Timer(const TIMER& tTimer) { this->m_tTimer = tTimer; }
};

