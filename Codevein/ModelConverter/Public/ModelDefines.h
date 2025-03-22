#pragma once

#include "Process.h"
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#pragma warning (disable : 4800)

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
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

namespace Model
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_TOOL, LEVEL_LOGO, LEVEL_GAMEPLAY1, LEVEL_LOADING, LEVEL_END };
}

using namespace Model;

using namespace Assimp;

#include "ModelMacro.h"
#include "Model_Com_Macro.h"
#include "Model_Layer_Macro.h"
#include "Model_Proto_Macro.h"
