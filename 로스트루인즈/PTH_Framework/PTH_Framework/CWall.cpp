#include "pch.h"
#include "CWall.h"
#include "CCharacter.h"
#include "CGameMgr.h"
#include "CLineMgr.h"
#include "CBmpMgr.h"

CWall::CWall() : m_pMyBmp(nullptr)
{
	Initialize();
}

CWall::CWall(const ESCAID& eScaID, const RECT& tRect, const wstring& strFileLoad) : CSceneActive(eScaID, tRect), m_pMyBmp(nullptr)
, m_eWallID(WALL_LR)
{
	m_pMyBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(strFileLoad.c_str());
	Initialize();
}

CWall::~CWall()
{
	Release();
}

void CWall::Initialize()
{
	CSceneActive::Initialize();
	// 보여주기 위한
	m_iR = 0;
	m_iG = 255;
	m_iB = 0;
}

bool CWall::Collision_Obj(CObj *& pObj, const VECTOR2& v2ScrollPos)
{
	if (nullptr == pObj)
		return false;
	RECT rc;
	if (OBJ_PLAYER == pObj->Get_ObjID())
	{
		CCharacter* pCharacter = static_cast<CCharacter*>(pObj);
		CWall* pWall = nullptr;
		if (FALSE == IntersectRect(&rc, &pCharacter->Get_Rect(), &m_tRect))
		{
			if (true == m_bCollision)
			{
				m_bCollision = false;
				pCharacter->Set_CollWall(pWall);
			}
			return false;
		}
		m_bCollision = true;
		pWall = this;
		pCharacter->Set_CollWall(pWall);
		return false;
	}
	else if (OBJ_MONSTER == pObj->Get_ObjID())
	{
		CWall* pWall = nullptr;
		CCharacter* pCharacter = static_cast<CCharacter*>(pObj);
		if (FALSE == IntersectRect(&rc, &pCharacter->Get_Rect(), &m_tRect))
		{
			if (true == m_bCollision)
			{
				pCharacter->Set_CollWall(pWall);
			}
			return false;
		}
		pWall = this;
		pCharacter->Set_CollWall(pWall);
	}
	return false;
}

void CWall::Render(HDC _hdc, const VECTOR2 & v2ScrollPos, const bool& IsRender)
{
	if (false == Is_Not_Render(v2ScrollPos))
		return;

	RECT rRect = m_tRect;
	// 현재 애니메이션이 그려질 위치
	GdiTransparentBlt(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x),
		static_cast<int>(m_tRect.top + v2ScrollPos.y), static_cast<int>(m_pMyBmp->Get_Width()), static_cast<int>(m_pMyBmp->Get_Height()),
		// 그릴 애니메이션 그림의 위치
		m_pMyBmp->Get_MemDC(), 0, 0, m_pMyBmp->Get_Width(), m_pMyBmp->Get_Height(), RGB(0, 0, 0));

	CSceneActive::Render(_hdc, v2ScrollPos, IsRender);
}

void CWall::Release()
{
}
