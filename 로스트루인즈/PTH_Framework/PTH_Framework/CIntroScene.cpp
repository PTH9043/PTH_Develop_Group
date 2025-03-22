#include "pch.h"
#include "CIntroScene.h"
#include "CMyBmp.h"
#include "CGameMgr.h"
#include "CObjMgr.h"
#include "CPlayer.h"
#include "CBar.h"
#include "CHpBar.h"
#include "CMpBar.h"
#include "CText.h"
#include "CActionSlot.h"
#include "CItemSlot.h"
#include "CInventory.h"
#include "CSword.h"
#include "CShop.h"
#include "CDemonAssasin.h"
#include "CButtonText.h"
#include "CDoor.h"
#include "CGameScene1.h"

CIntroScene::CIntroScene() : m_pSelectButtonText(nullptr), m_iSelectIndex(0)
{
	m_eSceneID = SCN_INTRO;
	m_vecNextSceneList.emplace_back(SCN_STG1);
	Initialize();
}

CIntroScene::~CIntroScene()
{
	Release();
}

void CIntroScene::Initialize()
{
	File_Load();

	CScene::Initialize();

	m_pSceneBmp = new CMyBmp();
	m_pSceneBmp->Load_Bmp(L"Resources\\Map\\Intro\\Intro.bmp");

	m_vecButtonList.resize(2);
	m_vecButtonList[BUTTON_START] = new CButtonText(nullptr, L"Bauhaus 93", RGBINFO(103, 107, 114));
	m_vecButtonList[BUTTON_START]->Set_CurPos(VECTOR2(330, 400));
	m_vecButtonList[BUTTON_START]->Set_Size(VECTOR2(30.f, 0.f));
	m_vecButtonList[BUTTON_START]->Set_Text(wstring(L"게임 시작"));
	m_vecButtonList[BUTTON_START]->Set_ClickInfo(RGBINFO(131, 172, 255));
	m_vecButtonList[BUTTON_END] = new CButtonText(nullptr, L"Bauhaus 93", RGBINFO(103, 107, 114));
	m_vecButtonList[BUTTON_END]->Set_CurPos(VECTOR2(330, 450));
	m_vecButtonList[BUTTON_END]->Set_Size(VECTOR2(30.f, 0.f));
	m_vecButtonList[BUTTON_END]->Set_Text(wstring(L"게임 종료"));
	m_vecButtonList[BUTTON_END]->Set_ClickInfo(RGBINFO(131, 172, 255));
	m_pSelectButtonText = m_vecButtonList[BUTTON_START];
	m_pSelectButtonText->Set_Selected(true);
	m_iSelectIndex = BUTTON_START;

	m_strFootSoundStr = L"";
}

void CIntroScene::Update()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"IntroStage.wav", SOUND_BGM, g_fSound);
	Button_Select();
	if (m_vecButtonList[BUTTON_START] == m_pSelectButtonText)
	{
		if (true == CGameMgr::Get_Instance()->Get_InputMgr()->Is_Press(VK_SPACE))
			CGameMgr::Get_Instance()->Get_SceneMgr()->Change_Stage(false, RECT{});
	}
	if (m_vecButtonList[BUTTON_END] == m_pSelectButtonText)
	{
		if (true == CGameMgr::Get_Instance()->Get_InputMgr()->Is_Press(VK_SPACE))
			PostQuitMessage(0);
	}           
}
void CIntroScene::Late_Update()
{
}

void CIntroScene::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
{
	BitBlt(_hDC, static_cast<int>(0), static_cast<int>(0),
		m_pSceneBmp->Get_Width(), m_pSceneBmp->Get_Height(), m_pSceneBmp->Get_MemDC(), 0, 0, SRCCOPY);
	for (auto& iter : m_vecButtonList)
		iter->Render(_hDC, v2ScrollPos);
}

void CIntroScene::Collision(CObj *& pObj, const ESCAID& eScaID)
{

}

void CIntroScene::Scene_Change()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->StopSound(SOUND_BGM);
	CGameMgr::Get_Instance()->Get_CScrollMgr()->Set_Pos(VECTOR2(0.f, 0.f));
	CGameMgr::Get_Instance()->Get_ObjMgr()->Active_All_Obj(false);
	static_cast<CPlayer*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Player())->Reback();
	static_cast<CGameScene1*>(CGameMgr::Get_Instance()->Get_SceneMgr()->Get_Scene(SCN_STG1))->Set_First(false);
}

void CIntroScene::Release()
{
	for (auto& iter : m_vecButtonList)
		DELETE_MEMORY(iter);

	m_vecButtonList.clear();
	DELETE_MEMORY(m_pSceneBmp);
}

void CIntroScene::Button_Select()
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();

	bool bIsUp = pInputMgr->Is_Press(VK_UP);
	bool bIsDown = pInputMgr->Is_Press(VK_DOWN);

	if ((false == (bIsUp || bIsDown)))
		return;

	m_vecButtonList[m_iSelectIndex]->Set_Selected(false);

	if (true == bIsUp)
	{
		--m_iSelectIndex;
		if (m_iSelectIndex < 0)
			m_iSelectIndex = 0;
	}
	else if (true == bIsDown)
	{
		++m_iSelectIndex;
		if (m_iSelectIndex >= static_cast<int>(m_vecButtonList.size() - 1))
			m_iSelectIndex = static_cast<int>(m_vecButtonList.size() - 1);
	}

	m_vecButtonList[m_iSelectIndex]->Set_Selected(true);
	m_pSelectButtonText = m_vecButtonList[m_iSelectIndex];
}

void CIntroScene::File_Load()
{
	CGameMgr::Get_Instance()->Get_BmpMgr()->Initialize();
	CGameMgr::Get_Instance()->Get_SoundMgr()->Initialize();

	CPlayer* pPlayer = new CPlayer();
	pPlayer->Initialize();	// Bar UI Setting 
	m_pPlayer = pPlayer;
	CBar* pHpBar = new CHpBar(pPlayer, L"HP_RED", L"BAR_BACKGROUND", tagRGB(255, 255, 255), 20);
	pHpBar->Init_Pos(VECTOR2(1.f, 15.f));
	CBar* pMpBar = new CMpBar(pPlayer, L"MP_BLUE", L"BAR_BACKGROUND", tagRGB(255, 255, 255), 15);
	pMpBar->Init_Pos(VECTOR2(1.f, 40.f));

	CInventory* pInven = new CInventory(pPlayer, L"INVEN_MAIN", L"INVEN_BACK", RGBINFO(0, 0, 0));
	// Set Player
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	CDoor*	 m_pDoor = new CDoor();
	m_pDoor->Set_CurPos(VECTOR2(265.f, 1775.f));
	// =====================
	pObjMgr->Add_Object(OBJ_PLAYER, pPlayer);
	// UI Set 
	pObjMgr->Add_Object(OBJ_UI, pHpBar);
	pObjMgr->Add_Object(OBJ_UI, pMpBar);
	pObjMgr->Add_Object(OBJ_UI, pInven);

	CShop* pShop = new CShop();
	pObjMgr->Add_Object(OBJ_WORLD, pShop);
	pObjMgr->Add_Object(OBJ_WORLD_2, m_pDoor);
	CGameMgr::Get_Instance()->Get_ObjMgr()->Active_All_Obj(false);
	m_bFileLoad = true;
}

