#include "pch.h"
#include "CUserInterface.h"
#include "CMyBmp.h"

CUserInterface::CUserInterface()
{
	Initialize();
}


CUserInterface::~CUserInterface()
{
	Release();
}

void CUserInterface::Initialize()
{
	m_eObjID = OBJ_UI;
}

int CUserInterface::Update()
{
	Update_Rect();
	return OBJ_NOEVENT;
}

void CUserInterface::Late_Update()
{
}

void CUserInterface::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{

}

void CUserInterface::Collision_Actor(CObj *& pObj)
{
}

void CUserInterface::Release()
{
}

void CUserInterface::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + m_v2Size.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y + m_v2Size.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + m_v2DrawSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y + m_v2DrawSize.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x - v2RealSize.y, m_v2CurPos.y - v2RealSize.y);
}

void CUserInterface::Draw_Img_GDI(HDC _hdc, const VECTOR2& v2DrawPos,
	const VECTOR2& v2DrawSize, CMyBmp *& pMyBmp, const RGBINFO& tInfo)
{
	GdiTransparentBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, pMyBmp->Get_Width(), pMyBmp->Get_Height(),
		RGB(tInfo.iR, tInfo.iG, tInfo.iB));
}

void CUserInterface::Draw_Img_Blt(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp)
{
	BitBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, SRCCOPY);
}

void CUserInterface::Draw_Img_Str(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp, const DWORD& dwValue)
{
	StretchBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, pMyBmp->Get_Width(), pMyBmp->Get_Height(),
		dwValue);
}

void CUserInterface::Draw_Img_Alpha(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp,  const int& iAlphaValue)
{
	BLENDFUNCTION _bf;
	_bf.BlendOp = 0;
	_bf.BlendFlags = 0;
	_bf.SourceConstantAlpha = iAlphaValue;
	_bf.AlphaFormat = AC_SRC_OVER;

	GdiAlphaBlend(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, pMyBmp->Get_Width(), pMyBmp->Get_Height(),
		_bf);
}
