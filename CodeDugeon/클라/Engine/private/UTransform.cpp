#include "EngineDefine.h"
#include "UTransform.h"
#include "UMethod.h"
#include "UShaderConstantBuffer.h"
#include "UGameInstance.h"
#include "UShader.h"

_bool  UTransform::m_isMotionBlurOn = true;

UTransform::UTransform(CSHPTRREF<UDevice> _spDevice) :
	UComponent(_spDevice),
	// Shader 
	m_TransformBuffer{ nullptr },
	m_stTransformParam{},
	m_mWorldMatrix{ _float4x4::Identity },
	m_mChangeWorldMatrix{ _float4x4::Identity },
	m_vQuaternion{ _float4::Zero },
	m_vScale{ 1.f, 1.f, 1.f },
	m_isNotApplyRotate{ false },
	m_isNotApplyPos{ false },
	m_isNotApplyScale{ false },
	m_spParentsTransform{ nullptr },
	m_vVelocity{ 0.0f, 0.0f, 0.0f },
	m_vJumpvelocity{ 0.0f, 12.f, 0.0f},
	m_vGravity{ 0.0f, -9.81f, 0.0f }
{
}

UTransform::UTransform(const UTransform& _rhs) :
	UComponent(_rhs),
	// Shader 
	m_TransformBuffer{ nullptr },
	m_stTransformParam{},
	m_mWorldMatrix{ _float4x4::Identity },
	m_mChangeWorldMatrix{ _float4x4::Identity },
	m_vQuaternion{ _float4::Zero },
	m_vScale{ 1.f, 1.f, 1.f },
	m_isNotApplyRotate{ false },
	m_isNotApplyPos{ false },
	m_isNotApplyScale{ false },
	m_spParentsTransform{ nullptr },
	m_vVelocity{ _rhs.m_vVelocity }, 
	m_vJumpvelocity{ 0.0f, 12.f, 0.0f },
	m_vGravity{ 0.0f, -9.81f, 0.0f }
{
}

const _float3 UTransform::GetAngles() const
{
	_float3 vEuler = m_vQuaternion.ToEuler();
	vEuler.x = DirectX::XMConvertToDegrees(vEuler.x);
	vEuler.y = DirectX::XMConvertToDegrees(vEuler.y);
	vEuler.z = DirectX::XMConvertToDegrees(vEuler.z);
	return vEuler;
}

const _float UTransform::GetXAngle() const
{
	return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().x);
}

const _float UTransform::GetYAngle() const
{
	return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().y);
}

const _float UTransform::GetZAngle() const
{
	return DirectX::XMConvertToDegrees(m_vQuaternion.ToEuler().z);
}

void UTransform::SetScale(const _float3& _vScale)
{
	if (_vScale == m_vScale)
		return;

	m_mWorldMatrix = m_mWorldMatrix.MatrixSetScaling(_vScale);
	m_vScale = _vScale;
}

const _float4x4 UTransform::GetParentsMatrix()
{
	if (nullptr == m_spParentsTransform)
		return m_mChangeWorldMatrix;

	return _float4x4();
}

void UTransform::SetParent(CSHPTRREF<UTransform> _spTransform)
{
	RETURN_CHECK(nullptr == _spTransform, ;);
	RETURN_CHECK(this == _spTransform.get(), ;);
	m_spParentsTransform = _spTransform;
}

void UTransform::SetNewWorldMtx(const _float4x4& _newworldMtx)
{
	m_mWorldMatrix = _newworldMtx;
	TransformUpdate();
}

void UTransform::Free()
{
	m_spParentsTransform.reset();
}

HRESULT UTransform::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	// 이미 만들어진 Shader ConstnatBuffer를 가져옴
	spGameInstance->GetPreAllocatedConstantBuffer(PREALLOCATED_TRANSFORM, m_TransformBuffer);
	return S_OK;
}

HRESULT UTransform::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}

