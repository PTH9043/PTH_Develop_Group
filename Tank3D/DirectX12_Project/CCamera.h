#pragma once
#include "CActor.h"

class CPipeLine;
class CEmptyObject;

class CCamera abstract : public CActor
{
public:
	typedef struct tagCamDesc
	{
		tagCamDesc() = default;
		tagCamDesc(const _float& _fWinWidth, const _float& _fWinHeight, const _float& _fNear, const _float& _fFar, 
			const _float3& _vLook, const _float3& _vPos)
			: fWinWidth(_fWinWidth), fWinHeight(_fWinHeight), fNear(_fNear), fFar(_fFar), vLook(_vLook), vPos(_vPos)
		{}

		_float fWinWidth{ 0.f };
		_float fWinHeight{ 0.f };
		_float fForv{ 0.f };
		_float fNear{ 0.f };
		_float fFar{ 0.f };
		_float3 vLook{ _float3(0.f, 0.f, 0.f) };
		_float3 vPos{ _float3(0.f, 0.f, 0.f) };

		_float	fCamSpeed{ 0 };
	}CAMDESC;
protected:
	explicit CCamera() = default;
	explicit CCamera(const CCamera& _rhs) : CActor(_rhs){}
	virtual ~CCamera() = default;
public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas);
	// Free
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

	void BindMatrix();
	void BindMatrix(CPipeLine* _pPipeLine);
public:
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render(const HDC& _hDC);

public:
	const _float4x4& Get_ProjMatrix() const { return m_ProjMatrix; }
	const _float& Get_CamSpeed() const { return m_fCamSpeed; }
private:
	// Proj Matrix
	_float4x4	m_ProjMatrix = IDENTIFY;
	_float			m_fCamSpeed{ 0.f };
};

