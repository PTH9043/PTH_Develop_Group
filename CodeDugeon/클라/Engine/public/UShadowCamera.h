#pragma once

#include "UCamera.h"

BEGIN(Engine)

class UShadowCamera : public UCamera {
public:
	typedef struct DepthTexInfo
	{
		DepthTexInfo() = default;
		DepthTexInfo(const D3D12_VIEWPORT& _ShadowCamViewport, const D3D12_RECT& _ShadowCamRect) : ShadowCamViewport(_ShadowCamViewport), ShadowCamRect(_ShadowCamRect)
		{}
		D3D12_VIEWPORT ShadowCamViewport ;
		D3D12_RECT ShadowCamRect;
	}DTVALUE;
	UShadowCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UShadowCamera(const UShadowCamera& _rhs);
	DESTRUCTOR(UShadowCamera)

public:
	virtual void Free() override;
	CLONE_MACRO(UShadowCamera, "UShadowCamera::Clone To Failed")
		virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	
	DTVALUE _ShadowCamValue;

	void SetShadowCamViewportInfo(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ);
	void SetShadowCamRectInfo(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	D3D12_VIEWPORT GetShadowCamViewPort() { return _ShadowCamValue.ShadowCamViewport; }
	D3D12_RECT GetShadowCamRect() { return _ShadowCamValue.ShadowCamRect; }
	DepthTexInfo GetShadowCamViewportRectInfo() { return _ShadowCamValue; }
	_float GetShadowCamViewPortWidth() { return _ShadowCamValue.ShadowCamViewport.Width; }
	_float GetShadowCamViewPortHeight() { return _ShadowCamValue.ShadowCamViewport.Height; }
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta)  override;
};

END

