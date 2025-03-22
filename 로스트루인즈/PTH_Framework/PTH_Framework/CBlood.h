#pragma once
#include "CEffect.h"

class CBlood : public CEffect
{
public:
	explicit CBlood();
	virtual ~CBlood();
protected:
	const int BLOOD1_L = 0;
	const int BLOOD1_R = 1;
	const int BLOOD2_L = 2;
	const int BLOOD2_R = 3;
	vector<CMyBmp*> m_vecBmpList;
public:
	// CEffect을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	virtual void Obj_Active(const bool& IsActive) override;
protected:
	virtual void Update_Rect() override;

public:
	void Set_BloodBmp(const int& iIndex);
};

