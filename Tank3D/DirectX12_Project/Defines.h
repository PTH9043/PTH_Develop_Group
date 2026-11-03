#pragma once 
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <unordered_map>
#include <list>
#include <array>
#include<iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")


#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>


using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;


#include "CDelegate.h"
#include "CSmartPointerFactory.h"

#include "DxRedefines.h"
#include "DxTypedef.h"
#include "DxEnumes.h"
#include "DxStruct.h"
#include "DxMethod.h"
#include "DxMacro.h"

namespace DXVALUE
{
	static const unsigned int g_iWindowWidth = 640;
	static const unsigned int g_iWindowHeight = 480;

	static const float g_iWindowHalfWidth = (float)g_iWindowWidth * 0.5f;
	static const float g_iWindowHalfHeight = (float)g_iWindowHeight * 0.5f;

	static const unsigned char g_ucDirForward = 0x01;
	static const unsigned char g_ucDirBackward = 0x02;
	static const unsigned char g_ucDirLeft = 0x04;
	static const unsigned char g_ucDirRight = 0x08;
	static const unsigned char g_ucDirUp = 0x10;
	static const unsigned char g_ucDirDown = 0x20;

	static const unsigned char g_ucKeyBoard_W = 'W';
	static const unsigned char g_ucKeyBoard_A = 'A';
	static const unsigned char g_ucKeyBoard_S = 'S';
	static const unsigned char g_ucKeyBoard_D = 'D';

	static const float g_fEpsilon = 1.0e-6f;

	static const _float4x4 g_Idetify = _float4x4( 1.f, 0, 0, 0,
																		0, 1.f, 0, 0,
																		0, 0, 1.f, 0,
																		0, 0, 0, 1.f );

	static const _float3 g_Up = _float3(0.f, 1.f, 0.f);
	static const _float3 g_Right = _float3(1.f, 0.f, 0.f);
	static const _float3 g_BasicScale = _float3(1.f, 1.f, 1.f);
	static const _float3 g_Vector3Zero = _float3(0.f, 0.f, 0.f);

	static const _float	g_Gravity = -0.98f;
}

#define WINDOW_WIDTH					DXVALUE::g_iWindowWidth
#define WINDOW_HEIGHT					DXVALUE::g_iWindowHeight

#define WINDOW_HALF_WIDTH		DXVALUE::g_iWindowHalfWidth
#define WINDOW_HALF_HEIGHT		DXVALUE::g_iWindowHalfHeight

#define DIR_FORWARD						DXVALUE::g_ucDirForward
#define DIR_BACKWARD					DXVALUE::g_ucDirBackward
#define DIR_LEFT									DXVALUE::g_ucDirLeft
#define DIR_RIGHT								DXVALUE::g_ucDirRight
#define DIR_UP										DXVALUE::g_ucDirUp
#define DIR_DOWN								DXVALUE::g_ucDirDown

#define KEYBOARD_W						DXVALUE::g_ucKeyBoard_W
#define KEYBOARD_A							DXVALUE::g_ucKeyBoard_A
#define KEYBOARD_S							DXVALUE::g_ucKeyBoard_S
#define KEYBOARD_D							DXVALUE::g_ucKeyBoard_D

#define EPSILON									DXVALUE::g_fEpsilon
#define IDENTIFY									DXVALUE::g_Idetify
#define UP_VECTOR							DXVALUE::g_Up
#define RIGHT_VECTOR						DXVALUE::g_Right
#define BASIC_SCALE							DXVALUE::g_BasicScale
#define VECTOR3_ZERO					DXVALUE::g_Vector3Zero
#define GRAVITY									DXVALUE::g_Gravity

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

template<typename T>
static shared_ptr<T> CreateNew() {
	shared_ptr<T> pInstance = CSmartPointerFactory<T>::CreateShared();
	if (FAILED(pInstance->NativeConstruct())) {
		Safe_Free(pInstance);
		_string ID = typeid(T).name();
		DEBUG_MSG(ID + "Create To Failed");
	}
	return pInstance;
}


template<typename T>
static shared_ptr<T> CreateProto() {
	shared_ptr<T> pInstance = CSmartPointerFactory<T>::CreateShared();
	if (FAILED(pInstance->NativeConstruct_Prototype())) {
		Safe_Free(pInstance);
		_string ID = typeid(T).name();
		DEBUG_MSG(ID + "Create To Failed");
	}
	return pInstance;
}

template<typename T, typename... Args>
static shared_ptr<T> CreateNew(Args... args) {
	shared_ptr<T> pInstance = CSmartPointerFactory<T>::CreateShared();
	if (FAILED(pInstance->NativeConstruct(args...))) {
		Safe_Free(pInstance);
		_string ID = typeid(T).name();
		DEBUG_MSG(ID + "Create To Failed");
	}
	return pInstance;
}


template<typename T, typename... Args>
static shared_ptr<T> CreateProto(Args...  args) {
	shared_ptr<T> pInstance = CSmartPointerFactory<T>::CreateShared();
	if (FAILED(pInstance->NativeConstruct_Prototype(args...))) {
		Safe_Free(pInstance);
		_string ID = typeid(T).name();
		DEBUG_MSG(ID + "Create To Failed");
	}
	return pInstance;
}