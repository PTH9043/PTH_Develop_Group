#include "pch.h"
#include "CEffect.h"
#include "CGameMgr.h"

CEffect::CEffect() : m_tLifeTimer(TIMER(1000)), m_pDrawBmp(nullptr), m_pObj(nullptr)
{
	Initialize();
}

CEffect::~CEffect()
{
	Release();
}

void CEffect::Initialize()
{
}

int CEffect::Update()
{
	return OBJ_NOEVENT;
}

void CEffect::Late_Update()
{
}

void CEffect::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{

}

void CEffect::Collision_Actor(CObj *& pObj)
{
}

void CEffect::Release()
{

}

void CEffect::Update_Rect()
{

}
