#pragma once

#include "Process.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

// Windows 
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Client_Com_Macro.h"
#include "Client_Layer_Macro.h"
#include "Client_Proto_Macro.h"


extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern unsigned int		g_iCurrentLevel;

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOGO, LEVEL_MODE, LEVEL_SELECT, LEVEL_BATTLE, LEVEL_RATTAN, LEVEL_STORY, LEVEL_MINIGAME, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_ENDING, LEVEL_END };

	enum IMGUICLASS_TYPE
	{
		IMGCLASS_MAIN,  IMGCLASS_SHADERVIEW  ,
		IMGCLASS_OBJVIEW, IMGCLASS_LEVELCON,  IMGUI_AICONTROLLER , IMGCLASS_END
	};

	typedef enum EDIR : unsigned long
	{
		DIR_FORWARD = 0b00000001,
		DIR_BACK = 0b00000010,
		DIR_RIGHT = 0b00000100,
		DIR_LEFT = 0b00001000,
		DIR_FORRIGHT = 0b00000101,
		DIR_FORELEFT = 0b00001001,
		DIR_BACKRIGHT = 0b00000110,
		DIR_BACKLEFT = 0b00001010,
		DIR_END
	}DIR;
} 

using namespace Client;

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#define GRAVITY_VALUE -9.8f