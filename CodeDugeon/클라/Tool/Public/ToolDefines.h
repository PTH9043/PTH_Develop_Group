#ifndef _PTH_FRAMEWORK_TOOL_PUBLIC_TOOLDEFINES_H
#define _PTH_FRAMEWORK_TOOL_PUBLIC_TOOLDEFINES_H

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "AssimpD.lib")
#else
#pragma comment(lib, "Assimp.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "EngineD.lib")
#else
#pragma comment(lib, "Engine.lib")
#endif

#include "EngineDefine.h"


namespace Tool
{
	namespace DXVALUE
	{
		static const unsigned short g_iWindowWidth = 1280;
		static const unsigned short g_iWindowHeight = 1080;

		static const float g_iWindowHalfWidth = (float)g_iWindowWidth * 0.5f;
		static const float g_iWindowHalfHeight = (float)g_iWindowHeight * 0.5f;
	}

	enum SCENE {
		SCENE_MAIN, SCENE_END
	};

	enum class IMGTAG : _ubyte {
		MAIN, MODELCONVERTER, CAMERATOOL,PARTICLETOOL, ROTATIONEFFECTTOOL, BLOODEFFECTTOOL, FIRETOOL, FOGTOOL ,ANIMCONTROLVIEW, NAVITOOL, MAPTOOL, EMPTY
	};

#define WINDOW_WIDTH					Tool::DXVALUE::g_iWindowWidth
#define WINDOW_HEIGHT					Tool::DXVALUE::g_iWindowHeight

#define WINDOW_HALF_WIDTH		Tool::DXVALUE::g_iWindowHalfWidth
#define WINDOW_HALF_HEIGHT		Tool::DXVALUE::g_iWindowHalfHeight

#define FIRST_RESOURCE_FOLDER L"..\\..\\Resource"
}

#include "ToolMaker.h"

using namespace Tool;


#endif