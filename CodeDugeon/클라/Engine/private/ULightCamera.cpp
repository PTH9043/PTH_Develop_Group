#include "EngineDefine.h"
#include "ULightCamera.h"

ULightCamera::ULightCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCamera(_spDevice, _wstrLayer, _eCloneType)
{
}

ULightCamera::ULightCamera(const ULightCamera& _rhs) :
	UCamera(_rhs)
{
}

void ULightCamera::Free()
{
}

HRESULT ULightCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT ULightCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	SetCamID(SHADOWLIGHT_CAMERA_ID);
	return __super::NativeConstructClone(_vecDatas);
}

void ULightCamera::TickActive(const _double& _dTimeDelta)
{
}

void ULightCamera::LateTickActive(const _double& _dTimeDelta)
{
}
