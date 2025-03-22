#pragma once
#include "CText.h"

class CButtonText : public CText
{
public:
	explicit CButtonText();
	explicit CButtonText(CObj* pObj, const TCHAR* szFontName, const RGBINFO& tInfo);
	virtual ~CButtonText();
protected:
	RGBINFO			m_tRGBClickInfo;
	bool					m_bSelected;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
public:
	const bool& Is_Selected() const { return m_bSelected; }

	void Set_Selected(const bool& IsSelected) { this->m_bSelected = IsSelected; };

	void Set_ClickInfo(const RGBINFO& tRgbInfo) { this->m_tRGBClickInfo = tRgbInfo; }
};


