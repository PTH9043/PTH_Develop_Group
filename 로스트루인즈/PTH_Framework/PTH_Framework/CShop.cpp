#include "pch.h"
#include "CShop.h"
#include "CObj.h"
#include "CPlayer.h"
#include "CGameMgr.h"
#include "CSword.h"
#include "CWeapon.h"
#include "CGradius.h"
#include "CArrow.h"
#include "CBow.h"
#include "COneHandSword.h"
#include "CTwoHandSword.h"
#include "CDagger.h"
#include "CPosion.h"

CShop::CShop() : m_pCustomer(nullptr), m_pSelectSlot(nullptr), m_pShopBmp(nullptr), m_pThingsBmp(nullptr),
m_pImgBmp(nullptr), m_pBackBmp(nullptr),
m_tTranspercyRGB(RGBINFO())
, m_v2StartSlotPos(VECTOR2()), m_v2SpawnSlotPos(VECTOR2()), m_v2SelectSlotPos(VECTOR2())
, m_v2ShopImgPos(VECTOR2()), m_v2ShopDrawSize(VECTOR2()), m_v2ShopSize(VECTOR2()),
m_iInvenIndex(0)
, m_iSaveSlotLength(0), m_iCheckUnderSlot(0), m_iSelectIndex(0), m_iShowStart(0), m_iShowEnd(7),
m_bUse(false), m_pItemBmp(nullptr), m_pEquipBmp(nullptr), m_pGoldText(nullptr), m_bCollision(false), m_pExplainText(nullptr)
{
	Initialize();
}

CShop::~CShop()
{
	Release();
}

void CShop::Initialize()
{
	m_v2ShopImgPos = VECTOR2(50.f, 100.f);
	m_v2ShopDrawSize = VECTOR2(700.f, 450.f);
	m_v2ShopSize = VECTOR2(650.f, 400.f);

	m_v2StartSlotPos = m_v2ShopImgPos + VECTOR2(48.f, 140.f);
	m_v2SpawnSlotPos = VECTOR2(0.f, 35.f);
	m_v2SelectSlotPos = m_v2StartSlotPos + VECTOR2(10.f, 10.f);
	m_pShopBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"SHOP");
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
	m_v2DrawSize = VECTOR2(100.f, 100.f);
	m_v2Size = m_v2DrawSize;
	m_iID = ID_WORLDOBJ_SHOP;
	m_bEnable = true;

	Update_Rect();
	// Gold Text 할당
	m_pGoldText = new CText(this, L"맑은고딕", RGBINFO(0, 0, 0));
	m_pGoldText->Set_Size(VECTOR2(15.f, 0.f));
	m_pExplainText = new CText(this, L"맑은고딕", RGBINFO(0, 0, 0));
	m_pExplainText->Set_Size(VECTOR2(15.f, 0.f));

	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	// --------------- Long Sword ---------------
	CSword* pSword = new COneHandSword(nullptr, L"LONGSWORD", L"LONGSWORD", SWORD_ONE, 1, 100);
	pSword->Set_Name(L"Long Sword");
	Add_Item(pSword);
	// --------------- Gradius ---------------
	pSword = new COneHandSword(nullptr, L"GRADIUS", L"GRADIUS", SWORD_ONE, 3, 200);
	pSword->Set_Name(L"Gradius");
	Add_Item(pSword);
	// --------------- HOLYSWORD ---------------
	pSword = new COneHandSword(nullptr, L"HOLYSWORD", L"HOLYSWORD", SWORD_ONE, 4, 300);
	pSword->Set_Name(L"HollySword");
	Add_Item(pSword);
	// --------------- BloodAxe ---------------
	pSword = new CTwoHandSword(nullptr, L"BLOODAXE", L"BLOODAXE", SWORD_TWO, 3, 200);
	pSword->Set_Name(L"BloodAxe");
	Add_Item(pSword);
	// --------------- ZWEIHANDER ---------------
	pSword = new CTwoHandSword(nullptr, L"ZWEIHANDER", L"ZWEIHANDER", SWORD_TWO, 5, 400);
	pSword->Set_Name(L"Zweihander");
	Add_Item(pSword);
	// --------------- Thunder Hamer ---------------
	pSword = new CTwoHandSword(nullptr, L"THUNDERHAMER", L"THUNDERHAMER", SWORD_TWO, 7, 600);
	pSword->Set_Name(L"Thunder Hamer");
	Add_Item(pSword);
	// --------------- Rustay Dagger ---------------
	pSword = new CDagger(nullptr, L"RUSTYDAGGER", L"RUSTYDAGGER", SWORD_DAGGER, 1, 200);
	pSword->Set_Name(L"Rustay Dagger");
	Add_Item(pSword);
	// --------------- Rituald Dagger ---------------
	pSword = new CDagger(nullptr, L"RITUALDAGGER", L"RITUALDAGGER", SWORD_DAGGER, 2, 300);
	pSword->Set_Name(L"Rituald Dagger");
	Add_Item(pSword);
	// --------------- Holy Dagger ---------------
	pSword = new CDagger(nullptr, L"HOLYDAGGER", L"HOLYDAGGER", SWORD_DAGGER, 3, 400);
	pSword->Set_Name(L"Holy Dagger ");
	Add_Item(pSword);
	// --------------- BOW ---------------
	CBow* pBow = new CBow(nullptr, L"BOW", L"BOW");
	pBow->Set_Name(L"BOW");
	Add_Item(pBow);
	// --------------- Atltari---------------
	CPosion* pPosion = nullptr;
	pPosion = new CPosion(nullptr, L"ALTARI", L"ALTARI", POT_HP, 50, 1);
	pPosion->Set_Name(L"Altari");
	Add_Item(pPosion);
	// --------------- Broccoli ---------------
	pPosion = new CPosion(nullptr, L"BROCCOLI", L"BROCCOLI", POT_HP, 75, 2);
	pPosion->Set_Name(L"Broccoli");
	Add_Item(pPosion);
	// --------------- Bread ---------------
	pPosion = new CPosion(nullptr, L"BREAD", L"BREAD", POT_HP, 100, 3);
	pPosion->Set_Name(L"Bread");
	Add_Item(pPosion);
	// --------------- Carrot ---------------
	pPosion = new CPosion(nullptr, L"CARROT", L"CARROT", POT_MP, 100, 1);
	pPosion->Set_Name(L"Carrot");
	Add_Item(pPosion);
	// --------------- CHEEZE  ---------------
	pPosion = new CPosion(nullptr, L"CHEEZE", L"CHEEZE", POT_MP, 200, 2);
	pPosion->Set_Name(L"Cheeze");
	Add_Item(pPosion);
	// --------------- COFFEE  ---------------
	pPosion = new CPosion(nullptr, L"COFFEE", L"COFFEE", POT_MP, 1000, 10);
	pPosion->Set_Name(L"Coffee");
	Add_Item(pPosion);
	// --------------- COLA  ---------------
	pPosion = new CPosion(nullptr, L"COLA", L"COLA", POT_HP, 200, 5);
	pPosion->Set_Name(L"Cola");
	Add_Item(pPosion);
	// --------------- HAMBUGGER  ---------------
	pPosion = new CPosion(nullptr, L"HAMBUGGER", L"HAMBUGGER", POT_HP, 500, 7);
	pPosion->Set_Name(L"Hamberger");
	Add_Item(pPosion);
	// --------------- HPPOSSION  ---------------
	pPosion = new CPosion(nullptr, L"HPPOSSION", L"HPPOSSION", POT_HP, 1000, 10);
	pPosion->Set_Name(L"Hp Possion");
	Add_Item(pPosion);
	// --------------- MPPOSSION  ---------------
	pPosion = new CPosion(nullptr, L"MPPOSSION", L"MPPOSSION", POT_MP, 800, 8);
	pPosion->Set_Name(L"Mp Possion");
	Add_Item(pPosion);
}

