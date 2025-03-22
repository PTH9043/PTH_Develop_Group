#include "pch.h"
#include "CBossScene1.h"
#include "CGameMgr.h"
#include "CCharacter.h"
#include "CSceneActive.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CMyBmp.h"
#include "CObjMgr.h"
#include "CFloor.h"
#include "CWall.h"
#include "CPotal.h"
#include "CObj.h"
#include "CCastle.h"
#include "CObjMgr.h"
#include "CPlayer.h"
#include "CDownWall.h"
#include "CSword.h"
#include "CGradius.h"
#include "CShop.h"

CBossScene1::CBossScene1()
{
	m_eSceneID = SCN_BOSS1;
	m_vecNextSceneList.emplace_back(SCN_STG1);
	Initialize();
}
CBossScene1::~CBossScene1()
{
	Release();
}

void CBossScene1::Initialize()
{
	CScene::Initialize();
	m_pSceneBmp = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"BOSSMAP1");
	Load(L"Data\\BossMap1.dat");
}

void CBossScene1::Update()
{
	Reset_Info();
}

void CBossScene1::Late_Update()
{
	Collision(m_pPlayer, SCA_POTAL);
	Collision(m_pPlayer, SCA_WALL);
}

void CBossScene1::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
{
	BitBlt(_hDC, static_cast<int>(v2ScrollPos.x), static_cast<int>(v2ScrollPos.y),
		m_pSceneBmp->Get_Width(), m_pSceneBmp->Get_Height(), m_pSceneBmp->Get_MemDC(), 0, 0, SRCCOPY);
	// 여기에 그림
	for (int i = 0; i < SCA_END; ++i)
	{
		for (auto&iter : m_lsSceneActiveList[i])
			iter->Render(_hDC, v2ScrollPos, CGameMgr::Get_Instance()->Is_DevelopMode());
	}
	if (true == CGameMgr::Get_Instance()->Is_DevelopMode())
	{
		for (int i = 0; i < SP_END; ++i)
		{
			for (auto&iter : m_lsSpawnList[i])
			{
				Rectangle(_hDC, static_cast<int>(iter.left + v2ScrollPos.x), static_cast<int>(iter.top + v2ScrollPos.y), 
					static_cast<int>(iter.right + v2ScrollPos.x), static_cast<int>(iter.bottom + v2ScrollPos.y));
			}
		}
	}
}

void CBossScene1::Collision(CObj *& pObj, const ESCAID & eScaID)
{
	CScene::Collision(pObj, eScaID);
}

void CBossScene1::Scene_Change()
{
	CGameMgr::Get_Instance()->Get_SoundMgr()->StopSound(SOUND_BGM);
	Player_Spawn();
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> MonsterList = pObjMgr->Get_ObjList(OBJ_MONSTER);
	for (auto& iter : MonsterList)
	{
		iter->Set_Dead(true);
		static_cast<CMonster*>(iter)->Set_DeadCount(10);
	}
	pObjMgr->Active_All_Obj(false);
	CGameMgr::Get_Instance()->Get_LineMgr()->Release();
	for (auto& iter : m_lsSceneActiveList[SCA_FLOOR])
	{
		CLine* pLine = static_cast<CFloor*>(iter)->Get_Line();
		CGameMgr::Get_Instance()->Get_LineMgr()->Insert(pLine);

	}

	CCastle* pCastle = new CCastle(static_cast<CPlayer*>(pObjMgr->Get_Player()));
	RECT monsterRect = m_lsSpawnList[SP_MONSTER].front();
	VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((monsterRect.right - monsterRect.left) / 2.f) + monsterRect.left),
		static_cast<float>(((monsterRect.bottom - monsterRect.top) / 2.f) + monsterRect.top));
	pCastle->Set_CurPos(v2SpawnPos);
	pCastle->Set_Name(wstring(L"Castle"));
	pObjMgr->Add_Object(OBJ_MONSTER, pCastle);
	// ==================================================================
	pObjMgr->Active_All_Obj(true);
	CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"BossStage1.wav", SOUND_BGM, g_fSound);
	// ==================================================================
}

void CBossScene1::Release()
{
	CScene::Release();
}

void CBossScene1::Player_Spawn()
{
	// CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();
	// Player Spawn
	VECTOR2 v2SpawnPos = m_pPlayer->Get_CurPos();
	CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
	pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));
	VECTOR2 v2Value = VECTOR2() - v2SpawnPos;
}

void CBossScene1::Save_MonsterList()
{

}

void CBossScene1::Load(const TCHAR * pPath)
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
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"BM1_FLOOR1")));
		}
		else if (DPY_FLOOR2 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"BM1_FLOOR2")));
		}
		else if (DPY_FLOOR3 == eDpy)
		{
			m_lsSceneActiveList[SCA_FLOOR].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"BM1_FLOOR3")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CWall(SCA_WALL, rc, wstring(L"BM1_WALL1")));
		}
		else if (DPY_WALL2 == eDpy)
		{
			m_lsSceneActiveList[SCA_WALL].emplace_back(new CDownWall(SCA_WALL, rc, wstring(L"BM1_WALL2")));
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
