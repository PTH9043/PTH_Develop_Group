#pragma once
#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Client)

class CCamera_Logo final : public CCamera
{
private:
	explicit CCamera_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Logo(const CCamera_Logo& _rhs);
	virtual ~CCamera_Logo() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	static CCamera_Logo* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END