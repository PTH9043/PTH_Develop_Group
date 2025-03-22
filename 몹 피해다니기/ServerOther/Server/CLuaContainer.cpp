#include "Defines.h"
#include "CLuaContainer.h"

CLuaContainer::CLuaContainer()
{
}

CLuaContainer::~CLuaContainer()
{
}

void CLuaContainer::LoadLuaScript(const char* _pName)
{
	lua_State* pMonsterLuaScrip = LoadLuaData(_pName);
	m_LuaContainer.insert(std::pair<_string, lua_State*>(_pName, pMonsterLuaScrip));
}

void CLuaContainer::RegisterMethodToLuaScript( int  (*_pFunction)(lua_State*), const char* _pLuaName, const char* _pFunctionName)
{
	const auto& iter =m_LuaContainer.find(_pLuaName);
	if (iter == m_LuaContainer.end())
		return;

	lua_register(iter->second, _pFunctionName, _pFunction);
}

lua_State* CLuaContainer::GetLuaState(const char* _pName)
{
	const auto& iter = m_LuaContainer.find(_pName);
	if (iter == m_LuaContainer.end())
		return nullptr;
	return iter->second;
}

lua_State* CLuaContainer::LoadLuaData(const char* _pName)
{
	_string PATH = LUA_PATH;
	PATH += _pName;
	PATH += ".lua";

	lua_State* pState = luaL_newstate();
	luaL_openlibs(pState);
	luaL_loadfile(pState, PATH.c_str());
	lua_pcall(pState, 0, 0, 0);
	return pState;
}
