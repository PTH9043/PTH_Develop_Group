#include "pch.h"
#include "CItemSlot.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"

CItemSlot::CItemSlot() : m_pBackImg(nullptr)
{
}

CItemSlot::CItemSlot(CObj * pOwner, const TCHAR * szSlotPath, const TCHAR * szBackImg, const RGBINFO & tInfo)
	: CSlot(pOwner, szSlotPath, tInfo), m_pBackImg(nullptr)
{
	m_pBackImg = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szBackImg);
	Initialize();
}

CItemSlot::~CItemSlot()
{
	Release();
}

void CItemSlot::Initialize()
{
	if (nullptr == m_pOwner)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_CHARCTER_ERROR", MB_OK);
		return;
	}

	m_v2DrawSize = VECTOR2(60.f, 60.f);

	Update_Rect();
}

int CItemSlot::Update()
{
	return OBJ_NOEVENT;
}

void CItemSlot::Late_Update()
{
}

void CItemSlot::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(3.f, 3.f), m_v2DrawSize - VECTOR2(6.f, 6.f), m_pBackImg, 200);
	Draw_Img_GDI(_hdc, m_v2CurPos - VECTOR2(1.f, 1.f), m_v2DrawSize + VECTOR2(1.f, 1.f), m_pSlotImg, m_tTranspercyRGB);
	/*  추가해야함.... */
}

void CItemSlot::Collision_Actor(CObj *& pObj)
{
}

void CItemSlot::Release()
{
	m_pBackImg = nullptr;
}
