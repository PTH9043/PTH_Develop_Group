#pragma once
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct tagOBBDesc
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBBDESC;
public:
	typedef struct tagColliderDesc
	{
		_float3		vScale;
		_float3		vTranslation;
	}COLLIDERDESC;
private:
	explicit CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType);
	explicit CCollider(const CCollider& _rhs);
	virtual ~CCollider() = default;

public:
	const TYPE& Get_Type() const { return m_eType; }
	const _bool& Is_Collision() const { return m_bCollision; }

	_float3 Get_CurPosV3();
	_vector Get_CurPos();

	_float3 Get_CurScaleV3();
	_vector Get_CurScale();

	 _float3 Get_Scale();
	_float3 Get_Translate();

	_matrix	Get_TransformMatrix() { return XMLoadFloat4x4(&m_TransformMatrix); }

	void Init_Collision() { m_bCollision = false; }
	void Set_Scale(const _float3& _v3Scale);
	void Set_Translate(const _float3& _v3Translate);
#ifdef _DEBUG
	void Set_Color(const _vector& _v4) { XMStoreFloat4(&m_v4Color, _v4); }
#endif // DEBUG

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Prototype_NotClone(void* _pArg = nullptr);
	virtual HRESULT NativeConstruct_Clone(void* _pArg);
public:
	void		Tick(_fmatrix TransformMatrix);
#ifdef _DEBUG
public:
	virtual HRESULT Render_Debug() override;
#endif // _DEBUG
public:
	_bool Is_Collision(CCollider* pCollider);
	_bool Is_Collision_AABB(CCollider* pCollider);
	_bool Is_Collision_OBB(CCollider* pCollider);
	_bool Is_Collision_SPHERE(CCollider* pCollider);

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	_float3 Compute_MinPoint();
	_float3 Compute_MaxPoint();
	OBBDESC Compute_OBBDesc();

private:
	BoundingBox* m_pAABB_Original = nullptr;
	BoundingOrientedBox* m_pOBB_Original = nullptr;
	BoundingSphere* m_pSphere_Original = nullptr;

	BoundingBox* m_pAABB = nullptr;
	BoundingOrientedBox* m_pOBB = nullptr;
	BoundingSphere* m_pSphere = nullptr;
	TYPE										m_eType = TYPE_END;

	_bool										m_bCollision = false;
	_float4x4									m_TransformMatrix;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	_float4									m_v4Color;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType);
	static CCollider* Create_NotClone(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType, void* _pArg = nullptr);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free();
};

END