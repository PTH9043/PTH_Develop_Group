#include "pch.h"
#include "CSpawner.h"

CSpawner::CSpawner()
	: m_pOwner(nullptr),  m_v2CurPos(VECTOR2())
{
	ZeroMemory(&m_tRect, sizeof(RECT));
}

CSpawner::CSpawner(CScene * pOwner, const RECT & tRect, const VECTOR2 & v2Pos)
	: m_pOwner(pOwner), m_tRect(tRect), m_v2CurPos(v2Pos)
{
	Initialize();
}

CSpawner::~CSpawner()
{
	Release();
}

void CSpawner::Initialize()
{
}

void CSpawner::Spawn()
{
}

void CSpawner::Release()
{
	m_pOwner = nullptr;;
}
