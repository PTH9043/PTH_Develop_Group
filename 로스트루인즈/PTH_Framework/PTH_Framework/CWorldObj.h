#pragma once
#include "CObj.h"
#include "CMyBmp.h"

class CWorldObj abstract : public CObj
{
protected:
	explicit CWorldObj();
public:
	virtual ~CWorldObj();
protected:
	CMyBmp*			m_pImgBmp;
public:
	// 
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj*& pObj) PURE;
	virtual void Release() PURE;
protected:
	virtual void Fall();

	void Draw_Img_GDI(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const RGBINFO& tInfo);
	void Draw_Img_Blt(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp);
	void Draw_Img_Str(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const DWORD& dwValue);
	void Draw_Img_Alpha(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const int& iAlphaValue);
};

