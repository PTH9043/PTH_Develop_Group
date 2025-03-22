#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CCollision abstract : public CComponent
{
public:
	typedef struct tagCollDesc
	{
		CTransform* pTransform = nullptr;
		_float3 v3Pos;
		_float3 v3Size;
	}COLLDESC;
public:
	enum TYPE { TYPE_SPEHRE, TYPE_CUBE, TYPE_END };
protected:
	explicit CCollision(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCollision(const CCollision& _rhs);
	virtual ~CCollision() = default;
public:
	const TYPE& Get_Type() const { return m_eType; }
	CTransform* Get_Transform() { return m_pTransform; }
	_fvector Get_Size() { return XMLoadFloat3(&m_v3Size); }
	_fvector Get_Pos() { return XMLoadFloat3(&m_v3Pos); }
	// Pos
	void Set_Pos(const _fvector& _vPos) { XMStoreFloat3(&m_v3Pos, _vPos); }
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	// Collision
	virtual _bool Is_CollisionPos(const _fvector& _v3MousePos) = 0;
	virtual _bool Is_CollisionOtherComp(CCollision* _pCollision) = 0;
	virtual _bool Is_CollisionOtherComp(const TYPE& _eType, const _fvector _v3OtherPos, const _fvector& _v3OtherSize) = 0;
protected:
	// Type
	TYPE m_eType			= TYPE_END;
	// Coll Desc
	CTransform* m_pTransform = nullptr;
	// Position
	_float3						m_v3Pos;
	// Collision
	_float3						m_v3Size;
public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END