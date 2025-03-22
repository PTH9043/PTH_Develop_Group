#pragma once
#include "UComponent.h"

BEGIN(Engine)
class UShaderConstantBuffer;
class UShader;

class UTransform : public UComponent{
public:
	UTransform(CSHPTRREF<UDevice> _spDevice);
	UTransform(const UTransform& _rhs);
	DESTRUCTOR(UTransform)
public:
	const _float3& GetScale() { return m_vScale; }
	const _float3 GetAngles() const;
	const _float GetXAngle() const;
	const _float GetYAngle() const;
	const _float GetZAngle() const;
	const _float4x4 GetWorldMatrixTP() { TransformUpdate();  return XMMatrixTranspose(XMLoadFloat4x4(&m_mChangeWorldMatrix)); }
	const _float4x4 GetWorldMatrixInv() { TransformUpdate();   return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_mChangeWorldMatrix)); }
	const _float4x4& GetWorldMatrix() const { return m_mWorldMatrix; }
	const _quaternion& GetQuaternion() { return m_vQuaternion; }

	const _float4x4& GetChangeMatrix() { TransformUpdate();   return m_mChangeWorldMatrix; }
	// Get Parents Local Matrix
	const _float4x4 GetParentsMatrix();

	const _float3& GetRight() const { return *((_float3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_RIGHT][0]); }
	const _float3& GetUp() const { return *((_float3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_UP][0]); }
	const _float3& GetLook() const { return *((_float3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_LOOK][0]); }
	const _float3& GetPos() const { return *((_float3*)&m_mWorldMatrix.m[DirectX::PTH::MATROW_POS][0]); }

	const _float3& GetPrevPos() const { return  *((_float3*)&m_stTransformParam.mPrevWorldMatrix.m[DirectX::PTH::MATROW_POS][0]); }

	const _float3& GetChangeRight() const { return *((_float3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_RIGHT][0]); }
	const _float3& GetChangeUp() const { return *((_float3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_UP][0]); }
	const _float3& GetChangeLook() const { return *((_float3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_LOOK][0]); }
	const _float3& GetChangePos() const { return *((_float3*)&m_mChangeWorldMatrix.m[DirectX::PTH::MATROW_POS][0]); }

	const SHPTR<UTransform>& GetParentsTransform() const { return m_spParentsTransform; }

	const _float3& GetJumpVelocity() const { return m_vJumpvelocity; }

	void SetScale(const _float3& _vScale);
	void SetParent(CSHPTRREF<UTransform> _spTransform);

	void SetRight(const _float3& _vRight) { m_mWorldMatrix.Set_Right(_vRight); }
	void SetUp(const _float3& _vUp) { m_mWorldMatrix.Set_Up(_vUp); }
	void SetLook(const _float3& _vLook) { m_mWorldMatrix.Set_Look(_vLook); }
	void SetPos(const _float3& _vPos) { m_mWorldMatrix.Set_Pos(_vPos); }

	void SetNotApplyRotate(const _bool _isActive) { this->m_isNotApplyRotate = _isActive; }
	void SetNotApplyPos(const _bool _isActive) { this->m_isNotApplyPos = _isActive; }
	void SetNotApplyScale(const _bool _isActive) { this->m_isNotApplyScale = _isActive; }
	void SetParentsTransform(CSHPTRREF<UTransform> _spTransform) { this->m_spParentsTransform = _spTransform; }
	void SetNewWorldMtx(const _float4x4& _newworldMtx);

	static _bool IsMotionBlurOn() { return m_isMotionBlurOn; }
	static void EnableAllMotionBlurOn() { m_isMotionBlurOn = true; }
	static void DisableAllMotionBlurOn() { m_isMotionBlurOn = false; }

	void EnableMotionblurOn() { m_stTransformParam.iMotionBlurOn = 1; }
	void DisableMotionblurOn() { m_stTransformParam.iMotionBlurOn = 0; }
public:
	virtual void Free() override;
	CLONE_MACRO(UTransform, "UTransform::Cloen To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _stDatas) override;

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
	void MovePos(const _float3& _vPos);
	void TranslatePos(const _float3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void TranslateDir(const _float3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed);
	void TranslateTrans(CSHPTRREF<UTransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	// Translate To Pos Not Y
	void TranslatePosNotY(const _float3& _vPos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void TranslateDirNotY(const _float3& _vDir, const _double& _dTimeDelta, const _float& _fSpeed);
	void TranslateTransNotY(CSHPTRREF<UTransform> _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);

	void RotateFix(const _float3& _vStandardAngle, const _float _fTurnAnge);
	// Just Angle
	void RotateFix(const _float3& _vAngle);

	void RotateFixNotApplyRadians(const _float3& _vStandardAngle, const _float _fTurnAnge);
	void RotateFixNotApplyRadians(const _float3& _vAngle);
	// Quaternion
	void RotateFix(const _float4& _vQuaternion);
	void RotateTurn(const _float3& _vAxis, const _float& _fAngleSpeed, const _double& _dTimeDelta);
	void RotateTurn(const _float3& _vAxis, const _float& _fAngle);
	void RotateTurn(const _float4& _vQuaternion);
	void RotateTurn(const _float3& _vAngle);

	void SetDirection(const _float3& direction);
	void SetDirection(const _float3& targetDirection, float deltaTime, float rotationSpeed);
	void SetDirectionFixedUp(const _float3& direction);
	void SetDirectionFixedUp(const _float3& targetDirection, float deltaTime, float rotationSpeed);

	void LookAt(const _float3& _vTargetPos);
	void LookAtWithFixedUp(const _float3& _vTargetPos);
	void LookAtWithFixedUp(const _float3& _vTargetPos, float DeltaTime, float RotationSpeed);
	// Compute Distance
	const _float ComputeDistance(const _float3& _vPos);
	const _float ComputeDistanceSq(const _float3& _vPos);
	// Shader 파일을 불러와서 바인드 하는 함수
	HRESULT BindTransformData(CSHPTRREF< UShader> _spShader);
	// Shader 파일을 불러와서 바인드 하는 함수
	HRESULT BindTransformData(CSHPTRREF< UShader> _spShader, const _float4x4& _mTransform);
	// Shader 파일을 불러와서 바인드 하는 함수
	HRESULT BindTransformData(CSHPTRREF<UShader> _spShader, CAMID _RenderCamID);

	void GravityFall(const _double& _deltaTime);
	void DisableGravity();

	void DisableJump();
	void JumpMovement(const _double& _deltaTime);
	_float3 GetRotationValue();

#ifdef _USE_IMGUI
public:
	virtual void ShowObjectInfo() override;
#endif
private:
	// Shader 
	SHPTR<UShaderConstantBuffer>	m_TransformBuffer;
	TRANSFORMPARAM							m_stTransformParam{};
	// Needs Value 
	_float4x4												m_mWorldMatrix;
	_float4x4												m_mChangeWorldMatrix;
	_quaternion											m_vQuaternion;
	_float3													m_vScale;
	// Parents Location
	_bool														m_isNotApplyRotate;
	_bool														m_isNotApplyPos;
	_bool														m_isNotApplyScale;

	SHPTR<UTransform>							m_spParentsTransform;


	//2024-05-24 이성현 중력 구현
    _float3 m_vVelocity; 
	const _float3 m_vGravity;
	_float3 m_vJumpvelocity;

	static _bool									m_isMotionBlurOn;
};

END