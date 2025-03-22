#pragma once
#include "Base.h"
#include "VIBuffer_Cell.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
public:
	typedef struct tagCellDesc
	{
		_float3				vPoints[POINT_END];
		_float3				vLine[LINE_END];
		_float3				vNormal[LINE_END];
		_int						iNeighbor[LINE_END] = { -1, -1, -1 };
		_uint					iIndex = -1;
	}CELLDESC;

private:
	explicit CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(const POINT& _ePoint) { return XMLoadFloat3(&m_vPoints[_ePoint]); }
	_float3 Get_PointV3(const POINT& _ePoint) { return m_vPoints[_ePoint]; }

	_vector Get_Line(const LINE& _eLine) { return XMLoadFloat3(&m_vLine[_eLine]); }
	_float3 Get_LineV3(const LINE& _eLine) { return m_vLine[_eLine]; }

	_uint Get_Index() const { return m_iIndex; }

	void Set_Point(const POINT& _ePoint, _fvector& _vPos) { XMStoreFloat3(&m_vPoints[_ePoint], _vPos); }
	void Set_PointV3(const POINT& _ePoint, const _float3& _v3Pos) { XMStoreFloat3(&m_vPoints[_ePoint], XMLoadFloat3(&_v3Pos)); }
	// Get Points
	const _float3* Get_Points() { return m_vPoints; }
	const _float3* Get_Lines() { return m_vLine; }
	const _float3* Get_Normal() { return m_vNormal; }
	const _int* Get_Neighbor() { return m_iNeighbor; }

	const _float3& Get_CenterPosV3()  const { return m_vCenterPos; }
	_vector Get_CenterPos() { return XMLoadFloat3(&m_vCenterPos); }

	void Set_Neighbor(const LINE& _eLine, const _uint& _iIndex) { m_iNeighbor[_eLine] = _iIndex; }

	_bool IsSame(_float3* _pPoints);
public:
	HRESULT NativeConstruct(const _float3* _pPoints, const _uint& _iIndex);
	HRESULT NativeConstruct(const CELLDESC& _tCellDesc);
#ifdef _DEBUG
	void ReBuffer();
#endif
public:
	_bool IsIn(const _fvector& _vPos, _int* _pNeightborIndex = nullptr, _float3* _pLine = nullptr);
	_bool ComparePoints(const _fvector& _PointA, const _fvector& _PointB);
	_bool ComparePoints(CCell* _pCell);
	// Compute_Height
	_float Compute_Height(_vector vPosition);
#ifdef _DEBUG
public:
	HRESULT Render_Debug();
#endif // _DEBUG
public:
	// Get Cross Result
	HRESULT Get_CrossResult(_float3* _pCrossResult);
	// Cross sort
	HRESULT Resort_Points();
protected:
	void Make_NormalAndLine();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	_float3				m_vPoints[POINT_END];
	_float3				m_vLine[LINE_END];
	_float3				m_vNormal[LINE_END];
	_float3				m_vCenterPos = _float3(0.f, 0.f, 0.f);
	_int						m_iNeighbor[LINE_END] = { -1, -1, -1 };
	_int 					m_iIndex = 0;

#ifdef _DEBUG
private:
	CVIBuffer_Cell* m_pVIBuffer = nullptr;
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints,
		const _uint& _iIndex);
	static CCell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CELLDESC& _tCellDesc);
	virtual void Free() override;
};

END