#include "pch.h"
#include "CGameScene3.h"
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

CGameScene3::CGameScene3()
{
	m_eSceneID = SCN_STG2;
	m_vecNextSceneList.reserve(2);
	m_vecNextSceneList.emplace_back(SCN_STG2);
	m_vecNextSceneList.emplace_back(SCN_STG4);
	Initialize();
}

CGameScene3::~CGameScene3()
{
	Release();
}

void CGameScene3::Initialize()
{
	CScene::Initialize();
	m_pSceneBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"Map3");
	Load(L"Data\\Map3.dat");
}

void CGameScene3::Update()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"16134498_MotionElements_heaven-gates-ambient.wav", SOUND_BGM, g_fSound);
	Collision(m_pPlayer, SCA_POTAL);
	Fall_Player();
	Reset_Info();
}

void CGameScene3::Late_Update()
{
	Collision(m_pPlayer, SCA_WALL);
}

void CGameScene3::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
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

void CGameScene3::Collision(CObj *& pObj, const ESCAID & eScaID)
{
	CScene::Collision(pObj, eScaID);
}

void CGameScene3::Scene_Change()
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
	// Monster Spawn
	int r = 0;
	for (auto& iter : m_lsSpawnList[SP_MONSTER])
	{
		RECT tMonsterRect = iter;
		VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((tMonsterRect.right - tMonsterRect.left) / 2.f) + tMonsterRect.left),
			static_cast<float>(((tMonsterRect.bottom - tMonsterRect.top) / 2.f) + tMonsterRect.top));

		r = rand() % 3;
		if (r == 0)
		{
			CDemonAssasin* pDemonAssasin = new CDemonAssasin(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			pDemonAssasin->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, pDemonAssasin);
		}
		else if(r == 1)
		{
			CDemonAxe* pDemonAxe = new CDemonAxe(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			pDemonAxe->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, pDemonAxe);
		}
		else
		{
			CAbomiGreen* pAbmoiGreen = new CAbomiGreen(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			pAbmoiGreen->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, pAbmoiGreen);
		}
	}
	// Shot 배치 // 
	Player_Spawn();
	// pObjMgr->Active_All_Obj(true);
	pObjMgr->Active_Obj(OBJ_MONSTER, true);
	pObjMgr->Active_Obj(OBJ_PLAYER, true);
	pObjMgr->Active_Obj(OBJ_WORLD, true);
	pObjMgr->Active_Obj(OBJ_UI, true);
}

void CGameScene3::Release()
{
}

void CGameScene3::Save_MonsterList()
{
}

void CGameScene3::Load(const TCHAR * pPath)
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
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP3_FLOOR1")));
		}
		else if (DPY_FLOOR2 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP3_FLOOR2")));
		}
		else if (DPY_FLOOR3 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP3_FLOOR3")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CWall(SCA_WALL, rc, wstring(L"MAP3_WALL1")));
		}
		else if (DPY_WALL2 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CDownWall(SCA_WALL, rc, wstring(L"MAP3_WALL2")));
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

void CGameScene3::Player_Spawn()
{
	VECTOR2 v2SpawnPos = m_pPlayer->Get_CurPos();
	CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
	pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));
}
