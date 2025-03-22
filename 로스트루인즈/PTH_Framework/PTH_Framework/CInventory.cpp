#include "pch.h"
#include "CInventory.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"
#include "CObjMgr.h"
#include "CInputMgr.h"
#include "CSword.h"
#include "CWeapon.h"
#include "CGradius.h"
#include "CPosion.h"

CInventory::CInventory() : m_pOwner(nullptr), m_pSelectSlot(nullptr), m_pImgBmp(nullptr), m_pBackBmp(nullptr),
m_tTranspercyRGB(RGBINFO())
, m_v2StartSlotPos(VECTOR2()), m_v2SpawnSlotPos(VECTOR2()), m_v2SelectSlotPos(VECTOR2()), m_iInvenIndex(0)
, m_iSaveSlotLength(0), m_iCheckUnderSlot(0), m_iSelectIndex(0), m_iShowStart(0), m_iShowEnd(7),
m_bUse(false), m_pItemBmp(nullptr), m_pEquipBmp(nullptr), m_pGoldText(nullptr), m_pExplainText(nullptr)
{
	ZeroMemory(&m_tSlotRect, sizeof(RECT));
}

CInventory::CInventory(CPlayer *& pPlayer, const TCHAR * szPathImg, const TCHAR* szPathBack, const RGBINFO& tInfo)
	: m_pOwner(pPlayer), m_pSelectSlot(nullptr), m_pImgBmp(nullptr), m_pBackBmp(nullptr)
	, m_tTranspercyRGB(tInfo)
	, m_v2StartSlotPos(VECTOR2()), m_v2SpawnSlotPos(VECTOR2()), m_v2SelectSlotPos(VECTOR2()), m_iInvenIndex(0)
	, m_iSaveSlotLength(0), m_iCheckUnderSlot(0), m_iSelectIndex(0), m_iShowStart(0), m_iShowEnd(7)
	, m_bUse(false), m_pItemBmp(nullptr), m_pEquipBmp(nullptr), m_pGoldText(nullptr), m_pExplainText(nullptr)
{
	ZeroMemory(&m_tSlotRect, sizeof(RECT));

	m_pImgBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"INVEN_MAIN");
	m_pBackBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"INVEN_BACK");
	m_pItemBackBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_RESET");
	m_pItemBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_IMG");
	m_pEquipBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"EQUIP_IMG");
	m_pThingsBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"THINGS_IMG");
	m_vecInvenSlot[0].clear();
	m_vecInvenSlot[0].reserve(9999);
	m_vecInvenSlot[1].clear();
	m_vecInvenSlot[1].reserve(9999);
	m_vecInvenSlot[2].clear();
	m_vecInvenSlot[2].reserve(9999);
	Initialize();
}

CInventory::~CInventory()
{
	Release();
}

void CInventory::Initialize()
{
	if (nullptr == m_pOwner)
	{
		MessageBox(g_hWnd, L"Warnning", L"INVEN_CHARCTER_ERROR", MB_OK);
		return;
	}
	m_v2CurPos = VECTOR2(50.f, 100.f);
	m_v2DrawSize = VECTOR2(700.f, 450.f);
	m_v2Size = VECTOR2(650.f, 400.f);
	// 주인에게 자기 자신 등록
	m_pOwner->Set_Inven(this);
	m_v2StartSlotPos = m_v2CurPos + VECTOR2(48.f, 140.f);
	m_v2SpawnSlotPos = VECTOR2(0.f, 35.f);
	m_v2SelectSlotPos = m_v2StartSlotPos + VECTOR2(10.f, 10.f);
	Update_Rect();
	m_vecActionSlot.reserve(4);
	m_vecItemSlot.reserve(2);
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	m_vecActionSlot.emplace_back(new CActionSlot(m_pOwner, L"ACT_SLOT", L"ACT_BACK", RGBINFO(0, 0, 0), 0));
	m_vecActionSlot[0]->Set_CurPos(VECTOR2(500.f, 5.f));
	m_vecActionSlot.emplace_back(new CActionSlot(m_pOwner, L"ACT_SLOT", L"ACT_BACK", RGBINFO(0, 0, 0), 1));
	m_vecActionSlot[1]->Set_CurPos(VECTOR2(565.f, 5.f));
	m_vecActionSlot.emplace_back(new CActionSlot(m_pOwner, L"ACT_SLOT", L"ACT_BACK", RGBINFO(0, 0, 0), 2));
	m_vecActionSlot[2]->Set_CurPos(VECTOR2(660.f, 5.f));
	m_vecActionSlot.emplace_back(new CActionSlot(m_pOwner, L"ACT_SLOT", L"ACT_BACK", RGBINFO(0, 0, 0), 3));
	m_vecActionSlot[3]->Set_CurPos(VECTOR2(725.f, 5.f));

	m_vecItemSlot.emplace_back(new CItemSlot(m_pOwner, L"ITEM_SLOT", L"ITEM_BACK", RGBINFO(0, 0, 0)));
	m_vecItemSlot[0]->Set_CurPos(VECTOR2(300.f, 5.f));
	m_vecItemSlot.emplace_back(new CItemSlot(m_pOwner, L"ITEM_SLOT", L"ITEM_BACK", RGBINFO(0, 0, 0)));
	m_vecItemSlot[1]->Set_CurPos(VECTOR2(365.f, 5.f));

	// Gold Text 할당
	m_pGoldText = new CText(this, L"맑은고딕", RGBINFO(0, 0, 0));
	m_pGoldText->Set_Size(VECTOR2(15.f, 0.f));
	m_pExplainText = new CText(this, L"맑은고딕", RGBINFO(0, 0, 0));
	m_pExplainText->Set_Size(VECTOR2(15.f, 0.f));
}

