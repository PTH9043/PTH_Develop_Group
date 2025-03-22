#include "EngineDefine.h"
#include "UComponent.h"

UComponent::UComponent(CSHPTRREF<UDevice> _spDevice)
	: UCloneObject(_spDevice)
{
}

UComponent::UComponent(const UComponent& _rhs) :
	UCloneObject(_rhs)
{
}

void UComponent::Free()
{
}

HRESULT UComponent::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UComponent::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	return __super::NativeConstructClone(_tDatas);
}