#pragma once
#include "CScene.h"

class CGameScene1 : public CScene
{
public:
	explicit CGameScene1();
	virtual ~CGameScene1();
protected:
	bool m_bIsFirst;
public:
	// Initalize
	virtual void Initialize() override;
	// Update
	virtual void Update()override;
	// Late Upate
	virtual void Late_Update()override;
	// Render
	virtual void Render(HDC _hDC, const VECTOR2& v2ScrollPos)override;
	// Collision
	virtual void Collision(CObj*& pObj, const ESCAID& eScaID)override;
	// Reset Value
	virtual void Scene_Change()override;
	// Release
	virtual void Release()override;
protected:
	// Save Monster Spawn
	virtual void Save_MonsterList();

	virtual void Fall_Player() override;

	virtual void Load(const TCHAR* pPath) override;

public:
	void Set_First(const bool& bFirst) { this->m_bIsFirst = bFirst; }
};

