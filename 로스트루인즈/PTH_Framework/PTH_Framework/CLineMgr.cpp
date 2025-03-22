#include "pch.h"
#include "CLineMgr.h"


CLineMgr::CLineMgr() : CATCH_OBJ_VALUE(10)
{
}

CLineMgr::CLineMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr), CATCH_OBJ_VALUE(12)
{
}

CLineMgr::~CLineMgr()
{
	Release();
}

void CLineMgr::Initialize()
{
}

int CLineMgr::Update()
{
	return 0;
}

void CLineMgr::Render(HDC _hdc, const VECTOR2& v2ScrollPos)
{
	for (auto& iter : m_LineList)
		iter->Render(_hdc, v2ScrollPos);
}

void CLineMgr::Release()
{
	m_LineList.clear();
}

bool CLineMgr::Collision_Line(VECTOR2& v2Pos, float * pY)
{
	if (m_LineList.empty())
		return false;

	CLine* pTargeLine = nullptr;
	for (auto& iter : m_LineList)
		if (v2Pos.x >= iter->Get_Info().v2LeftPos.x && v2Pos.x <= iter->Get_Info().v2RightPos.x)
		{
			float fBig = iter->Get_Info().v2LeftPos.y;
			float fSmall = iter->Get_Info().v2RightPos.y;
			if (fBig < fSmall)
			{
				float temp = fBig;
				fBig = fSmall;
				fSmall = temp;
			}

			if (v2Pos.y <= fBig + CATCH_OBJ_VALUE && fSmall - CATCH_OBJ_VALUE <= v2Pos.y)
			{
				pTargeLine = iter;
				break;
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

CLine * CLineMgr::Get_Collision_Line(VECTOR2 & v2Pos, float * pY)
{
	if (m_LineList.empty())
		return nullptr;

	CLine* pTargeLine = nullptr;
	for (auto& iter : m_LineList)
		if (v2Pos.x >= iter->Get_Info().v2LeftPos.x && v2Pos.x <= iter->Get_Info().v2RightPos.x)
		{
			float fBig = iter->Get_Info().v2LeftPos.y;
			float fSmall = iter->Get_Info().v2RightPos.y;
			if (fBig < fSmall)
			{
				float temp = fBig;
				fBig = fSmall;
				fSmall = temp;
			}

			if (v2Pos.y <= fBig + CATCH_OBJ_VALUE && fSmall - CATCH_OBJ_VALUE <= v2Pos.y)
			{
				pTargeLine = iter;
				break;
			}
		}
	if (nullptr == pTargeLine)
		return nullptr;

	float		x1 = pTargeLine->Get_Info().v2LeftPos.x;
	float		x2 = pTargeLine->Get_Info().v2RightPos.x;

	float		y1 = pTargeLine->Get_Info().v2LeftPos.y;
	float		y2 = pTargeLine->Get_Info().v2RightPos.y;

	*pY = ((y2 - y1) / (x2 - x1)) * (v2Pos.x - x1) + y1;
	return pTargeLine;
}
