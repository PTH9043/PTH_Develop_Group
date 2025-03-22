#include "pch.h"
#include "CAnimClip_N.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"
#include "CObj.h"

CAnimClip_N::CAnimClip_N() : m_pClip(nullptr)
{
}

CAnimClip_N::CAnimClip_N(const TCHAR * szPath)
{
	m_pClip = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szPath);
}

CAnimClip_N::CAnimClip_N(const POINT & tLPoint, const POINT & tRPoint, const TCHAR * szPath) :
	CAnimClip(tLPoint, tRPoint)
{
	m_pClip = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szPath);
}


CAnimClip_N::~CAnimClip_N()
{
	m_pClip = nullptr;
}

void CAnimClip_N::Play(HDC hDC, CObj *& pObj, HDC hDrawDC, const VECTOR2 & v2ScrollPos, const int & iRed, const int & iGreen, const int & iBlue)
{
	if (nullptr == pObj)
		return;
	if (nullptr == m_pClip)
		return;
	RECT rRect = pObj->Get_DrawRect();
	VECTOR2 v2Scale = pObj->Get_DrawSize(); 
	// 현재 애니메이션이 그려질 위치
	GdiTransparentBlt(hDC, static_cast<int>(rRect.left + v2ScrollPos.x),
		static_cast<int>(rRect.top + v2ScrollPos.y), static_cast<int>(v2Scale.x), static_cast<int>(v2Scale.y),
		// 그릴 애니메이션 그림의 위치
		m_pClip->Get_MemDC(), m_tStartPos.x, m_tStartPos.y, m_tSize.x, m_tSize.y, RGB(iRed, iGreen, iBlue));
}
