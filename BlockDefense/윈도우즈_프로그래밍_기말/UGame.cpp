#include "UGame.h"
#include"Data.h"
#include"AActor.h"
#include"ABackground.h"
#include"ABackground1.h"
#include"uPlayerManager.h"
#include"UShopManager.h"
#include"UReinForceManager.h"
#include"UInfoManager.h"
#include"UEnemyManager.h"
#include"UStageManager.h"

void UGame::Initialize()
{
	GetClientRect(hGameHwnd, &rGameClientRect);
	//HDC hdc = GetDC(hGameHwnd);
	clPictureList = new CustomList<UPicture*>();
	InitTexture();
	aBackground = new ABackground1(this, 1);
	aBackground->SetWidth(static_cast<double>(rGameClientRect.right));
	aBackground->SetHeight(static_cast<double>(rGameClientRect.bottom));
	aBackground->SetScale(Vector2{});
	aBackground->SetPos(Vector2{ static_cast<double>(rGameClientRect.right) / 2, static_cast<double>(rGameClientRect.bottom) / 2 });
	uPlayerManager = new UPlayerManager(this);
	uShopManager = new UShopManager(this);
	uReinForceManager = new UReinForceManager(this);
	uInfoManager = new UInfoManager(this);
	uEnemyManager = new UEnemyManager(this);
	uStageManager = new UStageManager(this);
	SetTimer(hGameHwnd, 1, DELTA_TIME, NULL);
	WindowInterfaceInit();
//	ReleaseDC(hGameHwnd, hdc);
	bIsGameEnd = false;
}

void UGame::Update()
{
	if (bIsGameEnd == false)
	{
		aBackground->Update(DELTA_TIME * 0.001f);
		uPlayerManager->Update(DELTA_TIME * 0.001f);
		uStageManager->Update(DELTA_TIME * 0.001f);
		InvalidateRect(hGameHwnd, NULL, false);
	}
}

void UGame::RenderScreen(HDC hdc)
{
	if (bIsGameEnd == false)
	{
		if(hGameScreen == NULL)	hGameScreen = CreateCompatibleBitmap(hdc, rGameClientRect.right, rGameClientRect.bottom);
		HDC memDC1 = CreateCompatibleDC(hdc), memDC2 = CreateCompatibleDC(memDC1), memDC3 = CreateCompatibleDC(memDC2);
		HBITMAP oldBit1 = static_cast<HBITMAP>(SelectObject(memDC1, hGameScreen));
		BitBlt(hdc, 0, 0, rGameClientRect.right, rGameClientRect.bottom, memDC1, 0, 0, SRCCOPY);
		aBackground->Draw(memDC1, memDC2, oldBit1);
		uPlayerManager->Draw(memDC1, memDC3, oldBit1);
		uStageManager->Draw(memDC1, memDC3, oldBit1);
		DeleteDC(memDC2); 	DeleteDC(memDC3);
		DeleteObject(oldBit1); DeleteDC(memDC1);
		ReleaseDC(hGameHwnd, hdc);
		// 화면을 그린다.
		DrawScreen(hdc);
	}
}

void UGame::LeftMouseButtonClick(LPARAM lParam)
{
	if (bIsGameEnd == false)
	{
		v2MousePosition = Vector2{ static_cast<double>(LOWORD(lParam)),  static_cast<double>(HIWORD(lParam)) };
		uPlayerManager->ClickLeftMouserButton(v2MousePosition);
		bIsLeftMouseClick = true;
	}
}

void UGame::UnLeftMouseButtonClick()
{
	if (bIsGameEnd == false)
	{
		bIsLeftMouseClick = false;
	}
}

