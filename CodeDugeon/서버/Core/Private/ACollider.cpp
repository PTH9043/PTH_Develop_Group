#include "CoreDefines.h"
#include "ACollider.h"
#include "ATransform.h"

namespace Core {


	ACollider::ACollider(const TYPE& _eType, const COLLIDERDESC& _CollisionDesc):
		m_eType(_eType),
		m_spAABB_Original{ nullptr },
		m_spOBB_Original{ nullptr },
		m_spSphere_Original{ nullptr },
		m_spAABB{ nullptr },
		m_spOBB{ nullptr },
		m_spSphere{ nullptr },
		m_isCollision{ false },
		m_mTransformMatrix{ _float4x4::Identity },
		m_vCurScale{ 0.f, 0.f, 0.f },
		m_vModelScale{ 0.f, 0.f, 0.f },
		m_vTranslate{ 0.f, 0.f, 0.f },
		m_vPos{ 0.f, 0.f, 0.f },
		m_vObbNormals{}
	{
		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original = Create<DirectX::BoundingBox>(Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f));
			break;
		case TYPE_OBB:
			m_spOBB_Original = Create<DirectX::BoundingOrientedBox>(Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f), Vector4(0.f, 0.f, 0.f, 1.f));
			break;
		case TYPE_SPHERE:
			m_spSphere_Original = Create<DirectX::BoundingSphere>(Vector3::Zero, 0.5f);
			break;
		}

		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original->Center = _CollisionDesc.vTranslation;
			m_spAABB_Original->Extents = Vector3(_CollisionDesc.vScale.x, _CollisionDesc.vScale.y, _CollisionDesc.vScale.z);
			m_vCurScale = _CollisionDesc.vScale;
			m_spAABB = Create<DirectX::BoundingBox>(*m_spAABB_Original.get());
			break;
		case TYPE_OBB:
			m_spOBB_Original->Center = _CollisionDesc.vTranslation;
			m_spOBB_Original->Extents = Vector3(_CollisionDesc.vScale.x, _CollisionDesc.vScale.y, _CollisionDesc.vScale.z);
			m_vCurScale = _CollisionDesc.vScale;
			m_spOBB = Create<DirectX::BoundingOrientedBox>(*m_spOBB_Original.get());
			break;
		case TYPE_SPHERE:
			m_spSphere_Original->Center = _CollisionDesc.vTranslation;
			m_spSphere_Original->Radius = _CollisionDesc.vScale.x;
			m_vCurScale = { _CollisionDesc.vScale.x, _CollisionDesc.vScale.x, _CollisionDesc.vScale.x };
			m_spSphere = Create<DirectX::BoundingSphere>(*m_spSphere_Original.get());
			break;
		}
		m_vModelScale = m_vCurScale;
	}

	const Vector3& ACollider::GetCurPos()
	{
		m_vPos = Vector3::Zero;
		switch (m_eType)
		{
		case TYPE_AABB:
			if (nullptr != m_spAABB)
				m_vPos = m_spAABB->Center;
			break;
		case TYPE_OBB:
			if (nullptr != m_spOBB)
				m_vPos = m_spOBB->Center;
			break;
		case TYPE_SPHERE:
			if (nullptr != m_spSphere)
				m_vPos = m_spSphere->Center;
			break;
		}
		return m_vPos;
	}

	const Vector3& ACollider::GetScale()
	{
		return m_vCurScale;
	}

	const Vector3& ACollider::GetTranslate()
	{
		return m_vTranslate;
	}

	void ACollider::SetScale(const Vector3& _vScale)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original->Extents = _vScale;
			m_vCurScale = _vScale;
			break;
		case TYPE_OBB:
			m_spOBB_Original->Extents = _vScale;
			m_vCurScale = _vScale;
			break;
		case TYPE_SPHERE:
			m_spSphere_Original->Radius = _vScale.x;
			m_vCurScale = { _vScale.x, _vScale.x, _vScale.x };
			break;
		}
		m_vModelScale = m_vCurScale;
	}

	void ACollider::SetScaleToFitModel(const Vector3& minVertex, const Vector3& maxVertex)
	{
		Vector3 size = (maxVertex - minVertex) * 0.5f;
		Vector3 center = (maxVertex + minVertex) * 0.5f;

		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original->Center = center;
			m_spAABB_Original->Extents = size;
			m_vModelScale = size * 2;
			break;
		case TYPE_OBB:
			m_spOBB_Original->Center = center;
			m_spOBB_Original->Extents = size;
			m_vModelScale = size * 2;
			break;
		case TYPE_SPHERE:
			m_spSphere_Original->Radius = (size).x;
			m_vModelScale = { (size * 2).x, (size * 2).x, (size * 2).x };
			break;
		}
	}

	void ACollider::SetTranslate(const Vector3& _vTranslate)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original->Center = _vTranslate;
			break;
		case TYPE_OBB:
			m_spOBB_Original->Center = _vTranslate;
			break;
		case TYPE_SPHERE:
			m_spSphere_Original->Center = _vTranslate;
			break;
		}
		m_vTranslate = _vTranslate;
	}

	void ACollider::SetTransform(const Vector3& _vPos, const Vector4& _vQuaternion)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
			m_mTransformMatrix = _float4x4::Identity;
			m_mTransformMatrix.Set_Pos(_vPos);
			m_spAABB_Original->Transform(*m_spAABB.get(), m_mTransformMatrix);
			break;
		case TYPE_OBB:
			m_mTransformMatrix = XMMatrixRotationQuaternion(_vQuaternion);
			m_mTransformMatrix.Set_Pos(_vPos);
			m_spOBB_Original->Transform(*m_spOBB.get(), m_mTransformMatrix);
			SetOBBNormals(m_mTransformMatrix);
			break;
		case TYPE_SPHERE:
			m_mTransformMatrix = XMMatrixRotationQuaternion(_vQuaternion);
			m_mTransformMatrix.Set_Pos(_vPos);
			m_spSphere_Original->Transform(*m_spSphere.get(), m_mTransformMatrix);
			break;
		}
	}

	void ACollider::SetTransform(CSHPTRREF<ATransform> _spTransform)
	{
		RETURN_CHECK(nullptr == _spTransform, ;);
		switch (m_eType)
		{
		case TYPE_AABB:
			m_mTransformMatrix = _float4x4::Identity;
			m_mTransformMatrix.Set_Pos(_spTransform->GetPos());
			m_spAABB_Original->Transform(*m_spAABB.get(), m_mTransformMatrix);
			break;
		case TYPE_OBB:
			m_mTransformMatrix = _spTransform->GetWorldMatrix();
			m_spOBB_Original->Transform(*m_spOBB.get(), m_mTransformMatrix);
			m_vModelScale = Vector3(m_spOBB->Extents) * 2;
			SetOBBNormals(m_mTransformMatrix);
			break;
		case TYPE_SPHERE:
			m_mTransformMatrix = _spTransform->GetWorldMatrix();
			m_spSphere_Original->Transform(*m_spSphere.get(), m_mTransformMatrix);
			m_vModelScale = Vector3((m_spSphere->Radius, m_spSphere->Radius, m_spSphere->Radius));
			break;
		}
	}

	void ACollider::SetTransform(const _float4x4& _Matrix)
	{
		_float4x4 Matrix = m_mTransformMatrix * _Matrix;
		switch (m_eType)
		{
		case TYPE_AABB:
			m_spAABB_Original->Transform(*m_spAABB.get(), Matrix);
			m_vModelScale = Vector3(m_spAABB->Extents) * 2;
			break;
		case TYPE_OBB:
			m_spOBB_Original->Transform(*m_spOBB.get(), Matrix);
			m_vModelScale = Vector3(m_spOBB->Extents) * 2;
			SetOBBNormals(Matrix);
			break;
		case TYPE_SPHERE:
			m_spSphere_Original->Transform(*m_spSphere.get(), Matrix);
			m_vModelScale = Vector3(m_spSphere->Radius);
			break;
		}
	}

	void ACollider::Free()
	{
	}

	_bool ACollider::IsCollision(CSHPTRREF<ACollider> _pCollider)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
		{
			TYPE		eTargetType = _pCollider->m_eType;

			if (TYPE_AABB == eTargetType)
				m_isCollision = m_spAABB->Intersects(*_pCollider->m_spAABB.get());
			if (TYPE_OBB == eTargetType)
				m_isCollision = m_spAABB->Intersects(*_pCollider->m_spOBB.get());
			if (TYPE_SPHERE == eTargetType)
				m_isCollision = m_spAABB->Intersects(*_pCollider->m_spSphere.get());
		}
		break;
		case TYPE_OBB:
		{
			TYPE		eTargetType = _pCollider->m_eType;

			if (TYPE_AABB == eTargetType)
				m_isCollision = m_spOBB->Intersects(*_pCollider->m_spAABB.get());
			if (TYPE_OBB == eTargetType)
				m_isCollision = m_spOBB->Intersects(*_pCollider->m_spOBB.get());
			if (TYPE_SPHERE == eTargetType)
				m_isCollision = m_spOBB->Intersects(*_pCollider->m_spSphere.get());
		}
		break;
		case TYPE_SPHERE:
		{
			TYPE		eTargetType = _pCollider->m_eType;

			if (TYPE_AABB == eTargetType)
				m_isCollision = m_spSphere->Intersects(*_pCollider->m_spAABB.get());
			if (TYPE_OBB == eTargetType)
				m_isCollision = m_spSphere->Intersects(*_pCollider->m_spOBB.get());
			if (TYPE_SPHERE == eTargetType)
				m_isCollision = m_spSphere->Intersects(*_pCollider->m_spSphere.get());
		}
		break;

		}
		_pCollider->m_isCollision = m_isCollision;

		return m_isCollision;
	}

	_bool ACollider::IsCollisionWithRay(const Vector3& _vOrigin, const Vector3& _vDirection, _float* _pDist)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
		{
			m_isCollision = m_spAABB->Intersects(_vOrigin, _vDirection, *_pDist);
		}
		break;
		case TYPE_OBB:
		{
			m_isCollision = m_spOBB->Intersects(_vOrigin, _vDirection, *_pDist);
		}
		break;
		case TYPE_SPHERE:
		{
			m_isCollision = m_spSphere->Intersects(_vOrigin, _vDirection, *_pDist);
		}
		break;
		}

		return m_isCollision;
	}

	void ACollider::SetOBBNormals(const _float4x4& transformMatrix)
	{
		Vector3 right = XMVector3TransformNormal(Vector3(1.0f, 0.0f, 0.0f), transformMatrix);
		Vector3 up = XMVector3TransformNormal(Vector3(0.0f, 1.0f, 0.0f), transformMatrix);
		Vector3 forward = XMVector3TransformNormal(Vector3(0.0f, 0.0f, 1.0f), transformMatrix);

		m_vObbNormals[0] = right;
		m_vObbNormals[1] = -right;
		m_vObbNormals[2] = up;
		m_vObbNormals[3] = -up;
		m_vObbNormals[4] = forward;
		m_vObbNormals[5] = -forward;
	}

	Vector3 ACollider::CalculateOBBCollisionNormal(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingOrientedBox& box2) const
	{
		XMVECTOR Axes[15];
		XMVECTOR Center1 = XMLoadFloat3(&box1.Center);
		XMVECTOR Center2 = XMLoadFloat3(&box2.Center);
		XMVECTOR Extents1 = XMLoadFloat3(&box1.Extents);
		XMVECTOR Extents2 = XMLoadFloat3(&box2.Extents);

		XMFLOAT4 quaternion1 = box1.Orientation;
		XMFLOAT4 quaternion2 = box2.Orientation;

		XMMATRIX Rot1 = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion1));
		XMMATRIX Rot2 = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion2));

		// OBB 1과 OBB 2의 회전 행렬에서 축을 추출합니다.
		for (int i = 0; i < 3; ++i)
		{
			Axes[i] = Rot1.r[i];
			Axes[i + 3] = Rot2.r[i];
		}

		// 모든 축의 외적을 계산하여 충돌 가능한 축을 찾습니다.
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				Axes[6 + i * 3 + j] = XMVector3Cross(Axes[i], Axes[3 + j]);
			}
		}

		float minPenetration = FLT_MAX;  // 최소 침투값을 초기화합니다.
		Vector3 bestAxis = Vector3::Zero;  // 최적 축을 초기화합니다.

		// 모든 축에 대해 충돌 테스트를 수행합니다.
		for (int i = 0; i < 15; ++i)
		{
			XMVECTOR axis = Axes[i];  // 현재 축을 가져옵니다.

			// 축의 길이가 0이 아니면 정규화합니다.
			if (!XMVector3Equal(axis, XMVectorZero()))
			{
				axis = XMVector3Normalize(axis);  // 축을 정규화합니다.

				// 각 OBB의 축영(projection)을 계산합니다.
				float projection1 = fabs(XMVectorGetX(XMVector3Dot(axis, Rot1.r[0])) * XMVectorGetX(Extents1)) +
					fabs(XMVectorGetX(XMVector3Dot(axis, Rot1.r[1])) * XMVectorGetY(Extents1)) +
					fabs(XMVectorGetX(XMVector3Dot(axis, Rot1.r[2])) * XMVectorGetZ(Extents1));

				float projection2 = fabs(XMVectorGetX(XMVector3Dot(axis, Rot2.r[0])) * XMVectorGetX(Extents2)) +
					fabs(XMVectorGetX(XMVector3Dot(axis, Rot2.r[1])) * XMVectorGetY(Extents2)) +
					fabs(XMVectorGetX(XMVector3Dot(axis, Rot2.r[2])) * XMVectorGetZ(Extents2));

				// 두 OBB의 중심점 사이의 거리를 계산합니다.
				float dot1 = XMVectorGetX(XMVector3Dot(Center1, axis));
				float dot2 = XMVectorGetX(XMVector3Dot(Center2, axis));
				float distance = fabs(dot1 - dot2);

				// 축영과 중심점 사이의 거리를 비교하여 겹침 여부를 확인합니다.
				float penetration = projection1 + projection2 - distance;

				if (penetration < 0.0f)
				{
					return Vector3::Zero; // 충돌이 없는 경우
				}

				// 최소 오버랩 축을 찾습니다.
				if (penetration < minPenetration)
				{
					minPenetration = penetration;
					bestAxis = Vector3(XMVectorGetX(axis), XMVectorGetY(axis), XMVectorGetZ(axis));
				}
			}
		}

		return bestAxis;  // 최적 축을 반환합니다.
	}

	Vector3 ACollider::CalculateOBBSphereCollisionNormal(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingSphere& sphere) const
	{
		Vector3 sphereCenter = sphere.Center;
		float sphereRadius = sphere.Radius;

		Vector3 boxCenter = box.Center;
		XMVECTOR boxExtents = XMLoadFloat3(&box.Extents);
		XMFLOAT4 boxOrientation = box.Orientation;
		XMVECTOR quaternion = XMLoadFloat4(&boxOrientation);

		// OBB의 축을 구합니다.
		XMVECTOR boxAxisX = XMVector3Rotate(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), quaternion);
		XMVECTOR boxAxisY = XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), quaternion);
		XMVECTOR boxAxisZ = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), quaternion);

		// 구체의 중심을 OBB의 로컬 좌표계로 변환
		XMVECTOR localSphereCenter = XMVector3InverseRotate(sphereCenter - boxCenter, quaternion);

		// OBB의 각 축에 대해 구체의 중심과의 거리 계산
		Vector3 closestPoint = localSphereCenter;
		closestPoint = XMVectorClamp(closestPoint, -boxExtents, boxExtents);

		Vector3 vRotate = XMVector3Rotate(closestPoint, quaternion);
		// 로컬 좌표계에서 가장 가까운 점을 다시 월드 좌표계로 변환
		closestPoint = vRotate + boxCenter;

		// 구체의 중심과 가장 가까운 점 사이의 거리 계산
		XMVECTOR delta = closestPoint - sphereCenter;
		float distance = XMVectorGetX(XMVector3Length(delta));

		// 충돌 여부 판단
		if (distance > sphereRadius)
		{
			return Vector3::Zero; // 충돌이 없는 경우
		}

		// 충돌이 발생한 경우 충돌 노멀 계산
		XMVECTOR collisionNormal = XMVector3Normalize(delta);

		return Vector3(XMVectorGetX(collisionNormal), XMVectorGetY(collisionNormal), XMVectorGetZ(collisionNormal));
	}

	Vector3 ACollider::GetCollisionNormal(CSHPTRREF<ACollider> _pCollider)
	{
		Vector3 collisionNormal = Vector3::Zero;
		switch (m_eType)
		{
		case TYPE_AABB:
			break;
		case TYPE_OBB:
			collisionNormal = CalculateOBBCollisionNormal(*_pCollider->m_spOBB, *m_spOBB);
			break;
		case TYPE_SPHERE:
			collisionNormal = CalculateOBBSphereCollisionNormal(*_pCollider->m_spOBB, *m_spSphere);
			break;
		}

		return collisionNormal;
	}

	void ACollider::GetBoundingOrientedBoxCorners(const CSHPTRREF<DirectX::BoundingOrientedBox> box, Vector3* Corners)
	{
		static const XMVECTOR cornerOffsets[BoundingOrientedBox::CORNER_COUNT] =
		{
			XMVectorSet(-1.f, -1.f, -1.f, 0.f),
			XMVectorSet(1.f, -1.f, -1.f, 0.f),
			XMVectorSet(1.f, 1.f, -1.f, 0.f),
			XMVectorSet(-1.f, 1.f, -1.f, 0.f),
			XMVectorSet(-1.f, -1.f, 1.f, 0.f),
			XMVectorSet(1.f, -1.f, 1.f, 0.f),
			XMVectorSet(1.f, 1.f, 1.f, 0.f),
			XMVectorSet(-1.f, 1.f, 1.f, 0.f)
		};

		XMVECTOR center = XMLoadFloat3(&box->Center);
		XMVECTOR extents = XMLoadFloat3(&box->Extents);
		XMVECTOR orientation = XMLoadFloat4(&box->Orientation);
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(orientation);

		for (size_t i = 0; i < BoundingOrientedBox::CORNER_COUNT; ++i)
		{
			XMVECTOR localCorner = XMVectorMultiply(cornerOffsets[i], extents);
			XMVECTOR rotatedCorner = XMVector3Transform(localCorner, rotationMatrix);
			XMVECTOR worldCorner = XMVectorAdd(rotatedCorner, center);
			XMStoreFloat3(&Corners[i], worldCorner);
		}
	}

	Vector3 ACollider::GetFurthestPointFromBoundingOrientedBoxCenter(const CSHPTRREF<DirectX::BoundingOrientedBox> box)
	{
		Vector3 corners[BoundingOrientedBox::CORNER_COUNT];
		GetBoundingOrientedBoxCorners(box, corners);

		Vector3 furthestPoint = corners[0];
		float maxDistanceSquared = 0.0f;
		Vector3 center = XMLoadFloat3(&box->Center);

		for (size_t i = 0; i < BoundingOrientedBox::CORNER_COUNT; ++i)
		{
			XMVECTOR corner = XMLoadFloat3(&corners[i]);
			XMVECTOR distanceVector = XMVectorSubtract(corner, center);
			float distanceSquared = XMVectorGetX(XMVector3LengthSq(distanceVector));

			if (distanceSquared > maxDistanceSquared)
			{
				maxDistanceSquared = distanceSquared;
				furthestPoint = corners[i];
			}
		}
		return furthestPoint;
	}

	Vector3 ACollider::GetHeightAdjustedPointFromCenter(const CSHPTRREF<DirectX::BoundingOrientedBox> box, _bool minus)
	{
		XMVECTOR center = XMLoadFloat3(&box->Center);
		XMVECTOR extents = XMLoadFloat3(&box->Extents);
		XMVECTOR orientation = XMLoadFloat4(&box->Orientation);

		// Adjust the point in the y direction by the extent in the y direction
		XMVECTOR yOffset;
		if (minus) {
			yOffset = XMVectorSet(0.0f, XMVectorGetY(-extents), 0.0f, 0.0f);
		}
		else {
			yOffset = XMVectorSet(0.0f, XMVectorGetY(extents), 0.0f, 0.0f);
		}

		// Apply the rotation to the yOffset
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(orientation);
		XMVECTOR rotatedYOffset = XMVector3Transform(yOffset, rotationMatrix);

		// Calculate the new point
		XMVECTOR adjustedPoint = XMVectorAdd(center, rotatedYOffset);

		Vector3 result;
		XMStoreFloat3(&result, adjustedPoint);

		return result;
	}

}