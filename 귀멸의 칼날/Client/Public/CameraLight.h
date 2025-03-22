#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCameraLight : public CCamera
{
private:
	explicit CCameraLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCameraLight(const CCameraLight& _rhs);
	virtual ~CCameraLight() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void Set_LightViewMatrix(_vector vPos, _vector vLookAt);

private:
	// Move
	void	FreeCam_Move(const _double& _dTimeDelta);

public:
	static CCameraLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
