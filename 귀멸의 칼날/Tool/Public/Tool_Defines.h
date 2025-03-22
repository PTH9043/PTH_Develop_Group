#pragma once
#include "Process.h"
#include "..\\Default\targetver.h"
#define WIN32_LEAN_AND_MEAN            

#include "ImGuiFileLoad_Tool.h"

// Windows 
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <direct.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ ) 
#define new DBG_NEW 

#endif

extern HINSTANCE		g_hInst;
extern HWND				g_hWnd;

extern bool				g_bGameRun;
extern bool				g_bToolReset;

extern unsigned int		g_iCurrentLevel;

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL
	{
		LEVEL_STATIC, LEVEL_TOOL, LEVEL_LOADING, LEVEL_REMOVE, LEVEL_END
	};

	enum IMGUICLASS_TYPE
	{
		IMGCLASS_MAIN, IMGCLASS_MAPVIEWER, IMGCLASS_FILEPATH, IMGCLASS_UI, IMGCLASS_EFFECT,
		IMGCLASS_ANIMVIEWER, IMGCLASS_CAMVIEWER, IMGCLASS_SHADERVIEW, IMGCLASS_CUTSCENE, IMGCLASS_END
	};
}

using namespace Tool;

#include"Tool_Proto_Macro.h"
#include "Tool_Layer_Macro.h"
#include "Tool_Com_Macro.h"