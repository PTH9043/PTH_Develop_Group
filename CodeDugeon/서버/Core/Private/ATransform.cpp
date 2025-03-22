#include "CoreDefines.h"
#include "ATransform.h"

namespace Core {

	ATransform::ATransform() :
		m_mWorldMatrix{ _float4x4::Identity },
		m_mChangeWorldMatrix{ _float4x4::Identity },
		m_vQuaternion{},
		m_vScale{},
		m_isNotApplyRotate{ false },
		m_isNotApplyPos{ false },
		m_isNotApplyScale{ false },
		m_spParentsTransform{ nullptr },
		m_vVelocity{  },
		m_vGravity{  },
		m_vJumpvelocity{  }
	{
	}

	const Vector3 ATransform::GetAngles() const
	{
		Vector3 vEuler = m_vQuaternion.ToEuler();
		vEuler.x = DirectX::XMConvertToDegrees(vEuler.x);
		vEuler.y = DirectX::XMConvertToDegrees(vEuler.y);
		vEuler.z = DirectX::XMConvertToDegrees(vEuler.z);
		return vEuler;
	}

	const _float ATransform::GetXAngle() const
	{
		return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().x);
	}

	const _float ATransform::GetYAngle() const
	{
		return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().y);
	}

	const _float ATransform::GetZAngle() const
	{
		return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().z);
	}

	const _float4x4 ATransform::GetParentsMatrix()
	{
		if (nullptr == m_spParentsTransform)
			return m_mChangeWorldMatrix;

		return _float4x4::Identity;
	}

	void ATransform::SetScale(const Vector3& _vScale)
	{
		if (_vScale == m_vScale)
			return;

		std::atomic_thread_fence(std::memory_order_seq_cst);
		m_mWorldMatrix = m_mWorldMatrix.MatrixSetScaling(_vScale);
		m_vScale = _vScale;
	}

	void ATransform::SetParent(CSHPTRREF<ATransform> _spTransform)
	{
		RETURN_CHECK(nullptr == _spTransform, ;);
		RETURN_CHECK(this == _spTransform.get(), ;);
		m_spParentsTransform = _spTransform;
	}

	void ATransform::SetNewWorldMtx(const _float4x4& _newworldMtx)
	{
		m_mWorldMatrix = _newworldMtx;
		TransformUpdate();
	}

	void ATransform::Free()
	{
	}

	void ATransform::TransformUpdate()
	{
		READ_SPINLOCK(m_WorldSpinLock);
		WRITE_SPINLOCK(m_ChangeSpinLock);
		if (nullptr != m_spParentsTransform) {

			_float4x4 Matrix = m_spParentsTransform->GetParentsMatrix();
			if (true == m_isNotApplyScale)
			{
				Matrix.MatrixSetScaling(Vector3{1.f, 1.f, 1.f});
			}
			if (true == m_isNotApplyPos)
			{
				Matrix.Set_Up(Vector3::Zero);
			}
			if (true == m_isNotApplyRotate)
			{
				Matrix.MatrixSetRotationFix(Vector3::Zero);
			}
			m_mChangeWorldMatrix = m_mWorldMatrix * Matrix;
		}
		else {
			m_mChangeWorldMatrix = m_mWorldMatrix;
		}
	}

	void ATransform::MoveForward(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetLook();

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}

	void ATransform::MoveBack(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetLook();

		vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}

	void ATransform::MoveLeft(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetRight();

		vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}

	void ATransform::MoveRight(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetRight();

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}

	void ATransform::MoveForwardNotY(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetLook();

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0.f;
		SetPos(vPosition);
	}

	void ATransform::MoveBackNotY(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetLook();

		vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0.f;
		SetPos(vPosition);
	}

	void ATransform::MoveLeftNotY(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetRight();

		vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0.f;
		SetPos(vPosition);
	}

	void ATransform::MoveRightNotY(const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();
		Vector3 vLook = GetLook();

		vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0.f;
		SetPos(vPosition);
	}

	void ATransform::MovePos(const Vector3& _vPos)
	{
		SetPos(GetPos() + _vPos);
	}

	void ATransform::TranslatePos(const Vector3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
	{
		if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
			Vector3 vPosition = GetPos();
			Vector3 vLook = _vPos - vPosition;

			vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
			SetPos(vPosition);
		}
	}

	void ATransform::TranslateDir(const Vector3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();

		vPosition += _vDir * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}

	void ATransform::TranslateTrans(CSHPTRREF<ATransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
	{
		RETURN_CHECK(nullptr == _pTransform, ; );
		const Vector3& _vPos = _pTransform->GetPos();
		if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
			Vector3 vPosition = GetPos();
			Vector3 vLook = _vPos - vPosition;

			vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
			SetPos(vPosition);
		}
	}

	void ATransform::TranslatePosNotY(const Vector3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
	{
		if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
			Vector3 vPosition = GetPos();
			Vector3 vLook = _vPos - vPosition;

			vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
			vLook.y = 0;
			SetPos(vPosition);
		}
	}

	void ATransform::TranslateDirNotY(const Vector3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed)
	{
		Vector3 vPosition = GetPos();

		vPosition += _vDir * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0;
		SetPos(vPosition);
	}

	void ATransform::TranslateTransNotY(CSHPTRREF<ATransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
	{
		RETURN_CHECK(nullptr == _pTransform, ; );
		const Vector3& _vPos = _pTransform->GetPos();
		if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
			Vector3 vPosition = GetPos();
			Vector3 vLook = _vPos - vPosition;

			vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
			vPosition.y = 0.f;
			SetPos(vPosition);
		}
	}

	void ATransform::RotateFix(const Vector3& _vStandardAngle, const _float _fTurnAnge)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = _quaternion::CreateFromAxisAngle(_vStandardAngle, DirectX::XMConvertToRadians(_fTurnAnge));

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(Vector3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
	}

	void ATransform::RotateFix(const Vector3& _vAngle)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 vAngle;
		vAngle.x = DirectX::XMConvertToRadians(_vAngle.x);
		vAngle.y = DirectX::XMConvertToRadians(_vAngle.y);
		vAngle.z = DirectX::XMConvertToRadians(_vAngle.z);
		m_vQuaternion = _quaternion::CreateFromYawPitchRoll(vAngle);

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(Vector3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
	}

	void ATransform::RotateFixNotApplyRadians(const Vector3& _vStandardAngle, const _float _fTurnAnge)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = _quaternion::CreateFromAxisAngle(_vStandardAngle, _fTurnAnge);

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(Vector3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
	}

	void ATransform::RotateFixNotApplyRadians(const Vector3& _vAngle)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = _quaternion::CreateFromYawPitchRoll(_vAngle);

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(Vector3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
	}

	void ATransform::RotateFix(const Vector4& _vQuaternion)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = _vQuaternion;

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(Vector3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(Vector3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
	}

	void ATransform::RotateTurn(const Vector3& _vAxis, const _float& _fAngleSpeed, const _double& _dTimeDelta)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = DirectX::XMQuaternionRotationAxis(_vAxis, (_float)(DirectX::XMConvertToRadians(_fAngleSpeed) * _dTimeDelta));
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
	}

	void ATransform::RotateTurn(const Vector3& _vAxis, const _float& _fAngle)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = DirectX::XMQuaternionRotationAxis(_vAxis, (_float)(DirectX::XMConvertToRadians(_fAngle)));
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
	}

	void ATransform::RotateTurn(const Vector4& _vQuaternion)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vQuaternion = _vQuaternion;

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
	}

	void ATransform::RotateTurn(const Vector3& _vAngle)
	{
		Vector3 vAngle;
		vAngle.x = DirectX::XMConvertToRadians(_vAngle.x);
		vAngle.y = DirectX::XMConvertToRadians(_vAngle.y);
		vAngle.z = DirectX::XMConvertToRadians(_vAngle.z);
		m_vQuaternion = _quaternion::CreateFromYawPitchRoll(vAngle);

		_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
	}

	void ATransform::SetDirection(const Vector3& direction)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 vPosition = GetPos();
		Vector4 vLook = XMVector3Normalize(direction);

		Vector4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector3::Up), vLook));
		Vector4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));

		// Calculate the rotation matrix
		_float4x4 rotationMatrix(
			vRight,
			vUp,
			vLook,
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Right) * m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Up) * m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Forward) * m_vScale.z, RotationMatrix));
	}

	void ATransform::SetDirection(const Vector3& targetDirection, float deltaTime, float rotationSpeed)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 vCurrentLook = GetLook();
		Vector4 vTargetLook = XMVector3Normalize(targetDirection);

		// Compute the lerp factor based on delta time and rotation speed
		float lerpFactor = rotationSpeed * deltaTime;
		lerpFactor = std::clamp(lerpFactor, 0.0f, 1.0f);

		Vector3 n = DirectX::XMVectorLerp(vCurrentLook, vTargetLook, lerpFactor);
		Vector4 vLook = XMVector3Normalize(n);

		Vector4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector3::Up), vLook));
		Vector4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));


		_float4x4 rotationMatrix(
			vRight,
			vUp,
			vLook,
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Right) * m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Up) * m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Forward) * m_vScale.z, RotationMatrix));
	}

	void ATransform::SetDirectionFixedUp(const Vector3& direction)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 FixedUp = Vector3(0, 1, 0);
		Vector3 vPosition = GetPos();
		Vector4 vLook = XMVector3Normalize(direction);
		Vector4 vUp = DirectX::XMVector3Normalize(FixedUp);
		Vector4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vUp, vLook));

		_float4x4 rotationMatrix(
			vRight,
			vUp,
			vLook,
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Right) * m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Up) * m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Forward) * m_vScale.z, RotationMatrix));
	}

	void ATransform::SetDirectionFixedUp(const Vector3& targetDirection, float deltaTime, float rotationSpeed)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 FixedUp = Vector3(0, 1, 0);
		Vector3 vCurrentLook = GetLook();
		Vector4 vTargetLook = XMVector3Normalize(targetDirection);

		float lerpFactor = rotationSpeed * deltaTime;
		lerpFactor = std::clamp(lerpFactor, 0.0f, 1.0f);

		Vector3 n = DirectX::XMVectorLerp(vCurrentLook, vTargetLook, lerpFactor);
		Vector4 vLook = XMVector3Normalize(n);

		Vector4 vUp = DirectX::XMVector3Normalize(FixedUp);

		Vector4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vUp, vLook));

		_float4x4 rotationMatrix(
			vRight,
			vUp,
			vLook,
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Right) * m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Up) * m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Forward) * m_vScale.z, RotationMatrix));
	}

	void ATransform::LookAt(const Vector3& _vTargetPos)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 vPosition = GetPos();
		Vector4 vLook = XMVector3Normalize(_vTargetPos - vPosition);

		Vector4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector3::Up), vLook));
		Vector4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));

		// 회전 행렬 계산
		_float4x4 rotationMatrix(
			vRight,
			vUp,
			vLook,
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
		_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

		SetRight(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Right) * m_vScale.x, RotationMatrix));
		SetUp(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Up) * m_vScale.y, RotationMatrix));
		SetLook(DirectX::XMVector3TransformNormal(XMLoadFloat3(&Vector3::Forward) * m_vScale.z, RotationMatrix));
	}

	void ATransform::LookAtWithFixedUp(const Vector3& _vTargetPos)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 FixedUp = Vector3(0, 1, 0);
		Vector3 MyPosition = GetPos();
		Vector3 vLook = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(_vTargetPos, MyPosition));
		Vector3 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vLook, FixedUp));


		SetRight(vRight * m_vScale.x);
		SetUp(FixedUp * m_vScale.y);
		SetLook(vLook * m_vScale.z);
	}

	void ATransform::LookAtWithFixedUp(const Vector3& _vTargetPos, float DeltaTime, float RotationSpeed)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		Vector3 FixedUp = Vector3(0, 1, 0);
		Vector3 MyPosition = GetPos();
		Vector3 vLook = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(_vTargetPos, MyPosition));
		Vector3 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vLook, FixedUp));

		// 현재 객체의 시선 방향과 목표 방향 사이의 각도를 구합니다.

		float AngleBetween = acosf(GetLook().Dot(vLook));

		// 회전 속도에 시간을 곱하여 회전할 각도를 계산합니다.
		float RotationAngle = RotationSpeed * DeltaTime;

		// 회전할 각도가 실제 각도보다 작거나 같으면 바로 목표 방향을 향하도록 설정합니다.
		if (RotationAngle >= AngleBetween)
		{
			SetRight(vRight * m_vScale.x);
			SetUp(FixedUp * m_vScale.y);
			SetLook(vLook * m_vScale.z);
		}
		else
		{
			// 선형 보간(Lerp)을 사용하여 현재 각도에서 목표 각도로 부드럽게 회전합니다.
			Vector3 NewLook = DirectX::XMVector3Normalize(DirectX::XMVectorLerp(GetLook(), vLook, DeltaTime));
			Vector3 NewRight = DirectX::XMVector3Normalize(XMVector3Cross(NewLook, FixedUp));

			SetRight(NewRight * m_vScale.x);
			SetUp(FixedUp * m_vScale.y);
			SetLook(NewLook * m_vScale.z);
		}
	}

	const _float ATransform::ComputeDistance(const Vector3& _vPos)
	{
		return Vector3::Length(GetPos() - _vPos);
	}

	const _float ATransform::ComputeDistanceSq(const Vector3& _vPos)
	{
		return Vector3::LengthSquared(GetPos() - _vPos);
	}

	void ATransform::GravityFall(const _double& _deltaTime)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_vVelocity += m_vGravity * static_cast<_float>(_deltaTime);
		Vector3 vPosition = GetPos();
		vPosition += m_vVelocity * static_cast<_float>(_deltaTime);
		SetPos(vPosition);
	}

	void ATransform::DisableGravity()
	{
		m_vVelocity = Vector3(0.0f, 0.0f, 0.0f);
	}

	void ATransform::DisableJump()
	{
		m_vJumpvelocity = Vector3(0.0f, 12.f, 0.0f);
	}

	void ATransform::JumpMovement(const _double& _deltaTime)
	{
		m_vJumpvelocity += m_vGravity * static_cast<_float>(_deltaTime);
		Vector3 vPosition = GetPos();
		vPosition += m_vJumpvelocity * static_cast<_float>(_deltaTime);
		SetPos(vPosition);
	}

	Vector3 ATransform::GetRotationValue()
	{
		thread_local static DirectX::PTH::OUTMATRIX OutMatrix;
		TransformUpdate();
		OutMatrix = m_mChangeWorldMatrix.Get_OutMatrix();
		return OutMatrix.vRot;
	}
}