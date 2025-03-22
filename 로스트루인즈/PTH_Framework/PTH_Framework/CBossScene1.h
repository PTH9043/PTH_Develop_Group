#pragma once
#include "CScene.h"

class CBossScene1 : public CScene
{
public:
	explicit CBossScene1();
	virtual ~CBossScene1();
protected:
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

	virtual void Player_Spawn() override;
	// Save Monster Spawn
	virtual void Save_MonsterList();

	virtual void Load(const TCHAR* pPath) override;
};

