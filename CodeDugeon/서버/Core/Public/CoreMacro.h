#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_COREMACRO_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_COREMACRO_H


/*
==========================
					SHOW
==========================
*/
/*
@ Date: 2023-12-28, Writer: 박태현
@ Explain
- 함수를 실행시킬 때, 레퍼런스를 통해 해당하는 변수의 값이 바뀔 수 있을 경우를 나타냄
*/
#define REF_OUT


/*
@ Date: 2023-12-28,  Writer: 박태현
@ Explain
- 함수 정의할 때, 레퍼런스를 통해 해당하는 변수의 값이 바뀔 수 있을 경우를 나타냄
*/
#define REF_IN


/*
@ Date: 2023-12-28, Writer: 박태현
@ Explain
- 레퍼런스 함수를 리턴하는 경우를 나타낸다. 
*/
#define REF_RETURN

/*
@ Date: 2023-12-28. Writer: 박태현
@ Explain
- 함수를 실행시킬 때,  Pointer를 통해 해당하는 변수의 값이 바뀔 수 있음을 나타냄
*/
#define POINTER_OUT 


/*
@ Date: 2023-12-28,  Writer: 박태현
@ Explain
- 함수를 정의할 때,  Pointer를 통해 해당하는 변수의 값이 바뀔 수 있음을 나타냄
*/
#define POINTER_IN

/*
@ Date: 2023-12-28, Writer: 박태현
@ Explain
- 읽기 전용 변수를 나타낸다.
*/
#define READONLY

/*
@ Date: 2023-12-30, Writer: 박태현
@ Explain
- 가상함수라는 것을 나타내는 매크로
*/
#define PURE = 0

/*
==========================
					LOCK	
==========================
@ Date: 2023-12-28
@ Writer: 박태현
@ Explain
- 매크로로 Lock들을 다룰때 사용한다.
*/

#define READ_SPINLOCK(Lock) \
 Core::AReadFastSpinLockguard readLockGuard{&Lock};



#define WRITE_SPINLOCK(Lock) \
Core::AWriteFastSpinLockguard writeLockGuard{&Lock};

/*
==========================
					DEBUG
==========================
*/

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- 강제로 크래쉬를 일으키는 매크로
*/
#define CRASH(cause) 	\
{											\
	int* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(COND) \
{									\
	if (!(COND))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(COND);	\
	}								\
}


/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- DEBUG_MACRO를 위한 FILE_LINE_FUNCTION등을 정의 
*/
#define DEBUG_MACRO_TO_METHOD \
__FILE__, __LINE__, __FUNCTION__ 
#define LOG_MACRO_TO_METHOD(x) \
typeid(x).name() , __LINE__, __FUNCTION__


/*
==========================
					CLASS
==========================
*/


/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- NO_COPY 매크로가 존재하면 복제 불가한 클래스라는 말이다. 
*/
#define NO_COPY(ClassName)							\
public: \
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete; 

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- 소멸자에 Free를 호출하게 만드는 매크로
*/
#define DESTRUCTOR(ClassName) virtual ~ClassName() {Free();}

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- 헤더파일에 싱글톤에 사용할 함수와 변수를 정의
*/
#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static  SHPTR<ClassName> GetInstance();					\
	static  void DestoryInstance();		\
private:												\
	static SHPTR<ClassName> m_pInstance;

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- 헤더파일에서 정의한 함수와 변수를 CPP파일에서 초기화 그리고 정의하여 싱글톤이 되도록한다. 
*/
#define IMPLEMENT_SINGLETON(ClassName)					\
SHPTR<ClassName> ClassName::m_pInstance = nullptr;			\
SHPTR<ClassName> ClassName::GetInstance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = Core::PTH::MakeShared<ClassName>(std::make_shared<ClassName>());		\
	return m_pInstance;									\
}														\
void ClassName::DestoryInstance(){  m_pInstance.reset(); }		

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain
- 싱글톤 클래스의 값을 받아오는 매크로
*/
#define GET_INSTANCE(CLASSNAME) [](){ \
  SHPTR<CLASSNAME> pInstance = CLASSNAME::GetInstance();	\
    return pInstance; \
}();

/*
==========================
			Type List Define 
==========================
*/

/*
@ Date: 2024-01-01, Writer: 박태현
@ Explain
- TypeList를 생성하기 위한 매크로 
*/
#define DEFCLARE_TL using TL = TL;  int typeId;
#define INIT_TL(Type) typeId = Core::IndexOf<TL, Type>::value;

/*
==========================
			LOG MACRO
==========================
*/

//#define LOGPRINTOUT_CLASSLINE(LogManager, String) LogManager->PrintOut("%s\nFILE:%s, LINE:%d\n", String, typeid(this).name(), __LINE__);
//#define LOGPRINTOUT(LogManager, Printf, String) LogManager->PrintOut(Printf, String);
//#define FILEPRINTOUT_CLASSLINE(LogManager, String) LogManager->FileOut("%s\nFILE:%s, LINE:%d\n", String, typeid(this).name(), __LINE__);
//#define FILEPRINTOUT(LogManager, Printf, String) LogManager->FileOut(Printf, String);
/*
==========================
			CONSTRUCTOR 
==========================
*/

/*
@ Date: 2024-01-02,  Writer: 박태현
@ Explain
-  OBJECT 생성자에 정의되어 있는 변수를 매크로화 시켰다. 
*/
#define OBJCON_CONSTRUCTOR Core::SHPTR<Core::ACoreInstance> _spCoreInstance
#define OBJCON_CONDATA _spCoreInstance 

/*
@ Date: 2024-01-02,  Writer: 박태현
@ Explain
-  Session 생성자에 정의되어 있는 변수를 매크로화 시켰다.
*/
#define SESSION_CONSTRUCTOR OBJCON_CONSTRUCTOR,  Asio::ip::tcp::socket _TcpSocket, \
 SESSIONID _ID
#define SESSION_CONDATA(SessionType) OBJCON_CONDATA, std::move(_TcpSocket), _ID, SessionType
/*
==========================
					ECT
==========================
*/

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain: namespace의 시작과 끝을 구분하는 영역 지정
*/
#define BEGIN(NAME) namespace NAME{
#define END }

/*
@ Date: 2023-12-26, Writer: 박태현
@ Explain: 조건에 맞으면 리턴하는 메크로
*/
#define RETURN_CHECK(SITUATION, RETURN) if(SITUATION) return RETURN;

/*
@ Date: 2024-03-24, Writer: 박태현
@ Explain: SHPTR을 사용하는 클래스들을 미리 정의해 놓는다. 
*/
#define USING_SHARED_PTR(name)	using name##Ref = SHPTR<class name>;


/*
@ Date: 2024-03-28, Writer: 박태현
@ Explain: WKPTR을 사용하는 클래스들을 미리 정의해 놓는다. 
*/
#define USING_WEAK_PTR(name)	using name##Ref = WKPTR<class name>;


#endif // _SERVERFRAMEWORK_CORE_PUBLIC_COREMACRO_H