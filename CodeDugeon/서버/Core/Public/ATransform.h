#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ATRANFORM_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ATRANFORM_H

#include "ACoreBase.h"

BEGIN(Core)
/*
@ Date: 2024-07-03
@ Writer: 박태현
@ Explain
- 실제 게임에서 오브젝트가 사용하는 Transform
*/
class CORE_DLL ATransform : public ACoreBase {
public:
	ATransform();
	DESTRUCTOR(ATransform)
public:
	const Vector3& GetScale() { return m_vScale; }
	const Vector3 GetAngles() const;
	const _float GetXAngle() const;
	const _float GetYAngle() const;
	const _float GetZAngle() const;
	const _float4x4 GetWorldMatrixTP() { TransformUpdate(); return XMMatrixTranspose(XMLoadFloat4x4(&m_mChangeWorldMatrix)); }
	const _float4x4 GetWorldMatrixInv() { TransformUpdate();  return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_mChangeWorldMatrix)); }
	const _float4x4 GetWorldMatrix() const { READ_SPINLOCK(m_WorldSpinLock); return m_mWorldMatrix;}

	const _float4x4& GetChangeMatrix() { TransformUpdate();   return m_mChangeWorldMatrix; }
	// Get Parents Local Matrix
	const _float4x4 GetParentsMatrix();

	const Vector3 GetRight() const { READ_SPINLOCK(m_WorldSpinLock); return *((Vector3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_RIGHT][0]); }
	const Vector3 GetUp() const { READ_SPINLOCK(m_WorldSpinLock); return *((Vector3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_UP][0]); }
	const Vector3 GetLook() const { READ_SPINLOCK(m_WorldSpinLock); return *((Vector3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_LOOK][0]); }
	const Vector3 GetPos() const { READ_SPINLOCK(m_WorldSpinLock); return *((Vector3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_POS][0]); }

	const Vector3 GetChangeRight() const { READ_SPINLOCK(m_ChangeSpinLock); return *((Vector3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_RIGHT][0]); }
	const Vector3 GetChangeUp() const { READ_SPINLOCK(m_ChangeSpinLock);  return *((Vector3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_UP][0]); }
	const Vector3 GetChangeLook() const { READ_SPINLOCK(m_ChangeSpinLock);  return *((Vector3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_LOOK][0]); }
	const Vector3 GetChangePos() const { READ_SPINLOCK(m_ChangeSpinLock); return *((Vector3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_POS][0]); }

	const SHPTR<ATransform>& GetParentsTransform() const { return m_spParentsTransform; }

	const Vector3 GetJumpVelocity() const { return m_vJumpvelocity; }

	void SetScale(const Vector3& _vScale);
	void SetParent(CSHPTRREF<ATransform> _spTransform);

	void SetRight(const Vector3& _vRight) { WRITE_SPINLOCK(m_WorldSpinLock); m_mWorldMatrix.Set_Right(_vRight); }
	void SetUp(const Vector3& _vUp) { WRITE_SPINLOCK(m_WorldSpinLock); m_mWorldMatrix.Set_Up(_vUp); }
	void SetLook(const Vector3& _vLook) { WRITE_SPINLOCK(m_WorldSpinLock); m_mWorldMatrix.Set_Look(_vLook); }
	void SetPos(const Vector3& _vPos) { WRITE_SPINLOCK(m_WorldSpinLock); m_mWorldMatrix.Set_Pos(_vPos); }

	void SetNotApplyRotate(const _bool _isActive) {this->m_isNotApplyRotate = _isActive; }
	void SetNotApplyPos(const _bool _isActive) {  this->m_isNotApplyPos = _isActive; }
	void SetNotApplyScale(const _bool _isActive) { this->m_isNotApplyScale = _isActive; }
	void SetParentsTransform(CSHPTRREF<ATransform> _spTransform) {  this->m_spParentsTransform = _spTransform; }
	void SetNewWorldMtx(const _float4x4& _newworldMtx);
public:
	virtual void Free() override;
public:
	void TransformUpdate();
	// Move 
	void MoveForward(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveBack(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveLeft(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveRight(const _double& _dTimeDelta, const _float& _fSpeed);
	// Move Not Y
	void MoveForwardNotY(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveBackNotY(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveLeftNotY(const _double& _dTimeDelta, const _float& _fSpeed);
	void MoveRightNotY(const _double& _dTimeDelta, const _float& _fSpeed);
	void MovePos(const Vector3& _vPos);
	void TranslatePos(const Vector3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void TranslateDir(const Vector3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed);
	void TranslateTrans(CSHPTRREF<ATransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	// Translate To Pos Not Y
	void TranslatePosNotY(const Vector3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void TranslateDirNotY(const Vector3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed);
	void TranslateTransNotY(CSHPTRREF<ATransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);

	void RotateFix(const Vector3& _vStandardAngle, const _float _fTurnAnge);
	// Just Angle
	void RotateFix(const Vector3& _vAngle);
	void RotateFixNotApplyRadians(const Vector3& _vStandardAngle, const _float _fTurnAnge);
	void RotateFixNotApplyRadians(const Vector3& _vAngle);
	// Quaternion
	void RotateFix(const Vector4& _vQuaternion);
	void RotateTurn(const Vector3& _vAxis, const _float& _fAngleSpeed, const _double& _dTimeDelta);
	void RotateTurn(const Vector3& _vAxis, const _float& _fAngle);
	void RotateTurn(const Vector4& _vQuaternion);
	void RotateTurn(const Vector3& _vAngle);

	void SetDirection(const Vector3& direction);
	void SetDirection(const Vector3& targetDirection, float deltaTime, float rotationSpeed);
	void SetDirectionFixedUp(const Vector3& direction);
	void SetDirectionFixedUp(const Vector3& targetDirection, float deltaTime, float rotationSpeed);

	void LookAt(const Vector3& _vTargetPos);
	void LookAtWithFixedUp(const Vector3& _vTargetPos);
	void LookAtWithFixedUp(const Vector3& _vTargetPos, float DeltaTime, float RotationSpeed);
	// Compute Distance
	const _float ComputeDistance(const Vector3& _vPos);
	const _float ComputeDistanceSq(const Vector3& _vPos);

	void GravityFall(const _double& _deltaTime);
	void DisableGravity();

	void DisableJump();
	void JumpMovement(const _double& _deltaTime);
	Vector3 GetRotationValue();
private:
	// Needs Value 
	_float4x4												m_mWorldMatrix;
	_float4x4												m_mChangeWorldMatrix;
	_quaternion											m_vQuaternion;
	Vector3													m_vScale;
	// Parents Location
	_bool														m_isNotApplyRotate;
	_bool														m_isNotApplyPos;
	_bool														m_isNotApplyScale;

	SHPTR<ATransform>							m_spParentsTransform;

	AFastSpinLock										m_WorldSpinLock;
	AFastSpinLock										m_ChangeSpinLock;

	//2024-05-24 이성현 중력 구현
	Vector3 m_vVelocity;
	const Vector3 m_vGravity;
	Vector3 m_vJumpvelocity;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ATRANFORM_H