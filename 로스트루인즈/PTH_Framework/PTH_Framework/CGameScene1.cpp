#include "pch.h"
#include "CGameScene1.h"
#include "CBmpMgr.h"
#include "CGameMgr.h"
#include "CCharacter.h"
#include "CSceneActive.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CMyBmp.h"
#include "CObjMgr.h"
#include "CFloor.h"
#include "CWall.h"
#include "CDownWall.h"
#include "CPotal.h"
#include "CObj.h"
#include "CLineMgr.h"
#include "CAbomiGreen.h"
#include "CPlayer.h"
#include "CDemonAssasin.h"
#include "CDemoness.h"
#include "CZombieGirl.h"
#include "CSkeletonArcher.h"
#include "CDemonAxe.h"
#include "CShop.h"

CGameScene1::CGameScene1() : m_bIsFirst(false)
{
	m_eSceneID = SCN_STG1;
	m_vecNextSceneList.reserve(2);
	m_vecNextSceneList.emplace_back(SCN_STG2);
	m_vecNextSceneList.emplace_back(SCN_BOSS1);
	Initialize();
}
CGameScene1::~CGameScene1()
{
	Release();
}

void CGameScene1::Initialize()
{
	CScene::Initialize();
	m_pSceneBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"Map1");
	Load(L"Data\\Map1.dat");
}

void CGameScene1::Update()
{
	Reset_Info();
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"14260083_MotionElements_mysterious-harp.wav", SOUND_BGM, g_fSound);
	Collision(m_pPlayer, SCA_POTAL);
	Fall_Player();
}

void CGameScene1::Late_Update()
{
	Collision(m_pPlayer, SCA_WALL);
}

void CGameScene1::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
{
	BitBlt(_hDC, static_cast<int>(v2ScrollPos.x), static_cast<int>(v2ScrollPos.y),
		m_pSceneBmp->Get_Width(), m_pSceneBmp->Get_Height(), m_pSceneBmp->Get_MemDC(), 0, 0, SRCCOPY);
	// 여기에 그림
	for (int i = 0; i < SCA_END; ++i)
	{
		for (auto&iter : m_lsSceneActiveList[i])
			iter->Render(_hDC, v2ScrollPos, CGameMgr::Get_Instance()->Is_DevelopMode());
	}
}

void CGameScene1::Collision(CObj *& pObj, const ESCAID& eScaID)
{
	CScene::Collision(pObj, eScaID);
}

void CGameScene1::Scene_Change()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->StopSound(SOUND_BGM);
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();
	CGameMgr::Get_Instance()->Get_LineMgr()->Release();

	for (auto& iter : pObjMgr->Get_ObjList(OBJ_EQUIP))
		iter->Obj_Active(false);
	// Line 배치
	for (auto& iter : m_lsSceneActiveList[SCA_FLOOR])
	{
		CLine* pLine = static_cast<CFloor*>(iter)->Get_Line();
		CGameMgr::Get_Instance()->Get_LineMgr()->Insert(pLine);
	}
	if (false == m_bIsFirst)
	{
		Player_Spawn();
		m_bIsFirst = true;

		for (auto& iter : pObjMgr->Get_ObjList(OBJ_MONSTER))
		{
			iter->Set_Dead(true);
			static_cast<CMonster*>(iter)->Set_DeadCount(10);
		}
	}
	else
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pObjMgr->Get_Player());
		if (SCN_STG2 == CGameMgr::Get_Instance()->Get_SceneMgr()->Get_CurScene()->Get_SceneID())
		{
			pPlayer->Set_CurPos(VECTOR2(pPlayer->Get_CurPos().x, 100.f));
		}

		RECT tPlayerRect = m_lsSpawnList[SP_PLAYER].front();
		VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((tPlayerRect.right - tPlayerRect.left) / 2.f) + tPlayerRect.left),
			static_cast<float>(((tPlayerRect.bottom - tPlayerRect.top) / 2.f) + tPlayerRect.top));
		CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
		pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));

		for (auto& iter : pObjMgr->Get_ObjList(OBJ_ITEM))
			iter->Set_Dead(true);
		for (auto& iter : pObjMgr->Get_ObjList(OBJ_MONSTER))
		{
			iter->Set_Dead(true);
			static_cast<CMonster*>(iter)->Set_DeadCount(10);
		}
	}
	// Monster Spawn
	int r = 0;
	for (auto& iter : m_lsSpawnList[SP_MONSTER])
	{
		RECT tMonsterRect = iter;
		VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((tMonsterRect.right - tMonsterRect.left) / 2.f) + tMonsterRect.left),
			static_cast<float>(((tMonsterRect.bottom - tMonsterRect.top) / 2.f) + tMonsterRect.top));

		r = rand() % 2;
		if (r == 0)
		{
			CAbomiGreen* pAbomieGreen = new CAbomiGreen(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			pAbomieGreen->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, pAbomieGreen);
		}
		else
		{
			CZombieGirl* pZombieGirl = new CZombieGirl(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			pZombieGirl->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, pZombieGirl);
		}
	}

