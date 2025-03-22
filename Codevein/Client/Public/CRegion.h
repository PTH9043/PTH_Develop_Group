#pragma once
#include "Client_Defines.h"
#include"CBase.h"

BEGIN(Engine)
class CNavigation;
class CCell;
class CCollider;
class CRenderer;
END

BEGIN(Client)
class CGameObject_Client;
class CCubeObj;
class CWorldObj;

class CRegion : public CBase
{
public:
#ifdef _DEBUG
	typedef struct tagCubeObjs
	{
		CCell*				pCell = nullptr;
		CCubeObj*	pObj1 = nullptr;
		CCubeObj*	pObj2 = nullptr;
		CCubeObj*	pObj3 = nullptr;

		void Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CCell* _pCell);
		void Rebalance();
		void Delete(CCell* _pCell);
		void Release();
		void Render_Objs(CRenderer* _pRenderer);
	}CUBOBJS;
#endif
private:
	explicit CRegion(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CRegion() = default;
public:
	CNavigation* Get_Navigation() { return m_pNavigation; }

	const _uint& Get_Index() const { return m_iIndex; }
	void Set_Index(const _uint& _iIndex) { this->m_iIndex = _iIndex; }

	list<CWorldObj*> Get_WorldObjList() { return m_WorldObjList; }
public:
	virtual HRESULT NativeConstruct();
#ifdef _DEBUG
	virtual HRESULT Add_Render();
#endif
	HRESULT Add_Cell(class CCell* _pCell);
	HRESULT Modify_Cells();
	HRESULT Show_Cells();
	HRESULT Clear_Cell();

	HRESULT LoadFile(const _wstring& _strFile);
	HRESULT SaveFile(const _wstring& _strFile);
	// Is Collision
	_bool Is_Collision(CCollider* _pCollider);
	// Control Collider
	void Control_Collider();
	// Add Neighbor Region
	void Add_NeighborRegion(CRegion* _pRegion);
	// C
	void Clear_NeightborRegion() { m_NeighborRegion.clear(); }
	// Collision Ohter Region
	CRegion* Is_Collision_OtherRegion(CCollider* _pCollider);
	// Add World Objects
	void Update_WorldObjects();
protected:
	CNavigation*						m_pNavigation = nullptr;
	_uint										m_iIndex = 0;
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	// NeighborRegion 
	list<CRegion*>					m_NeighborRegion;
	// World Obj List
	list<CWorldObj*>				m_WorldObjList;
#ifdef _DEBUG
	list<CUBOBJS>					m_CubeObjList;
	// Rederer
	CRenderer*							 m_pRenderer = nullptr;
#endif
public:
	static CRegion* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual void Free() override;
};

END