#pragma once
#include "CComponent.h"
class CPolygon;

typedef vector<shared_ptr<CPolygon>> POLYGONS;

class CBuffer : public CComponent {
protected:
	explicit CBuffer() = default;
	explicit CBuffer(const CBuffer& _rhs);
	virtual ~CBuffer() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas);
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
public:
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
	HRESULT Render(const HDC& _hDC);
private:
	void Draw2DLine(const HDC& _hDC,  _float3 _vPrev,  _float3 _vNext, 
		const shared_ptr<class CGameInstance>& _pGameInstance);
protected:
	void Bind_Polygon(const POLYGONS& _value);
public:
	const shared_ptr<POLYGONS>& Get_Vector() { return m_PolygonDatas; }
	void Set_Color(const _float3& _vColor) { this->m_vColor = _vColor; }
private:
	std::shared_ptr<POLYGONS>		m_PolygonDatas{ nullptr };
	_float3												m_vColor{};
	 shared_ptr<_uint>						m_PolygonCnt{ 0 };
};