int CInventory::Update()
{
	for (auto& iter : m_vecActionSlot)
		iter->Update();

	for (auto& iter : m_vecItemSlot)
		iter->Update();


	if (false == m_bUse)
		return OBJ_NOEVENT;
	Judge_SlotList();
	Judge_Select_Slot();
	Check_SlotList();	
	Register_To_Player_Use();

	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
		iter->Update();

	return OBJ_NOEVENT;
}

void CInventory::Late_Update()
{
	if (false == m_bUse)
		return;

	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
		iter->Late_Update();
}

void CInventory::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	for (auto& iter : m_vecActionSlot)
		iter->Render(_hdc, v2ScrollPos);
	for (auto& iter : m_vecItemSlot)
		iter->Render(_hdc, v2ScrollPos);

	if (false == m_bUse)
		return;

	Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(25.f, 25.f), m_v2Size, m_pBackBmp, 50);
	Draw_Img_GDI(_hdc, m_v2CurPos, m_v2DrawSize, m_pImgBmp, m_tTranspercyRGB);

	if (2 == m_iInvenIndex)
	{
		Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, 255);
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp, m_tTranspercyRGB);
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, m_tTranspercyRGB);
	}
	else if (1 == m_iInvenIndex)
	{
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, m_tTranspercyRGB);
		Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp,255);
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, m_tTranspercyRGB);
	}
	else if (0 == m_iInvenIndex)
	{
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, m_tTranspercyRGB);
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp, m_tTranspercyRGB);
		Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, 255);
	}
	Draw_Img_Alpha(_hdc, m_v2CurPos + VECTOR2(25.f, 25.f), m_v2Size, m_pBackBmp, 50);
	if (nullptr != m_pSelectSlot)
	{
		CMyBmp* pBmp = m_pSelectSlot->Get_Item()->Get_Img();
		Draw_Img_Blt(_hdc, m_v2CurPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), m_pItemBackBmp);
		Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), pBmp, m_tTranspercyRGB);

		if (nullptr != m_pSelectSlot)
		{
			CMyBmp* pBmp = m_pSelectSlot->Get_Item()->Get_Img();
			Draw_Img_Blt(_hdc, m_v2CurPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), m_pItemBackBmp);
			Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), pBmp, m_tTranspercyRGB);
			m_pGoldText->Set_CurPos(m_v2CurPos + VECTOR2(490.f, 180.f));
			m_pExplainText->Set_CurPos(m_v2CurPos + VECTOR2(490.f, 200.f));
			TCHAR szGold[100];
			wsprintf(szGold, L"골드: %d", m_pSelectSlot->Get_Item()->Get_Gold());
			m_pGoldText->Set_Text(wstring(szGold));
			CEquip* pEquip = dynamic_cast<CEquip*>(m_pSelectSlot->Get_Item());
			if (nullptr != pEquip)
			{
				wsprintf(szGold, L"공격력: %d", pEquip->Get_Status().iAtkPower);
				m_pExplainText->Set_Text(wstring(szGold));
			}
			else
			{
				CPosion* pPosion = dynamic_cast<CPosion*>(m_pSelectSlot->Get_Item());
				if (nullptr != pPosion)
				{
					TCHAR szGold[100];
					if (POT_HP == pPosion->Get_PosionType())
						wsprintf(szGold, L"체력 회복력: %d", pPosion->Get_RecoverValue());
					else if (POT_MP == pPosion->Get_PosionType())
						wsprintf(szGold, L"마나 회복력: %d", pPosion->Get_RecoverValue());
					m_pExplainText->Set_Text(wstring(szGold));
				}
			}
			m_pGoldText->Render(_hdc, v2ScrollPos);
			m_pExplainText->Render(_hdc, v2ScrollPos);
		}
	}

	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
		iter->Render(_hdc, v2ScrollPos);
}

