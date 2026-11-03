#pragma once
#include<Windows.h>
#include"resource.h"
#include"Data.h"

class UGame
{
public:
	UGame() {}
	UGame(HINSTANCE inst) { this->hInst = inst; }
	virtual ~UGame() { Reset(); }
	// 게임 클래스 초기화 함수
	void Initialize();
	// 업데이트 함수
	void Update();
	// 화면을 렌더링함
	void RenderScreen(HDC hdc);
	// 왼쪽 마우스 버튼 클릭
	void LeftMouseButtonClick(LPARAM lParam);
	// 왼쪽 마우스 버튼 땟을 때
	void UnLeftMouseButtonClick();
	// 윈도우 버튼 커멘드
	void WM_ButtonCommand(HWND hWnd, WPARAM wParam, DLGPROC shopProc);
	// 드래그
	void Drag(LPARAM lParam);
	// 텍스처를 가져오는 함수
	UPicture* GetTexture(const TCHAR* str);
	// 텍스처를 로딩해놓은 함수
	UPicture* LoadTexture(const TCHAR* load, const TCHAR* str, int width, int height);
	// 턴이 지나면 텍스트를 업데이트 하는 함수
	void UpdateTurnText();
	// 생존 목숨을 업데이트 하는 함수
	void UpdateSurviveNum();
	// 소지 골드를 업데이트 하는 함수
	void UpdateHaveGold();
	// 다시 리 겜하기
	void Regame();
protected:
	void InitTexture();
	// 윈도우의 버튼, 등의 것들을 생성하는 함수
	void WindowInterfaceInit();
	// 화면을 실제로 그리는 함수
	void DrawScreen(HDC hdc);
	// 리셋하는 함수
	void Reset();
public:
	void SetInstance(HINSTANCE inst) { this->hInst = inst; }
	HINSTANCE GetInst() const { return hInst; }
	void SetWindowHWND(HWND hwnd) { this->hWnd = hwnd; }
	HWND GetWindowHWND() const { return hWnd; }
	void SetGameHWND(HWND hwnd) { this->hGameHwnd = hwnd; }
	HWND GetGameHWND() const { return hGameHwnd; }

	class ABackground* GetBackground() const { return aBackground; }
	class UPlayerManager* GetPlayerManager() const { return uPlayerManager; }
	class UShopManager* GetShopManager() const { return uShopManager; }
	class UReinForceManager* GetReinForceManager() const { return uReinForceManager; }
	class UInfoManager* GetInfoManager() const { return uInfoManager; }
	class UEnemyManager* GetEnemyManager() const { return uEnemyManager; }
	class UStageManager* GetStageManager() const { return uStageManager; }

	RECT GetGameClientRect() const { return rGameClientRect; }

	Vector2 GetMousePosition() const { return v2MousePosition; }
	bool IsLeftMouseClick() const { return bIsLeftMouseClick; }

	void SetRightMouseClick(bool mouseClick) { this->bIsRightMouseClick = mouseClick; }
	bool IsRightMouseClick() const { return bIsRightMouseClick; }

	void SetWLDIalog(DLGPROC wldl) { this->dlWinLoseDialog = wldl; }
	DLGPROC GetWLDialog() const { return dlWinLoseDialog; }
protected:
	HWND hTurnText = NULL, hSurviveText = NULL, hHaveGold = NULL;
	HWND hButtonArr[5] = { NULL };
	DLGPROC dlWinLoseDialog = NULL;
	HINSTANCE hInst = NULL;
	HWND hWnd = NULL, hGameHwnd = NULL;
	HBITMAP hGameScreen = NULL;
	RECT rGameClientRect = RECT{ 0, 0, 0, 0 };
	CustomList<UPicture*>* clPictureList = nullptr;
	class ABackground* aBackground = nullptr;
	class UPlayerManager* uPlayerManager = nullptr;
	class UShopManager* uShopManager = nullptr;
	class UReinForceManager* uReinForceManager = nullptr;
	class UInfoManager* uInfoManager = nullptr;
	class UEnemyManager* uEnemyManager = nullptr;
	class UStageManager* uStageManager = nullptr;
	Vector2 v2MousePosition;
	bool bIsLeftMouseClick = false, bIsRightMouseClick = false, bIsGameEnd = false;
};

