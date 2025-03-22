#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ALUALOADER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ALUALOADER_H

BEGIN(Core)
/*
@ Date: 2024-01-13, Writer: 박태현
@ Explain
- Lua Script를 로드하는 Loader 클래스입니다. 
 해당 lua Script를 로드할 때, 경로와 .lua를 제외하고 이름만 적어서 로드합니다.
*/
class ALuaLoader final {
public:
	// 파일 이름만 적으면 로드가 됩니다. 
	ALuaLoader(const _string& _strFilename);
	~ALuaLoader();
public:
	void LuaFunctionCall(const _char* _FunctionName);
	
	template<class T>
	requires CheckNumber<T>
	void PushNumber(T _number){
		lua_tonumber(m_pLuaState, _number);
	}
	void PushString(const _char* _String);
	void PushBoolean(_bool _boolean);

	void LuaFunctionEnd(const _int _parameterNum, const _int _returnNum);

	template<class T>
	requires CheckNumber<T>
	// -1은 스택 최상위에 있다는 이야기이다.
	T ToNumber(_int _order = -1) 
	{ 
		T value = lua_tonumber(m_pLuaState, _order); 
		lua_pop(m_pLuaState, _order);
		return value;
	}
	_bool ToBoolean(_int _order = -1);
	void* ToPointer(_int _order = -1);
public: /* get set */
	lua_State* GetLuaState() const { return m_pLuaState; }

private:
	_bool LuaCreate(const _string& _strFilename);

private:
	static constexpr const char* LUA_PATH{ "..\\..\\LuaScripts\\" };
	static constexpr const char* LUA_EXT{ ".lua" };
private:
	lua_State*		m_pLuaState;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ULUALOADER_H