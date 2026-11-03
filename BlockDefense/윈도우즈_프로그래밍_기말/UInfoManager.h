#pragma once

#include"UManager.h"

class UInfoManager : public UManager
{
public:
	UInfoManager() {}
	UInfoManager(UGame* game) { this->uGame = game; }
	virtual ~UInfoManager() { Reset(); }

	void Initialize() override { }
	void IFCW_Initi(HWND hWnd);
	// 강화소 명령어
	void IFCW_Command(HWND hWnd, WPARAM wParam);
	// 그리는 함수
	void IFCW_Draw(HWND hWnd, HDC hdc);

	void Reset() override;

protected:
	void Create();
	void UpdateSelectTowerInfo();
public:
	void SetSelectedTower(class ATower* tower);
	ATower* GetSelectedTower() const { return aSelectedTower; }

	bool IsStart() const { return bIsStart; }
protected:
	HWND hIF_Hwnd = NULL;
	HWND hIF_TextGold = NULL, hIF_ListExplain = NULL;
	HWND hIF_ButtonSL = NULL, hIF_ButtonExit = NULL;
	bool bIsStart = false;
	class ATower* aSelectedTower = nullptr;
};