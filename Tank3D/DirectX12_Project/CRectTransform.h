#pragma once
#include "CComponent.h"

class CTransform;

class CRectTransform  : public CComponent {
public:
	typedef struct tagRectTransformDesc
	{
		// Pos, Size
		_float2 v2Pos;
		_float2 v2Size;
		// Tr
		weak_ptr<CTransform> pTransform;
	}RECTDESC;
protected:
	explicit CRectTransform() = default;
	CRectTransform(const CRectTransform& _rhs) : CComponent(_rhs) {}
	virtual ~CRectTransform() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

public:
	// Tick 
	virtual void Tick(const _double& _dTimeDelta);
	// Judge Mouse On Rect
	_bool Is_MouseOnRect(const _float2& _v2MousePos);
protected:
	// Resize
	void ResizeMinMax(const _float2& _vPos, const _float2& _vSize);
public:
	// Set Ratio
	void SetRatio(const _float _fRatio) { this->m_fRatio = _fRatio; }
public:
	const _float2& GetPos() const { return m_v2Pos; }
	const _float2& GetSize() const { return m_v2Size; }
private:
	weak_ptr<CTransform>		m_pTransform{};
	// Min Max 
	_float2									m_v2MinPos{ 0.f, 0.f };
	_float2									m_v2MaxPos{0.f, 0.f};
	// Pos, Size
	_float2									m_v2Pos{ 0.f, 0.f };
	_float2									m_v2Size{ 0.f, 0.f };
	// Projection
	//_float4x4								m_ProjectMatrix{ IDENTIFY };
	// Ratio 
	_float										m_fRatio{ 0.f };
};

