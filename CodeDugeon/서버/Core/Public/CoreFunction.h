#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_COREFUNCTION_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_COREFUNCTION_H

namespace Core
{
#pragma region FUNCTION 

	/*
	@ Date: 2024-01-10,  Writer: 박태현
	@ Explain
	- 앱을 만들어서 등록하는 함수이다.
	*/
	template<class T>
	T* CreateAndRegisterApp()
	{
		T* App = new T;
		Core::g_RegisterApp = App;
		return App;
	}
	/*
	@ Date: 2024-01-09,  Writer: 박태현
	@ Explain
	- 현재 밀리세컨드를 구하는 함수이다. 
	*/
	static _llong CurrentMilliseconds()
	{
		auto Time = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(Time.time_since_epoch()).count();
	}
	/*
	@ Date: 2024-01-13,  Writer: 박태현
	@ Explain
	- 현재 밀리세컨드를 구하는 함수이다.
	*/
	static _llong CurrentTime()
	{
		auto Time = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(Time.time_since_epoch()).count();
	}

	/*
	@ Date: 2024-01-05,  Writer: 박태현
	@ Explain
	- _temp가 _Value를 넘었는지 확인하는 함수
	*/
	template<class T, class Value >
	requires (CheckNumber<T> && CheckNumber<Value>)
	static T CheckOverValue(const T& _temp, const Value& _Value)
	{
		T convert = static_cast<T>(_Value);
		if (_temp >= convert)
		{
			return convert;
		}
		return _temp;
	}
	/*
	@ Date: 2024-01-05,  Writer: 박태현
	@ Explain
	- 메모리를 초기화하기 위한 함수, Window함수인 ZeroMemory를 쓰지 않기 위함이다.
	*/
	template<class T>
	static void MemoryInitialization(T* _pData, const size_t _bufferSize)
	{
		constexpr static _int ZEROMEMORY{ 0 };
		std::memset(_pData, ZEROMEMORY, _bufferSize);
	}

	/*
	@ Date: 2024-01-05,  Writer: 박태현
	@ Explain
	- 메모리를 초기화하기 위한 함수, Window함수인 ZeroMemory를 쓰지 않기 위함이다.
	*/
	template<class T>
	static void MemoryInitialization(T& _Data)
	{
		constexpr static _int ZEROMEMORY{ 0 };
		std::memset(&_Data, ZEROMEMORY, sizeof(T));
	}

