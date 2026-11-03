#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS 
#include "UInfoManager.h"
#include"ATower.h"
#include"USimpleSpriteComponent.h"
#include"UPlayerManager.h"
#include"ABackground.h"

void UInfoManager::IFCW_Initi(HWND hWnd)
{
	bIsStart = true;
	hIF_Hwnd = hWnd;
	Create();
}

void UInfoManager::IFCW_Command(HWND hWnd, WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case IFCW_ID_BUTTON_SELL:
		if (aSelectedTower != nullptr)
		{
			uGame->GetPlayerManager()->SetGold(uGame->GetPlayerManager()->GetGold() + aSelectedTower->GetSellGold());
			uGame->GetPlayerManager()->GetTowerList()->DeleteToData(aSelectedTower);
			uGame->GetBackground()->GetTowerPosList()->GetData(aSelectedTower->GetPosIndex())->SetDraw(true);
			uGame->GetBackground()->GetTowerPosList()->GetData(aSelectedTower->GetPosIndex())->SetPutonTower(false);
			aSelectedTower = nullptr;
			int count = static_cast<int>(SendMessage(hIF_ListExplain, LB_GETCOUNT, 0, 0));
			if (count > 0)
				for (int i = 0; i < count; i++)
					SendMessage(hIF_ListExplain, LB_DELETESTRING, 0, (LPARAM)i);

			uGame->UpdateHaveGold();
			InvalidateRect(hIF_Hwnd, NULL, true);
		}
		break;
	case IFCW_ID_BUTTON_EXIT:
		bIsStart = false;
		aSelectedTower = nullptr;
		DestroyWindow(hWnd);
		break;
	}
}

void UInfoManager::IFCW_Draw(HWND hWnd, HDC hdc)
{
	if (aSelectedTower != nullptr)
	{
		HDC memDC1 = CreateCompatibleDC(hdc);
		UPicture* picture = aSelectedTower->GetSimpleSpriteComp()->GetPicture();
		SelectObject(memDC1, picture->GetBitmap());
		StretchBlt(hdc, 140, 0, 300, 280, memDC1, 0, 0, picture->GetWidth(), picture->GetHeight(), SRCCOPY);
		DeleteDC(memDC1);
	}
	else Rectangle(hdc, 140, 0, 440, 280);
}

void UInfoManager::Reset()
{
	bIsStart = false;
	aSelectedTower = nullptr;
}

void UInfoManager::Create()
{
	hIF_ListExplain = CreateWindow(L"listbox", L"ListBox", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY, 140, 280, 300, 150, hIF_Hwnd,
		(HMENU)(IFCW_ID_LIST_EXPLAIN), uGame->GetInst(), NULL);
	hIF_ButtonSL = CreateWindow(L"button", L"판매", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, 500, 100, 50, hIF_Hwnd,
		(HMENU)(IFCW_ID_BUTTON_SELL), uGame->GetInst(), NULL);
	hIF_ButtonExit = CreateWindow(L"button", L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 500, 100, 50, hIF_Hwnd,
		(HMENU)(IFCW_ID_BUTTON_EXIT), uGame->GetInst(), NULL);
	hIF_TextGold = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | BS_TEXT, 140, 430, 300, 20, hIF_Hwnd,
		(HMENU)(IFCW_ID_LIST_EXPLAIN), uGame->GetInst(), NULL);
}

void UInfoManager::UpdateSelectTowerInfo()
{
	if (aSelectedTower != nullptr)
	{
		TCHAR explains[5][100] = { NULL };
		swprintf(explains[0], L"공격력: %.1f", aSelectedTower->GetTowerStatus().GetAttackPower());
		swprintf(explains[1], L"공격 범위: %.1fm", aSelectedTower->GetTowerStatus().GetAttackRange());
		swprintf(explains[2], L"공격 속도: %.1f", aSelectedTower->GetTowerStatus().GetAttackSpeed());
		swprintf(explains[3], L"강화 등급: %d", aSelectedTower->GetTowerStatus().GetReinforceGrade());
		swprintf(explains[4], L"판매 비용: %d", aSelectedTower->GetSellGold());
		int count = static_cast<int>(SendMessage(hIF_ListExplain, LB_GETCOUNT, 0, 0));
		if (count > 0)
			for (int i = 0; i < count; i++)
				SendMessage(hIF_ListExplain, LB_DELETESTRING, 0, (LPARAM)i);

		SendMessage(hIF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[0]);
		SendMessage(hIF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[1]);
		SendMessage(hIF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[2]);
		SendMessage(hIF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[3]);
		SendMessage(hIF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[4]);
	}
}

void UInfoManager::SetSelectedTower(ATower* tower)
{
	if (aSelectedTower != tower)
	{
		aSelectedTower = tower;
		int payGold = (aSelectedTower->GetPayGold() / 10) * 9, reinforceGold = (aSelectedTower->GetPrevReinForceGold() / 10) * 9;
		aSelectedTower->SetSellGold(payGold + reinforceGold);
		UpdateSelectTowerInfo();
		InvalidateRect(hIF_Hwnd, NULL, false);
	}
}
