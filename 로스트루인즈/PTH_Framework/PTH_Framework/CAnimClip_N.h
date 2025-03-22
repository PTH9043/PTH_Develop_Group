#pragma once
#include"CAnimClip.h"

class CMyBmp;

class CAnimClip_N : public CAnimClip
{
public:
	explicit CAnimClip_N();
	explicit CAnimClip_N(const TCHAR* szPath);
	explicit CAnimClip_N(const POINT& tLPoint, const POINT& tRPoint, const TCHAR* szPath);
	virtual ~CAnimClip_N();
protected:
	CMyBmp* m_pClip;
public:
	virtual void Play(HDC hDC, CObj*& pObj, HDC  hDrawDC, const VECTOR2& v2ScrollPos, const int& iRed, const int& iGreen, const int& iBlue) override;
public:
	const POINT Get_StartPos() const { return m_tStartPos; }
	const POINT Get_Size() const { return m_tSize; }
};

