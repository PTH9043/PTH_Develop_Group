#include "pch.h"
#include "CButtonText.h"

CButtonText::CButtonText() : m_bSelected(false)
{
}

CButtonText::CButtonText(CObj* pObj, const TCHAR * szFontName, const RGBINFO & tInfo)
	: CText(pObj, szFontName, tInfo), m_bSelected(false)
{
	Initialize();
}

CButtonText::~CButtonText()
{
	Release();
}

void CButtonText::Initialize()
{
	m_tRGBClickInfo = RGBINFO();
}

int CButtonText::Update()
{
	if (true == m_bDead)
		return OBJ_DEADEVENT;

	return OBJ_NOEVENT;
}

void CButtonText::Late_Update()
{
}

void CButtonText::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (false == m_bSelected)
	{
		m_hFont = CreateFont(static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y), 0, 0,
			0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, m_szFontName);
		m_hOldFont = static_cast<HFONT>(SelectObject(_hdc, m_hFont));
		SetTextColor(_hdc, RGB(m_tRGBInfo.iR, m_tRGBInfo.iG, m_tRGBInfo.iB));
		SetBkMode(_hdc, TRANSPARENT);
	}
	else
	{
		m_hFont = CreateFont(static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y), 0, 0,
			0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, m_szFontName);
		m_hOldFont = static_cast<HFONT>(SelectObject(_hdc, m_hFont));
		SetTextColor(_hdc, RGB(m_tRGBClickInfo.iR, m_tRGBClickInfo.iG, m_tRGBClickInfo.iB));
		SetBkMode(_hdc, TRANSPARENT);
	}
	TextOut(_hdc, static_cast<int>(m_v2CurPos.x), static_cast<int>(m_v2CurPos.y), m_strText.c_str(),
		lstrlen(m_strText.c_str()));
	SelectObject(_hdc, m_hOldFont);
	DeleteObject(m_hFont);
}

void CButtonText::Collision_Actor(CObj *& pObj)
{
}

void CButtonText::Release()
{
}
