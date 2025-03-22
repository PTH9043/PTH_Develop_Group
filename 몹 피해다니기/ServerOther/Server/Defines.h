#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <concurrent_priority_queue.h>
#include <cmath>
#include "..\..\protocol_2023.h"
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <string>
#include <vector>
#include <concurrent_vector.h>
#include <array>
#include <algorithm>
#include <Windows.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "PTHLib/CusMath.h"
#include <list>
#include <fstream>
#include <numeric>
#include <random>
#include <concurrent_unordered_set.h>
#include<set>

extern "C"
{
#include "include\lua.h"
#include "include\lauxlib.h"
#include "include\lualib.h"
}

using namespace std;


using _string = std::string;
using ID = int;
using _float = float;
using _uint = unsigned int;
using _byte = signed char;
using _float2 = DirectX::PTH::Vector2;
using MAPARR = std::array<std::array<_byte, W_WIDTH>, W_HEIGHT>;
// SITUATION을 만족시켰으면 리턴
#define RETURN_CHECK(SITUATION, RETURN) \
if(SITUATION){  return RETURN; }

// 가운데 Values를 조건을 SITUATION을 만족 시켰으면 실행
#define RETURN_CHECK_RUNVALUES(SITUATION, VALUES, RETURN) \
if(SITUATION){ VALUES return RETURN; }

#define RETURN_CHECK_FAILED(SITUATION, RETURN) \
if(FAILED(SITUATION)){ return RETURN; }

#define RETURN_CHECK_RUNVALUES_FAILED(SITUATION, VALUES,RETURN) \
if(FAILED(SITUATION)){ VALUES return RETURN; }

template<typename T>
static void Safe_Delete(T& pPointer)
{
	if (nullptr != pPointer)
	{
		delete pPointer;
		pPointer = nullptr;
	}
}

template<typename T>
static void Safe_Delete_Array(T& pPointer)
{
	if (nullptr != pPointer)
	{
		delete[] pPointer;
		pPointer = nullptr;
	}
}