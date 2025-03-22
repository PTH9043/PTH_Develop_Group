#pragma once
namespace Engine {
	template <typename... Args>
	constexpr char* GetArgsTypeName()
	{
		return typeid(Args...).name();
	}

#pragma region FUNCTION 
	template<typename T>
	void Safe_Delete(T& _t) {
		delete _t;
		_t = nullptr;
	}

	template<typename T>
	void Safe_Delete_Array(T& _t) {
		delete[] _t;
		_t = nullptr;
	}

	template<typename T>
	bool Is_Nullptr_Debug(T& _data, const char* FILE,
		const unsigned int& LINE, const char* FUNC)
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
			str += std::to_string(LINE);
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


	static void Debugging_Message(const char* FILE,
		const unsigned int& LINE, const char* FUNC, const _string& _pMessage)
	{
#ifdef _DEBUG
		char	szMessage[MAX_PATH] = "";
		_string ClassName = _pMessage;
		_string str = "";
		str += "\nFILE: ";
		str += FILE;
		str += "\n LINE: ";
		str += std::to_string(LINE);
		str += "\n";
		str += FUNC;
		ClassName.append(str);
		MessageBoxA(0, ClassName.c_str(), nullptr, MB_OK);
#endif
	}

	static void Check_Error(const HRESULT& _eResult, const char* FILE,
		const unsigned int& LINE, const char* FUNC) {
		std::string str = std::to_string(_eResult);
		str += "\nFILE: ";
		str += FILE;
		str += "\n LINE: ";
		str += std::to_string(LINE);
		str += "\n";
		str += FUNC;
		MessageBoxA(0, str.c_str(), nullptr, MB_OK);
	}

	template<typename T>
	static void ErrorToCreateClass(T& _rhs) {
		MessageBoxA(0, typeid(T).name(), nullptr, MB_OK);
	}

	// 생성자가 동작하는지 체크하고 동작하면 리턴하는 함수 NativeConstruct 함수가 없어도 된다. 
	template<class T, class ...Args>
		requires ConstructWidthArgsCheck<T, Args...>
	static SHPTR<T> Create(Args&&... args) {
		SHPTR<T> pInstance{ Make::MakeShared<T>(args...) };
		return pInstance;
	}

