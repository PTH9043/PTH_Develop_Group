#include "CoreDefines.h"
#include "ALuaLoader.h"

namespace Core {

	ALuaLoader::ALuaLoader(const _string& _strFilename) : m_pLuaState{ nullptr}
	{
		if (false == LuaCreate(_strFilename))
		{
			CRASH("LuaScript Load Failed");
		}
	}

	ALuaLoader::~ALuaLoader()
	{
		lua_close(m_pLuaState);
	}

	void ALuaLoader::LuaFunctionCall(const _char* _FunctionName)	{
		lua_getglobal(m_pLuaState, _FunctionName);
	}

	void ALuaLoader::PushString(const _char* _String)	{
		lua_pushstring(m_pLuaState, _String);
	}

	void ALuaLoader::PushBoolean(_bool _boolean){
		lua_pushboolean(m_pLuaState, _boolean);
	}

	void ALuaLoader::LuaFunctionEnd(const _int _parameterNum, const _int _returnNum)
	{
		lua_call(m_pLuaState, _parameterNum, _returnNum);
	}

	_bool ALuaLoader::ToBoolean(_int _order)
	{
		_int value = lua_toboolean(m_pLuaState, _order);
		lua_pop(m_pLuaState, _order);
		return value == 1;
	}

	void* ALuaLoader::ToPointer(_int _order)
	{
		const void* Ptr = lua_topointer(m_pLuaState, _order);
		lua_pop(m_pLuaState, _order);
		return const_cast<void*>(Ptr);
	}

	_bool ALuaLoader::LuaCreate(const _string& _strFilename)
	{
		// 합쳐진 경로
		_string Path = LUA_PATH + _strFilename + LUA_EXT;

		// 루아 상태 생성 후, 표준 라이브러리 로드
		m_pLuaState = luaL_newstate();
		luaL_openlibs(m_pLuaState);
		RETURN_CHECK(luaL_dofile(m_pLuaState, Path.c_str()) != LUA_OK, false);
		// 루아 로드 성공 
		return true;
	}
}