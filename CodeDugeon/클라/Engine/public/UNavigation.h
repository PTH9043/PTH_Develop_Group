#pragma once

#include "UComponent.h"
#include "UCell.h"

BEGIN(Engine)
class UTransform;
class UCollider;
using CELLCONTAINER = std::vector<SHPTR<UCell>>;

class UNavigation : public UComponent{
public:
	typedef struct tagNavDesc {
		_uint iCurIndex{ 0 };
	}NAVDESC;

	struct CellPathNode {
		SHPTR<UCell> cell;
		_float cost;
		_float heuristic;
		_float totalCost;
		SHPTR<UCell> parent;

		bool operator>(const CellPathNode& other) const {
			return totalCost > other.totalCost;
		}
	};

	struct PathFindingState {
		std::priority_queue<CellPathNode, VECTOR<CellPathNode>, std::greater<CellPathNode>> openSet;
		std::unordered_map<SHPTR<UCell>, _float> costSoFar;
		std::unordered_map<SHPTR<UCell>, SHPTR<UCell>> cameFrom;
		SHPTR<UCell> endCell;
		bool pathFound = false;
		VECTOR<SHPTR<UCell>> path;
	};

public:
	UNavigation(CSHPTRREF <UDevice> _spDevice);
	UNavigation(const UNavigation& _rhs);
	DESTRUCTOR(UNavigation)
public:
	CLONE_MACRO(UNavigation, "Navigation::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	//HRESULT NativeConstruct(const VECTOR<SHPTR< UVIBufferTerrain>>& _vecTerrain);
	/*HRESULT NativeConstruct(const VECTOR<_float3>& _vecPosList);*/
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	// Compute Height
	const _float  ComputeHeight(const _float3& _vPosition);
	void ComputeHeight(CSHPTRREF<UTransform> _spTransform);
	// Move
	_bool IsMove(const _float3& _vPosition, SHPTR<UCell>& _spCell);
	// Find
	SHPTR<UCell> FindCell(const _float3& _vPosition);
	SHPTR<UCell> FindCellWithoutUpdate(const _float3& _vPosition);
	SHPTR<UCell> FindCell(const _int& _iIndex);
	SHPTR<UCell> FindCellWithoutUpdate(const _int& _iIndex);
	// Is Collision
	_bool IsCollision(SHPTR<UCollider>& _pCollider);
	// Insert Cell
	void AddCell(SHPTR<UCell>& _spCell);
	// SaveLoad
	_bool Load(const _wstring& _wstrPath);
	_bool Save(const _wstring& _wstrPath);

	CSHPTR<CELLCONTAINER> GetCells() const { return m_spCellContainer; }
	const _int& GetCurIndex() const { return m_iCurIndex; }
	void SetCurIndex(const _int& _iIndex) { m_iCurIndex = _iIndex; }
	// Get Collider
	CSHPTRREF<UCollider> GetCollider() { return m_spCollider; }
	CSHPTRREF<UCell> GetCurCell() { return m_spCurCell; }
	CSHPTRREF<UCell> GetPrevCell() { return m_spPrevCell; }

	void SetCurCell(CSHPTRREF<UCell> newCell) { m_spCurCell = newCell; SetCurIndex(newCell->GetIndex()); }
	// Ready Neighbor
	HRESULT ReadyNeighbor();

	/*
	@ Date: 2024-05-14, Writer: 이성현
	@ Explain
	-  슬라이딩 벡터 계산 함수.
	*/
	_float3 ClampPositionToCell(const _float3& position);

	/*
	@ Date: 2024-06-03, Writer: 이성현
	@ Explain
	-  A* 알고리즘
	*/
	_float Heuristic(const _float3& a, const _float3& b);
	UNavigation::PathFindingState StartPathFinding(const _float3& start, const _float3& end, CSHPTRREF<UCell> _startCell, CSHPTRREF<UCell> _destCell);
	bool StepPathFinding(PathFindingState& state);
	VECTOR<_float3> OptimizePath(const VECTOR<SHPTR<UCell>>& path, const _float3& start, const _float3& end);
	bool LineTest(const _float3& start, const _float3& end);

	SHPTR<UCell> ChooseRandomNeighborCell(int iterations);

private:
	SHPTR<CELLCONTAINER>		m_spCellContainer;
	SHPTR<UCell>				m_spCurCell;
	SHPTR<UCell>				m_spPrevCell;
	_int						m_iPrevIndex;
	_int						m_iCurIndex;
	SHPTR<UCollider>			m_spCollider;

};

END

