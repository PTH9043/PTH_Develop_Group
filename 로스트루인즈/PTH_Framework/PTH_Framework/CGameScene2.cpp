#include "pch.h"
#include "CGameScene2.h"
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

CGameScene2::CGameScene2()
{
	m_eSceneID = SCN_STG2; 
	m_vecNextSceneList.reserve(2);
	m_vecNextSceneList.emplace_back(SCN_STG1);
	m_vecNextSceneList.emplace_back(SCN_STG3);
	Initialize();
}

CGameScene2::~CGameScene2()
{
	Release();
}

void CGameScene2::Initialize()
{
	CScene::Initialize();
	m_pSceneBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"Map2");
	Load(L"Data\\Map2.dat");
}

void CGameScene2::Update()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"16134498_MotionElements_heaven-gates-ambient.wav", SOUND_BGM, g_fSound);
	Collision(m_pPlayer, SCA_POTAL);
	Reset_Info();
	Fall_Player();
}

void CGameScene2::Late_Update()
{
}

void CGameScene2::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
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

void CGameScene2::Collision(CObj *& pObj, const ESCAID & eScaID)
{
	CScene::Collision(pObj, eScaID);
}

void CGameScene2::Scene_Change()
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
	// Shot 배치 // 
	for (auto& iter : pObjMgr->Get_ObjList(OBJ_ITEM))
		iter->Set_Dead(true);

	for (auto& iter : pObjMgr->Get_ObjList(OBJ_MONSTER))
	{
		iter->Set_Dead(true);
		static_cast<CMonster*>(iter)->Set_DeadCount(10);
	}
	CPlayer * pPlayer = static_cast<CPlayer*>(pObjMgr->Get_Player());
	VECTOR2 v2SpawnPos;
	for (auto& iter : m_lsSpawnList[SP_MONSTER])
	{
		RECT tMonsterRect = iter;
		v2SpawnPos = VECTOR2(static_cast<float>(((tMonsterRect.right - tMonsterRect.left) / 2.f) + tMonsterRect.left),
			static_cast<float>(((tMonsterRect.bottom - tMonsterRect.top) / 2.f) + tMonsterRect.top));

		CDemonAxe* pDemonAxe = new CDemonAxe(pPlayer);
		pDemonAxe->Set_CurPos(v2SpawnPos);
		pObjMgr->Add_Object(OBJ_MONSTER, pDemonAxe);
	}
	pPlayer->Set_CurPos(VECTOR2(pPlayer->Get_CurPos().x, v2SpawnPos.y));
	// Shot 배치 // 
	Player_Spawn();
	// pObjMgr->Active_All_Obj(true);
	pObjMgr->Active_Obj(OBJ_MONSTER, true);
	pObjMgr->Active_Obj(OBJ_PLAYER, true);
	pObjMgr->Active_Obj(OBJ_WORLD, true);
	pObjMgr->Active_Obj(OBJ_UI, true);
}

void CGameScene2::Release()
{
}

void CGameScene2::Save_MonsterList()
{
}

void CGameScene2::Load(const TCHAR * pPath)
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
	RECT			rc;
	EDPYID		eDpy;
	while (true)
	{
		ReadFile(hFile, &rc, sizeof(RECT), &dwByte, nullptr);
		ReadFile(hFile, &eDpy, sizeof(EDPYID), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		if (DPY_FLOOR1 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP2_FLOOR1")));
		}
		else if (DPY_FLOOR2 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP2_FLOOR1")));
		}
		else if (DPY_FLOOR3 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP2_FLOOR1")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CWall(SCA_WALL, rc, wstring(L"MAP2_WALL1")));
		}
		else if (DPY_WALL2 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CDownWall(SCA_WALL, rc, wstring(L"MAP2_WALL1")));
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

void CGameScene2::Player_Spawn()
{
	// CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();
// Player Spawn
	VECTOR2 v2SpawnPos = m_pPlayer->Get_CurPos();
	CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
	pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));
}
