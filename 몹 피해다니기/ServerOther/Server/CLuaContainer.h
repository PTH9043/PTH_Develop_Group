#pragma once

class CLuaContainer  {
private:
	using LUACONTINER = concurrency::concurrent_unordered_map<_string, lua_State*>;
	LUACONTINER		m_LuaContainer{};

	const _string			LUA_PATH = "..\\Resource\\";
public:
	CLuaContainer();
	~CLuaContainer();

public:
	void LoadLuaScript(const char* _pName);
	void RegisterMethodToLuaScript(int  (*_pFunction)(lua_State*), const char* _pLuaName, const char* _pFunctionName);
	lua_State* GetLuaState(const char* _pName);

private:
	lua_State* LoadLuaData(const char* _pName);
};

