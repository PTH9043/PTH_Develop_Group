#pragma once
#include "CBar.h"

class CHpBar : public CBar
{
public:
	explicit CHpBar();
	explicit CHpBar(CCharacter* pOwner, const TCHAR* szMainPath,
		const TCHAR* szBgPath, const RGBINFO& tInfo, const int& iBarValue);
	virtual ~CHpBar();

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
	// Calculate Bar
	virtual void Calculate_Bar() override;
};

