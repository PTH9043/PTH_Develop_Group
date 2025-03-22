#pragma once
#pragma warning (disable : 4251)
#pragma region DX12

#pragma comment (lib, "d3d12")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "d3dcompiler")
#pragma comment (lib, "dinput8")


#ifdef _DEBUG
#pragma comment (lib, "DirectXTKd12")
#pragma comment (lib, "fmodL_vc")
#else
#pragma comment (lib, "DirectXTK12")
#pragma comment (lib, "fmod_vc")
#endif

#pragma comment (lib, "libxl")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imstb_rectpack.h"
#include "ImGui/imstb_textedit.h"
#include "ImGui/imstb_truetype.h"

#include "ImGui/ImGuizmo.h"
#include "ImGui/ImSequencer.h"
#include "ImGui/ImZoomSlider.h"
#include "ImGui/ImCurveEdit.h"
#include "ImGui/GraphEditor.h"

static const char* CLONETYPE_STRING{ "CloneType" };
static const char* CLONETYPE_STATIC_STRING{ "CLONETYPE_STATIC" };
static const char* CLONETYPE_ONCE_STRING{ "CLONETYPE_ONCE" };

// Use Atomic Shared PTr
#define _USE_ATOMIC
// Use Debugging Values
#define _USE_DEBUGGING
#define _USE_IMGUI
//#define _EDIT_NAVI

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")


#include <WS2tcpip.h>
#include <MSWSock.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi.h>
#include <DXGIDebug.h>

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXTK/GraphicsMemory.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/DescriptorHeap.h>
#include <DirectXTK/ResourceUploadBatch.h>
#include "DirectXTK/d3dx12.h"


#include "MathLib/CusMath.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "DirectXTK/GamePad.h"

#include <Windows.h>
#include <iostream>
#include <array>
#include <concepts>
#include <string_view>
#include <mutex>
#include <thread>
#include <set>
#include <unordered_set>
#include <stack>
#include <queue>
#include <list>
#include <vector>
#include <set>
#include <random>
#include <filesystem>
#include <unordered_map>
#include <atomic>
#include <fstream>
#include <locale>
#include <map>

#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <concurrent_priority_queue.h>
#include <concurrent_unordered_map.h>

#include "EnginePacket.h"
#include "EngineConcept.h"
#include "UDelegate.h"
#include "CustomMemory.h"
#include "CustomString.h"
#include "EngineTypedef.h"

#include "EngineEnum.h"
#include "EngineStruct.h"
#include "EngineFunction.h"
#include "EngineMacro.h"
#include "EngineMake.h"
#include "EngineShaderParam.h"

#include "UDelegate.h"

#pragma endregion DX12

namespace Engine {
	namespace DXVALUE {
		static const _float3					 g_BasicScale = _float3(1.f, 1.f, 1.f);
		static constexpr _uint				 g_maxRegisterCnt{ REGISTER_CNT };
		static const _tchar*					 g_OutFolderExtension{ L".bin" };
	}

	// Thread Local ID
	static thread_local int						g_threadID = 0;
}

#define BASIC_SCALE												Engine::DXVALUE::g_BasicScale
#define MAX_REGISTER_CNT								Engine::DXVALUE::g_maxRegisterCnt
#define EMPTY_WSTR											Engine::DXVALUE::g_EmptyWstr
#define DEFAULT_OUTFOLDEREXTENSION		Engine::DXVALUE::g_OutFolderExtension

using namespace Engine;
using FILEGROUPS = UNORMAP<_string, SHPTR<FILEGROUP>>;
using FOLDERPAIR = std::pair<_wstring, SHPTR<FILEGROUP>>;
using FILEPAIR = std::pair<_wstring, SHPTR<FILEDATA>>;

#define INVALID_MINUS_STAGEVALUE -1

