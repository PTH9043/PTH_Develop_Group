#pragma once
#include "CWorldObj.h"

class CDoor : public CWorldObj
{
public:
	explicit CDoor();
	virtual ~CDoor();
protected:
	CAnimator*				m_pAnimator;
	bool					m_bUseKey;
	int						m_iState;
public:

	// CWorldObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

protected:
	virtual void Update_Rect() override;
	// Animator
	void Animatior();
};

