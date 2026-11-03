#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS 
#include "UShopManager.h"
#include"ATower.h"
#include"ATower1.h"
#include"ATower2.h"
#include"ATower3.h"
#include"ATower4.h"
#include"ATower5.h"
#include"ATower6.h"
#include"ATower7.h"
#include"ATower8.h"
#include"ATower9.h"
#include"ATower10.h"
#include"ATower11.h"
#include"ATower12.h"
#include"UPlayerManager.h"
#include"USimpleSpriteComponent.h"
#include"ABackground.h"

void UShopManager::Initialize()
{
	clTowerList = new CustomList<ATower*>();
	clTowerList->Insert(new ATower1(uGame));clTowerList->Insert(new ATower2(uGame));
	clTowerList->Insert(new ATower3(uGame)); clTowerList->Insert(new ATower4(uGame));
	clTowerList->Insert(new ATower5(uGame)); clTowerList->Insert(new ATower6(uGame));
	clTowerList->Insert(new ATower7(uGame)); clTowerList->Insert(new ATower8(uGame));
	clTowerList->Insert(new ATower9(uGame)); clTowerList->Insert(new ATower10(uGame));
	clTowerList->Insert(new ATower11(uGame)); clTowerList->Insert(new ATower12(uGame));

	clPictureList = new CustomList<UPicture*>();
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower1.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower7.bmp", 80, 80));
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower2.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower8.bmp", 80, 80));
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower3.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower9.bmp", 80, 80));
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower4.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower10.bmp", 80, 80));
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower5.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower11.bmp", 80, 80));
	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower6.bmp", 80, 80));	clPictureList->Insert(uGame->LoadTexture(L"BMP/", L"Tower12.bmp", 80, 80));
}

void UShopManager::SHDialog_Init(HWND shHwnd, WPARAM wParam)
{
	HWND tower1 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER1);
	SendMessage(tower1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower1.bmp"));
	HWND tower2 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER2);
	SendMessage(tower2, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower2.bmp"));
	HWND tower3 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER3);
	SendMessage(tower3, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower3.bmp"));
	HWND tower4 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER4);
	SendMessage(tower4, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower4.bmp"));
	HWND tower5 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER5);
	SendMessage(tower5, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower5.bmp"));
	HWND tower6 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER6);
	SendMessage(tower6, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower6.bmp"));
	HWND tower7 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER7);
	SendMessage(tower7, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower7.bmp"));
	HWND tower8 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER8);
	SendMessage(tower8, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower8.bmp"));
	HWND tower9 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER9);
	SendMessage(tower9, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower9.bmp"));
	HWND tower10 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER10);
	SendMessage(tower10, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower10.bmp"));
	HWND tower11 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER11);
	SendMessage(tower11, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower11.bmp"));
	HWND tower12 = GetDlgItem(shHwnd, SHDL_BUTTON_TOWER12);
	SendMessage(tower12, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GetTexture(L"Tower12.bmp"));
	HWND goldText = GetDlgItem(shHwnd, SHDL_TEXT_GOLD_NUM);
	TCHAR gStr[100] = { NULL };
	wsprintf(gStr, L"소지 골드: %d", uGame->GetPlayerManager()->GetGold());
	SetDlgItemText(shHwnd, SHDL_TEXT_GOLD_NUM, gStr);

	uGame->GetBackground()->SetPutonState(true);
}

void UShopManager::SHDialog_Command(HWND shHwnd, WPARAM wParam)
{
	TowerButton(shHwnd, wParam);
	PurchaseButton(shHwnd, wParam);
}

