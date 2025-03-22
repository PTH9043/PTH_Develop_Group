#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_UNAVIGATION_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_UNAVIGATION_H

#include "ACoreBase.h"

BEGIN(Core)
/*
@ Date: 2023-01-14, Writer: 박태현
@ Explain
-  해당 길 위에 있지 않는지 판단하고, 내가 원하는 위치까지를 찾아줄 클래스
 Index 같은 경우 공용 값이 아니므로 datarace가 일어날 일이 존재하지 않는다. 그래서 atomic을 쓰지 않는다.
*/
class ACell;
class ATransform;
class ACollider;
using CELLCONTAINER = VECTOR<SHPTR<ACell>>;

class CORE_DLL ANavigation final : public ACoreBase {
public:
	typedef struct tagNavDesc {
		_uint iCurIndex{ 0 };
	}NAVDESC;
public:
	ANavigation();
	ANavigation(const ANavigation& _rhs);
	DESTRUCTOR(ANavigation)
public:
	_bool NativeConstruct(const _string& _Paths);
	// Compute Height
	const _float  ComputeHeight(const Vector3& _vPosition);
	void ComputeHeight(CSHPTRREF<ATransform> _spTransform);
	// Move
	_bool IsMove(const Vector3& _vPosition, SHPTR<ACell>& _spCell);
	_bool IsMove(_int _iCurOnCellIndex, const Vector3& _vPosition, SHPTR<ACell>& _spCell);
	// Find
	SHPTR<ACell> FindCell(const Vector3& _vPosition);
	SHPTR<ACell> FindCellWithoutUpdate(const Vector3& _vPosition);
	SHPTR<ACell> FindCell(const _int& _iIndex);
	SHPTR<ACell> FindCellWithoutUpdate(const _int& _iIndex);
	// Is Collision
	_bool IsCollision(SHPTR<ACollider>& _pCollider);
	// Insert Cell
	void AddCell(SHPTR<ACell>& _spCell);
	// SaveLoad
	_bool Load(const _string& _wstrPath);

	CELLCONTAINER GetCells() const { return m_CellContainer; }
	const _int GetCurIndex() const { return m_iCurIndex; }
	void SetCurIndex(const _int& _iIndex) { m_iCurIndex = _iIndex; }
	// Get Collider
	CSHPTRREF<ACollider> GetCollider() { return m_spCollider; }
	CSHPTRREF<ACell> GetCurCell() { return m_spCurCell; }
	CSHPTRREF<ACell> GetPrevCell() { return m_spPrevCell; }

	SHPTR<ACell> GetCell(const _int _index) { return m_CellContainer[_index]; }

	void SetCurCell(CSHPTRREF<ACell> newCell);
	// Ready Neighbor
	_bool ReadyNeighbor();

	/*
	@ Date: 2024-05-14, Writer: 이성현
	@ Explain
	-  슬라이딩 벡터 계산 함수.
	*/
	Vector3 ClampPositionToCell(const Vector3& position);

	/*
	@ Date: 2024-06-03, Writer: 이성현
	@ Explain
	-  A* 알고리즘
	*/
	_float Heuristic(const Vector3& a, const Vector3& b);
	VECTOR<Vector3> ComputePath(PathFindingState& _PathFindingState);
	PathFindingState StartPathFinding(SHPTR<ATransform> _spStartTr, SHPTR<ATransform> _spEndTr);
	PathFindingState StartPathFinding(const Vector3 start, const Vector3 end);
	PathFindingState StartPathFinding(const Vector3& start, const Vector3& end, CSHPTRREF<ACell> _startCell, CSHPTRREF<ACell> _endCell);
	bool StepPathFinding(PathFindingState& state);
	VECTOR<Vector3> OptimizePath(const VECTOR<SHPTR<ACell>>& path, const Vector3& start, const Vector3& end);
	bool LineTest(const Vector3& start, const Vector3& end);
private:
	virtual void Free() override;
private:
	CELLCONTAINER						m_CellContainer;
	SHPTR<ACell>							m_spCurCell;
	SHPTR<ACell>							m_spPrevCell;
	ATOMIC<_int>							m_iPrevIndex;
	ATOMIC<_int>							m_iCurIndex;
	SHPTR<ACollider>					m_spCollider;
};

END

#endif //_SERVERFRAMEWORK_CORE_PUBLIC_UNAVIGATION_H