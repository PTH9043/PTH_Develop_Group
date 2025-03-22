#include "pch.h"
#include "CGameScene4.h"
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

CGameScene4::CGameScene4()
{
	m_eSceneID = SCN_STG4;
	m_vecNextSceneList.reserve(1);
	m_vecNextSceneList.emplace_back(SCN_STG3);
	Initialize();
}

CGameScene4::~CGameScene4()
{
	Release();
}

void CGameScene4::Initialize()
{
	CScene::Initialize();
	m_pSceneBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"Map4");
	Load(L"Data\\Map4.dat");
}

void CGameScene4::Update()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"12555450_MotionElements_upbeat-deephouse-loopable.wav", SOUND_BGM, g_fSound);
	Collision(m_pPlayer, SCA_POTAL);
	Fall_Player();
	Reset_Info();
}

void CGameScene4::Late_Update()
{
	Collision(m_pPlayer, SCA_WALL);
}

void CGameScene4::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
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

void CGameScene4::Collision(CObj *& pObj, const ESCAID & eScaID)
{
	CScene::Collision(pObj, eScaID);
}

void CGameScene4::Scene_Change()
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

		r = rand() % 7;
		if (r == 0)
		{
			CDemonAssasin* p1 = new CDemonAssasin(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			p1->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, p1);
		}
		else if (r == 1 || r == 2 || r == 3)
		{
			CDemoness* p2 = new CDemoness(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			p2->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, p2);
		}
		else
		{
			CSkeletonArcher* p3 = new CSkeletonArcher(static_cast<CPlayer*>(pObjMgr->Get_Player()));
			p3->Set_CurPos(v2SpawnPos);
			pObjMgr->Add_Object(OBJ_MONSTER, p3);
		}
	}
	// Shot 배치 // 
	Player_Spawn();
//	pObjMgr->Active_All_Obj(true);
	pObjMgr->Active_Obj(OBJ_MONSTER, true);
	pObjMgr->Active_Obj(OBJ_PLAYER, true);
	pObjMgr->Active_Obj(OBJ_WORLD, true);
	pObjMgr->Active_Obj(OBJ_UI, true);
}

void CGameScene4::Release()
{
}

void CGameScene4::Save_MonsterList()
{
}

void CGameScene4::Load(const TCHAR * pPath)
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
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP4_FLOOR1")));
		}
		else if (DPY_FLOOR2 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP4_FLOOR2")));
		}
		else if (DPY_FLOOR3 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"MAP4_FLOOR3")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CWall(SCA_WALL, rc, wstring(L"MAP4_WALL1")));
		}
		else if (DPY_WALL2 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CDownWall(SCA_WALL, rc, wstring(L"MAP4_WALL2")));
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

void CGameScene4::Player_Spawn()
{
	VECTOR2 v2SpawnPos = m_pPlayer->Get_CurPos();
	CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
	pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));
}
