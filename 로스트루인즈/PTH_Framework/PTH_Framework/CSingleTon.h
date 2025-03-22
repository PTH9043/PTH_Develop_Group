#pragma once
#include "Include.h"


template <typename T>
class CSingleTon
{
protected:
	explicit CSingleTon(){}
	virtual ~CSingleTon(){}
protected:
	static T* m_pInstance;
public:
	static T* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new T;
		return m_pInstance;
	}
	static void Destory_Instance(){	DELETE_MEMORY(m_pInstance); }
protected:
};

template<typename T>
T* CSingleTon<T>::m_pInstance = nullptr;
