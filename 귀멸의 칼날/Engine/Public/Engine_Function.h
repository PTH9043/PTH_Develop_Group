#pragma once

namespace Engine
{
	template<typename T>
	void Swap(T& _t1, T& _t2)
	{
		T temp = _t1;
		_t1 = _t2;
		_t2 = temp;
	}

	template<typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete[] pPointer;
			pPointer = nullptr;
		}
	}

#ifdef _DEBUG
	template<typename T>
	unsigned long Safe_AddRef_M(T& pInstance, const _char* FILE ,
		const _uint& LINE , const _char* TIME )
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->AddRef(FILE, LINE, TIME);
		}

		return dwRefCnt;
	}
	template<typename T>
	 unsigned long Safe_Release_M(T& pInstance, const _char* FILE,
		const _uint& LINE, const _char* TIME)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release(FILE, LINE, TIME);
			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}
#else

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}
#endif

	template<typename T>
	unsigned long Safe_AddRef_DV(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release_DV(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	class CTag_Finder
	{
	private:
		const wchar_t*	m_pStr;

	public:
		CTag_Finder(const wchar_t* pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (!lstrcmp(m_pStr, Pair.first))
				return true;

			return false;
		}
	};

	class CTag_Finder_WS
	{
	private:
		const wchar_t* m_pStr;

	public:
		CTag_Finder_WS(const wchar_t* pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (!lstrcmp(m_pStr, Pair.first.c_str()))
				return true;

			return false;
		}
	};

	template<typename T>
	_bool Is_Nullptr_M(T& _data, const _char* FILE,
		const _uint& LINE, const _char* FUNC)
	{
		if (nullptr == _data)
		{
			char	szMessage[MAX_PATH] = "";
			strcpy_s(szMessage, typeid(T).name());
			_string ClassName = szMessage;
			_string str = " Is Nullptr";
			str += "\nFILE: ";
			str += FILE;
			str += "\n LINE: ";
			str += to_string(LINE);
			str += "\n";
			str += FUNC;
			ClassName.append(str);
#ifdef _DEBUG
			MessageBoxA(0, ClassName.c_str(), nullptr, MB_OK);
#endif
			return true;
		}
		return false;
	}

	static void DEBUGGIN_MESSAGE_PTH(const _string& _pMessage)
	{
#ifdef _DEBUG
		MessageBoxA(0, _pMessage.c_str(), nullptr, MB_OK);
#endif
	}
}

