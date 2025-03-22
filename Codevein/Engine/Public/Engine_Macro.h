#pragma once

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define BEGIN(Namespace) namespace Namespace { 

#define END }

#define COM_TRANSFORM L"Com_Transform"

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MSG_BOX(Message)\
MessageBox(0, TEXT(Message), TEXT("System Error"), MB_OK) \


#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

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

#define BEGIN_SINGLETON(ClassName, pInstance)			\
	ClassName* pInstance = ClassName::Get_Instance();	\
	if(nullptr == pInstance) {														\
	char	szMessage[MAX_PATH] = "";									\
	strcpy_s(szMessage, typeid(ClassName).name());		\
	strcat_s(szMessage, "is nullptr");										\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}				\
	else {																							\
	pInstance->AddRef();}		

#define END_SINGLETON(pInstance)									\
	if(nullptr != pInstance) {														\
	pInstance->Release();}																	

#define TARGET_DIFFUSE_DEFFERD L"Target_Diffuse_Deffered"
#define TARGET_NORMAL_DEFFERD L"Target_Normal_Deffered"
#define TARGET_SHADE_DEFFERD	 L"Target_Shade_Deffered"
#define TARGET_BLUR_DEFFERD L"Target_Blur_Defferd"
#define TARGET_WORLDPOS_DEFFERD L"Target_WorldPos_Defferd"
#define TARGET_DEPTH_LIGHT  L"Target_Depth_Light"
#define TARGET_SPECULAR_LIGHT L"Target_Specular_Light"

#define TARGET_DIFFUSE_ALPHABLEND L"Target_Diffuse_AlphaBlend"
#define TARGET_BLEND_ALPHABLEND L"Target_Blend_AlphaBlend"


#define MRT_DEFFED					L"MRT_Deferred"
#define MRT_LIGHTACC				L"MRT_LightAcc"
#define MRT_ALPHABLEND			L"MRT_AlphaBlend"
#define MRT_BLUR						L"MRT_Blur"