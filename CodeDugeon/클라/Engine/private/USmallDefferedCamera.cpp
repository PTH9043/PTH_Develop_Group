#include "EngineDefine.h"
#include "USmallDefferedCamera.h"


USmallDefferedCamera::USmallDefferedCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCamera(_spDevice, _wstrLayer, _eCloneType)
{
}

USmallDefferedCamera::USmallDefferedCamera(const USmallDefferedCamera& _rhs) :
	UCamera(_rhs)
{
}

void USmallDefferedCamera::Free()
{
}

HRESULT USmallDefferedCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT USmallDefferedCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	SetCamID(SMALL_DEFFERED_CAMERA_ID);
	return __super::NativeConstructClone(_vecDatas);
}

void USmallDefferedCamera::TickActive(const _double& _dTimeDelta)
{
}

void USmallDefferedCamera::LateTickActive(const _double& _dTimeDelta)
{
}
