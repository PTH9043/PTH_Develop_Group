#pragma once
#include "Include.h"

class CSceneActive;
class CSceneMgr;
class CObj;
class CMyBmp;
class CPlayer;

class CScene abstract
{
public:
	explicit CScene();
	virtual ~CScene();
protected:
	// depoly 
	list<CSceneActive*>		m_lsSceneActiveList[SCA_END];
	list<RECT>						m_lsSpawnList[SP_END];
	CMyBmp*						m_pSceneBmp;
	CObj*								m_pPlayer;
	ESCENEID						m_eSceneID;
	vector<ESCENEID>			m_vecNextSceneList;

	wstring							m_strFootSoundStr;
public:
	// Initalize
	virtual void Initialize();
	// Update
	virtual void Update();
	// Late Upate
	virtual void Late_Update();
	// Render
	virtual void Render(HDC _hDC, const VECTOR2& v2ScrollPos);
	// Collision
	virtual void Collision(CObj*& pObj, const ESCAID& eScaID);
	// Reset Value
	virtual void Scene_Change();
	// Release
	virtual void Release();
protected:
	virtual void Load(const TCHAR* pPath);

	virtual void Player_Spawn();

	virtual void Fall_Player();

	void Reset_Info();
public:
	const ESCENEID& Get_SceneID() const { return m_eSceneID; }
	vector<ESCENEID> Get_SceneList() { return m_vecNextSceneList; }

	list<CSceneActive*> Get_SceneActiveList(const ESCAID& eScaID) { return m_lsSceneActiveList[eScaID]; }
	list<RECT> Get_SpawnList(const ESPID& eSpID) { return m_lsSpawnList[eSpID]; }

	void Set_Player(CObj* pPlayer) { this->m_pPlayer = pPlayer; }
};