void CInventory::Collision_Actor(CObj *& pObj)
{
}

void CInventory::Release()
{
	for (auto& iter : m_vecActionSlot)
		DELETE_MEMORY(iter);

	for (auto& iter : m_vecItemSlot)
		DELETE_MEMORY(iter);

	for (auto& iter : m_vecInvenSlot[0])
	{
		iter->Set_Dead(true);
		DELETE_MEMORY(iter);
	}
	for (auto& iter : m_vecInvenSlot[1])
	{
		CGameMgr::Get_Instance()->Get_ObjMgr()->Register_Remove(OBJ_EQUIP, iter);
		DELETE_MEMORY(iter);
	}

	m_vecInvenSlot[0].clear();
	m_vecInvenSlot[1].clear();
	// Clear 
	DELETE_MEMORY(m_pGoldText);
	DELETE_MEMORY(m_pExplainText);
}

void CInventory::Add_Item(CObj * pItem)
{
	if (nullptr == pItem)
		return;

	int iIndex = 0;
	CEquip* pEquip = dynamic_cast<CEquip*>(pItem);
	if (nullptr != pEquip)
	{
		pEquip->Set_ItemPicked(true);
	}
	else
		iIndex = 1;

	static_cast<CItem*>(pItem)->Set_Owner(m_pOwner);
	for (auto& iter : m_vecInvenSlot[iIndex])
	{
		//	if (static_cast<CItem*>(pItem)->Get_ID() == iter->Get_Item()->Get_ID())
		if (static_cast<CItem*>(pItem)->Get_Name() == iter->Get_Item()->Get_Name())
		{
			iter->Insert_Item(pItem);
			return;
		}
	}

	CInvenSlot* pSlot = new CInvenSlot(this, L"INVEN_SLOT", RGBINFO(100, 100, 100));
	pSlot->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos * static_cast<float>(m_vecInvenSlot[iIndex].size())));
	pSlot->Initialize();
	pSlot->Insert_Item(pItem);
	pSlot->Set_RestrictRect(m_tSlotRect);
	pSlot->Set_Index(static_cast<int>(m_vecInvenSlot[iIndex].size()));

	if (m_iInvenIndex == iIndex)
	{
		if (m_vecInvenSlot[m_iInvenIndex].size() <= 0)
		{
			m_pSelectSlot = pSlot;
			m_pSelectSlot->Set_Selected(true);
		}
	}

	m_vecInvenSlot[iIndex].emplace_back(pSlot);
	// 슬롯 위치 초기화
	pSlot->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(m_iSaveSlotLength));
	pSlot->Set_Enable(true);
	++m_iSaveSlotLength;
}