void UGame::WM_ButtonCommand(HWND hWnd, WPARAM wParam, DLGPROC Proc)
{
	LPCTSTR lpszRFName = TEXT("강화소"), lpszIFName = TEXT("정보창");
	switch (LOWORD(wParam))
	{
	case MW_ID_BUTTON_START:
		uStageManager->SettingStage();
		break;
	case MW_ID_BUTTON_SHOP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_SHDL), this->hWnd, Proc);
		break;
	case MW_ID_BUTTON_INTENSTY:
		if (uReinForceManager->IsStart() == false)
		{
			uReinForceManager->RFCW_Initi(
				CreateWindow(lpszRFName, NULL, WS_CHILD | WS_VISIBLE | WS_POPUPWINDOW | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION,
					RF_START_POS_X, RF_START_POS_Y, RF_START_WIDTH, RF_START_HEIGHT, this->hWnd, NULL, hInst, NULL)
			);
		}
		break;
	case MW_ID_BUTTON_CHEAT:
		if (uPlayerManager->GetGold() <= 100000000)
			uPlayerManager->SetGold(uPlayerManager->GetGold() + 99999);
		else uPlayerManager->SetGold(100000000);
		UpdateHaveGold();
		break;
	case MW_ID_BUTTON_INFO:
		if (uInfoManager->IsStart() == false)
		{
			uInfoManager->IFCW_Initi(
				CreateWindow(lpszIFName, NULL, WS_CHILD | WS_VISIBLE | WS_POPUPWINDOW | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION,
					RF_START_POS_X, RF_START_POS_Y, RF_START_WIDTH, RF_START_HEIGHT, this->hWnd, NULL, hInst, NULL)
			);
		}
		break;
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

void UGame::Drag(LPARAM lParam)
{
	if (bIsGameEnd == false)
	{
		v2MousePosition = Vector2{ static_cast<double>(LOWORD(lParam)),  static_cast<double>(HIWORD(lParam)) };
		uPlayerManager->MouseDrag(v2MousePosition);
	}
}

UPicture* UGame::GetTexture(const TCHAR* str)
{
	UPicture* picture = nullptr;
	int len = clPictureList->GetLength();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			picture = clPictureList->GetData(i);
			if (lstrcmp(picture->GetName(), str) == 0)
				return picture;
		}
	}
	return picture;
}

UPicture* UGame::LoadTexture(const TCHAR* load, const TCHAR* str, int width, int height)
{
	TCHAR file[100];
	wsprintf(file, L"%s%s", load, str);
	HBITMAP bitmap = static_cast<HBITMAP>(LoadImage(NULL, file,
		IMAGE_BITMAP, width, height, LR_LOADFROMFILE));
	if (bitmap != NULL)
	{
		BITMAP bmp_info;
		GetObject(bitmap, sizeof(BITMAP), &bmp_info);
		UPicture* Picture = new UPicture(bitmap, str, bmp_info.bmWidth, bmp_info.bmHeight);
		return Picture;
	}
	return nullptr;
}

void UGame::UpdateTurnText()
{
	TCHAR turnStrArr[100];
	wsprintf(turnStrArr, L"턴: %d", uStageManager->GetStageCount());
	SendMessage(hTurnText, LB_DELETESTRING, 0, (LPARAM)0);
	SendMessage(hTurnText, LB_ADDSTRING, 0, (LPARAM)turnStrArr);
	InvalidateRect(hWnd, NULL, false);
}

void UGame::UpdateSurviveNum()
{
	TCHAR surviveStrArr[100];
	wsprintf(surviveStrArr, L"목숨 : %d", uPlayerManager->GetSurviveCount());
	SendMessage(hSurviveText, LB_DELETESTRING, 0, (LPARAM)0);
	SendMessage(hSurviveText, LB_ADDSTRING, 0, (LPARAM)surviveStrArr);
	InvalidateRect(hWnd, NULL, false);
}

void UGame::UpdateHaveGold()
{
	TCHAR haveGold[100];
	wsprintf(haveGold, L"소지 골드 : %d", uPlayerManager->GetGold());
	SendMessage(hHaveGold, LB_DELETESTRING, 0, (LPARAM)0);
	SendMessage(hHaveGold, LB_ADDSTRING, 0, (LPARAM)haveGold);
	InvalidateRect(hWnd, NULL, false);
}