void UTransform::TransformUpdate()
{
	if (nullptr != m_spParentsTransform) {

		_float4x4 Matrix = m_spParentsTransform->GetParentsMatrix();
		if (true == m_isNotApplyScale)
		{
			Matrix.MatrixSetScaling(BASIC_SCALE);
		}
		if (true == m_isNotApplyPos)
		{
			Matrix.Set_Up(_float3::Zero);
		}
		if (true == m_isNotApplyRotate)
		{
			Matrix.MatrixSetRotationFix(_float3::Zero);
		}
		m_mChangeWorldMatrix = m_mWorldMatrix * Matrix;
	}
	else {
		m_mChangeWorldMatrix = m_mWorldMatrix ;
	}
}

void UTransform::MoveForward(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetLook();

	vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	SetPos(vPosition);
}

void UTransform::MoveBack(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetLook();

	vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	SetPos(vPosition);
}

void UTransform::MoveLeft(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetRight();

	vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	SetPos(vPosition);
}

void UTransform::MoveRight(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetRight();

	vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	SetPos(vPosition);
}

void UTransform::MoveForwardNotY(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetLook();

	vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	vPosition.y = 0.f;
	SetPos(vPosition);
}

void UTransform::MoveBackNotY(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetLook();

	vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	vPosition.y = 0.f;
	SetPos(vPosition);
}

void UTransform::MoveLeftNotY(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetRight();

	vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	vPosition.y = 0.f;
	SetPos(vPosition);
}

void UTransform::MoveRightNotY(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();
	_float3 vLook = GetLook();

	vPosition -= vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
	vPosition.y = 0.f;
	SetPos(vPosition);
}

void UTransform::MovePos(const _float3& _vPos)
{
	SetPos(GetPos() + _vPos);
}

void UTransform::TranslatePos(const _float3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
		_float3 vPosition = GetPos();
		_float3 vLook = _vPos - vPosition;

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}
}

void UTransform::TranslateDir(const _float3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();

	vPosition += _vDir * static_cast<_float>(_dTimeDelta) * _fSpeed;
	SetPos(vPosition);
}

void UTransform::TranslateTrans(CSHPTRREF<UTransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	RETURN_CHECK(nullptr == _pTransform, ; );
	const _float3& _vPos = _pTransform->GetPos();
	if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
		_float3 vPosition = GetPos();
		_float3 vLook = _vPos - vPosition;

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		SetPos(vPosition);
	}
}

void UTransform::TranslatePosNotY(const _float3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
		_float3 vPosition = GetPos();
		_float3 vLook = _vPos - vPosition;

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vLook.y = 0;
		SetPos(vPosition);
	}
}

void UTransform::TranslateDirNotY(const _float3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_float3 vPosition = GetPos();

	vPosition += _vDir * static_cast<_float>(_dTimeDelta) * _fSpeed;
	vPosition.y = 0;
	SetPos(vPosition);
}

void UTransform::TranslateTransNotY(CSHPTRREF<UTransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	RETURN_CHECK(nullptr == _pTransform, ; );
	const _float3& _vPos = _pTransform->GetPos();
	if (ComputeDistance(_vPos) <= _fLimitDistance * _fLimitDistance) {
		_float3 vPosition = GetPos();
		_float3 vLook = _vPos - vPosition;

		vPosition += vLook * static_cast<_float>(_dTimeDelta) * _fSpeed;
		vPosition.y = 0.f;
		SetPos(vPosition);
	}
}

void UTransform::RotateFix(const _float3& _vStandardAngle, const _float _fTurnAnge)
{
	m_vQuaternion = _quaternion::CreateFromAxisAngle(_vStandardAngle, DirectX::XMConvertToRadians(_fTurnAnge));

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
}

