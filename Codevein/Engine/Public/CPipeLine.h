#pragma once
#include "CBase.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
public:
	DECLARE_SINGLETON(CPipeLine);
private:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_TransformMatrix(const TRANSFORMSTATE& _eTransState) { return XMLoadFloat4x4(&m_StateMatrix[_eTransState]); }
	_vector Get_CamPosition() const { return XMLoadFloat3(&m_v3CamPosition); }
	_float3 Get_CamPositionF3() { return m_v3CamPosition; }
	_float4x4 Get_TransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState);
	void Set_Transform(const TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix) { XMStoreFloat4x4(&m_StateMatrix[_eState], _StateMatrix ); }

public:
	void Tick();

private:
	_float4x4 m_StateMatrix[D3DTS_END];
	_float3 m_v3CamPosition;

public:
	virtual void Free() override;
};

END