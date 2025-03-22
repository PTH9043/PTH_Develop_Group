#pragma once
#include "pch.h"

class CDeleteObj
{
public:
	template<typename T>
	void operator()(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}
};

class CDeleteMap
{
public:
	template<typename T>
	void operator()(T& Pair)
	{
		if (Pair.second)
		{
			delete Pair.second;
			Pair.second = nullptr;
		}
	}
};

class CTag_Finder
{
private:
	const TCHAR*	m_pStr;

public:
	CTag_Finder(const TCHAR* pStr)
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