void UTransform::RotateFix(const _float3& _vAngle)
{
	_float3 vAngle;
	vAngle.x = DirectX::XMConvertToRadians(_vAngle.x);
	vAngle.y = DirectX::XMConvertToRadians(_vAngle.y);
	vAngle.z = DirectX::XMConvertToRadians(_vAngle.z);
	m_vQuaternion = _quaternion::CreateFromYawPitchRoll(vAngle);

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
}

void UTransform::RotateFixNotApplyRadians(const _float3& _vStandardAngle, const _float _fTurnAnge)
{
	m_vQuaternion = _quaternion::CreateFromAxisAngle(_vStandardAngle, _fTurnAnge);

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
}

void UTransform::RotateFixNotApplyRadians(const _float3& _vAngle)
{
	m_vQuaternion = _quaternion::CreateFromYawPitchRoll(_vAngle);

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
}

void UTransform::RotateFix(const _float4& _vQuaternion)
{
	m_vQuaternion = _vQuaternion;

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3{ 1.f, 0.f, 0.f } *m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3{ 0.f, 1.f, 0.f } *m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3{ 0.f, 0.f, 1.f } *m_vScale.z, RotationMatrix));
}

void UTransform::RotateTurn(const _float3& _vAxis, const _float& _fAngleSpeed, const _double& _dTimeDelta)
{
	m_vQuaternion = DirectX::XMQuaternionRotationAxis(_vAxis, (_float)(DirectX::XMConvertToRadians(_fAngleSpeed) * _dTimeDelta));
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
}

void UTransform::RotateTurn(const _float3& _vAxis, const _float& _fAngle)
{
	m_vQuaternion = DirectX::XMQuaternionRotationAxis(_vAxis, (_float)(DirectX::XMConvertToRadians(_fAngle)));
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
}

void UTransform::RotateTurn(const _float4& _vQuaternion)
{
	m_vQuaternion = _vQuaternion;

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
}

void UTransform::RotateTurn(const _float3& _vAngle)
{
	_float3 vAngle;
	vAngle.x = DirectX::XMConvertToRadians(_vAngle.x);
	vAngle.y = DirectX::XMConvertToRadians(_vAngle.y);
	vAngle.z = DirectX::XMConvertToRadians(_vAngle.z);
	m_vQuaternion = _quaternion::CreateFromYawPitchRoll(vAngle);

	_float4x4 RotationMatrix = _float4x4::CreateFromQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(GetRight(), RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(GetUp(), RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(GetLook(), RotationMatrix));
}

void UTransform::SetDirection(const _float3& direction)
{
	_float3 vPosition = GetPos();
	_float4 vLook = XMVector3Normalize(direction);

	_float4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(_float3::Up, vLook));
	_float4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));

	// Calculate the rotation matrix
	_float4x4 rotationMatrix(
		vRight,
		vUp,
		vLook,
		_float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3::Right * m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3::Up * m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3::Forward * m_vScale.z, RotationMatrix));

}

void UTransform::SetDirection(const _float3& targetDirection, float deltaTime, float rotationSpeed)
{
	_float3 vCurrentLook = GetLook();
	_float4 vTargetLook = XMVector3Normalize(targetDirection);

	// Compute the lerp factor based on delta time and rotation speed
	float lerpFactor = rotationSpeed * deltaTime;
	lerpFactor = std::clamp(lerpFactor, 0.0f, 1.0f);

	_float3 n = DirectX::XMVectorLerp(vCurrentLook, vTargetLook, lerpFactor);
	_float4 vLook = XMVector3Normalize(n);

	_float4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(_float3::Up, vLook));
	_float4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));


	_float4x4 rotationMatrix(
		vRight,
		vUp,
		vLook,
		_float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3::Right * m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3::Up * m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3::Forward * m_vScale.z, RotationMatrix));
}

