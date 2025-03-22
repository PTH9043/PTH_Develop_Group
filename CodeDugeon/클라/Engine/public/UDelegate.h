#pragma once

namespace Engine {

	template<typename Ref, typename ...Args>
	class UDelegate {
	public:
		using FuncType = std::function<Ref(Args...)>;
	private:
		FuncType		m_Funcs{ nullptr };
	public:
		UDelegate() = default;
		UDelegate(const FuncType& _pFunction) :
			m_Funcs{ _pFunction }
		{}

		template<typename T>
		void Bind(T&& _Func) {
			m_Funcs = _Func;
		}

		void Invoke(Args... args) {
			m_Funcs(args...);
		}

		template <typename T>
		static UDelegate<Ref(Args...)> MakeMethod(T&& _Func)
		{
			UDelegate<Ref(Args...)> Delegate;
			Delegate.Bind(_Func);
			return Delegate;
		}

		static std::shared_ptr<UDelegate<Ref, Args...>> Create(const std::function<Ref(Args...)>& _pFunction) {
			std::shared_ptr<UDelegate<Ref, Args...>> pInstnace = std::make_shared<UDelegate<Ref, Args...>>(_pFunction);
			return pInstnace;
		}
	};

}


