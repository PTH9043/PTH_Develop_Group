#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustCulling final : public CBase
{
	DECLARE_SINGLETON(CFrustCulling)
public:
	typedef struct tagFrustDesc
	{
		tagFrustDesc() = default;
		tagFrustDesc(const _float& _fFovy, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
			: fFovy(_fFovy), fAspect(_fAspect), fNear(_fNear), fFar(_fFar)
		{}
		_float fFovy = 0.f;
		_float fAspect = 0.f;
		_float fNear = 0.f;
		_float fFar = 0.f;
	}FRUSTDESC;
private:
	explicit CFrustCulling();
	virtual ~CFrustCulling() = default;
public:
	// NativeConstruct Frust Cull
	HRESULT NativeConstruct_FrustCull(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const CFrustCulling::FRUSTDESC& _tFrustDesc);
	// Update Matrix 
	void Update_Matrix();
	void Update_LocalMatrix(_fmatrix _WorldMatrix);
	// Rendering
	_bool Is_RenderingSphere(class CTransform* _pTransform, const _float& _fRadius = 1.f);
	// Rendering
	_bool Is_RenderingSphere(const _vector& _vPos, const _float& _fRadius = 1.f);

	_bool Is_Rendering_InLocal(const _vector& _vPos, const _float& _fRange = 0.f);
protected:
	// Judge Coll
	_bool Is_JudgeColl(const _float& _fDistnace, const _float& _fRadius = 1.f);
protected:
	_float4x4 m_ProjMatrixInv;
	_float3		m_FrustPosition[8] = {};
	_float3		m_WorldPosition[8] = {};
	_float4		m_vPlaneInWorld[6] = {};
	_float4		m_vPlaneInLocal[6] = {};
	FRUSTDESC m_tFrustDesc;
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	virtual void Free() override;
};

END