#pragma once

#include "UCamera.h"

BEGIN(Client)
class CMainCamera;

class CFreeModeCamera : public UCamera {
public:
	CFreeModeCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CFreeModeCamera(const CFreeModeCamera& _rhs);
	DESTRUCTOR(CFreeModeCamera)
public:
	CLONE_MACRO(CFreeModeCamera, "CFreeModeCamera::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void SetMainCamera(CSHPTRREF<CMainCamera> _spMainCamera) { this->m_spMainCamera = _spMainCamera; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
private:
	SHPTR<CMainCamera>		m_spMainCamera;
};

END