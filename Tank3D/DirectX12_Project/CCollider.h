#pragma once
#include "CComponent.h"

class CGameObject;
class CActor;
class CRenderer;

typedef enum class ColliderType {

	COLL_OBB, COLL_AABB, COLL_SPHERE, COLL_END
}COLLIDERTYPE;

class CCollider abstract : public CComponent {
public:
	typedef  struct tagColliderDesc {
		_float3 vPosition{};
		_float3 vScale{0.5f, 0.5f, 0.5f};
	}COLLDESC;
protected:
	explicit CCollider() = default;
	explicit CCollider(const COLLIDERTYPE _eType) :m_eColliderType(_eType) {}
	CCollider(const CCollider& _rhs);
	virtual ~CCollider() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) = 0;
	virtual void Free() override;
protected:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
public:
	virtual void Tick(const _double& _dTimeDelta) override;

	// Is Collider
	virtual _bool IsCollider(const shared_ptr<CCollider>& _pCollider) = 0;
#ifdef _DEBUG
public:
	void Add_RendererDebugActor(const shared_ptr<CRenderer>& _pRenderer);
protected:
	void CreateDebugActor();
private:
	// Create Actor 
	void CreateActor(const _wstring& _strProtoTag, const _wstring& _pCloneTag, 
		const VOIDDATAS& _tDatas = {});
#endif
public:
	const COLLIDERTYPE Get_CollderType() const { return m_eColliderType; }
	// Pos Scale
	const _float3& Get_Position() const { return m_vPosition; }
	const _float3& Get_Scale() const { return m_vScale; }
public:
	// Pos, Scale
	void Set_Position(const _float3& _vPos) { this->m_vPosition = _vPos; }
	virtual void Set_Scale(const _float3& _vScale) { this->m_vScale = _vScale; }

#ifdef _DEBUG
	const shared_ptr<CActor>& Get_ShowActor() const { return m_pShowActor; }
#endif
private:
	COLLIDERTYPE		m_eColliderType{};
	_float3					m_vPosition{};
	_float3					m_vScale{};
#ifdef _DEBUG
	shared_ptr<CActor> m_pShowActor = nullptr;
#endif
};

