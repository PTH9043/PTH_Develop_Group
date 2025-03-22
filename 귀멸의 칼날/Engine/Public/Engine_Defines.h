#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 6031)
#pragma warning (disable : 4099)

#pragma region Direct11

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

#include <d3dcompiler.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/GeometricPrimitive.h>
#include <DirectXTK/SimpleMath.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "DirectXTK/GamePad.h"

#pragma endregion

#pragma region C++ STL
#include<Windows.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include<string>
#include <random>
#include<iostream>
#ifdef _DEBUG
#include<ostream>
#include <fstream>
#endif

using namespace DirectX;
using namespace TriangleTests;
using namespace std;

#pragma endregion

#pragma region Engine Headers

#include "Engine_ReDefine.h"
#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"
#include "Engine_Struct.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

using namespace Engine;

#pragma endregion