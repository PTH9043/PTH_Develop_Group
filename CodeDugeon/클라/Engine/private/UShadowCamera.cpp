#include "EngineDefine.h"
#include "UShadowCamera.h"

UShadowCamera::UShadowCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCamera(_spDevice, _wstrLayer, _eCloneType)
{
}

UShadowCamera::UShadowCamera(const UShadowCamera& _rhs) :
	UCamera(_rhs)
{
}

void UShadowCamera::Free()
{
}

HRESULT UShadowCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UShadowCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	SetCamID(SHADOWLIGHT_CAMERA_ID);
	return __super::NativeConstructClone(_vecDatas);
}

void UShadowCamera::TickActive(const _double& _dTimeDelta)
{
}

void UShadowCamera::LateTickActive(const _double& _dTimeDelta)
{
}

void UShadowCamera::SetShadowCamViewportInfo(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	_ShadowCamValue.ShadowCamViewport.TopLeftX = float(xTopLeft);
	_ShadowCamValue.ShadowCamViewport.TopLeftY = float(yTopLeft);
	_ShadowCamValue.ShadowCamViewport.Width = float(nWidth);
	_ShadowCamValue.ShadowCamViewport.Height = float(nHeight);
	_ShadowCamValue.ShadowCamViewport.MinDepth = fMinZ;
	_ShadowCamValue.ShadowCamViewport.MaxDepth = fMaxZ;
}

void UShadowCamera::SetShadowCamRectInfo(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	_ShadowCamValue.ShadowCamRect.left = xLeft;
	_ShadowCamValue.ShadowCamRect.top = yTop;
	_ShadowCamValue.ShadowCamRect.right = xRight;
	_ShadowCamValue.ShadowCamRect.bottom = yBottom;
}
