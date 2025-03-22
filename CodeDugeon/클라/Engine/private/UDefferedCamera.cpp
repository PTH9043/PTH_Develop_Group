#include "EngineDefine.h"
#include "UDefferedCamera.h"

UDefferedCamera::UDefferedCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCamera(_spDevice, _wstrLayer, _eCloneType)
{
}

UDefferedCamera::UDefferedCamera(const UDefferedCamera& _rhs) :
	UCamera(_rhs)
{
}

void UDefferedCamera::Free()
{
}

HRESULT UDefferedCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UDefferedCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	SetCamID(DEFFERED_CAMERA_ID);
	return __super::NativeConstructClone(_vecDatas);
}

void UDefferedCamera::TickActive(const _double& _dTimeDelta)
{
}

void UDefferedCamera::LateTickActive(const _double& _dTimeDelta)
{
}