void UGame::Regame()
{
	uPlayerManager->SetGold(500);
	uStageManager->SetStageCount(1);
	uPlayerManager->SetSurviveCount(20);
	uPlayerManager->GetTowerList()->Clear();
	UpdateHaveGold();
	UpdateSurviveNum();
	UpdateTurnText();
}

void UGame::InitTexture()
{
	clPictureList->Insert(LoadTexture(L"BMP/", L"background_map_1.bmp", 2000, 1500));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower_alpha1.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower_alpha2.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower1.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower2.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower3.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower4.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower5.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower6.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower7.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower8.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower9.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower10.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower11.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Tower12.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Bullet.bmp", 512, 512));
	clPictureList->Insert(LoadTexture(L"BMP/", L"Bullet_alpha.bmp", 512, 512));
}

void UGame::WindowInterfaceInit()
{
	TCHAR turnStrArr[100];
	wsprintf(turnStrArr, L"턴: %d", uStageManager->GetStageCount());
	hTurnText = CreateWindow(L"listbox", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 620, 100, 25, hWnd
		, (HMENU)(MW_ID_TEXT_TURN), hInst, NULL);
	SendMessage(hTurnText, LB_ADDSTRING, 0, (LPARAM)turnStrArr);
	TCHAR surviveStrArr[100];
	wsprintf(surviveStrArr, L"목숨 : %d", uPlayerManager->GetSurviveCount());
	hSurviveText = CreateWindow(L"listbox", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 170, 620, 150, 25, hWnd
		, (HMENU)(MW_ID_TEXT_TURN), hInst, NULL);
	SendMessage(hSurviveText, LB_ADDSTRING, 0, (LPARAM)surviveStrArr);
	TCHAR haveGold[100];
	wsprintf(haveGold, L"소지 골드 : %d", uPlayerManager->GetGold());
	hHaveGold = CreateWindow(L"listbox", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 330, 620, 150, 25, hWnd
		, (HMENU)(MW_ID_TEXT_HAVEGOLD), hInst, NULL);
	SendMessage(hHaveGold, LB_ADDSTRING, 0, (LPARAM)haveGold);
	hButtonArr[0] = CreateWindow(L"button", L"게임 시작", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 700, 100, 50, hWnd
		, (HMENU)(MW_ID_BUTTON_START), hInst, NULL);
	hButtonArr[1] = CreateWindow(L"button", L"상점 열기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 700, 100, 50, hWnd
	, (HMENU)(MW_ID_BUTTON_SHOP), hInst, NULL);
	hButtonArr[2] = CreateWindow(L"button", L"강화하기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 280, 700, 100, 50, hWnd
		, (HMENU)(MW_ID_BUTTON_INTENSTY), hInst, NULL);
	hButtonArr[3] = CreateWindow(L"button", L"정보보기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, 700, 100, 50, hWnd
		, (HMENU)(MW_ID_BUTTON_INFO), hInst, NULL);
	hButtonArr[4] = CreateWindow(L"button", L"치트", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 540, 700, 100, 50, hWnd
		, (HMENU)(MW_ID_BUTTON_CHEAT), hInst, NULL);
}

void UGame::DrawScreen(HDC hdc)
{
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP hOld = static_cast<HBITMAP>(SelectObject(memDC, hGameScreen));
	BitBlt(hdc, 0, 0, rGameClientRect.right, rGameClientRect.bottom, memDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
}

void UGame::Reset()
{
	bIsGameEnd = true;
	DELETE_MEMORY(uPlayerManager);	DELETE_MEMORY(aBackground);
	DELETE_MEMORY(clPictureList); DELETE_MEMORY(uShopManager);
	DELETE_MEMORY(uReinForceManager); DELETE_MEMORY(uInfoManager);
	KillTimer(hWnd, 1);
	DeleteObject(hInst); DeleteObject(hWnd); 
	DeleteObject(hGameHwnd); DeleteObject(hGameScreen);
	for (int i = 0; i < 3; i++) DeleteObject(hButtonArr[i]);
	DeleteObject(hTurnText);
}