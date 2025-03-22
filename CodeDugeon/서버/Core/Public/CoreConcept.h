#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_CORECONCEPT_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_CORECONCEPT_H

#include <iostream>
#include <concepts>

namespace Core
{
	/*
	@ Date: 2024-01-05, Writer: 박태현
	@ Explain: 해당하는 Template가 숫자인지 확인
	*/
	template <typename T>
	concept CheckNumber = std::integral<T> || std::floating_point<T>;

	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain: 같은 놈인지, 부모 타입인지, operator = 연산 수행 가능한지 
	*/
	template <class T, class U>
	concept SameType = std::is_same_v<T, U>();
	template <class T, class U>
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain: 부모 자식인지 확인하는 coencept
	*/
	concept ParentsChildType = std::is_base_of_v<T*, U*>() || std::is_base_of_v<U*, T*>();
	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain: T에서 U로 변환이 가능한지 확인하는 concept
	*/
	template<class T, class U>
	concept Convertible = std::is_convertible_v<T*, U*>();


	template <typename Base, typename Derived>
	concept BASE_CHECK = std::is_base_of_v<Base, Derived>;

	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain: Args...에서 펼친 변수들과 생성자의 변수들이 맞는지 확인
	*/
	template<typename T, class ...Args>
	concept ConstructArgsCheck = std::is_constructible_v<T, Args...>;

	/*
	@ Date: 2023-12-26, Writer: 박태현
	@ Explain: 클래스에 NativeConstruct 함수가 정의되어있는지 확인
	*/
	template <typename T, typename... Args>
	concept CheckToSameMethodArgs = requires(T * t, Args&&... args) {
		{ t->NativeConstruct(args...) } -> std::same_as<bool>;
	};
	/*
	@ Date: 2024-01-06, Writer: 박태현
	@ Explain: ProtoType 클래스인지 체크하는 concept
	*/
	template<typename T>
	concept CheckProtoType = requires(T * _t) {
		{_t->ByteSizeLong() }->std::same_as<size_t>;
	};

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain: SmartPointer인지 확인하는 Concept 
	*/
	template <typename T>
	concept SmartPointer = requires(T t) {
		{ t.get() } -> std::same_as<typename T::element_type*>;
		{ t.reset() } -> std::same_as<void>;
	};

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain: Pointer의 배열인지 확인하는 conecept
	*/
	template <typename T>
	concept PointerArray = requires(T t) {
		{ *t } -> std::same_as<std::remove_extent_t<T>>;  
	};

	template <typename T, typename U>
	concept ConvertibleFrom = (SameType<T, U> || ParentsChildType<T, U> || Convertible<T, U>);

	/*
	@ Date: 2024-02-05, Writer: 박태현
	@ Explain: String인지 확인하는 concept
	*/
	template<class T>
	concept CheckStdStrings = std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>;

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
	concept CheckCustomString = CheckStdStrings<TString> || CheckChar<TChar>;

}


#endif // _SERVERFRAMEWORK_CORE_PUBLIC_CORECONCEPT_H