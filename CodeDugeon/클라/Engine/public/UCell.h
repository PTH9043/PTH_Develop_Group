#pragma once
#include "UObject.h"
//#define _EDIT_NAVI

BEGIN(Engine)
class UDefaultCell;
class UVIBufferCell;
class URenderer;
class UPawn;

class UCell : public UObject{
public:
	enum POINT : _uint { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE : _uint { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	// CellDecs
	typedef struct tagCellDesc {
		ARRAY<_float3, POINT_END>	vPoints{};
		ARRAY<_float3, LINE_END>	vLine{};
		ARRAY<_float3, LINE_END>	vNormal{};
		ARRAY<_int, LINE_END>		iNeighbor{-1, -1, -1};
		_int						iIndex{ -1 };
		_bool						bisJumpable{ false };
	}CELLDECS;

public:
	UCell(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(UCell)
	DESTRUCTOR(UCell)
public:
	const _float3& GetPoint(const POINT& _ePoint) const { return m_arrPoints[_ePoint]; }
	const _float3& GetLine(const LINE& _eLine) const { return m_arrLines[_eLine]; }
	const ARRAY<_float3, POINT_END>& GetPoints() const { return m_arrPoints; }
	const ARRAY<_float3, LINE_END>& GetLines() const { return m_arrLines; }
	const ARRAY<_float3, LINE_END>& GetNormal() const { return m_arrNormals; }
	const ARRAY<_int, LINE_END>& GetNeighbor() const { return m_arrNeighbors; }
	const _float3& GetCenterPos() const { return m_vCenterPos; }
	const _int& GetIndex() const { return m_iIndex; }

	void SetPoint(const POINT& _ePoint, const _float3& _vPoint) { m_arrPoints[_ePoint] = _vPoint; }
	void SetCenterPos(const _float3& _vCenterPos) { this->m_vCenterPos = _vCenterPos; }
	void SetNeighbor(const LINE& _eLine, const _uint& _iIndex) { m_arrNeighbors[_eLine] = _iIndex; }
	void SetIndex(const _uint& _index) { m_iIndex = _index; }
public:
	virtual void Free() override;
	using	UObject::NativeConstruct;
	virtual  HRESULT NativeConstruct() override;
	// Points
	HRESULT NativeConstruct(ARRAY<_float3, POINT_END>& _Points, const _uint _iIndex);
	HRESULT NativeConstruct(const CELLDECS& _tCellDesc);

public:
	_bool IsIn(const _float3& _vPos, _int& _iNeightborIndex, _float3& _vLine);
	_bool IsComparePoints(const _float3& _vPointA, const _float3& _vPointB);
	_bool IsComparePoint(CSHPTRREF<UCell> _pCell);
	const _float ComputeHeight(const _float3& _vPosition);
	const _float3& GetColor() const { return m_f3Color; }

	_float3 GetClosestPointOnEdges(const _float3& position) const;
	_float3 ClosestPointOnLine(const _float3& lineStart, const _float3& lineEnd, const _float3& point) const;

	_float GetHeightAtXZ(const _float& x, const _float& z) const;

	_float3 GetXZatHeight(const _float& y) const;

	const _bool& GetJumpableState() const { return m_bisJumpable; }
	void SetJumpableState(_bool _isJump) { m_bisJumpable = _isJump; }
#ifdef _EDIT_NAVI
	CSHPTRREF<UDefaultCell> GetCellPawn() { return m_spCellPawn; }
	CSHPTRREF<UVIBufferCell> GetCellVIBuffer() { return m_spCellVIBuffer; }

	void ReBuffer();
	void ReRender();
	void AddCellRenderGroup();
#endif
private:
	// CrossResult
	void CalculateCrossResult(ARRAY<_float3, POINT_END>& _arrPointsEnd);
	void ResortPoints();
	void MakeLineAndNormal();
	
private:
	// Points
	ARRAY<_float3, POINT_END>		m_arrPoints;
	// Line
	ARRAY<_float3, LINE_END>		m_arrLines;
	// Normal
	ARRAY<_float3, LINE_END>		m_arrNormals;
	// Neighbor
	ARRAY<_int, LINE_END>			m_arrNeighbors;
	// CenterPos
	_float3							m_vCenterPos;
	// Index
	_int							m_iIndex;
	// Planes
	_float4							m_vPlane;
	// Color
	_float3							m_f3Color;

	_bool							m_bisJumpable;
#ifdef _EDIT_NAVI
private:
	SHPTR<UVIBufferCell> m_spCellVIBuffer;
	SHPTR<UDefaultCell> m_spCellPawn;
#endif // _DEBUGGING
};

END
