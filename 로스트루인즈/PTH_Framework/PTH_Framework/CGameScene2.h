#pragma once
#include "CScene.h"

class CGameScene2 : public CScene
{
public:
	explicit CGameScene2();
	virtual ~CGameScene2();
protected:

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

	virtual void Load(const TCHAR* pPath) override;

	virtual void Player_Spawn() override;
};


