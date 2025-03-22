#pragma once
#include <concepts>
#include <iostream>

namespace Engine
{
	template <class T, class U>
	concept SameType = std::is_same_v<T, U>();
	template <class T, class U>
	concept ParentsChildType = std::is_base_of_v<T*, U*>() || std::is_base_of_v<U*, T*>();
	template<class T, class U>
	concept Convertible = std::is_convertible_v<T*, U*>();

	template <typename T>
	concept SharedPtrConvertible = requires(T t) {
		{ std::shared_ptr<T>(std::move(t)) };
	};
	/*
	@ Date: 2024-02-04, Writer: 박태현
	@ Explain: ProtoType 클래스인지 체크하는 concept
	*/
	template<typename T>
	concept CheckProtoType = requires(T * _t) {
		{_t->ByteSizeLong() }->std::same_as<size_t>;
	};
	template <typename T, typename U>
	concept ConvertibleFrom = (SameType<T, U> || ParentsChildType<T, U> || Convertible<T, U>);

	// 이 ConCept은 생성자 인자들이 제대로 있는지 확인하는 ConCept이다. 
	template<typename T, class ...Args>
	concept ConstructWidthArgsCheck = std::is_constructible_v<T, Args...>;

	// 생성자가 있는지 체크
	template <typename T, typename DevicePtr, typename GpuCommandPtr>
	concept ConstructArgsMatch = std::is_constructible_v<T, DevicePtr, GpuCommandPtr>;

	// 이 Conecept은 T::NativeConstruct의 인자가 맞는지 확인하는 Concept이다. 
	template <typename T, typename... Args>

	// 해당 함수가 있는지 확인해서 인자들을 비교한다. 그리고 HRESULT 라는 반환 값이 동일한지 체크
	concept CheckToSameMethodArgs = requires(T * t, Args&&... args) {
		{ t->NativeConstruct(args...) } -> std::same_as<HRESULT>;
	};

	/*
	@ Date: 2024-02-25, Writer: 박태현
	@ Explain: Pointer인지 확인하는 Concept
	*/
	template<class T>
	concept IsPointer = true == std::is_pointer_v<T>;
	/*
	@ Date: 2024-02-25, Writer: 박태현
	@ Explain: Pointer가 아닌지 확인하는 Concept 
	*/
	template<class T>
	concept IsNotPointer = false == IsPointer<T>;

	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: filesystem으로 변환 가능한지 확인하는 concept
	*/
	template <class TChar>
	concept ConvertibleTCharToPath= requires(const TChar * str) {
		std::filesystem::path{ str };
	};
	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: filesystem으로 변환 가능한지 확인하는 concept
	*/
	template <class TString>
	concept ConvertibleTStringToPath = requires(const TString& str) {
		std::filesystem::path{ str };
	};

	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: String인지 확인하는 concept
	*/
	template<class T>
	concept CheckStdStrings =  std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>;

	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: char, wchar_t 인지 확인하는 concept 
	*/
	template<class T>
	concept CheckChar = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: CustomString 용 Concept으로 filesystem으로 변환 가능한지, TString의 값이 std::string, std::wstring 맞는지 확인
	*/
	template<class TChar, class TString>
	concept CheckCustomString = CheckStdStrings<TString> || CheckChar<TChar> || ConvertibleTCharToPath<TChar> || ConvertibleTStringToPath<TString>;

	/*
	@ Date: 2024-02-03, Writer: 박태현
	@ Explain: 해당하는 Template가 숫자인지 확인
	*/
	template<class T>
	concept CheckNumber = std::is_integral_v<T> || std::is_floating_point_v<T>;

	template<class T>
	concept CheckAtomicNumber = std::is_same_v<T, std::atomic<unsigned int>> || std::is_same_v<T, std::atomic<float>>
		|| std::is_same_v<T, std::atomic<short>> || std::is_same_v<T, std::atomic<long>> || std::is_same_v<T, std::atomic<unsigned short>>
		|| std::is_same_v<T, std::atomic<int>> || std::is_same_v<T, std::atomic<unsigned long>> || std::is_same_v<T, std::atomic<unsigned long long>>
		|| std::is_same_v<T, std::atomic<long long>> || std::is_same_v<T, std::atomic<unsigned char>> || std::is_same_v<T, std::atomic<signed char>>;

	template <typename T, typename U>
	concept SharedPtrConvertibleFrom = SharedPtrConvertible<U> && (SameType<T, U> || ParentsChildType<T, U>);
	template<typename T, typename U>
	concept AtomicPtrConvertibleFrom = (SameType<T, U> || ParentsChildType<T, U>);

}