void UTransform::SetDirectionFixedUp(const _float3& direction)
{
	_float3 FixedUp = _float3(0, 1, 0);
	_float3 vPosition = GetPos();
	_float4 vLook = XMVector3Normalize(direction);
	_float4 vUp = DirectX::XMVector3Normalize(FixedUp);
	_float4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_float4x4 rotationMatrix(
		vRight,
		vUp,
		vLook,
		_float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3::Right * m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3::Up * m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3::Forward * m_vScale.z, RotationMatrix));
}

void UTransform::SetDirectionFixedUp(const _float3& targetDirection, float deltaTime, float rotationSpeed)
{
	_float3 FixedUp = _float3(0, 1, 0);
	_float3 vCurrentLook = GetLook();
	_float4 vTargetLook = XMVector3Normalize(targetDirection);

	float lerpFactor = rotationSpeed * deltaTime;
	lerpFactor = std::clamp(lerpFactor, 0.0f, 1.0f);

	_float3 n = DirectX::XMVectorLerp(vCurrentLook, vTargetLook, lerpFactor);
	_float4 vLook = XMVector3Normalize(n);

	_float4 vUp = DirectX::XMVector3Normalize(FixedUp);

	_float4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_float4x4 rotationMatrix(
		vRight,
		vUp,
		vLook,
		_float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3::Right * m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3::Up * m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3::Forward * m_vScale.z, RotationMatrix));
}


void UTransform::LookAt(const _float3& _vTargetPos)
{
	_float3 vPosition = GetPos();
	_float4 vLook = XMVector3Normalize(_vTargetPos - vPosition);

	_float4 vRight = DirectX::XMVector3Normalize(XMVector3Cross(_float3::Up, vLook));
	_float4 vUp = DirectX::XMVector3Normalize(XMVector3Cross(vLook, vRight));

	// 회전 행렬 계산
	_float4x4 rotationMatrix(
		vRight,
		vUp,
		vLook,
		_float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	m_vQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	_float4x4 RotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vQuaternion);

	SetRight(DirectX::XMVector3TransformNormal(_float3::Right * m_vScale.x, RotationMatrix));
	SetUp(DirectX::XMVector3TransformNormal(_float3::Up * m_vScale.y, RotationMatrix));
	SetLook(DirectX::XMVector3TransformNormal(_float3::Forward * m_vScale.z, RotationMatrix));
}

void UTransform::LookAtWithFixedUp(const _float3& _vTargetPos)
{
	_float3 FixedUp = _float3(0, 1, 0);
	_float3 MyPosition = GetPos();
	_float3 vLook = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(_vTargetPos, MyPosition));
	_float3 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vLook, FixedUp));
	

	SetRight(vRight* m_vScale.x);
	SetUp(FixedUp* m_vScale.y);
	SetLook(vLook* m_vScale.z);
}

void UTransform::LookAtWithFixedUp(const _float3& _vTargetPos, float DeltaTime, float RotationSpeed)
{
	_float3 FixedUp = _float3(0, 1, 0);
	_float3 MyPosition = GetPos();
	_float3 vLook = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(_vTargetPos, MyPosition));
	_float3 vRight = DirectX::XMVector3Normalize(XMVector3Cross(vLook, FixedUp));

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
		_float3 NewLook = DirectX::XMVector3Normalize(DirectX::XMVectorLerp(GetLook(), vLook, DeltaTime));
		_float3 NewRight = DirectX::XMVector3Normalize(XMVector3Cross(NewLook, FixedUp));

		SetRight(NewRight * m_vScale.x);
		SetUp(FixedUp * m_vScale.y);
		SetLook(NewLook * m_vScale.z);
	}
}

const _float UTransform::ComputeDistance(const _float3& _vPos)
{
	return _float3::Length(GetPos() - _vPos);
}

const _float UTransform::ComputeDistanceSq(const _float3& _vPos)
{
	return _float3::LengthSquared(GetPos() - _vPos);
}

