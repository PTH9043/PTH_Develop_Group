#pragma once

#include "UCamera.h"

BEGIN(Engine)

class  ULightCamera : public UCamera {
public:
	ULightCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	ULightCamera(const ULightCamera& _rhs);
	DESTRUCTOR(ULightCamera)
public:
	virtual void Free() override;
	CLONE_MACRO(ULightCamera, "ULightCamera::Clone To Failed")
		virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta)  override;
};

END

