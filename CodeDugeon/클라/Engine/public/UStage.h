#pragma once

#include "UComponent.h"
#include <map>
#include "UCell.h"

BEGIN(Engine)
class URegion;
class UCell;
class UCollider;

using REGIONLIST = VECTOR<SHPTR<URegion>>;

class UStage : public UComponent
{
public:

	UStage(CSHPTRREF<UDevice> _spDevice);
	UStage(const UStage& _rhs);
	DESTRUCTOR(UStage)

public:
	CLONE_MACRO(UStage, "Stage::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	
	CSHPTRREF<REGIONLIST> GetRegionList() { return m_spRegionList; }
	SHPTR<URegion> GetRegion(const _uint& _iIndex);


#ifdef _EDIT_NAVI
	HRESULT AddRender(const _uint& _iIndex, _int& begin, _int& end);
	HRESULT AddRenderAll(const _uint& _iIndex);
	HRESULT AddCell(const _uint& _iCellIndex, SHPTR<UCell>& _pCell);
	HRESULT RearrangeCells(const _uint& _iCellIndex);
	HRESULT ShowCells(const _uint& _iCellIndex);
	HRESULT ClearCell(const _uint& _iCellIndex);
	HRESULT SetColor(const _uint& _iCellIndex);
	HRESULT SetRegionName(const _uint& _iCellIndex);
	HRESULT FlushDeleteCells();
	HRESULT CreateRegion();
	_int SelectRegion();
	HRESULT Delete_Region(_uint& _iIndex);
	// Is Collision
	_bool Is_Collision(SHPTR<UCollider>& _pCollider, SHPTR<URegion>* _ppOut = nullptr);

#endif
	_bool Load();
	_bool Save(const _wstring& _wstrPath);

	HRESULT SetCellJumpState(const _uint& _iCellIndex);

	// Update Region
	void UpdateRegion();
	// Add Arround Region
	void AddArroundRegion();

protected:
	SHPTR<REGIONLIST> m_spRegionList;
	_bool	m_bInitRegion;

};

END