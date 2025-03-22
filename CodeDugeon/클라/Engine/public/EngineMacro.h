#pragma once


#define DEBUG_MACRO_TO_METHOD \
__FILE__, __LINE__, __FUNCTION__ 

#define DEBUG_MSG(Message) \
Debugging_Message(DEBUG_MACRO_TO_METHOD, Message);

#define BEGIN(NAME) namespace NAME{
#define END }

#define RETURN_CHECK(SITUATION, RETURN) if(SITUATION) {  return RETURN; }
#define RETURN_CHECK_FAILED(SITUATION, RETURN) if(FAILED(SITUATION)) return RETURN;
#define RETURN_CHECK_DXOBJECT(SITUATION, RETURN)  \
{ HRESULT hr = SITUATION; if (FAILED(hr)) { Check_Error(hr, DEBUG_MACRO_TO_METHOD);  return RETURN; } }

#define RETURN_CHECK_CRASH(SITUATION, RETURN, MSG) {  if(SITUATION) { CRASH(MSG);} return RETURN;  }

#define Is_Nullptr(T) \
Is_Nullptr_Debug(T, DEBUG_MACRO_TO_METHOD)

#define NO_COPY(ClassName)							\
private: \
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete; 

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static  SHPTR<ClassName> GetInstance();					\
	static void DestroyInstance() { m_pInstance.reset(); }\
private:												\
	static SHPTR<ClassName> m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
SHPTR<ClassName> ClassName::m_pInstance = nullptr;			\
SHPTR<ClassName> ClassName::GetInstance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = std::make_shared<ClassName>();		\
	return m_pInstance;									\
}														

#define GET_INSTANCE(CLASSNAME) [](){ \
  SHPTR<CLASSNAME> pInstance = CLASSNAME::GetInstance();	\
    return pInstance; \
}();

#pragma region 바꿔야하는구간이다. 

#define CLONE_MACRO(CLASS, DEBUG) \
public: \
 virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _tDatas) override {   \
SHPTR<CLASS> pInstance { Make::MakeShared<CLASS>(*this)}; \
if(FAILED(pInstance->NativeConstructClone(_tDatas))) \
{ \
DEBUG_MSG(DEBUG) \
pInstance.reset(); \
} \
return pInstance; \
}


#define CLONE_THIS(CLASS)	\
virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _tDatas) override { \
return ThisShared<UCloneObject>(); \
} 



#define DESTRUCTOR(CLASS) \
public: \
virtual ~CLASS() { Free(); }


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
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- 레퍼런스 함수를 리턴하는 경우를 나타낸다.
*/
#define REF_RETURN

/*
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- 함수를 실행시킬 때,  Pointer를 통해 해당하는 변수의 값이 바뀔 수 있음을 나타냄
*/
#define POINTER_OUT 


/*
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- 함수를 정의할 때,  Pointer를 통해 해당하는 변수의 값이 바뀔 수 있음을 나타냄
*/
#define POINTER_IN

/*
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- 가상함수라는 것을 나타내는 매크로
*/
#define PURE = 0


#pragma endregion 바꿔야하는구간

