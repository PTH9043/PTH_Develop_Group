#include "pch.h"
#include "CSceneMgr.h"
#include "CIntroScene.h"
#include "CGameScene1.h"
#include "CGameScene2.h"
#include "CGameScene3.h"
#include "CGameScene4.h"
#include "CBossScene1.h"
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
#include "CPotal.h"

CSceneMgr::CSceneMgr() : m_pCurStage(nullptr)
{
	Initialize();
}

CSceneMgr::CSceneMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr), m_pCurStage(nullptr)
{
	Initialize();
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Initialize()
{
	// Stage Setting
	m_vecSceneList.clear();
	m_vecSceneList.resize(SCN_END);
	m_vecSceneList[SCN_INTRO] = new CIntroScene();
	m_vecSceneList[SCN_STG1] = new CGameScene1();
	m_vecSceneList[SCN_STG2] = new CGameScene2();
	m_vecSceneList[SCN_STG3] = new CGameScene3();
	m_vecSceneList[SCN_STG4] = new CGameScene4();
	m_vecSceneList[SCN_BOSS1] = new CBossScene1();
	m_pCurStage = m_vecSceneList[SCN_INTRO];

}

int CSceneMgr::Update()
{
	if (nullptr != m_pCurStage)
		m_pCurStage->Update();
	return 0;
}

void CSceneMgr::Late_Update()
{
	if (nullptr != m_pCurStage)
		m_pCurStage->Late_Update();
}

void CSceneMgr::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (nullptr != m_pCurStage)
		m_pCurStage->Render(_hdc, v2ScrollPos);
}

void CSceneMgr::Release()
{
	m_pCurStage = nullptr;
	for (auto& iter : m_vecSceneList)
		DELETE_MEMORY(iter);
	m_vecSceneList.clear();
}

void CSceneMgr::Change_Stage(const bool& bIsChange, const RECT& tRect)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CGameMgr::Get_Instance()->Get_ObjMgr()->Get_Player());
	if (true == bIsChange)
	{
		for (auto& iter : m_pCurStage->Get_SceneList())
		{
			CScene* pScene = m_vecSceneList[iter];
			if (nullptr != pScene)
			{
				if (m_pCurStage != pScene)
				{
					RECT rc;
					bool find = false;
					for (auto Spawn : pScene->Get_SceneActiveList(SCA_POTAL))
					{
						RECT SpawnRect = Spawn->Get_Rect();
						SpawnRect.left = tRect.left - 10;
						SpawnRect.right = tRect.right + 10;
						SpawnRect.bottom += 200;
						SpawnRect.top -= 200;
						if (TRUE == IntersectRect(&rc, &SpawnRect, &tRect))
						{
							if (pPlayer->Get_CurPos().x >= MAP_X / 2.f)
							{
								if (Spawn->Get_CenterPos().x <= MAP_X / 2.f)
								{
									pPlayer->Set_CurPos(VECTOR2(Spawn->Get_CenterPos().x + 50.f, Spawn->Get_CenterPos().y));
									find = true;
									break;
								}
							}
							else
							{
								if (Spawn->Get_CenterPos().x >= MAP_X / 2.f)
								{
									pPlayer->Set_CurPos(VECTOR2(Spawn->Get_CenterPos().x - 50.f, Spawn->Get_CenterPos().y));
									find = true;
									break;
								}
							}
						}
					}
					if (true == find)
					{
						m_vecSceneList[iter]->Set_Player(pPlayer);
						m_vecSceneList[iter]->Scene_Change();
						m_pCurStage = m_vecSceneList[iter];
						break;
					}
				}
			}
		}
	}
	else
	{
		ESCENEID eSceneID = m_pCurStage->Get_SceneList().front();
		m_vecSceneList[eSceneID]->Set_Player(pPlayer);
		m_vecSceneList[eSceneID]->Scene_Change();
		m_pCurStage = m_vecSceneList[eSceneID];
	}
}


