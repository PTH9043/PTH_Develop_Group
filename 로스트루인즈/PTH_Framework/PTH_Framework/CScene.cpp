#include "pch.h"
#include "CScene.h"
#include "CMyBmp.h"
#include "CSceneActive.h"
#include "CScrollMgr.h"
#include "CGameMgr.h"
#include "CObjMgr.h"
#include "CFloor.h"
#include "CWall.h"
#include "CPotal.h"
#include "CObj.h"
#include "CShop.h"

CScene::CScene() : m_eSceneID(SCN_END),  m_pSceneBmp(nullptr), m_pPlayer(nullptr), m_strFootSoundStr(L"")
{
	for (int i = 0; i < SCA_END; ++i)
		m_lsSceneActiveList[i].clear();
	for (int i = 0; i < SP_END; ++i)
		m_lsSpawnList[i].clear();
}

CScene::~CScene()
{
	Release();
}

void CScene::Initialize()
{
}

void CScene::Update()
{

}

void CScene::Late_Update()
{
}

void CScene::Render(HDC _hDC, const VECTOR2 & v2ScrollPos)
{
	if (nullptr != m_pSceneBmp)
	{
		BitBlt(_hDC, 0, 0, WINCX, WINCY, m_pSceneBmp->Get_MemDC(), 0, 0, SRCCOPY);
	}
}

void CScene::Collision(CObj *& pObj, const ESCAID& eScaID)
{
	for (auto& iter : m_lsSceneActiveList[eScaID])
	{
		iter->Collision_Obj(pObj, VECTOR2());
	}
}

void CScene::Scene_Change()
{

}

void CScene::Release()
{
	for (int i = 0; i < SCA_END; ++i)
	{
		for (auto& it : m_lsSceneActiveList[i])
			DELETE_MEMORY(it);

		m_lsSceneActiveList[i].clear();
	}
	m_lsSpawnList->clear();

	m_pSceneBmp = nullptr;
}

void CScene::Load(const TCHAR* pPath)
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
			m_lsSceneActiveList[eDpy].emplace_back(new CFloor(SCA_FLOOR, rc, wstring(L"")));
		}
		else if (DPY_WALL1 == eDpy)
		{
			m_lsSceneActiveList[eDpy].emplace_back(new CWall(SCA_WALL, rc, wstring(L"")));
		}
		else if (DPY_POTAL == eDpy)
		{
			m_lsSceneActiveList[eDpy].emplace_back(new CPotal(SCA_POTAL, rc));
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

void CScene::Player_Spawn()
{
	CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();
	// Player Spawn
	RECT tPlayerRect = m_lsSpawnList[SP_PLAYER].front();
	VECTOR2 v2SpawnPos = VECTOR2(static_cast<float>(((tPlayerRect.right - tPlayerRect.left) / 2.f) + tPlayerRect.left),
		static_cast<float>(((tPlayerRect.bottom - tPlayerRect.top) / 2.f) + tPlayerRect.top));
	CShop* pShop = static_cast<CShop*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Shop());
	pShop->Set_CurPos(v2SpawnPos + VECTOR2(50.f, 0.f));
	VECTOR2 v2Value = VECTOR2() - v2SpawnPos;
	v2Value.x += 400.f;
	pScrollMgr->Set_Pos(v2Value);
	m_pPlayer->Set_CurPos(v2SpawnPos);
}

void CScene::Fall_Player()
{
	if (m_pPlayer->Get_CurPos().y >= MAP_Y * 3)
	{
		if (m_pPlayer->Get_CurPos().x >= MAP_X / 2.f)
		{
			for (auto& iter : m_lsSceneActiveList[SCA_POTAL])
			{
				if (iter->Get_CenterPos().x >= MAP_X / 2.f)
				{
					m_pPlayer->Set_CurPos(iter->Get_CenterPos());
				}
			}
		}
		else
		{
			for (auto& iter : m_lsSceneActiveList[SCA_POTAL])
			{
				if (iter->Get_CenterPos().x < MAP_X / 2.f)
				{
					m_pPlayer->Set_CurPos(iter->Get_CenterPos());
				}
			}
		}

		static_cast<CPlayer*>(m_pPlayer)->Change_State(CHAR_IDLE);
	}
}

void CScene::Reset_Info()
{
	CScene* pScene = CGameMgr::Get_Instance()->Get_SceneMgr()->Get_Scene(SCN_INTRO);
	if (nullptr != m_pPlayer)
	{
		if (true == static_cast<CPlayer*>(m_pPlayer)->Is_RealDead())
		{
			CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
			pObjMgr->Active_All_Obj(false);
			CGameMgr::Get_Instance()->Get_SceneMgr()->Set_CurScene(SCN_INTRO);
		}
	}
}

