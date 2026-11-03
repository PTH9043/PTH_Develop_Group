#pragma once
#include"UManager.h"

class UShopManager : public UManager
{
public:
	UShopManager(){}
	UShopManager(UGame* game) { this->uGame = game; }
	virtual ~UShopManager() { Reset(); }

	void Initialize() override;
	// 상점  다이얼로그 초기화
	void SHDialog_Init(HWND shHwnd, WPARAM wParam);
	// 상점 다이얼로그 커맨트
	void SHDialog_Command(HWND shHwnd, WPARAM wParam);

	void Reset() override { DELETE_MEMORY(clTowerList); }
protected:
	void TowerButton(HWND hWnd, WPARAM wParam);
	void PurchaseButton(HWND hWnd, WPARAM wParam);
	void SetPictureControl(HWND hWnd, const TCHAR* name, int index);

	HBITMAP GetTexture(const TCHAR* name);
protected:
	CustomList<UPicture*>* clPictureList = nullptr;
	CustomList<class ATower*>* clTowerList = nullptr;
	class ATower* aSelectedTower = nullptr;
	int iSelectedID = 0;
};

