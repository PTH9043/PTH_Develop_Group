#include "pch.h"
#include "CDamageText.h"

CDamageText::CDamageText()
{
}

CDamageText::CDamageText(CObj * pObj, const TCHAR * szFontName, const RGBINFO & tInfo)
	: CText(pObj, szFontName, tInfo)
{
	Initialize();
}

CDamageText::~CDamageText()
{
	Release();
}

void CDamageText::Initialize()
{
	m_tTimer.dwTime = GetTickCount();
	m_tTimer.iPassTime = 3000;
	
	m_v2Size = VECTOR2(20.f, 0.f);
}

int CDamageText::Update()
{
	if (true == m_tTimer.Is_PassTime())
	{
		m_bEnable = false;
	}
	m_v2CurPos.y -= 0.1f;
//	m_v2CurPos.x = m_pOwner->Get_CurPos().x;
	return OBJ_NOEVENT;
}

void CDamageText::Late_Update()
{
}

void CDamageText::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	m_hFont = CreateFont(static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y), 0, 0,
		0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, m_szFontName);
	m_hOldFont = static_cast<HFONT>(SelectObject(_hdc, m_hFont));
	SetTextColor(_hdc, RGB(m_tRGBInfo.iR, m_tRGBInfo.iG, m_tRGBInfo.iB));
	SetBkMode(_hdc, TRANSPARENT);
	TextOut(_hdc, static_cast<int>(m_v2CurPos.x + v2ScrollPos.x), static_cast<int>(m_v2CurPos.y + v2ScrollPos.y), m_strText.c_str(),
		lstrlen(m_strText.c_str()));
	SelectObject(_hdc, m_hOldFont);
	DeleteObject(m_hFont);
}

void CDamageText::Collision_Actor(CObj *& pObj)
{
}

void CDamageText::Release()
{
	
}

void CDamageText::Obj_Active(const bool & IsActive)
{
	m_bEnable = true;
	m_tTimer.dwTime = GetTickCount();
}
