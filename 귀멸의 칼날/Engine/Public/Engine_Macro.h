#pragma once

#define CHECK_CREATE_PROTOTYPE_NOTVALUE(INSTANCE, MESSAGE) \
if(FAILED(INSTANCE->NativeConstruct_Prototype())) {DEBUGGIN_MESSAGE_PTH(MESSAGE); Safe_Release(INSTANCE); }

#define CHECK_CREATE_PROTOTYPE_VALUE_1(INSTANCE, VALUE, MESSAGE)\
if (FAILED(INSTANCE->NativeConstruct_Prototype(VALUE))) { DEBUGGIN_MESSAGE_PTH(MESSAGE);Safe_Release(INSTANCE); }

#define CHECK_CREATE_CLONE(INSTANCE, VALUE, MESSAGE) \
if(FAILED(INSTANCE->NativeConstruct_Clone(VALUE))) {DEBUGGIN_MESSAGE_PTH(MESSAGE); Safe_Release(INSTANCE); }

#define DEBUG_RETURN_MSG_IFNOT(SITUATION, MESSAGE, RETURN) \
if(SITUATION){ DEBUGGIN_MESSAGE_PTH(MESSAGE); return RETURN; }

#define RETURN_IFNOT(SITUATION, RETURN) \
if(SITUATION){ return RETURN; }

#define DEBUG_RETURN_MSG(MESSAGE, RETURN) \
DEBUGGIN_MESSAGE_PTH(MESSAGE); return RETURN;

#define SETUP_CONSTANCE_VALUE(SHADER, NAME, VALUE) \
if(FAILED(SHADER->Set_RawValue(NAME, &VALUE, sizeof(VALUE)))) return E_FAIL;

#define SETUP_SRV(SHADER, NAME, VALUE) \
if(FAILED(SHADER->Set_SRV(NAME, &VALUE))) return E_FAIL;

#define SETUP_UAV(SHADER, NAME, VALUE) \
if(FAILED(SHADER->Set_UAV(NAME, &VALUE))) return E_FAIL;

#define SETUP_TEXTURE_SRV_NAME(SHADER, TEXTURE, NAME, FILENAME) \
if(FAILED(TEXTURE->SetUp_ConstantTable_FileName(SHADER, NAME, FILENAME))) return E_FAIL;

#define SETUP_TEXTURE_SRV_INDEX(SHADER, TEXTURE, NAME, INDEX) \
if(FAILED(TEXTURE->SetUp_ConstantTable_Index(SHADER, NAME, INDEX))) return E_FAIL;

#define DEBUG_MACRO_TO_METHOD \
__FILE__, __LINE__, __FUNCTION__ 

#ifdef _DEBUG
#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef(DEBUG_MACRO_TO_METHOD);}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release(DEBUG_MACRO_TO_METHOD);}														\
	}();			

#else
#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
	}();		

#endif

#define BEGIN(Namespace) namespace Namespace { 

#define END }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MSG_BOX(Message) MessageBox(0, TEXT(Message), TEXT("System Error"), MB_OK)
#define MSG_BUTTONBOX(Message) MessageBox(0, TEXT(Message), TEXT("저장파일 날림 주의 !!!!"), MB_ICONQUESTION | MB_YESNO)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#ifdef _DEBUG
#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned long Destroy_Instance();			\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned long ClassName::Destroy_Instance()				\
{														\
	unsigned long dwRefCnt = 0;							\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt  = m_pInstance->Release(DEBUG_MACRO_TO_METHOD);				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}
#else
#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned long Destroy_Instance();			\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned long ClassName::Destroy_Instance()				\
{														\
	unsigned long dwRefCnt = 0;							\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt  = m_pInstance->Release();				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}
#endif

#ifdef _DEBUG

#define Safe_Release(T) \
Safe_Release_M(T, DEBUG_MACRO_TO_METHOD)

#define Safe_AddRef(T) \
Safe_AddRef_M(T, DEBUG_MACRO_TO_METHOD)

#endif

#define Is_Nullptr(T) \
Is_Nullptr_M(T, DEBUG_MACRO_TO_METHOD)


