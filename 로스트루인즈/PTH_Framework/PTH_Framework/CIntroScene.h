#pragma once
#include "CScene.h"

class CButtonText;

class CAnimator;

class CIntroScene : public CScene
{
public:
	explicit CIntroScene();
	virtual ~CIntroScene();
protected:
	bool		m_bFileLoad;
	int		m_iLoadCount;

	vector<CButtonText*> m_vecButtonList;
	CButtonText* m_pSelectButtonText;
	int					m_iSelectIndex;

	const int BUTTON_START = 0;
	const int BUTTON_END = 1; 
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

	void Button_Select();

	void File_Load();
};