HRESULT UTransform::BindTransformData(CSHPTRREF< UShader> _spShader)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);
	RETURN_CHECK(nullptr == m_TransformBuffer, E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	TransformUpdate();
	m_stTransformParam.iCamIndex = spGameInstance->GetRenderCamID();
	m_stTransformParam.mPrevWorldMatrix = m_stTransformParam.mWorldMatrix;
	m_stTransformParam.mWorldMatrix = m_mChangeWorldMatrix.Transpose();
	return _spShader->BindCBVBuffer(m_TransformBuffer, &m_stTransformParam, GetTypeSize<TRANSFORMPARAM>());
}

HRESULT UTransform::BindTransformData(CSHPTRREF<UShader> _spShader, const _float4x4& _mTransform)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_stTransformParam.iCamIndex = spGameInstance->GetRenderCamID();
	m_stTransformParam.mPrevWorldMatrix = _mTransform;
	m_stTransformParam.mWorldMatrix = m_mChangeWorldMatrix.Transpose();
	return _spShader->BindCBVBuffer(m_TransformBuffer, &m_stTransformParam, GetTypeSize<TRANSFORMPARAM>());
}

HRESULT UTransform::BindTransformData(CSHPTRREF<UShader> _spShader, CAMID _RenderCamID)
{
	RETURN_CHECK(nullptr == _spShader, E_FAIL);
	TransformUpdate();
	m_stTransformParam.iCamIndex = _RenderCamID;
	m_stTransformParam.mPrevWorldMatrix = m_stTransformParam.mWorldMatrix;
	m_stTransformParam.mWorldMatrix = m_mChangeWorldMatrix.Transpose();
	return _spShader->BindCBVBuffer(m_TransformBuffer, &m_stTransformParam, GetTypeSize<TRANSFORMPARAM>());
}

void UTransform::DisableGravity()
{
	m_vVelocity = _float3(0.0f, 0.0f, 0.0f);
}

void UTransform::DisableJump()
{
	m_vJumpvelocity = _float3(0.0f, 12.f, 0.0f);
}

void UTransform::JumpMovement(const _double& _deltaTime)
{
	m_vJumpvelocity += m_vGravity * static_cast<_float>(_deltaTime);
	_float3 vPosition = GetPos();
	vPosition += m_vJumpvelocity * static_cast<_float>(_deltaTime);
	SetPos(vPosition);
}

_float3 UTransform::GetRotationValue()
{
	static DirectX::PTH::OUTMATRIX OutMatrix;
	OutMatrix = m_mChangeWorldMatrix.Get_OutMatrix();
	return OutMatrix.vRot;
}


void UTransform::GravityFall(const _double& _deltaTime)
{
	m_vVelocity += m_vGravity * static_cast<_float>(_deltaTime);
	_float3 vPosition = GetPos();
	vPosition += m_vVelocity * static_cast<_float>(_deltaTime);
	SetPos(vPosition);
}

#ifdef _USE_IMGUI
void UTransform::ShowObjectInfo()
{
	static const char* TransformName{ "Transform" };
	static const char* PositionName{ "Position" };
	static const char* ScaleName{ "Scale" };
	static const char* Rotation{ "Rotation" };
	static const char* ParentsName{ "Parents_Transform" };
	if (ImGui::TreeNode(TransformName))
	{
		_float3 vPos = GetPos();
		ImGui::DragFloat3(PositionName, &vPos.x, 0.1f);
		if (true == ImGui::DragFloat3(ScaleName, &m_vScale.x, 0.1f))
		{
			m_mWorldMatrix.MatrixSetScaling(m_vScale);
		}
		_float3 vRotation = m_vQuaternion.ToEuler();
		if (true == ImGui::DragFloat3(Rotation, &vRotation.x, 0.1f))
		{
			RotateFix(vRotation);
		}
		if (nullptr != m_spParentsTransform)
		{
			ImGui::Text(ParentsName);
			m_spParentsTransform->ShowObjectInfo();
		}
		ImGui::TreePop();
	}
}
#endif