#pragma once
#include "UCamera.h"

BEGIN(Engine)

class USmallDefferedCamera : public UCamera {
public:
	USmallDefferedCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	USmallDefferedCamera(const USmallDefferedCamera& _rhs);
	DESTRUCTOR(USmallDefferedCamera)

public:
	virtual void Free() override;
	CLONE_MACRO(USmallDefferedCamera, "USmallDefferedCamera::Clone To Failed")
		virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta)  override;
};

END
