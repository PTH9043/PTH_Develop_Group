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

}

