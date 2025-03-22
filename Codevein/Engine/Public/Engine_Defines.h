#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)


#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

#include <d3dcompiler.h>
#include <Effects11/d3dx11effect.h>
#include <Effects11/d3dxGlobal.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include<Windows.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include<string>
#include<iostream>

using namespace std;
using namespace DirectX;
using namespace TriangleTests;

#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

#ifndef ENGINE_EXPORTS
#ifdef _DEBUG
#ifdef UNICODE                                                                                      
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else                                                                                                    
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
#endif 
#endif
#endif

using namespace Engine;