	// 함수에서 값을 받아와서 객체를 만들어 내는 것
	template<class T, class ...Args>
		requires	CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateNative(Args&&... args) {
		static_assert(CheckToSameMethodArgs<T, Args...>, "NativeConstruct의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Make::MakeShared<T>() };
		if (FAILED(pInstance->NativeConstruct(args...))) {
			ErrorToCreateClass(pInstance);
			pInstance.reset();
		}
		return pInstance;
	}

	// 함수에서 값을 받아와서 객체를 만들어 내는 것
	template<class T, class ...Args>
		requires CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateNativeNotMsg(Args&&... args) {
		static_assert(CheckToSameMethodArgs<T, Args...>, "NativeConstruct의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Make::MakeShared<T>() };
		if (FAILED(pInstance->NativeConstruct(args...))) {
			pInstance.reset();
		}
		return pInstance;
	}

	// 생성자로 Device, GpuCommand를 받고 나머지 값들을 NativeCosntruct 함수에서 받는 것
	template<class T, class ...Args>
		requires  CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateConstructorNative(CSHPTRREF<class UDevice> _pDevice, Args&&... args) {
		SHPTR<T> pInstance{ Make::MakeShared<T>(_pDevice) };
		if (FAILED(pInstance->NativeConstruct(args...))) {
			ErrorToCreateClass(pInstance);
			pInstance.reset();
		}
		return pInstance;
	}

	// 생성자로 Device, GpuCommand를 받고 나머지 값들을 NativeCosntruct 함수에서 받는 것
	template<class T, class ...Args>
		requires CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateConstructorNativeNotMsg(CSHPTRREF<class UDevice> _pDevice, Args&&... args) {
		SHPTR<T> pInstance{ Make::MakeShared<T>(_pDevice) };
		if (FAILED(pInstance->NativeConstruct(args...))) {
			pInstance.reset();
		}
		return pInstance;
	}

	// 생성자에서만 값을 받아오는 것
	template<class T, class ...Args>
		requires ConstructWidthArgsCheck<T, Args...>
	&& CheckToSameMethodArgs<T>
		static SHPTR<T> CreateConstructorToNative(Args&&... args) {
		;
		static_assert(ConstructWidthArgsCheck<T, Args...>, "생성자의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Make::MakeShared<T>(args...) };
		if (FAILED(pInstance->NativeConstruct())) {
			ErrorToCreateClass(pInstance);
			pInstance.reset();
		}
		return pInstance;
	}

	// 생성자에서만 값을 받아오는 것
	template<class T, class ...Args>
		requires ConstructWidthArgsCheck<T, Args...>
	&& CheckToSameMethodArgs<T>
		static SHPTR<T> CreateConstructorToNativeNotMsg(Args&&... args) {
		static_assert(ConstructWidthArgsCheck<T, Args...>, "생성자의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Make::MakeShared<T>(args...) };
		if (FAILED(pInstance->NativeConstruct())) {
			pInstance.reset();
		}
		return pInstance;
	}

	// 자기 자신을 클론하는 함수 
	template<class T>
	static SHPTR<T> CloneThis(const T& _rhs)
	{
		SHPTR<T> pInstance{ Make::MakeShared<T>(_rhs) };
		return pInstance;
	}

	/*
	@ Date: 2023-01-09,  Writer: 박태현
	@ Explain
	- 현재 밀리세컨드를 구하는 함수이다.
	*/
	static _llong CurrentMilliseconds()
	{
		auto Time = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(Time.time_since_epoch()).count();
	}

	/*
	@ Date: 2023-01-05,  Writer: 박태현
	@ Explain
	- 해당 Thread를 nanoseconds만큼 쉬는 함수이다.
	*/
	template<class T>
		requires std::is_integral_v<T>
	static void ThreadNanoRelax(const T& _data)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(_data));
	}
	/*
	@ Date: 2024-01-05,  Writer: 박태현
	@ Explain
	- 해당 Thread를 MicroSeconds만큼 쉬는 함수이다.
	*/
	template<class T>
		requires std::is_integral_v<T>
	static void ThreadMicroRelax(const T& _data)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(_data));
	}
		/*
	@ Date: 2024-01-05,  Writer: 박태현
	@ Explain
	- 해당 Thread를 MiliSeconds만큼 쉬는 함수이다.
	*/
	template<class T>
		requires std::is_integral_v<T>
	static void ThreadMiliRelax(const T& _data)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(_data));
	}

	/*
	@ Date: 2024-02-04,  Writer: 박태현
	@ Explain
	- std::make_pair를 쓰기 싫어서 사용하는 함수
	*/
	template<class T1, class T2>
	static std::pair<T1, T2> MakePair(const T1& _t1, const T2& _t2) { return std::move(std::pair<T1, T2>(_t1, _t2)); }

	/*
	@ Date: 2024-02-04,  Writer: 박태현
	@ Explain
	- sizeof를 쓰면, 지역 변수 호출인데 이 함수를 쓰면 전역 변수 호출이라 새로운 임시 변수를 만들지 않는다. 
	*/
	template<class Type>
	static int GetTypeSize() { static SIZESTRUCTINT<Type> size; return size.VALUE_SIZE; }

	/*
	@ Date: 2024-02-04,  Writer: 박태현
	@ Explain
	- 들어온 수를 num의 배수만큼의 수를 리턴하는 함수
	*/
	template<int Number, class T>
	requires CheckNumber<T>
	static T MakeMultipleNumber(T number) {
		// 16의 배수로 만들기 위해 15를 더하고 16으로 나눕니다.
		T remainder = number % Number;
		if (remainder != 0) {
			number += (Number - remainder);
		}
		return number;
	}
	/*
	@ Date: 2024-02-04,  Writer: 박태현
	@ Explain
	- 들어온 수를 Divine 해주는 함수
	*/
	template<int Number, class T>
	requires CheckNumber<T>
	static T GetDivineNumber(T number) {
		return static_cast<T>(number / Number);
	}
	/*
	@ Date: 2024-02-23,  Writer: 박태현
	@ Explain
	-  const 가 붙은 포인터에서 const를 제거하여 다른 자식 포인터로 캐스팅해주는 함수이다.
	*/
	template<class ChangeT, class OriginT, class ConstT>
	static ChangeT remove_const(ConstT _Desc) {
		OriginT Desc = const_cast<OriginT>(_Desc);
		return static_cast<ChangeT>(Desc);
	}

#pragma endregion FUNCTION
}