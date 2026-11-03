#pragma once
#include "CCollider.h"

class CAABBCollider  : public CCollider{
protected:
	explicit CAABBCollider() : CCollider(COLLIDERTYPE::COLL_AABB) {}
	CAABBCollider(const CAABBCollider& _rhs);
	virtual ~CAABBCollider() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
public:
	virtual void Tick(const _double& _dTimeDelta) override;
	// Is Collider
	virtual _bool IsCollider(const shared_ptr<CCollider>& _pCollider) override;

public:
	const shared_ptr<DirectX::BoundingBox>& Get_CollisionBox() const { return m_pCollisionBox; }
	//virtual void Set_Scale(const _float3& _vScale) { __super::Set_Scale(_vScale); m_pOriginBox->Extents = _vScale; }
private:
	// Oriented Box
	shared_ptr<DirectX::BoundingBox> m_pOriginBox{ nullptr };
	// Bounding Oriendted Box 
	shared_ptr<DirectX::BoundingBox> m_pCollisionBox{ nullptr };
};

