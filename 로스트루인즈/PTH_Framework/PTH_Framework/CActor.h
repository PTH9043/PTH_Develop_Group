#pragma once
#include "CObj.h"

class CActor abstract : public CObj
{
public:
	explicit CActor();
	virtual ~CActor();

protected:
	// Animator
	CAnimator*		m_pAnimator;
	// 방향
	EDIR						m_eDirection;
	// 떨어지는 시간
	float						m_fDownTime;
	// Name 
	wstring				m_strName;
	// Jump Power 
	float						m_fJumpPower;
	// 점프하기 위한 중력 값
	float						m_fRestrictTime;
	// Gold
	int						m_iGold;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
protected:
public:
	// Get Gold
	const EDIR& Get_Direction() const { return m_eDirection; }
	const int& Get_Gold() const { return m_iGold; }
	// Gold
	void Set_Direction(const EDIR& eDirection) { this->m_eDirection = eDirection; }
	void Set_Gold(const int& iGold) { this->m_iGold = iGold; }
};

