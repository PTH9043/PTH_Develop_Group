#include "pch.h"
#include "CItem.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"

CItem::CItem() : m_pOwner(nullptr), m_pImgBmp(nullptr), m_eItemID(ITEM_END)
, m_bItemPicked(false), m_bUse(false), m_bCollision(false), m_fDiagonal(0.f), m_szImgPath(nullptr)

{
}

CItem::CItem(CObj* pOwner, const TCHAR * szImgPath, const wstring& strName)
	: m_pOwner(pOwner), m_szImgPath(szImgPath),
	m_pImgBmp(nullptr), m_eItemID(ITEM_END), m_bItemPicked(false), m_bUse(false)
	, m_bCollision(false), m_fDiagonal(0.f)
{
	m_pImgBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szImgPath);
	m_strName = strName;
	m_eObjID = OBJ_ITEM;
	m_tRestrictDraw = CGameMgr::Get_Instance()->Get_ClientRect();
	m_tRestrictDraw.left -= 100;
	m_tRestrictDraw.top -= 100;
	m_tRestrictDraw.right += 100;
	m_tRestrictDraw.bottom += 100;
}

CItem::~CItem()
{
	m_pOwner = nullptr;
	m_pImgBmp = nullptr;
}

void CItem::Fall()
{

	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2CurPos, &fY);
	if (false == bIsColl)
	{
		//자유낙하 공식	
		m_v2CurPos.y -= 50 * -9.8f * m_fDownTime;
		m_fDownTime += 0.001f;

		// 만약 충돌 상태이고 점프하지 않은 상태라면
		if (true == bIsColl)
		{
			// 그라운드를 true로 변경
			// 현재 Y값을 고정시킨다. 
			if (m_v2CurPos.y >= fY)
			{
				m_v2CurPos.y = fY;
			}
		}

		// 일정 시간 지날 때 마다 타이머 초기화
		if (m_fDownTime > m_fRestrictTime)
			m_fDownTime = m_fRestrictTime;
	}
}

bool CItem::Check_Pos_IsInRect(const VECTOR2 & v2ScrollPos, const RECT & tRect)
{
	RECT rc;
	rc = m_tRect;
	rc.left += static_cast<int>(v2ScrollPos.x);
	rc.right += static_cast<int>(v2ScrollPos.x);
	rc.top += static_cast<int>(v2ScrollPos.y);
	rc.bottom += static_cast<int>(v2ScrollPos.y);

	if (tRect.left <= rc.left && tRect.right >= rc.right)
	{
		if (tRect.top <= rc.top && tRect.bottom >= rc.bottom)
		{
			return true;
		}
	}
	return false;
}