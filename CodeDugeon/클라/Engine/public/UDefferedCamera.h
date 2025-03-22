#pragma once

#include "UCamera.h"

BEGIN(Engine)

class UDefferedCamera : public UCamera {
public:
	UDefferedCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UDefferedCamera(const UDefferedCamera& _rhs);
	DESTRUCTOR(UDefferedCamera)

public:
	virtual void Free() override;
	CLONE_MACRO(UDefferedCamera, "UDefferedCamera::Clone To Failed")
		virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta)  override;
};

END