void CInventory::Judge_SlotList()
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	bool bIsLeftKey = pInputMgr->Is_Press(VK_LEFT);
	bool bIsRightKey = pInputMgr->Is_Press(VK_RIGHT);

	if (bIsLeftKey)
	{
		if (nullptr != m_pSelectSlot)
			m_pSelectSlot->Set_Selected(false);
		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
			iter->Set_Enable(false);

		--m_iInvenIndex;
		if (0 > m_iInvenIndex)
			m_iInvenIndex = 0;

		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
			iter->Set_Enable(true);

		if (m_vecInvenSlot[m_iInvenIndex].size() > 0)
			m_pSelectSlot = m_vecInvenSlot[m_iInvenIndex].front();
		else
			m_pSelectSlot = nullptr;

		if (nullptr != m_pSelectSlot)
			m_pSelectSlot->Set_Selected(true);
		Reset_Info();
	}
	else if (bIsRightKey)
	{
		if (nullptr != m_pSelectSlot)
			m_pSelectSlot->Set_Selected(false);
		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
			iter->Set_Enable(false);

		++m_iInvenIndex;
		if (3 <= m_iInvenIndex)
			m_iInvenIndex = 2;

		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
			iter->Set_Enable(true);

		if (m_vecInvenSlot[m_iInvenIndex].size() > 0)
			m_pSelectSlot = m_vecInvenSlot[m_iInvenIndex].front();
		else
			m_pSelectSlot = nullptr;
		if (nullptr != m_pSelectSlot)
			m_pSelectSlot->Set_Selected(true);
		Reset_Info();
	}
}

void CInventory::Judge_Select_Slot()
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	bool bIsUpKey = pInputMgr->Is_Press(VK_UP);
	bool bIsDownKey = pInputMgr->Is_Press(VK_DOWN);

	int iIndex = m_iSelectIndex;	
	if (false == (bIsUpKey || bIsDownKey))
		return;

	if (true == bIsUpKey)
		--iIndex;
	if (true == bIsDownKey)
		++iIndex;

	if (0 > iIndex)
		return;
	else if (static_cast<int>(m_vecInvenSlot[m_iInvenIndex].size() - 1) < iIndex)
		return;

	int iMaxValue = static_cast<int>(m_vecInvenSlot[m_iInvenIndex].size() - 1);
	if (iMaxValue >= 7)
	{
		if (iIndex <= 7)
		{
			int iMultValue = 0;
			for (int i = 0; i <= 7; ++i)
			{
				m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
			}
			for (int i = 8; i < iMaxValue; ++i)
			{
				m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(VECTOR2());
			}
			m_iShowStart = 0;
			m_iShowEnd = 7;
		}
		else if (iIndex > 7)
		{
			if (bIsUpKey)
			{
				--m_iShowStart;
				--m_iShowEnd;
			}
			else if (bIsDownKey)
			{
				++m_iShowStart;
				++m_iShowEnd;
			}
			if (m_iShowStart > iMaxValue - 7)
				m_iShowStart = iMaxValue - 7;
			if (m_iShowEnd > iMaxValue)
				m_iShowEnd = iMaxValue;

			int iMultValue = 0;
			for (int i = 0; i < m_iShowStart; ++i)
			{
				m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(VECTOR2());
			}
			for (int i = m_iShowStart; i <= m_iShowEnd; ++i)
			{
				m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
			}
			int max = m_iShowEnd;
			if (max + 1 > iMaxValue)
				max = iMaxValue;
			for (int i = m_iShowEnd + 1; i <= iMaxValue; ++i)
			{
				m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(VECTOR2());
			}
		}
	}
	else
	{
		int iMultValue = 0;
		for (int i = 0; i <= static_cast<int>(m_vecInvenSlot[m_iInvenIndex].size() - 1); ++i)
		{
			m_vecInvenSlot[m_iInvenIndex][i]->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
		}
	}

	m_pSelectSlot->Set_Selected(false);
	m_pSelectSlot = m_vecInvenSlot[m_iInvenIndex][iIndex];
	m_pSelectSlot->Set_Selected(true);

	m_iSelectIndex = iIndex;
}

void CInventory::Reset_Info()
{
	// 슬롯 위치 초기화
	int iMultValue = 0;
	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
	{
		iter->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
	}
	m_iSelectIndex = 0;
}

void CInventory::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + m_v2Size.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y + m_v2Size.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + m_v2DrawSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y + m_v2DrawSize.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x - v2RealSize.y, m_v2CurPos.y - v2RealSize.y);

	m_tSlotRect.left = static_cast<long>(m_v2StartSlotPos.x);
	m_tSlotRect.right = static_cast<long>(m_tSlotRect.left + 400);
	m_tSlotRect.top = static_cast<long>(m_v2StartSlotPos.y);
	m_tSlotRect.bottom = static_cast<long>(m_tSlotRect.top + (m_v2SpawnSlotPos.y * 8));
}

