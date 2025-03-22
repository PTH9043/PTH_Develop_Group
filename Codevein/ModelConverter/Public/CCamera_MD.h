#pragma once
#include "ModelDefines.h"
#include "CCamera.h"

BEGIN(Model)

class CCamera_MD final : public CCamera
{
private:
	explicit CCamera_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_MD(const CCamera_MD& _rhs);
	virtual ~CCamera_MD() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Show Cam Control Data
	_bool Show_CamControlData();
protected:
	// Move
	void Move(const _double& _dDeltaSeconds);
public:
	static CCamera_MD* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END