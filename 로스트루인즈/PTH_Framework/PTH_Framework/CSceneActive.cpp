#include "pch.h"
#include "CSceneActive.h"
#include "CGameMgr.h"

CSceneActive::CSceneActive() : m_eSceneActiveID(SCA_END), m_bCollision(false), m_iR(0), m_iG(0), m_iB(0)
{
	ZeroMemory(&m_tRect, sizeof(RECT));
	ZeroMemory(&m_tDrawRect, sizeof(RECT));
}

CSceneActive::CSceneActive(const ESCAID& eScaID, const RECT & tRect)
	: m_eSceneActiveID(eScaID), m_tRect(tRect), m_bCollision(false), m_iR(0), m_iG(0), m_iB(0)
{
	m_tDrawRect = CGameMgr::Get_Instance()->Get_ClientRect();
	int iValue = 300;
	m_tDrawRect.left -= iValue;
	m_tDrawRect.right += iValue;
	m_tDrawRect.top -= iValue;
	m_tDrawRect.bottom += iValue;
}

CSceneActive::~CSceneActive()
{
}

void CSceneActive::Initialize()
{
	m_vecPosList.reserve(4);
	m_v2CenterPos = VECTOR2(static_cast<float>(((m_tRect.right - m_tRect.left) / 2.f) + m_tRect.left),
		static_cast<float>(((m_tRect.bottom - m_tRect.top) / 2.f) + m_tRect.top));
	// Left Up
	int iValue = 0;
	m_vecPosList.emplace_back(VECTOR2(static_cast<float>(m_tRect.left - iValue), static_cast<float>(m_tRect.top - iValue)));
	m_vecPosList.emplace_back(VECTOR2(static_cast<float>(m_tRect.right + iValue), static_cast<float>(m_tRect.top - iValue)));
	m_vecPosList.emplace_back(VECTOR2(static_cast<float>(m_tRect.left - iValue), static_cast<float>(m_tRect.bottom + iValue)));
	m_vecPosList.emplace_back(VECTOR2(static_cast<float>(m_tRect.right + iValue), static_cast<float>(m_tRect.bottom + iValue)));
}

void CSceneActive::Render(HDC _hdc, const VECTOR2& v2ScrollPos, const bool& IsRender)
{
	if (false == IsRender)
		return;

	if (false == Is_Not_Render(v2ScrollPos))
		return;

	HBRUSH hBrush = CreateSolidBrush(RGB(m_iR, m_iG, m_iB)); // GDI: 브러시 만들기
	HBRUSH oldBrush = (HBRUSH)SelectObject(_hdc, hBrush); // 새로운 브러시 선택하기
	Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
		static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));
	SelectObject(_hdc, oldBrush); // 이전의 브러시로 돌아가기
	DeleteObject(hBrush);
}

bool CSceneActive::Is_Not_Render(const VECTOR2 & v2ScrollPos)
{
	for (auto& iter : m_vecPosList)
	{
		if (true == Check_Pos_IsInRect(iter + v2ScrollPos, m_tDrawRect))
		{
			return true;
		}
	}
	return false;
}

bool CSceneActive::Check_Pos_IsInRect(const VECTOR2 & v2Pos, const RECT & tRect)
{
	if (tRect.left <= v2Pos.x && tRect.right >= v2Pos.x)
	{
		if (tRect.top <= v2Pos.y && tRect.bottom >= v2Pos.y)
		{
			return true;
		}
	}

	return false;
}
