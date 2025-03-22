#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)

public:
	typedef struct tagPickingDesc
	{
		tagPickingDesc() = default;
		tagPickingDesc(class CGameObject* _pGameObject, class CTransform* _pTransform, const _float3& _v3Pos = _float3(0.f, 0.f, 0.f), const _float& _fDis = 0.f)
			: pGameObject(_pGameObject), pTransform(_pTransform), v3PickingPos(_v3Pos), fDist(_fDis)
		{}
		class CGameObject* pGameObject = nullptr;
		class CTransform* pTransform = nullptr;
		_float3						v3PickingPos = _float3(0.f, 0.f, 0.f);
		_float							fDist = 0.f;
	}PICKINGDESC;
private:
	explicit CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT NativeConstruct(const HWND& _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Compute_RayInWorldSpace();
	// Picking (Use Late Tick)
	_bool Picking_Mesh(class CGameObject* _pGameObject, class CTransform* _pTransform, class CVIBuffer* _pVIBuffer, _float3* _pOut = nullptr);
	// Picking MEsh
	_bool Picking_Mesh(class CGameObject* _pGameObject, class CTransform* _pTransform, class CVIBuffer* _pVIBuffer, _matrix _BoneMatrix);
	// Picking Mesh Nav
	_bool Picking_Mesh_Nav(class CGameObject* _pGameObject, class CTransform* _pTransform, class CVIBuffer* _pVIBuffer);
	// Get Picking Object
	PICKINGDESC* Get_Picking_GOB();
	// Release
	void Release_Pickings();
private:
	// Judge Get
	_bool Add_GameObject(const PICKINGDESC& _tPickDesc, const _bool& _bIsPosPick = false);
private:
	// HWND
	HWND									m_hWnd = NULL;
	// Com °´Ã¼
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// Mouse Picking
	_bool										m_bMousePicking = false;
private:
	typedef list<PICKINGDESC> GOBJECTS;
	GOBJECTS m_PickList;
	// Pick Info
	PICKINGDESC m_tPickInfo;
	// Ray Pos
	_float3 m_v3RayDir;
	_float3 m_v3RayPos;
public:
	virtual void Free() override;
};

END