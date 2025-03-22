#pragma once
#include "CBase.h"
#include "CRegion.h"

BEGIN(Client)

class CStage : public CBase
{
private:
	explicit CStage(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CStage() = default;
public:
	map<_uint, CRegion*> Get_RegionList() { return m_RegionList; }
	CRegion* Get_Region(const _uint& _iIndex);

public:
	virtual HRESULT NativeConstruct();
#ifdef _DEBUG
	virtual HRESULT Render(const _uint& _iIndex);
	void All_Render();
#endif
public:
	HRESULT Add_Cell(const _uint& _iCellIndex, class CCell* _pCell);
	HRESULT Modify_Cells(const _uint& _iCellIndex);
	HRESULT Show_Cells(const _uint& _iCellIndex);
	HRESULT Clear_Cell(const _uint& _iCellIndex);

	HRESULT LoadFile(const _wstring& _strFile);
	HRESULT SaveFile(const _wstring& _strFile);

	HRESULT Create_Region(const _uint& _iIndex);
	_int Select_Region();
	void Control_Collider(const _uint& _iIndex);
	HRESULT Delete_Region(_uint& _iIndex);
	// Is Collision
	_bool Is_Collision(CCollider* _pCollider, CRegion** _ppOut = nullptr);
	// Is Picking
	_bool Is_Picking(const _uint& _iIndex = 0);
	// ModelPicking
	_bool Is_ModelPicking(const _uint& _iIndex = 0);
	// Update Region
	void Update_Region();
	// Add Arround Region
	void Add_ArroundReigon();
protected:
	ID3D11Device*					 m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	typedef map<_uint, CRegion*> REGIONLIST;
	REGIONLIST m_RegionList;

	CRegion* m_pDeleteRegion = nullptr;

	_bool m_bInitRegion = false;
public:
	static CStage* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual void Free() override;
};

END