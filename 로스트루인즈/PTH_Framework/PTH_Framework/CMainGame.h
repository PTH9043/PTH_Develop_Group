#pragma once
#include "Include.h"

class CMainGame
{
public:
	explicit CMainGame();
	virtual ~CMainGame();
protected:
	// 메인 DC
	HDC m_hDC;
protected:
	// 초기화
	void Initialize();
	// 파괴
	void Release();
public:
	void Update();
	void Late_Update();
	void Render();

};

