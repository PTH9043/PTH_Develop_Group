#include "pch.h"
#include "CCharacter.h"
#include "CAnimator.h"


CCharacter::CCharacter() : m_pCollWall(nullptr),
	m_eCharState(ECHARSTATE(CHAR_END)), m_tStatus(STATUS())
	, m_bJump(false), m_bIsGround(false), m_v2LeftPos(VECTOR2()), m_v2RightPos(VECTOR2())
, m_eCollDirection(DIR_END), m_eSaveDirection(DIR_END), m_bDamaged(false)
, m_bActiveUI(false), m_bCollisionWall(false), m_bCollisionDoor(false)

{
	ZeroMemory(&m_tAttackRect, sizeof(RECT));
}


CCharacter::~CCharacter()
{
}



void CCharacter::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	m_v2LeftPos = VECTOR2(m_v2CurPos.x - v2RealSize.x, m_v2CurPos.y);
	m_v2RightPos = VECTOR2(m_v2CurPos.x + v2RealSize.x, m_v2CurPos.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}