int CShop::Update()
{
	if (m_bCollision)
	{
		if (true == CGameMgr::Get_Instance()->Get_InputMgr()->Is_Press(VK_RETURN))
		{
			m_bUse = !m_bUse;
			m_pCustomer->Set_OpenShop(m_bUse);
		}
	}

	if (true == m_bUse)
	{
		for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
			iter->Update();

		Judge_SlotList();
		Judge_Select_Slot();
		Check_SlotList();
		Buy_Item();
	}
	Fall();
	Update_Rect();
	return OBJ_NOEVENT;
}

void CShop::Late_Update()
{
	if (false == m_bUse)
		return;

	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
	{
		iter->Late_Update();
	}
}

void CShop::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{

	if (nullptr != m_pShopBmp)
	{
		// 현재 애니메이션이 그려질 위치
		GdiTransparentBlt(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x),
			static_cast<int>(m_tRect.top + v2ScrollPos.y), static_cast<int>(m_v2Size.x), static_cast<int>(m_v2Size.y),
			// 그릴 애니메이션 그림의 위치
			m_pShopBmp->Get_MemDC(), 0, 0, m_pShopBmp->Get_Width(), m_pShopBmp->Get_Height(), RGB(0, 0, 0));
	}
	if (m_bUse)
	{
		Render_Shop(_hdc, v2ScrollPos);
	}
}

void CShop::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return;

	RECT rc;
	if (FALSE == IntersectRect(&rc, &m_tRect, &pObj->Get_Rect()))
	{
		m_bCollision = false;
		return;
	}

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);
	if (nullptr != pPlayer)
	{
		m_bCollision = true;
		m_pCustomer = pPlayer;
	}
}

void CShop::Release()
{
	for (auto& iter : m_vecInvenSlot[0])
		DELETE_MEMORY(iter);
	for (auto& iter : m_vecInvenSlot[1])
		DELETE_MEMORY(iter);
	for (auto& iter : m_vecInvenSlot[2])
		DELETE_MEMORY(iter);
	m_vecInvenSlot[0].clear();
	m_vecInvenSlot[1].clear();
	m_vecInvenSlot[2].clear();
	// Clear 
	DELETE_MEMORY(m_pGoldText);
	DELETE_MEMORY(m_pExplainText);
}

