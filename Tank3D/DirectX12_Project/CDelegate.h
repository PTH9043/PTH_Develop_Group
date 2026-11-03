#pragma once
#include <functional>
#include <iostream>


template <typename RETURN_TYPE, typename ...Args>
class CDelegate
{
public:
	CDelegate() = default;
	CDelegate(const std::function<RETURN_TYPE(Args...)>& _pFunction) :  
		m_Function{_pFunction}
	{}

	void Invoke(Args... args) {
		m_Function(args...);
	}

	 bool IsEmpty() {
		if (nullptr == m_Function)
			return true;

		return false;
	}

	static std::shared_ptr<CDelegate<RETURN_TYPE, Args...>> Create(const std::function<RETURN_TYPE(Args...)>& _pFunction) {
		std::shared_ptr<CDelegate<RETURN_TYPE, Args...>> pInstnace = std::make_shared<CDelegate<RETURN_TYPE, Args...>>(_pFunction);
		return pInstnace;
	}
private:
	std::function<RETURN_TYPE(Args...)> m_Function = nullptr;
};

