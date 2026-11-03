#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS 
#include "UReinForceManager.h"
#include"UPlayerManager.h"
#include"ATower.h"
#include"USimpleSpriteComponent.h"

void UReinForceManager::RFCW_Initi(HWND hWnd)
{
	bIsStart = true;
	hRF_Hwnd = hWnd;
	Create();
	UpdatePlayerHaveGold();
}

void UReinForceManager::RFCW_Command(HWND hWnd, WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case RFCW_ID_BUTTON_REINFORCE:
		if (aSelectedTower != nullptr)
		{
			int gold = uGame->GetPlayerManager()->GetGold() - aSelectedTower->GetPayReinForceGold();
			if (gold >= 0)
			{
				uGame->GetPlayerManager()->SetGold(gold);
				UTowerStatus towerStatus = aSelectedTower->GetTowerStatus(); towerStatus.ReinforceTower();
				aSelectedTower->SetTowerStatus(towerStatus);
				aSelectedTower->SetPrevReinForceGold(aSelectedTower->GetPayReinForceGold());
				aSelectedTower->SetPayReinForceGold(aSelectedTower->GetPayReinForceGold() 
					+	(int)(aSelectedTower->GetPayReinForceGold() * 0.35));
				UpdateSelectTowerInfo();
				UpdatePlayerHaveGold();
				uGame->UpdateHaveGold();
			}
		}
		else	MessageBox(hWnd, L"선택된 타워가 없습니다.", L"실패", 0);
		break;
	case RFCW_ID_BUTTON_EXIT:
		bIsStart = false;
		aSelectedTower = nullptr;
		DestroyWindow(hWnd);
		break;
	case IDCANCEL:
		bIsStart = false;
		aSelectedTower = nullptr;
		DestroyWindow(hWnd);
		break;
	}
}

void UReinForceManager::RFCW_Draw(HWND hWnd, HDC hdc)
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

void UReinForceManager::Reset()
{
	bIsStart = false;
	aSelectedTower = nullptr;
}

void UReinForceManager::UpdateSelectTowerInfo()
{
	if (aSelectedTower != nullptr)
	{
		TCHAR explains[7][100] = { NULL };
		swprintf(explains[0], L"공격력: %.1f", aSelectedTower->GetTowerStatus().GetAttackPower());
		swprintf(explains[1], L"공격 범위: %.1fm", aSelectedTower->GetTowerStatus().GetAttackRange());
		swprintf(explains[2], L"공격 속도: %.1f", aSelectedTower->GetTowerStatus().GetAttackSpeed());
		swprintf(explains[3], L"강화 등급당 파워 상승폭: %.1f", aSelectedTower->GetTowerStatus().GetIncreaseAttackPower());
		swprintf(explains[4], L"강화 등급당 속도 상승폭: %.1f", aSelectedTower->GetTowerStatus().GetIncreaseAttackSpeed());
		swprintf(explains[5], L"강화 등급: %d", aSelectedTower->GetTowerStatus().GetReinforceGrade());
		swprintf(explains[6], L"강화 비용: %d", aSelectedTower->GetPayReinForceGold());
		int count = static_cast<int>(SendMessage(hRF_ListExplain, LB_GETCOUNT, 0, 0));
		if (count > 0)
			for (int i = 0; i < count; i++)
				SendMessage(hRF_ListExplain, LB_DELETESTRING, 0, (LPARAM)i);

		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[0]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[1]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[2]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[3]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[4]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[5]);
		SendMessage(hRF_ListExplain, LB_ADDSTRING, 0, (LPARAM)explains[6]);
	}
}

void UReinForceManager::Create()
{
	hRF_ListExplain = CreateWindow(L"listbox", L"ListBox", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY, 140, 280, 300, 150, hRF_Hwnd,
		(HMENU)(RFCW_ID_LIST_EXPLAIN), uGame->GetInst(), NULL);
	hRF_ButtonRF = CreateWindow(L"button", L"강화", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, 500, 100, 50, hRF_Hwnd,
		(HMENU)(RFCW_ID_BUTTON_REINFORCE), uGame->GetInst(), NULL);
	hRF_ButtonExit = CreateWindow(L"button", L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 500, 100, 50, hRF_Hwnd,
		(HMENU)(RFCW_ID_BUTTON_EXIT), uGame->GetInst(), NULL);
	hRF_TextGold = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE  | BS_TEXT, 140, 430, 300, 20, hRF_Hwnd,
		(HMENU)(RFCW_ID_LIST_EXPLAIN), uGame->GetInst(), NULL);
}

void UReinForceManager::UpdatePlayerHaveGold()
{
	if (hRF_TextGold == NULL) return;
	TCHAR gStr[100] = { NULL };
	wsprintf(gStr, L"소지 골드: %d", uGame->GetPlayerManager()->GetGold());
	SetWindowText(hRF_TextGold, gStr);
}

void UReinForceManager::SetSelectedTower(ATower* tower)
{
	if (aSelectedTower != tower)
	{
		aSelectedTower = tower;
		UpdateSelectTowerInfo();
		InvalidateRect(hRF_Hwnd, NULL, false);
	}
}

