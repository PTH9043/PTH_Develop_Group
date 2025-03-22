#pragma once
#include"CAnimation.h"

class CAnimClip
{
public:
	explicit CAnimClip();
	explicit CAnimClip(const POINT& tLPoint, const POINT& tRPoint);
	virtual ~CAnimClip();
protected:
	// 애니메이션
	CAnimation* m_pAnimation;
	// Anim Pos
	POINT m_tStartPos;
	POINT m_tSize;
public:
	virtual void Play(HDC hDC, CObj*& pObj, HDC  hDrawDC, const VECTOR2& v2ScrollPos, const int& iRed, const int& iGreen, const int& iBlue);
public:
	const POINT Get_StartPos() const { return m_tStartPos; }
	const POINT Get_Size() const { return m_tSize; }
};

