#include "EngineDefine.h"
#include "UObject.h"
UObject::UObject(CSHPTRREF<UDevice> _spDevice)
	: m_spDevice(_spDevice)
{
}

UObject::UObject(const UObject& _rhs)
	: m_spDevice(_rhs.m_spDevice)
{

}

HRESULT UObject::NativeConstruct()
{
	RETURN_CHECK(nullptr == m_spDevice, E_FAIL);
	return S_OK;
}

#ifdef _USE_IMGUI
void UObject::ShowObjectInfo()
{

}
#endif