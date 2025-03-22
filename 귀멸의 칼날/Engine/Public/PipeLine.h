#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ,  D3DTS_PREVVIEW ,D3DTS_END };
public:
	DECLARE_SINGLETON(CPipeLine);
private:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_TransformMatrix(const TRANSFORMSTATE& _eTransState) { return XMLoadFloat4x4(&m_StateMatrix[_eTransState]); }
	_matrix Get_TransformMatrixInv(const TRANSFORMSTATE& _eTransState);
	_vector Get_CamPosition() const { return XMLoadFloat3(&m_vCamPosition); }
	_float3 Get_CamPositionF3() { return m_vCamPosition; }
	_float4x4 Get_TransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState) const;
	_float4x4 Get_TransformFloat4x4Inv_TP(const TRANSFORMSTATE& _eTransState) const;
	_float	Get_CamFar() const { return m_fCamFar; }

	void Set_Transform(const TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix);
	void Set_CamFar(_float fCamFar) { m_fCamFar = fCamFar; }

	_vector	Get_WorldToScreen(ID3D11DeviceContext* pContext, _vector vWorldPos);
	_vector Get_ScreenToWorld(ID3D11DeviceContext* pContext, _vector vScreenPos, _vector vDepthWorldPos);

public:
	_float4x4 Get_LightTransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState) const;
	_float3 Get_LightPositionF3() const { return m_vLightPosition; }
	_float	Get_LightFar() const { return m_fLightFar; }

	void	Set_LightTransform(const TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix);
	void	Set_LightFar(_float fLightFar) { m_fLightFar = fLightFar; }

	void	Set_LightViewMatrix(_vector vPosition, _vector vLookAt);
	void	Set_LightProjMatrix(const _float& fov, const _float& fAspect, const _float& fNear, const _float& fFar);

private:
	_float4x4			m_StateMatrix[D3DTS_END];
	_float3				m_vCamPosition;
	_float					m_fCamFar;

	_float4x4			m_LightMatrix[D3DTS_END];
	_float3				m_vLightPosition;
	_float					m_fLightFar;
public:
	virtual void Free() override;
};

END