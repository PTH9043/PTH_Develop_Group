#pragma once
#include "CAnimation.h"

class CAnimation_L : public CAnimation
{
public:
	explicit CAnimation_L();
	explicit CAnimation_L(const TCHAR* pAnimName);
	virtual ~CAnimation_L();
public:
	// Insert Animation
	virtual void Insert_Anim(CAnimClip*& pAnimClip) override;
	// 업데이트
	virtual int Update();
	// 렌더
	virtual void Render(HDC hDC, CObj*& pObj, const VECTOR2& v2ScrollPos);
	// Reset Animation
	virtual void Reset_Animation_Index() override { m_iAnimIndex = static_cast<int>(m_vecAnimClip.size() - 1); }

public:
};

