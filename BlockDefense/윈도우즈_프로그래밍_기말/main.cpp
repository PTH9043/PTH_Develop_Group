#include"UGame.h"
#include"UShopManager.h"
#include"UReinForceManager.h"
#include"UInfoManager.h"
#include"UStageManager.h"

LPCTSTR lpszClassName = TEXT("디펜스 게임");
LPCTSTR lpszWindowName = TEXT("디펜스 게임");
LPCTSTR lpszChildName = TEXT("Child");
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ReinForceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InfoProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ShopDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WinLoseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UGame* uGame = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	LPCTSTR lpszRFName = TEXT("강화소"), lpszIFName = TEXT("정보창");
	WNDCLASSEX WndClass{};
	MSG Message;
	HWND hWnd;
	uGame = new UGame(hInstance);
	WndClass.cbClsExtra = 0;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(244, 255, 255)));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClassName;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&WndClass);
	WndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.lpszClassName = lpszChildName;
	WndClass.lpfnWndProc = static_cast<WNDPROC>(ChildProc);
	RegisterClassEx(&WndClass);
	WndClass.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(240 , 240, 240)));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.lpszClassName = lpszRFName;
	WndClass.lpfnWndProc = static_cast<WNDPROC>(ReinForceProc);
	RegisterClassEx(&WndClass);
	WndClass.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(240, 240, 240)));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.lpszClassName = lpszIFName;
	WndClass.lpfnWndProc = static_cast<WNDPROC>(InfoProc);
	RegisterClassEx(&WndClass);
	hWnd = CreateWindow
	(
		lpszClassName,
		lpszWindowName,
		WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		WINDOW_START_POS_X,
		WINDOW_START_POS_Y,
		WINDOW_START_WIDTH,
		WINDOW_START_HEIGHT,
		NULL,
		static_cast<HMENU>(NULL),
		hInstance,
		NULL
	);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return static_cast<int>(Message.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		uGame->SetWindowHWND(hWnd);
		uGame->SetGameHWND(CreateWindow(lpszChildName, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			CHILD_START_POS_X, CHILD_START_POS_Y, CHILD_START_WIDTH, CHILD_START_HEIGHT, hWnd, NULL, uGame->GetInst(), NULL));
		uGame->Initialize();
		uGame->GetShopManager()->Initialize();
		uGame->SetWLDIalog((DLGPROC)WinLoseProc);
		break;
	case WM_COMMAND:
		uGame->WM_ButtonCommand(hWnd, wParam, (DLGPROC)ShopDialogProc);
		break;
	case WM_DESTROY:
		DELETE_MEMORY(uGame);
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{
	case WM_CREATE:

		break;
	case WM_LBUTTONDOWN:
		uGame->LeftMouseButtonClick(lParam);
		break;
	case WM_LBUTTONUP:
		uGame->UnLeftMouseButtonClick();
		break;
	case WM_RBUTTONDOWN:
		uGame->SetRightMouseClick(true);
		break;
	case WM_RBUTTONUP:
		uGame->SetRightMouseClick(false);
		break;
	case WM_MOUSEMOVE:
		uGame->Drag(lParam);
		break;
	case WM_TIMER:
		switch (LOWORD(wParam))
		{
		case 1:
			uGame->Update();
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		uGame->RenderScreen(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT ReinForceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{
	case WM_COMMAND:
		uGame->GetReinForceManager()->RFCW_Command(hWnd, wParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		uGame->GetReinForceManager()->RFCW_Draw(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		uGame->GetReinForceManager()->Reset();
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT InfoProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{
	case WM_COMMAND:
		uGame->GetInfoManager()->IFCW_Command(hWnd, wParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		uGame->GetInfoManager()->IFCW_Draw(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		uGame->GetInfoManager()->Reset();
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL ShopDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		uGame->GetShopManager()->SHDialog_Init(hWnd, wParam);
		break;
	case WM_COMMAND:
		uGame->GetShopManager()->SHDialog_Command(hWnd, wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	return false;
}

BOOL WinLoseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		uGame->GetStageManager()->SetWL(hWnd);
		uGame->GetStageManager()->WinLoseJudge(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case WLDL_RETURN:
			uGame->Regame();
			EndDialog(hWnd, 0);
			break;
		case WLDL_EXIT:
			PostQuitMessage(0);
			DELETE_MEMORY(uGame);
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	return false;
}
