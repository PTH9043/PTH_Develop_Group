#pragma once
#pragma warning (disable : 4244)
#pragma warning (disable : 4018)
#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS

#include "EngineDefine.h"
#include "ProtocolBuffer.h"

#ifdef _DEBUG
#pragma comment(lib, "EngineD.lib")
#ifdef _ENABLE_PROTOBUFF
#pragma comment(lib, "ProtocolBufferD")
#endif
#else
#pragma comment(lib, "Engine.lib")
#ifdef _ENABLE_PROTOBUFF
#pragma comment(lib, "ProtocolBuffer")
#endif
#endif


namespace Client
{
	namespace DXVALUE
	{
		static unsigned short g_iWindowWidth = 1280;
		static unsigned short g_iWindowHeight = 1080;

		static float g_iWindowHalfWidth = (float)g_iWindowWidth * 0.5f;
		static float g_iWindowHalfHeight = (float)g_iWindowHeight * 0.5f;
	}

	enum SCENE {
		SCENE_LOGO, SCENE_STAGE1, SCENE_STAGE2, SCENE_END
	};

	enum WARRIORPLAYERSTATE
	{
		RUNSHIFT = 0,
		MOVEFRONT,
		MOVEBACK,
		MOVELEFT,
		MOVERIGHT,
		WATTACK,
		SATTACK,
		RATTACK,
		COMBO,
		ROLL,
		HIT,
		JUMP,
		RUNNING,
		MAX_WARRIORSTATE_CNT,
	};

	enum SKILL_COOLTIME {
		Q_SKILL = 5,
		ONE_SKILL = 12,
		TWO_SKILL=15,
		R_SKILL = 10


	};

	struct NETWORKWARRIORINPUTDESC
	{
		_bool		runshift;
		_bool		moveFront;
		_bool		moveBack;
		_bool		moveLeft;
		_bool		moveRight;
		_bool		WAttack;
		_bool		SAttack;
		_bool		RAttack;
		_bool		combo;
		_bool		Roll;
		_bool		Hit;
		_bool		Jump;
		_bool		warriorPlayerJumpState;
		_bool		warriorPlayerFallingState;
		_bool		runningState;

		NETWORKWARRIORINPUTDESC() :
			runshift{ false }, moveFront{ false }, moveBack{ false }, moveLeft{ false }
			, moveRight{ false }, WAttack{ false }, SAttack{ false }, RAttack{ false }, combo{ false }
			, Roll{ false }, Hit{ false }, Jump{ false }, warriorPlayerJumpState{ false },
			warriorPlayerFallingState{ false }, runningState{ false }
		{}
		NETWORKWARRIORINPUTDESC(_bool _isRunShift, _bool _isMoveFront, _bool _isMoveBack,
			_bool _isMoveLeft, _bool _isMoveRight, _bool _isWAttack, _bool _isSAttack, _bool _isRAttack,
			_bool _isCombo, _bool _isRoll, _bool _isHit, _bool _isJump, _bool _isWarriorPlayerJumpState,
			_bool _isWarriorPlayerFaillingState, _bool _isRunningState) :
			runshift{ _isRunShift }, moveFront{ _isMoveFront }, moveBack{ _isMoveBack }, moveLeft{ _isMoveLeft }
			, moveRight{ _isMoveRight }, WAttack{ _isWAttack }, SAttack{ _isSAttack }, RAttack{ _isRAttack }, combo{ _isCombo }
			, Roll{ _isRoll }, Hit{ _isHit }, Jump{ _isJump }, warriorPlayerJumpState{ _isWarriorPlayerJumpState },
			warriorPlayerFallingState{ _isWarriorPlayerFaillingState }, runningState{ _isRunningState }
		{}
	};

#define WINDOW_WIDTH					Client::DXVALUE::g_iWindowWidth
#define WINDOW_HEIGHT					Client::DXVALUE::g_iWindowHeight

#define WINDOW_HALF_WIDTH		Client::DXVALUE::g_iWindowHalfWidth
#define WINDOW_HALF_HEIGHT		Client::DXVALUE::g_iWindowHalfHeight

#define FIRST_RESOURCE_FOLDER L"..\\..\\Resource"
#define MAX_LOADSTRING 100
}

#include "ClientProto.h"

static const char* IP_ADDRESS{ "127.0.0.1" };

using namespace Client;

