#include "EngineDefine.h"
#include "UResource.h"

UResource::UResource(CSHPTRREF<UDevice> _spDevice) :
	UCloneObject(_spDevice)
{
}

UResource::UResource(const UResource& _rhs) :
	UCloneObject(_rhs)
{
}

void UResource::Free()
{

}

HRESULT UResource::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UResource::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}
