
#include "UComponent.h"

BEGIN(Engine)
class UTransform;
class URenderer;
class UPawn;
class UCommand;
class UTableDescriptor;
class UDefaultDebugging;

class  UCollider final : public UComponent{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	struct COLLIDERDESC
	{
		_float3		vScale;
		_float3		vTranslation;

		COLLIDERDESC() : vScale{}, vTranslation{} {}
		COLLIDERDESC(const _float3& _vScale, const _float3& _vTranslation) : vScale{_vScale}, vTranslation{_vTranslation}{}
	};

	UCollider(CSHPTRREF <UDevice> _pDevice, const TYPE& _eType);
	UCollider(const UCollider& _rhs);
	DESTRUCTOR(UCollider)
public:
	const _float3& GetCurPos();
	const _float3& GetScale();
	const _float3& GetTranslate();
	const _float4x4& GetTransformMatrix() const { return m_mTransformMatrix; }

	void SetScale(const _float3& _vScale);
	void SetScaleToFitModel(const _float3& minVertex, const _float3& maxVertex);
	void SetTranslate(const _float3& _vTranslate);
	void SetTransform(const _float3& _vPos, const _float4& _vQuaternion);
	void SetTransform(CSHPTRREF< UTransform> _spTransform);
	void SetTransform(const _float4x4& _Matrix);
	// UComponent을(를) 통해 상속됨
	virtual void Free() override;
	CLONE_MACRO(UCollider, "UCollider::Clone To Failed")
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	_bool IsCollision(CSHPTRREF<UCollider> _pCollider);
	/*
	@ Date: 2024-03-11, Writer: 이성현
	@ Explain
	- collider와 ray의 충돌처리를 위한 함수
	*/
	_bool IsCollisionWithRay(const _float3& _vOrigin, const _float3& _vDirection, _float* _pDist);

	/*
	@ Date: 2024-06-11, Writer: 이성현
	@ Explain
	- 콜라이더 노말 추가
	*/
	void SetOBBNormals(const _float4x4& transformMatrix);
	_float3 CalculateOBBCollisionNormal(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingOrientedBox& box2) const;
	_float3 CalculateOBBSphereCollisionNormal(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingSphere& sphere) const;
	_float3 GetCollisionNormal(CSHPTRREF<UCollider> _pCollider);

#ifdef _USE_DEBUGGING

	void ChangeColliderColor(const _float3& _vChange);
	void AddRenderer(RENDERID _eID);

#endif
	void GetBoundingOrientedBoxCorners(const CSHPTRREF<DirectX::BoundingOrientedBox> box, _float3* Corners);
	_float3 GetFurthestPointFromBoundingOrientedBoxCenter(const CSHPTRREF<DirectX::BoundingOrientedBox> box);
	_float3 GetHeightAdjustedPointFromCenter(const CSHPTRREF<DirectX::BoundingOrientedBox> box,_bool minus);

	const SHPTR<DirectX::BoundingOrientedBox>& GetOBB() { return m_spOBB; }
private:
	SHPTR<DirectX::BoundingBox>					m_spAABB_Original;
	SHPTR<DirectX::BoundingOrientedBox>			m_spOBB_Original;
	SHPTR<DirectX::BoundingSphere>				m_spSphere_Original;

	SHPTR<DirectX::BoundingBox>					m_spAABB;
	SHPTR<DirectX::BoundingOrientedBox>			m_spOBB;
	SHPTR<DirectX::BoundingSphere>				m_spSphere;

	TYPE										m_eType;
	_bool										m_isCollision;
	_float4x4									m_mTransformMatrix;

	_float3										m_vModelScale;
	_float3										m_vCurScale;
	_float3										m_vTranslate;
	_float3										m_vPos;

	ARRAY<_float3, 6>							m_f3ObbNormals;

#ifdef _USE_DEBUGGING
	SHPTR<UDefaultDebugging>					m_spDebugDrawPawn;
#endif
};

END
