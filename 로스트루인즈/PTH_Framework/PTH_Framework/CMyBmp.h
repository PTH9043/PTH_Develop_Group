#pragma once
#include "Include.h"

class CMyBmp
{
public:
	explicit CMyBmp();
	virtual ~CMyBmp();

protected:
	HDC m_hMemDC;
	HBITMAP m_hBmp;
	HBITMAP m_hOldBmp;

	int m_iWidth;
	int m_iHeight;
public:
	void Load_Bmp(const TCHAR* pFilePath);
	void Release(void);

public:
	const HDC& Get_MemDC() const { return m_hMemDC; }
	const int Get_Width() const { return m_iWidth; }
	const int Get_Height() const { return m_iHeight; }
};

