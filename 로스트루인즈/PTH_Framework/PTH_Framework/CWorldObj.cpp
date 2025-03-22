#include "pch.h"
#include "CWorldObj.h"
#include "CGameMgr.h"

CWorldObj::CWorldObj() : m_pImgBmp(nullptr)
{
	m_eObjID = OBJ_WORLD_2;
}

CWorldObj::~CWorldObj()
{
	m_pImgBmp = nullptr;
}

void CWorldObj::Fall()
{
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2CurPos, &fY);
	if (false == bIsColl)
	{
		//자유낙하 공식	
		m_v2CurPos.y -= 200 * -9.8f * 0.008f;
	}
	else
	{
		// 그라운드를 true로 변경
		// 현재 Y값을 고정시킨다. 
		m_v2CurPos.y = fY;
	}
}


void CWorldObj::Draw_Img_GDI(HDC _hdc, const VECTOR2& v2DrawPos,
	const VECTOR2& v2DrawSize, CMyBmp *& pMyBmp, const RGBINFO& tInfo)
{
	GdiTransparentBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, pMyBmp->Get_Width(), pMyBmp->Get_Height(),
		RGB(tInfo.iR, tInfo.iG, tInfo.iB));
}

void CWorldObj::Draw_Img_Blt(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp)
{
	BitBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, SRCCOPY);
}

void CWorldObj::Draw_Img_Str(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp, const DWORD& dwValue)
{
	StretchBlt(_hdc, static_cast<int>(v2DrawPos.x), static_cast<int>(v2DrawPos.y),
		static_cast<int>(v2DrawSize.x), static_cast<int>(v2DrawSize.y),
		pMyBmp->Get_MemDC(),
		0, 0, pMyBmp->Get_Width(), pMyBmp->Get_Height(),
		dwValue);
}

void CWorldObj::Draw_Img_Alpha(HDC _hdc, const VECTOR2 & v2DrawPos, const VECTOR2 & v2DrawSize, CMyBmp *& pMyBmp, const int& iAlphaValue)
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
