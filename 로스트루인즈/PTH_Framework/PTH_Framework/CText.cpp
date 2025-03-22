#include "pch.h"
#include "CText.h"

CText::CText() : m_pOwner(nullptr), m_szFontName(nullptr), m_tRGBInfo(tagRGB())
, m_strText(L"") 
{
	Initialize();
}

CText::CText(CObj * pObj, const TCHAR * szFontName, const RGBINFO& tInfo)
	: m_pOwner(pObj), m_szFontName(szFontName), m_tRGBInfo(tInfo), m_strText(L"")
{
	Initialize();
}

CText::~CText()
{
	Release();
}

void CText::Initialize()
{
	m_v2Size = VECTOR2(100.f, 0.f);
	m_strText = L"dfafdasfasfsa";
}

int CText::Update()
{
	return 0;
}

void CText::Late_Update()
{
}

void CText::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	m_hFont = CreateFont(static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y), 0, 0,
		0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, m_szFontName);
	m_hOldFont = static_cast<HFONT>(SelectObject(_hdc, m_hFont));
	SetTextColor(_hdc, RGB(m_tRGBInfo.iR, m_tRGBInfo.iG, m_tRGBInfo.iB));
	SetBkMode(_hdc, TRANSPARENT);
	TextOut(_hdc, static_cast<int>(m_v2CurPos.x), static_cast<int>(m_v2CurPos.y), m_strText.c_str(), 
		lstrlen(m_strText.c_str()));
	SelectObject(_hdc, m_hOldFont);
	DeleteObject(m_hFont);
}

void CText::Collision_Actor(CObj *& pObj)
{
}

void CText::Release()
{
	DeleteObject(m_hFont);
	DeleteObject(m_hOldFont);
	m_hFont = NULL;
	m_hOldFont = NULL;
	m_pOwner = nullptr;
	m_szFontName = nullptr;
}
