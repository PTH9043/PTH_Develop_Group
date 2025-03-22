#include "pch.h"
#include "CAnimClip.h"
#include "CObj.h"
#include "CMyBmp.h"
#include "CScrollMgr.h"

CAnimClip::CAnimClip()
{
}

CAnimClip::CAnimClip(const POINT& tStartPoint, const POINT& tSize) : m_tStartPos(tStartPoint), m_tSize(tSize)
{
}

CAnimClip::~CAnimClip()
{

}

void CAnimClip::Play(HDC hDC, CObj*& pObj, HDC  hDrawDC, const VECTOR2& v2ScrollPos, const int& iRed, const int& iGreen, const int& iBlue)
{
	if (nullptr == pObj)
		return;
	RECT rRect = pObj->Get_DrawRect();
	VECTOR2 v2Scale = pObj->Get_DrawSize();
	// 현재 애니메이션이 그려질 위치
	GdiTransparentBlt(hDC, static_cast<int>(rRect.left + v2ScrollPos.x),
		static_cast<int>(rRect.top + v2ScrollPos.y), static_cast<int>(v2Scale.x), static_cast<int>(v2Scale.y),
		// 그릴 애니메이션 그림의 위치
		hDrawDC, m_tStartPos.x, m_tStartPos.y, m_tSize.x, m_tSize.y, RGB(iRed, iGreen, iBlue));
}
