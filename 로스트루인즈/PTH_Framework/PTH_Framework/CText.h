#pragma once
#include "CUserInterface.h"

class CText : public CUserInterface
{
public:
	explicit CText();
	explicit CText(CObj* pObj, const TCHAR* szFontName, const RGBINFO& tInfo);
	virtual ~CText();
protected:
	// 현재 주인
	CObj*						m_pOwner;
	RGBINFO					m_tRGBInfo;
	const TCHAR*			m_szFontName;
	wstring					m_strText;
	HFONT						m_hFont;
	HFONT						m_hOldFont;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
public:
	// Set 
	void Set_Text(wstring& text) { m_strText = text; }
};

