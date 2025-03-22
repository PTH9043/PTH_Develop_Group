#pragma once
#include "Process.h"
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include<iostream>

extern HINSTANCE		g_hInst;
extern HWND				g_hWnd;
extern bool					g_bGameRun;
extern unsigned int	g_iCurrentLevel;

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_TOOL, LEVEL_LOGO, LEVEL_GAMEPLAY1, LEVEL_GAMEPLAY2,
		LEVEL_LOADING, LEVEL_END };
}

using namespace Client;

#include "Client_Macro.h"
#include"Client_Proto_Macro.h"
#include "Client_Layer_Macro.h"
#include "Client_Com_Macro.h"
#include "Client_Struct.h"