	/*
	@ Date: 2024-01-05,  Writer: 박태현
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
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain
	- _origin의 값이 _old와 같을 때 _weak를 넣고 true 리턴
	*/
	template<class T>
	static bool CAS_VALUE(std::atomic<T>& _origin, T _old, T _weak)
	{
		return _origin.compare_exchange_strong(_old, _weak);
	}
	/*
	@ Date: 2024-01-02,  Writer: 박태현
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
	static int GetTypeSize() { constexpr static int SIZE{ sizeof(Type) }; return SIZE; }
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
	@ Date: 2024-12-26, Writer: 박태현
	@ Explain
	- 단순히 객체를 만드는 함수이다, 
	*/
	template<class T, class... Args>
		requires ConstructArgsCheck<T, Args...>
	static SHPTR<T> Create(Args&&... args) {
		SHPTR<T> pInstance = Core::MakeShared<T>(std::forward<Args>(args)...);
		return std::move(pInstance);
	}
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain
	- NatvieConstruct 함수를 호출 한 후 , 실패하면 nullptr 리턴, 함수에서 초기화
	*/
	template<class T, class ...Args>
		requires	CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateInitNative(Args&&... args) {
		static_assert(CheckToSameMethodArgs<T, Args...>, "NativeConstruct의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Core::MakeShared<T>() };
		if (false ==  (pInstance->NativeConstruct(args...))) {
#ifdef USE_DEBUG
			CRASH("SHPTR<T>::CreateInitNative");
#endif
			pInstance.reset();
		}
		return std::move(pInstance);
	}
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain
	- CreateInitNative 동일, 다만 메시지는 출력하지 않음
	*/
	template<class T, class ...Args>
		requires CheckToSameMethodArgs<T, Args...>
	static SHPTR<T> CreateInitNativeNotMsg(Args&&... args) {
		static_assert(CheckToSameMethodArgs<T, Args...>, "NativeConstruct의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Core::MakeShared<T>() };
		if (false == (pInstance->NativeConstruct(args...))) {
			pInstance.reset();
		}
		return std::move(pInstance);
	}
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain
	- NatvieConstruct 함수를 호출 한 후 , 실패하면 nullptr 리턴, 생성자에서 초기화, 
	Natvie함수가 있는지 확인 후, 생성자의 변수와 Args...의 변수가 동일한지 판단.
	*/
	template<class T, class ...Args>
		requires ConstructArgsCheck<T, Args...>
	&& CheckToSameMethodArgs<T>
		static SHPTR<T> CreateInitConstructor(Args&&... args) {
		static_assert(ConstructArgsCheck<T, Args...>, "생성자의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Core::MakeShared<T>(std::forward<Args>(args)...) };
		if (false == (pInstance->NativeConstruct())) {
#ifdef USE_DEBUG
			CRASH("SHPTR<T>::CreateInitConstructor");
#endif
			pInstance.reset();
		}
		return std::move(pInstance);
	}
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain
	- CreateInitConstructor 동일, 다만 메시지는 출력하지 않음
	*/
	template<class T, class ...Args>
		requires ConstructArgsCheck<T, Args...>
	&& CheckToSameMethodArgs<T>
		static SHPTR<T>  CreateInitConstructorNotMsg(Args&&... args) {
		static_assert(ConstructArgsCheck<T, Args...>, "생성자의 인자가 잘못되었습니다.");
		SHPTR<T> pInstance{ Core::MakeShared<T>(std::forward<Args>(args)...) };
		if (false ==  (pInstance->NativeConstruct())) {
			pInstance.reset();
		}
		return std::move(pInstance);
	}
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain
	- 자기 자신을 클론하는 함수이다. 
	*/
	template<class T>
	static SHPTR<T> CloneThis(const T& _rhs)
	{
		SHPTR<T> pInstance{ Core::MakeShared<T>(_rhs) };
		return std::move(pInstance);
	}
	/*
	@ Date: 2024-04-06, Writer: 박태현
	@ Explain
	- char을 조합하는 함수이다.
	*/
	static _string CombineRawString(const _char* _p1, const _char* _p2)
	{
		_string str(_p1);
		str += _p2;
		return std::move(str);
	}
	/*
	@ Date: 2024-07-03, Writer: 박태현
	@ Explain
	- wstring을 그냥 string으로 변환하는 함수
	*/
	static _string ConvertWToS(const _wstring& var)
	{
		_string str;
		size_t size;
		str.resize(var.size());
		wcstombs_s(&size, &str[0], str.size() + 1, var.c_str(), var.size());
		return std::move(str);
	}
	/*
	@ Date: 2024-07-03, Writer: 박태현
	@ Explain
	- wstring을 그냥 string으로 변환하는 함수
	*/
	static _wstring ConvertSToW(const _string& var)
	{
		_wstring wstr;
		size_t size;
		wstr.resize(var.size());
		mbstowcs_s(&size, wstr.data(), wstr.size() + 1, var.c_str(), var.size());
		return std::move(wstr);
	}
	/*
	@ Date: 2024-07-03, Writer: 박태현
	@ Explain
	- string을 읽는 함수 
	*/
	static void ReadString(std::ifstream& _if, _wstring& _wstr) 
	{
		_int size = 0;
		_if.read((_char*)&size, sizeof(int));
		_char* pText = AllocBuffer<_char>(size);
		::memset(pText, 0, static_cast<size_t>(size));
		_if.read((_char*)pText, size);
		_wstr = ConvertSToW(pText);
		ReleaseBuffer(pText);
	}
	/*
	@ Date: 2024-07-03, Writer: 박태현
	@ Explain
	- string을 읽는 함수
	*/
	static void ReadString(std::ifstream& _if, _string& _str)
	{
		_int size = 0;
		_if.read((_char*)&size, sizeof(int));
		_char* pText = AllocBuffer<_char>(size);
		::memset(pText, 0, static_cast<size_t>(size));
		_if.read((_char*)pText, size);
		_str = pText;
		ReleaseBuffer(pText);
	}

	static void SaveString(std::ofstream& _os, const _string& _str)
	{
		int size = (int)_str.length() + 1;
		_os.write((char*)&size, sizeof(int));
		_os.write((char*)_str.c_str(), size);
	}
	static void SaveString(std::ofstream& _os, const _wstring& _wstr)
	{
		SaveString(_os, ConvertWToS(_wstr));
	}

#pragma endregion FUNCTION
}

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_COREFUNCTION_H