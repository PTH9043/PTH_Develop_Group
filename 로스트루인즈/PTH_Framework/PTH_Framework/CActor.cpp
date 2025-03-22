#include "pch.h"
#include "CActor.h"


CActor::CActor() : m_pAnimator(nullptr), m_eDirection(DIR_END), m_fDownTime(0.f)
, m_fJumpPower(100.f), m_fRestrictTime(0.02f), m_iGold(0)
{
}


CActor::~CActor()
{
}

void CActor::Initialize()
{
}

int CActor::Update()
{
	return 0;
}

void CActor::Late_Update()
{
}

void CActor::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
}

void CActor::Collision_Actor(CObj *& pObj)
{
}

void CActor::Release()
{
}
