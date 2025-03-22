#pragma once
#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Client)

class CCinematicCamera final : public CCamera
{
public:
	typedef struct tagCMCAM
	{
		CAMDESC tCamDesc;
		_float3       v3TargetPosition = _float3(0.f, 0.f, 0.f);
		_float3		v3StartPosition = _float3(0.f, 0.f, 0.f);
	}CMCAM;
private:
	explicit CCinematicCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCinematicCamera(const CCinematicCamera& _rhs);
	virtual ~CCinematicCamera() = default;

public:
	void Set_TargetPosition(const _float3& _v3TargetPosition) { this->m_v3TargetPosition = _v3TargetPosition; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

private:
	// Target Position
	_float3		m_v3TargetPosition = _float3(0.f, 0.f, 0.f);
	_float3		m_v3StartPosition = _float3(0.f, 0.f, 0.f);
public:
	static CCinematicCamera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END