void CShop::Render_Shop(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Draw_Img_Alpha(_hdc, m_v2ShopImgPos + VECTOR2(25.f, 25.f), m_v2ShopSize, m_pBackBmp, 100);
	Draw_Img_GDI(_hdc, m_v2ShopImgPos, m_v2ShopDrawSize, m_pImgBmp, m_tTranspercyRGB);
	if (2 == m_iInvenIndex)
	{
		Draw_Img_Alpha(_hdc, m_v2ShopImgPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, 255);
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp, m_tTranspercyRGB);
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, m_tTranspercyRGB);
	}
	else if (1 == m_iInvenIndex)
	{
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, m_tTranspercyRGB);
		Draw_Img_Alpha(_hdc, m_v2ShopImgPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp, 255);
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, m_tTranspercyRGB);
	}
	else if (0 == m_iInvenIndex)
	{
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(170.f, 50.f), VECTOR2(50.f, 50.f), m_pThingsBmp, m_tTranspercyRGB);
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(110.f, 50.f), VECTOR2(50.f, 50.f), m_pItemBmp, m_tTranspercyRGB);
		Draw_Img_Alpha(_hdc, m_v2ShopImgPos + VECTOR2(50.f, 50.f), VECTOR2(50.f, 50.f), m_pEquipBmp, 255);
	}
	Draw_Img_Alpha(_hdc, m_v2ShopImgPos + VECTOR2(25.f, 25.f), m_v2ShopSize, m_pBackBmp, 50);
	if (nullptr != m_pSelectSlot)
	{
		CMyBmp* pBmp = m_pSelectSlot->Get_Item()->Get_Img();
		Draw_Img_Blt(_hdc, m_v2ShopImgPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), m_pItemBackBmp);
		Draw_Img_GDI(_hdc, m_v2ShopImgPos + VECTOR2(500.f, 120.f), VECTOR2(50.f, 50.f), pBmp, m_tTranspercyRGB);
		m_pGoldText->Set_CurPos(m_v2ShopImgPos + VECTOR2(490.f, 180.f));
		m_pExplainText->Set_CurPos(m_v2ShopImgPos + VECTOR2(490.f, 200.f));
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
			if(nullptr != pPosion)
			{
				TCHAR szGold[100];
				if(POT_HP == pPosion->Get_PosionType())
					wsprintf(szGold, L"체력 회복력: %d",  pPosion->Get_RecoverValue());
				else if(POT_MP == pPosion->Get_PosionType())
					wsprintf(szGold, L"마나 회복력: %d",  pPosion->Get_RecoverValue());
				m_pExplainText->Set_Text(wstring(szGold));
			}
		}
		m_pGoldText->Render(_hdc, v2ScrollPos);
		m_pExplainText->Render(_hdc, v2ScrollPos);
	}

	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
		iter->Render(_hdc, v2ScrollPos);
}

void CShop::Add_Item(CObj * pItem)
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


	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
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
	pSlot->Set_RestrictRect(m_tSlotRect);
	++m_iSaveSlotLength;
}

void CShop::Judge_SlotList()
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

void CShop::Judge_Select_Slot()
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	bool bIsUpKey = pInputMgr->Is_Press(VK_UP);
	bool bIsDownKey = pInputMgr->Is_Press(VK_DOWN);

	if (false == (bIsUpKey || bIsDownKey))
		return;

	int iIndex = m_iSelectIndex;
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

void CShop::Reset_Info()
{
	// 슬롯 위치 초기화
	int iMultValue = 0;
	for (auto& iter : m_vecInvenSlot[m_iInvenIndex])
	{
		iter->Set_CurPos(m_v2StartSlotPos + (m_v2SpawnSlotPos) * static_cast<float>(iMultValue++));
	}
	m_iSelectIndex = 0;
}

void CShop::Buy_Item()
{
	if (true == m_bCollision)
	{
		if (CGameMgr::Get_Instance()->Get_InputMgr()->Is_Press(VK_SPACE))
		{
			CItem* pItem = m_pSelectSlot->Get_Item();
			m_pCustomer->Get_Inventory()->Add_Item(pItem->Copy_Item());
		}
	}
}

void CShop::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + m_v2Size.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + m_v2DrawSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x - v2RealSize.y, m_v2CurPos.y - v2RealSize.y);

	m_tSlotRect.left = static_cast<long>(m_v2StartSlotPos.x);
	m_tSlotRect.right = static_cast<long>(m_tSlotRect.left + 400);
	m_tSlotRect.top = static_cast<long>(m_v2StartSlotPos.y);
	m_tSlotRect.bottom = static_cast<long>(m_tSlotRect.top + (m_v2SpawnSlotPos.y * 8));
}

void CShop::Check_SlotList()
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

bool CShop::Check_Pos_IsInRect(const VECTOR2 & v2Pos, const RECT & tRect)
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