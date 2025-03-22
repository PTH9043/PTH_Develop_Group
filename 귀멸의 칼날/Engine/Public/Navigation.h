#pragma once
#include "Component.h"
#include "Cell.h"

BEGIN(Engine)
class CCell;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNatviDesc
	{
		_uint iCurIndex;
	}NAVIDESC;

private:
	explicit CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CNavigation(const CNavigation& _rhs);
	virtual ~CNavigation() = default;

public:
	vector<CCell*>* Get_Cells() { return &m_Cells; }
	const _int& Get_CurIndex() const { return m_iCurrentIndex; }
	void Set_CurIndex(const _int& _iIndex) { m_iCurrentIndex = _iIndex; }
	const _int& Get_CurOhterIndex() const { return m_iCurOhterIndex; }
	// Get Collider
	class CCollider* Get_Collider() { return m_pCollider; }
	CCell* Get_CurCell() { return m_pCurCell; }
	void Add_Cells(CCell* _pCells);
public:
	// NativeConstruct Prototype
	HRESULT NativeConstruct_Prototype(const _tchar* _pPath);
	HRESULT NativeConstruct_Prototype_Init();
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	// Compute_Height
	_float Compute_Height(_vector vPosition);
	// Compute Slide Vector
	_float3 Compute_SlideVector(const _vector& vCurPos, const _vector& vTargetPos);

public:
	void Load_File(const _tchar* _pPath);
	void Save_File(const _tchar* _pPath, const _uint& _iIndex);
public:
	_bool Is_Move(const _fvector& _vPosition, CCell** _ppOut = nullptr);
	_bool Is_MoveOther(const _fvector& _vPosition, CCell** _pNode = nullptr);
	// Is Collision
	_bool Is_Collision(class CCollider* _pCollider);
	// Find Cell
	CCell* Find_Cell(const _vector& _vPos);
#ifdef _DEBUG
public:
	virtual HRESULT Render_Debug() override;
#endif // _DEBUG
private:
	_int					m_iCurrentIndex = 0;
	_int					m_iCurOhterIndex = 0;
private:
	typedef vector<CCell*> CELLS;
	CELLS m_Cells;
	// Cur Cell
	CCell* m_pCurCell = nullptr;
#ifdef _DEBUG
private:
	class CShader* m_pShader = nullptr;
#endif // _DEBUG
	class CCollider* m_pCollider = nullptr;
private:
	HRESULT Ready_Neighbor();
public:
	static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pPath);
	static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END