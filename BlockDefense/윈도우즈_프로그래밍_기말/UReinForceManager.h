#pragma once
#include"UManager.h"

class UReinForceManager : public UManager
{
public:
	UReinForceManager() {}
	UReinForceManager(UGame* game) { this->uGame = game; }
	virtual ~UReinForceManager() { Reset(); }

	void Initialize() override { }
	void RFCW_Initi(HWND hWnd);
	// 강화소 명령어
	void RFCW_Command(HWND hWnd, WPARAM wParam);
	// 그리는 함수
	void RFCW_Draw(HWND hWnd, HDC hdc);

	void Reset() override;

protected:
	void Create();
	void UpdatePlayerHaveGold();
	void UpdateSelectTowerInfo();
public:
	void SetSelectedTower(class ATower* tower);
	class ATower* GetSelectedTower() const { return aSelectedTower; }

	bool IsStart() const { return bIsStart; }

protected:
	HWND hRF_Hwnd = NULL;
	HWND hRF_TextGold = NULL, hRF_ListExplain = NULL;
	HWND hRF_ButtonRF = NULL, hRF_ButtonExit = NULL;
	bool bIsStart = false;
	class ATower* aSelectedTower = nullptr;
};