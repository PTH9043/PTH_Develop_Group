#include "pch.h"
#include "CFollowText.h"

CFollowText::CFollowText()
{
	Initialize();
}

CFollowText::CFollowText(CObj * pObj, const TCHAR * szFontName, const RGBINFO & tInfo)
	: CText(pObj, szFontName, tInfo)
{
	Initialize();
}

CFollowText::~CFollowText()
{
	Release();
}

void CFollowText::Initialize()
{
}

int CFollowText::Update()
{
	if(true == m_bDead)
	return OBJ_DEADEVENT;

	return OBJ_NOEVENT;
}

void CFollowText::Late_Update()
{
}

void CFollowText::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
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

void CFollowText::Collision_Actor(CObj *& pObj)
{
}

void CFollowText::Release()
{

}
