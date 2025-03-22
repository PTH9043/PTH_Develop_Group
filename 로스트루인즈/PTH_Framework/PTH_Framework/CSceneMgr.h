#pragma once
#include "CManager.h"

class CScene;

class CSceneMgr final : public CManager
{
public:
	explicit CSceneMgr();
	explicit CSceneMgr(CGameMgr*& pGameMgr);
	virtual ~CSceneMgr();
protected:
	vector<CScene*>		m_vecSceneList;
	CScene*						m_pCurStage;
public:
	// CManager을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	void Late_Update();
	void Render(HDC _hdc, const VECTOR2& v2ScrollPos);
	virtual void Release() override;

	void Change_Stage(const bool& bIsChange, const RECT& tRect);
public:
	// get
	CScene* Get_CurScene() { return m_pCurStage; }
	// Set Cur Scene 
	void Set_CurScene(const ESCENEID& eSceneID) { m_pCurStage = m_vecSceneList[eSceneID]; }

	CScene* Get_Scene(const ESCENEID& eID) { return m_vecSceneList[eID]; }
};

