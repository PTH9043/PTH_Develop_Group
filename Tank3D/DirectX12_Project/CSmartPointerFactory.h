#pragma once
#include <iostream>


template<typename T>
class  CSmartPointerFactory : public T
{
	//friend class T;
public:
	CSmartPointerFactory<T>() = default;
	CSmartPointerFactory<T>(const T& _rhs) : T(_rhs) {}
	virtual ~CSmartPointerFactory() = default;

	static std::shared_ptr<T> CreateShared()
	{
		return std::static_pointer_cast<T>(std::make_shared<CSmartPointerFactory<T>>());
	}

	static std::shared_ptr<T> CreateShared(T& _rhs)
	{
		return std::static_pointer_cast<T>(std::make_shared<CSmartPointerFactory<T>>(_rhs));
	}

};
