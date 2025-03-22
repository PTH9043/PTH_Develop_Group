#include "pch.h"
#include "CInvenSlot.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"
#include "CInventory.h"
#include "CWeapon.h"

CInvenSlot::CInvenSlot() : m_pText(nullptr), m_bSelected(false), m_bOverRestrict(false), m_bUse(false)
{
	ZeroMemory(&m_tRestrictDraw, sizeof(RECT));
}

CInvenSlot::CInvenSlot(CObj * pOwner, const TCHAR * szSlotPath, const RGBINFO & tInfo)
	: CSlot(pOwner, szSlotPath, tInfo), m_pText(nullptr), m_bSelected(false), m_bOverRestrict(false), m_bUse(false)
{
	ZeroMemory(&m_tRestrictDraw, sizeof(RECT));
}

CInvenSlot::~CInvenSlot()
{
	Release();
}

void CInvenSlot::Initialize()
{
	if (nullptr == m_pOwner)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_CHARCTER_ERROR", MB_OK);
		return;
	}
	m_v2DrawSize = VECTOR2(250.f, 30.f);
	m_pText = new CText(this, L"³ª´®°íµñ", tagRGB(0, 0, 0));
	m_pText->Set_Size(VECTOR2(20.f, 0.f));
	Update_Rect();
}

int CInvenSlot::Update()
{
	if (static_cast<int>(m_lsItemList.size()) < 0)
		return OBJ_DEADEVENT;

	Update_Rect();
	Update_Text(); 
	return OBJ_NOEVENT;
}

void CInvenSlot::Late_Update()
{
	RECT rc;
	if (FALSE == IntersectRect(&rc, &m_tDrawRect, &m_tRestrictDraw))
		m_bOverRestrict = true;
	else
		m_bOverRestrict = false;
}

void CInvenSlot::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (true == m_bOverRestrict)
		return;
	int iTranspercyValue = 100;
	if (true == m_bSelected)
		iTranspercyValue = 200;

	Draw_Img_Alpha(_hdc, m_v2CurPos , m_v2DrawSize, m_pSlotImg, iTranspercyValue);
	m_pText->Render(_hdc, v2ScrollPos);
	/*  Ãß°¡ÇØ¾ßÇÔ.... */
}

void CInvenSlot::Collision_Actor(CObj *& pObj)
{
}

void CInvenSlot::Release()
{
	for (auto& iter : m_lsItemList)
	{
		CGameMgr::Get_Instance()->Get_ObjMgr()->Register_Remove(OBJ_ITEM, iter);
		CGameMgr::Get_Instance()->Get_ObjMgr()->Register_Remove(OBJ_EQUIP, iter);
	}
	for (auto& iter : m_lsItemList)
	{
		DELETE_MEMORY(iter);
	}
	m_lsItemList.clear();

	DELETE_MEMORY(m_pText);
}

void CInvenSlot::Insert_Item(CObj *& pItem)
{
	for (auto& iter : m_lsItemList)
	{
		if (pItem == iter)
			return;
	}
	m_lsItemList.emplace_back(static_cast<CItem*>(pItem));
}

void CInvenSlot::ItemNum_Minus()
{
	if(m_lsItemList.begin() != m_lsItemList.end())
		m_lsItemList.erase(m_lsItemList.begin());
}

void CInvenSlot::Delete_Item()
{
	list<CItem*>::iterator it = m_lsItemList.begin();
	if (it != m_lsItemList.end())
	{
		DELETE_MEMORY(*it);
		m_lsItemList.erase(it);
	}
}

void CInvenSlot::Update_Text()
{
	CItem* pItem = m_lsItemList.front();
	wstring strInfo = L"";	
	if (nullptr != pItem)
	{
		strInfo = wstring(pItem->Get_Name() + L" ");
		WCHAR szLoad[100];
		wsprintf(szLoad, L"%d", static_cast<int>(m_lsItemList.size()));
		strInfo.append(szLoad);
		strInfo.append(L"°³");
	}
	m_pText->Set_Text(strInfo);
	m_pText->Set_CurPos(m_v2CurPos + VECTOR2(0.f, 7.f));
}
