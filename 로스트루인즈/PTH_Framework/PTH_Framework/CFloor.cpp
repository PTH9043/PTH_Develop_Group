#include "pch.h"
#include "CFloor.h"
#include "CLine.h"
#include "CCharacter.h"
#include "CGameMgr.h"
#include "CLineMgr.h"
#include "CBmpMgr.h"

CFloor::CFloor() : m_pLine(nullptr) , m_pMyBmp(nullptr)
{
	Initialize();
}

CFloor::CFloor(const ESCAID& eScaID, const RECT& tRect, const wstring& strFileLoad) : CSceneActive(eScaID, tRect), m_pLine(nullptr)
, m_pMyBmp(nullptr)
{
	m_pMyBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(strFileLoad.c_str());
	Initialize();
}

CFloor::~CFloor()
{
	Release();
}

void CFloor::Initialize()
{
	CSceneActive::Initialize();
	VECTOR2 v2LeftUpPos = VECTOR2(static_cast<float>(m_tRect.left), static_cast<float>(m_tRect.top));
	VECTOR2 v2RightUpPos = VECTOR2(static_cast<float>(m_tRect.right), static_cast<float>(m_tRect.top));
	m_pLine = new CLine(v2LeftUpPos, v2RightUpPos);
	// 보여주기 위한
	m_iR = 255;
	m_iG = 0;
	m_iB = 0;
}

bool CFloor::Collision_Obj(CObj *& pObj, const VECTOR2& v2ScrollPos)
{
	return false;
}

void CFloor::Render(HDC _hdc, const VECTOR2& v2ScrollPos, const bool& IsRender)
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

void CFloor::Release()
{
	DELETE_MEMORY(m_pLine);
}

bool CFloor::Collision_Line(VECTOR2 & v2Pos, float * pY)
{
	if (nullptr == m_pLine)
		return false;

	CLine* pTargeLine = nullptr;
	if (v2Pos.x >= m_pLine->Get_Info().v2LeftPos.x && v2Pos.x <= m_pLine->Get_Info().v2RightPos.x)
	{
		float fBig = m_pLine->Get_Info().v2LeftPos.y;
		float fSmall = m_pLine->Get_Info().v2RightPos.y;
		if (fBig < fSmall)
		{
			float temp = fBig;
			fBig = fSmall;
			fSmall = temp;
		}
		if (v2Pos.y <= fBig  && fSmall  <= v2Pos.y)
		{
			pTargeLine = m_pLine;
		}
	}
	if (nullptr == pTargeLine)
		return false;

	float		x1 = pTargeLine->Get_Info().v2LeftPos.x;
	float		x2 = pTargeLine->Get_Info().v2RightPos.x;

	float		y1 = pTargeLine->Get_Info().v2LeftPos.y;
	float		y2 = pTargeLine->Get_Info().v2RightPos.y;

	*pY = ((y2 - y1) / (x2 - x1)) * (v2Pos.x - x1) + y1;
	return true;
}
