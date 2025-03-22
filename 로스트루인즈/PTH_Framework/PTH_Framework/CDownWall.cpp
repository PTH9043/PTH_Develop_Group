#include "pch.h"
#include "CDownWall.h"
#include "CCharacter.h"
#include "CGameMgr.h"
#include "CLineMgr.h"
#include "CBmpMgr.h"

CDownWall::CDownWall()
{
}

CDownWall::CDownWall(const ESCAID & eScaID, const RECT & tRect, const wstring & strFileLoad) : CWall(eScaID, tRect, strFileLoad)
{
	Initialize();
}

CDownWall::~CDownWall()
{
	Release();
}

void CDownWall::Initialize()
{
	// 보여주기 위한
	m_iR = 0;
	m_iG = 255;
	m_iB = 0;
	m_eWallID = WALL_DW;
}

bool CDownWall::Collision_Obj(CObj *& pObj, const VECTOR2 & v2ScrollPos)
{
	return CWall::Collision_Obj(pObj, v2ScrollPos);
}

void CDownWall::Render(HDC _hdc, const VECTOR2 & v2ScrollPos, const bool & IsRender)
{
	CWall::Render(_hdc, v2ScrollPos, IsRender);
}

void CDownWall::Release()
{
}
