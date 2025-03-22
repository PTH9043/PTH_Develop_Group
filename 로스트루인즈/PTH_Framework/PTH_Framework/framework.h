#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <list>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm>
#include <time.h>

#include <Vfw.h>
#pragma comment(lib, "vfw32.lib")

#include <io.h>

#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include<crtdbg.h>
#ifndef DBG_NEW

#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif
#endif

using namespace std;