void CInventory::Check_SlotList()
{
	if (static_cast<int>(m_vecInvenSlot[m_iInvenIndex].size()) != m_iSaveSlotLength)
	{
		m_iSaveSlotLength = static_cast<int>(m_vecInvenSlot[m_iInvenIndex].size());
		int iMultValue = 0;
		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
		{
			iter->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
		}
	}
}

bool CInventory::Check_Pos_IsInRect(const VECTOR2 & v2Pos, const RECT & tRect)
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

void CInventory::Register_To_Player_Use()
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	if (0 == m_iInvenIndex)
	{
		bool bIsZ = pInputMgr->Is_Press('Z');
		bool bIsX = pInputMgr->Is_Press('X');

		if (false == bIsZ && false == bIsX)
			return;
		int iIndex = 0;
		if (bIsZ)
			iIndex = 2;
		if (bIsX)
			iIndex = 3;

		if (nullptr != m_pSelectSlot)
		{
			CEquip* pItem = static_cast<CEquip*>(m_pSelectSlot->Get_Item());
			if (nullptr != pItem)
			{
				CGameMgr::Get_Instance()->Get_ObjMgr()->Register_Remove(OBJ_EQUIP, pItem);
				CEquip* pCheckItem = m_pOwner->Get_Wearable_Equip(iIndex);
				if (bIsZ)
				{
					if (pItem == m_pOwner->Get_Wearable_Equip(3))
						return;
				}
				else if (bIsX)
				{
					if (pItem == m_pOwner->Get_Wearable_Equip(2))
						return;
				}
				if (pItem == pCheckItem)
				{
					m_pOwner->Get_Wearable_Equip(iIndex)->Obj_Active(false);
					m_pOwner->Set_Wearable_Equip(iIndex, nullptr);
					m_vecActionSlot[iIndex]->Set_WeableEquipBmp(nullptr);
					return;
				}
				if (nullptr == pCheckItem)
				{
					pItem->Set_ItemPicked(true);
					CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_EQUIP, pItem);
					pItem->Set_CenterPos(m_pOwner->Get_EquipPos());
					m_pOwner->Set_Wearable_Equip(iIndex, pItem);
					m_vecActionSlot[iIndex]->Set_WeableEquipBmp(pItem->Get_Img());
				}
			}
		}
	}
	else if (1 == m_iInvenIndex)
	{
		bool bIsSpace = pInputMgr->Is_Press(VK_SPACE);
		if (true == bIsSpace)
		{
			if (nullptr != m_pSelectSlot)
			{
				CPosion* pPosion = dynamic_cast<CPosion*>(m_pSelectSlot->Get_Item());
				if (nullptr != pPosion)
				{
					int iSize = static_cast<int>(m_pSelectSlot->Get_Size());
					if (POT_HP == pPosion->Get_PosionType())
					{
						STATUS status = m_pOwner->Get_Status();
						status.iCurHp += pPosion->Get_RecoverValue();
						if (status.iCurHp > status.iMaxHp)
							status.iCurHp = status.iMaxHp;
						m_pOwner->Set_Status(status);
					}
					else if (POT_MP == pPosion->Get_PosionType())
					{
						STATUS status = m_pOwner->Get_Status();
						status.iCurMp += pPosion->Get_RecoverValue();
						if (status.iCurMp > status.iMaxMp)
							status.iCurMp = status.iMaxMp;
						m_pOwner->Set_Status(status);
					}
					CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"EAT_Cookie_Bite_mono.wav", SOUND_PLAYER, g_fSound);
					if (iSize <= 1)
					{

						vector<CInvenSlot*>::iterator it = m_vecInvenSlot[m_iInvenIndex].begin();
						for (; it != m_vecInvenSlot[m_iInvenIndex].end();)
						{
							if ((*it) == m_pSelectSlot)
							{
								DELETE_MEMORY(*it);
								m_vecInvenSlot[m_iInvenIndex].erase(it);
								break;
							}
							else
								++it;
						}
						if (m_vecInvenSlot[m_iInvenIndex].size() > 0)
						{
							m_pSelectSlot = m_vecInvenSlot[m_iInvenIndex].front();
							m_pSelectSlot->Set_Selected(true);
						}
						else
							m_pSelectSlot = nullptr;

					}
					else
					{
						m_pSelectSlot->Delete_Item();
					}
				}
			}
		}
	}
}