// Render Target
#define TARGET_DIFFUSE_DEFERRED		L"Target_Diffuse_Deferred"
#define TARGET_NORMAL_DEFERRED		L"Target_Normal_Deferred"
#define TARGET_DEPTH				L"Target_Depth"
#define TARGET_EDGEGLOW				L"Target_EdgeGlow"
#define TARGET_MOTIONBLUR_DEFERRED	L"Target_MotionBlur_Deffered"
#define TARGET_MAPOBJDEPTH			L"Target_MapObjDepth"


#define TARGET_AMBIENT				L"Target_Ambient"
#define TARGET_SHADE_DEFERRED		L"Target_Shade_Deferred"
#define TARGET_SPECULAR_LIGHT		L"Target_Specular_Light"
#define TARGET_SHADOW				L"Target_Shadow"

#define	TARGET_SHADOW_DEPTH			L"Target_Shadow_Depth"

#define TARGET_NONALPHABLEND		L"Target_NonAlphaBlend"

#define TARGET_ALPHABLEND			L"Target_AlphaBlend"
#define TARGET_GLOW					L"Target_Glow"
#define TARGET_BLUR_DEGREE			L"Target_Blur_Degree"		
#define TARGET_MOTIONBLUR_ALPHABLEND	L"Target_MotionBlur_AlphaBlend"			

#define TARGET_TEMP					L"Target_Temp"
#define TARGET_TEMP2				L"Target_Temp2"

#define TARGET_SHAKE_DIFFUSE		L"Target_Shake_Diffuse"

#define TARGET_DIFFUSE_MOTIONBLUR_NONALPHA L"Target_Diffuse_MotionBlur_NonAlpha"
#define TARGET_JUDGEDRAW_MOTIONBLUR_NONALPHA L"Target_JudgeDraw_MotionBlur_NonAlpha"
#define TARGET_DIFFUSE_MOTIONBLUR_ALPHA L"Target_Diffuse_MotionBlur_Alpha"
#define TARGET_JUDGEDRAW_MOTIONBLUR_ALPHA L"Target_JudgeDraw_MotionBlur_Alpha"

#define TARGET_BACKBUFFER			L"Target_BackBuffer"

// 태현 추가
#define TARGET_DISTORTION_DIFFUSE	L"Target_Distortion_Diffuse "
#define TARGET_DISTORTION_BLUR		L"Target_Distortion_Blur"

// MRT
#define MRT_DEFERRED				L"MRT_Deferred"
#define MRT_LIGHTACC				L"MRT_LightAcc"
#define	MRT_SHADOW					L"MRT_Shadow"
#define MRT_NONALPHABLEND			L"MRT_NonAlphaBlend"
#define MRT_ALPHABLEND				L"MRT_AlphaBlend"
#define MRT_MOTIONBLUR_NONALPHA		L"MRT_MotionBlur_NonAlpha"
#define MRT_MOTIONBLUR_ALPHA		L"MRT_MotionBlur_Alpha"
#define MRT_TEMP					L"MRT_Temp"
#define MRT_TEMP2					L"MRT_Temp2"
#define MRT_BACKBUFFER				L"MRT_BackBuffer"
#define MRT_SHAKE					L"MRT_Shake"

// Process Handler
#define PH_SHADOW4x4				L"ProcessHandler_Shadow4x4"
#define PH_TEMP						L"ProcessHandler_Temp"
#define PH_DOWNSCALE				L"ProcessHandler_DownScale"
#define PH_UPSCALE					L"ProcessHandler_UpScale"
#define PH_BLUR						L"ProcessHandler_Blur"
#define PH_BLUR2					L"ProcessHandler_Blur2"
#define PH_HDR						L"ProcessHandler_HDR"

// Shader
#define SHADER_DEFERRED				L"Shader_Deferred"
#define SHADER_EDGEGLOW				L"Shader_EdgeGlow"
#define SHADER_LIGHT				L"Shader_Light"
#define SHADER_DISTORTION			L"Shader_Distortion"
#define SHADER_3DGLOW				L"Shader_3DGlow"
#define SHADER_MOTIONBLUR L"Shader_MotionBlur"

#define COM_TRANSFORM				L"Com_Tramsform"

// 태현 추가
#define MRT_DISTORTION		L"MRT_Distortion"

#define STANDARD_FRAME_VALUE 60