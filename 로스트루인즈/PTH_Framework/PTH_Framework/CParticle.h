#pragma once
#include "CEffect.h"
#include "CLine.h"

class CParticle : public CEffect
{
public:
	explicit CParticle();
	virtual ~CParticle();
protected:
	float	m_fMoveSpeed;
	float	m_fDownTime;
	float	m_fRestrictTime;
	CLine*	m_pLine;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
protected:
	// Update Rect
	virtual void Update_Rect() override;
	// Fall 
	virtual void Fall();
public:
	// get
	const float& Get_Speed() const { return m_fMoveSpeed; }
	const float& Get_DownTime() const { return m_fDownTime; }
	// set
	void Set_Speed(const float& fSpeed) { this->m_fMoveSpeed; }
	void Set_DownTime(const float& fDownTime) { this->m_fDownTime = fDownTime; }
};

