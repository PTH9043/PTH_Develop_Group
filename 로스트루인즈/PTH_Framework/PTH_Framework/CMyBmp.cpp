#include "pch.h"
#include "CMyBmp.h"

CMyBmp::CMyBmp()
{
}

CMyBmp::~CMyBmp()
{
	Release();
}

void CMyBmp::Load_Bmp(const TCHAR * pFilePath)
{
	HDC hDC = GetDC(g_hWnd);

	m_hMemDC = CreateCompatibleDC(hDC);
	m_hBmp  = static_cast<HBITMAP>(LoadImage(nullptr, pFilePath, 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	m_hOldBmp = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBmp));

	BITMAP tInfo{};
	GetObject(m_hBmp, sizeof(BITMAP), &tInfo);
	this->m_iWidth = static_cast<int>(tInfo.bmWidth);
	this->m_iHeight = static_cast<int>(tInfo.bmHeight);

	ReleaseDC(g_hWnd, hDC);
}

void CMyBmp::Release(void)
{
	static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hOldBmp));
	//불러온 오브젝트 삭제,현재 사용되고 있는 객체를 함부로 삭제하는 것을 방지
	DeleteObject(m_hBmp);
	DeleteObject(m_hOldBmp);
	//레퍼런스 카운터를 감소
	DeleteDC(m_hMemDC);
}