void UShopManager::TowerButton(HWND hWnd, WPARAM wParam)
{
	InvalidateRect(hWnd, NULL, false);
	switch (LOWORD(wParam))
	{
	case SHDL_BUTTON_TOWER1:
		SetPictureControl(hWnd, L"Tower1.bmp", 0);
		break;
	case SHDL_BUTTON_TOWER2:
		SetPictureControl(hWnd, L"Tower2.bmp", 1);
		break;
	case SHDL_BUTTON_TOWER3:
		SetPictureControl(hWnd, L"Tower3.bmp", 2);
		break;
	case SHDL_BUTTON_TOWER4:
		SetPictureControl(hWnd, L"Tower4.bmp", 3);
		break;
	case SHDL_BUTTON_TOWER5:
		SetPictureControl(hWnd, L"Tower5.bmp", 4);
		break;
	case SHDL_BUTTON_TOWER6:
		SetPictureControl(hWnd, L"Tower6.bmp", 5);
		break;
	case SHDL_BUTTON_TOWER7:
		SetPictureControl(hWnd, L"Tower7.bmp", 6);
		break;
	case SHDL_BUTTON_TOWER8:
		SetPictureControl(hWnd, L"Tower8.bmp", 7);
		break;
	case SHDL_BUTTON_TOWER9:
		SetPictureControl(hWnd, L"Tower9.bmp", 8);
		break;
	case SHDL_BUTTON_TOWER10:
		SetPictureControl(hWnd, L"Tower10.bmp", 9);
		break;
	case SHDL_BUTTON_TOWER11:
		SetPictureControl(hWnd, L"Tower11.bmp", 10);
		break;
	case SHDL_BUTTON_TOWER12:
		SetPictureControl(hWnd, L"Tower12.bmp", 11);
		break;
	}
}

void UShopManager::PurchaseButton(HWND hWnd, WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case SHDL_BUTTON_PURCHASE:
		if (aSelectedTower != nullptr)
		{
			int currentHaveGold = uGame->GetPlayerManager()->GetGold();
			if (currentHaveGold - aSelectedTower->GetPayGold() >= 0)
			{
				ATower* copyTower = aSelectedTower;
				copyTower->SetInit(true);
				uGame->GetPlayerManager()->SetSelectedTower(copyTower);
				aSelectedTower = nullptr;
				EndDialog(hWnd, 0);
			}
			else
				MessageBox(hWnd, L"골드가 부족합니다.", L"실패", 0);
		}
		break;
	case IDCANCEL:
		uGame->GetBackground()->SetPutonState(false);
		EndDialog(hWnd, 0);
		break;
	}
}

void UShopManager::SetPictureControl(HWND hWnd, const TCHAR * name, int index)
{
	ATower* tower = clTowerList->GetData(index);
	if (tower != nullptr)
	{
		HWND listWnd = GetDlgItem(hWnd, SHDL_LIST_TOWER_EXPLAIN);
		if (listWnd != NULL)
		{
			HDC hdc = GetDC(hWnd);
			HDC memDC1 = CreateCompatibleDC(hdc);
			UPicture* picture = uGame->GetTexture(name);
			SelectObject(memDC1, picture->GetBitmap());
			StretchBlt(hdc, 390, 80, 183, 182, memDC1, 0, 0, picture->GetWidth(), picture->GetHeight(),  SRCCOPY);
			TCHAR explains[6][100] = { NULL };
			swprintf(explains[0], L"공격력: %.1f", tower->GetTowerStatus().GetAttackPower());
			swprintf(explains[1], L"공격 범위: %.1fm", tower->GetTowerStatus().GetAttackRange());
			swprintf(explains[2], L"공격 속도: %.1f", tower->GetTowerStatus().GetAttackSpeed());
			swprintf(explains[3], L"강화 등급당 파워 상승폭: %.1f", tower->GetTowerStatus().GetIncreaseAttackPower());
			swprintf(explains[4], L"강화 등급당 속도 상승폭: %.1f", tower->GetTowerStatus().GetIncreaseAttackSpeed());
			swprintf(explains[5], L"지불할 금화: %d", tower->GetPayGold());
			int count = static_cast<int>(SendMessage(listWnd, LB_GETCOUNT, 0, 0));
			if (count > 0)
				for (int i = 0; i < count; i++)
					SendMessage(listWnd, LB_DELETESTRING, 0, (LPARAM)i);

			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[0]);
			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[1]);
			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[2]);
			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[3]);
			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[4]);
			SendMessage(listWnd, LB_ADDSTRING, 0, (LPARAM)explains[5]);
			aSelectedTower = tower;
			DeleteDC(memDC1);
			ReleaseDC(hWnd, hdc);
		}
	}
}

HBITMAP UShopManager::GetTexture(const TCHAR* name)
{
	UPicture* picture = nullptr;
	int len = clPictureList->GetLength();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			picture = clPictureList->GetData(i);
			if (lstrcmp(picture->GetName(), name) == 0)
				return picture->GetBitmap();
		}
	}
	return NULL;
}
