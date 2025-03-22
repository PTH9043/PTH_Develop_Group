#include "pch.h"
#include "CActionSlot.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"
#include "CPlayer.h"

CActionSlot::CActionSlot() : m_pWearableEquipBmp(nullptr), m_pBackImg(nullptr)
{

}

CActionSlot::CActionSlot(CObj * pOwner, const TCHAR * szSlotPath, const TCHAR * szBackImg, const RGBINFO & tInfo
	, const int& iIndex)
	: CSlot(pOwner, szSlotPath, tInfo), m_pWearableEquipBmp(nullptr), m_pBackImg(nullptr)
{
	m_pBackImg = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szBackImg);
	m_iIndex = iIndex;
	Initialize();
}

CActionSlot::~CActionSlot()
{
	Release();
}

void CActionSlot::Initialize()
{
	if (nullptr == m_pOwner)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_CHARCTER_ERROR", MB_OK);
		return;
	}

	m_v2DrawSize = VECTOR2(60.f, 60.f);
	Update_Rect();
}

int CActionSlot::Update()
{
	Check_Player_EquipItem();
	return OBJ_NOEVENT;
}

void CActionSlot::Late_Update()
{
}

void CActionSlot::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Draw_Img_GDI(_hdc, m_v2CurPos, m_v2DrawSize, m_pBackImg, m_tTranspercyRGB);
	Draw_Img_GDI(_hdc, m_v2CurPos - VECTOR2(1.f, 1.f), m_v2DrawSize + VECTOR2(1.f, 1.f), m_pSlotImg, m_tTranspercyRGB);
	/*  추가해야함.... */
	if (nullptr != m_pWearableEquipBmp)
	{
		VECTOR2 v2ControlSize = VECTOR2(3.f, 3.f);
		Draw_Img_GDI(_hdc, m_v2CurPos + v2ControlSize, m_v2DrawSize - v2ControlSize, m_pWearableEquipBmp
		, m_tTranspercyRGB);
	}
}

void CActionSlot::Collision_Actor(CObj *& pObj)
{
}

void CActionSlot::Release()
{
	m_pBackImg = nullptr;
}

void CActionSlot::Check_Player_EquipItem()
{
	if (nullptr != m_pOwner)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pOwner);
		if (nullptr != pPlayer)
		{
			CEquip* pItem = pPlayer->Get_Wearable_Equip(m_iIndex);
			if (nullptr != pItem)
			{
				m_pWearableEquipBmp = pItem->Get_Img();
			}
			else
				m_pWearableEquipBmp = nullptr;
		}
	}
}