//	CDemonAssasin* pDemonAssassin = new CDemonAssasin(static_cast<CPlayer*>(pObjMgr->Get_Player()));
//	pDemonAssassin->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
//	pObjMgr->Add_Object(OBJ_MONSTER, pDemonAssassin);

	//CDemoness* pDemoness = new CDemoness(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	//pDemoness->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
	//pObjMgr->Add_Object(OBJ_MONSTER, pDemoness);

	//CZombieGirl* pZombieGirl = new CZombieGirl(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	//pZombieGirl->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
	//pObjMgr->Add_Object(OBJ_MONSTER, pZombieGirl);

	////CAbomiGreen* pAbomieGreen = new CAbomiGreen(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	////pAbomieGreen->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
	////pObjMgr->Add_Object(OBJ_MONSTER, pAbomieGreen);

	//CSkeletonArcher* pArcher = new CSkeletonArcher(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	//pArcher->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
	//pObjMgr->Add_Object(OBJ_MONSTER, pArcher);

	//CDemonAxe* pDemonAxe = new CDemonAxe(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	//pDemonAxe->Set_CurPos(v2PlayerPos + VECTOR2(100.f, 0.f));
	//pObjMgr->Add_Object(OBJ_MONSTER, pDemonAxe);

	pObjMgr->Active_Obj(OBJ_MONSTER, true);
	pObjMgr->Active_Obj(OBJ_PLAYER, true);
	pObjMgr->Active_Obj(OBJ_WORLD, true);
	pObjMgr->Active_Obj(OBJ_UI, true);
}

void CGameScene1::Release()
{
	CScene::Release();
}

void CGameScene1::Save_MonsterList()
{
	
}

void CGameScene1::Fall_Player()
{
	if (m_pPlayer->Get_CurPos().y >= MAP_Y * 3)
	{
		RECT tPlayerRect = m_lsSpawnList[SP_PLAYER].front();
		VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((tPlayerRect.right - tPlayerRect.left) / 2.f) + tPlayerRect.left),
			static_cast<float>(((tPlayerRect.bottom - tPlayerRect.top) / 2.f) + tPlayerRect.top));
		m_pPlayer->Set_CurPos(v2SpawnPos);

		static_cast<CPlayer*>(m_pPlayer)->Change_State(CHAR_IDLE);
	}
}

void CGameScene1::Load(const TCHAR * pPath)
{
	HANDLE hFile = CreateFile
	(
		pPath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Load File", L"Fail", MB_OK);
		return;
	}
	DWORD		dwByte = 0;
	RECT rc;
	EDPYID eDpy;
	while (true)
	{
		ReadFile(hFile, &rc, sizeof(RECT), &dwByte, nullptr);
		ReadFile(hFile, &eDpy, sizeof(EDPYID), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		if (DPY_FLOOR1 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP1_FLOOR1")));
		}
		else if (DPY_FLOOR2 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP1_FLOOR2")));
		}
		else if (DPY_FLOOR3 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP1_FLOOR3")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CWall(SCA_WALL, rc, wstring(L"MAP1_WALL1")));
		}
		else if (DPY_WALL2 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CDownWall(SCA_WALL, rc, wstring(L"MAP1_WALL2")));
		}
		else if (DPY_POTAL == eDpy)
		{
			m_lsSceneActiveList[SCA_POTAL].emplace_back(new CPotal(SCA_POTAL, rc));
		}
		else if (DPY_MONSTERSPAWNER == eDpy)
		{
			m_lsSpawnList[SP_MONSTER].emplace_back(rc);
		}
		else if (DPY_PLAYERSPAWNER == eDpy)
		{
			m_lsSpawnList[SP_PLAYER].emplace_back(rc);
		}
	}
	CloseHandle(hFile);
}
