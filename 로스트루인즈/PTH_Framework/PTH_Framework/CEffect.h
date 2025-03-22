#pragma once
#include "CObj.h"
#include "CMyBmp.h"

class CEffect abstract : public CObj
{
public:
	explicit CEffect();
	virtual ~CEffect();
protected:
	CObj*						m_pObj;
	TIMER						m_tLifeTimer;
	CMyBmp*				m_pDrawBmp;
	float							m_fDiagonal;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj *& pObj) PURE;
	virtual void Release() PURE;

	virtual void Obj_Active(const bool& IsActive) override { m_bEnable = IsActive; m_tLifeTimer.dwTime = GetTickCount();  }
protected:
	virtual void Update_Rect() override;
public:
	void Set_Obj(CObj* pObj) { m_pObj = pObj; }
};

