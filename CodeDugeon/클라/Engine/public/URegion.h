#pragma once
#include "UComponent.h"
#include "UCell.h"

BEGIN(Engine)
class UNavigation;
class UCell;
class UCollider;
class URenderer;
class UDefaultDebugging;

class URegion : public UComponent
{
public:
#ifdef _EDIT_NAVI
	typedef struct tagCubeObjs
	{
		SHPTR<UCell> spCell;
		SHPTR<UDefaultDebugging> spCube1;
		SHPTR<UDefaultDebugging> spCube2;
		SHPTR<UDefaultDebugging> spCube3;

		void Create(SHPTRREF <UCell> _pCell);
		void Rebalance();
		void AddCubesRenderGroup();
	}CUBOBJS;
	HRESULT AddAllCellsRenderGroup();
	HRESULT AddCellsRenderGroup(_int& begin, _int& end);
#endif
	URegion(CSHPTRREF<UDevice> _spDevice);
	URegion(const URegion& _rhs);
	DESTRUCTOR(URegion)

public:
	CLONE_MACRO(URegion, "Region::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

public:
	SHPTR<UNavigation> GetNavigation() { return m_spNavigation; }
	const _uint& Get_Index() const { return m_iIndex; }
	const _wstring& Get_Name() const { return m_wsRegionName; }
	void Set_Index(const _uint& _iIndex) { this->m_iIndex = _iIndex; }
	void SetName(const _wstring& _regionName)
	{
		m_wsRegionName = _regionName;
	}
#ifdef _EDIT_NAVI
	HRESULT AddCell(SHPTR<UCell>& _pCell);
	HRESULT ShowCells();
	HRESULT RearrageCells();
	HRESULT RearrangeCellsByHeight();
	HRESULT ClearCell();
	HRESULT SetColor();
	HRESULT SetName();
	HRESULT DeleteLatestCell();
	HRESULT DeleteCell(const _uint& _iIndex);
	void FlushDeleteCells();
	const _bool& IsDeletion() const { return m_bDeletionEnabled; }
#endif
	_bool Load(const _wstring& _wstrPath);
	_bool Save(const _wstring& _wstrPath);

	// Is Collision
	_bool Is_Collision(SHPTR<UCollider>& _pCollider);

	void ClearNeightborRegion() { m_NeighborRegion.clear(); }
	// Add Neighbor Region
	void Add_NeighborRegion(CSHPTRREF<URegion> _pRegion);


private:
	_float3				m_f3Color;
protected:
	SHPTR<UNavigation>	m_spNavigation;
	_uint	m_iIndex = 0;
	LIST<WKPTR<URegion>>	m_NeighborRegion;
	_wstring m_wsRegionName;
	_bool m_bEditName;

#ifdef _EDIT_NAVI
	SET<SHPTR<UCell>> m_DeleteCellsList;
	_bool m_bDeletionEnabled;

	LIST<CUBOBJS>	m_CubeObjList;
	SET<SHPTR<UDefaultDebugging>> m_DeleteCubesList;
#endif
};

END