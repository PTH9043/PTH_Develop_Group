// DirectX12_Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "DirectX12_Project.h"
#include "Defines.h"
#include "CGameApp.h"
#include "CGameInstance.h"
#include "CTimer.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HWND g_hWnd{ NULL };
HINSTANCE hInst{ NULL };                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

shared_ptr<CGameApp> pGameApp = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DIRECTX12PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
    {
        pGameApp = CreateNew<CGameApp>(g_hWnd, hInst);
        // 기본 메시지 루프입니다:
        pGameInstance->Add_Timer(L"DELTA_SECONDS_TIMER");
        pGameInstance->Add_Timer(L"FPS_TIMER");
        shared_ptr<CTimer> DeltaTimer = pGameInstance->Find_Timer(L"DELTA_SECONDS_TIMER");
        shared_ptr<CTimer> FpsTimer = pGameInstance->Find_Timer(L"FPS_TIMER");
        _double TimeDelta = 0.0;
        _bool bDone = false;
        while (!bDone)
        {
            while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                {
                    bDone = true;
                    break;
                }
            }

            // Run Func
            DeltaTimer->Run_Func();

            TimeDelta += DeltaTimer->Get_TimeDelta();
            if (TimeDelta >= 1 / 60.f)
            {
                FpsTimer->Run_Func();
                TimeDelta = FpsTimer->Get_TimeDelta();

                pGameApp->Tick(TimeDelta);
                // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
                pGameApp->Render();

                TimeDelta = 0.0;
            }
        }
        DeltaTimer.reset();
        FpsTimer.reset();
    }
    pGameInstance.reset();
    Safe_Free(pGameApp);
    return (int) msg.wParam;
}
//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTX12PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   RECT rc{ 0, 0, WINDOW_WIDTH , WINDOW_HEIGHT };
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, true);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   g_hWnd = hWnd;
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(nullptr != pGameApp)
        pGameApp->KeyHandler(message, wParam);
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}