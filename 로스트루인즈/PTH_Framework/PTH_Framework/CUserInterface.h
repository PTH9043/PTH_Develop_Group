#pragma once
#include "CObj.h"

class CMyBmp;

class CUserInterface abstract : public CObj
{
public:
	explicit CUserInterface();
	virtual ~CUserInterface();
protected:
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

protected:
	virtual void Update_Rect() override;

	void Draw_Img_GDI(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const RGBINFO& tInfo);
	void Draw_Img_Blt(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp);
	void Draw_Img_Str(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const DWORD& dwValue);
	void Draw_Img_Alpha(HDC _hdc, const VECTOR2& v2DrawPos, const VECTOR2& v2DrawSize, CMyBmp*& pMyBmp, const int& iAlphaValue);
};

