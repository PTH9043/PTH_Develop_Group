#include "Transform.h"
#include "Navigation.h"
#include "Method.h"

CTransform::CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& _rhs)
	: CComponent(_rhs), m_WorldMatrix(_rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scale()
{
	_float3 v3Scale;

	v3Scale.x = XMVectorGetX(XMVector3Length(Get_Right()));
	v3Scale.y = XMVectorGetY(XMVector3Length(Get_Up()));
	v3Scale.z = XMVectorGetZ(XMVector3Length(Get_Look()));

	return v3Scale;
}

_float4x4 CTransform::Get_WorldFloat4x4_TP() const
{
	_float4x4		TransposeMatrix;
	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	return TransposeMatrix;
}

_float4x4 CTransform::Get_WorldFloat4x4Inv()
{
	_float4x4 WorldInv;
	XMStoreFloat4x4(&WorldInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	return WorldInv;
}

void CTransform::Set_Scale(const _float3& _v3Scale)
{
	_vector		vRight = Get_Right();
	_vector		vUp = Get_Up();
	_vector		vLook = Get_Look();

	vRight = XMVector3Normalize(vRight) * _v3Scale.x;
	vUp = XMVector3Normalize(vUp) * _v3Scale.y;
	vLook = XMVector3Normalize(vLook) * _v3Scale.z;

	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);
}

void CTransform::Bind_WorldMatrix(const _fmatrix& _fMatrix)
{
	_float3 vScale = Get_Scale();
	_float4x4 vMatrix;
	XMStoreFloat4x4(&vMatrix, _fMatrix);

	if (vScale.x == 0)
		vScale.x = 1;

	if (vScale.y == 0)
		vScale.y = 1;

	if (vScale.z == 0)
		vScale.z = 1;

	_vector vRight = XMLoadFloat4((_float4*)&vMatrix.m[STATE_RIGHT][0]);
	_vector vUp = XMLoadFloat4((_float4*)&vMatrix.m[STATE_UP][0]);
	_vector vLook = XMLoadFloat4((_float4*)&vMatrix.m[STATE_LOOK][0]);
	_vector vPos = XMLoadFloat4((_float4*)&vMatrix.m[STATE_POSITION][0]);

	Set_Right(XMVector3Normalize(vRight) * vScale.x);
	Set_Up(XMVector3Normalize(vUp) * vScale.y);
	Set_Look(XMVector3Normalize(vLook) * vScale.z);
	Set_Pos(vPos);

}

HRESULT CTransform::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;


	return S_OK;
}

void CTransform::Move_Forward(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	_vector vLook = Get_Look();

	vPosition += XMVector3Normalize(vLook) * (_float)(_dTimeDelta * _fSpeed);

	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Move_Back(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	_vector vLook = Get_Look();

	vPosition -= XMVector3Normalize(vLook) * (_float)(_dTimeDelta * _fSpeed);
	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Move_Left(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	_vector vRight = Get_Right();

	vPosition -= XMVector3Normalize(vRight) * (_float)(_dTimeDelta * _fSpeed);
	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Move_Right(const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	_vector vRight = Get_Right();

	vPosition += XMVector3Normalize(vRight) * (_float)(_dTimeDelta * _fSpeed);
	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Move_Pos(const _fvector& _v4TargetPos)
{
	_vector vPosition = _v4TargetPos;
	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Move_PosNotNavigation(const _fvector& _v4TargetPos)
{
	_vector vPosition = _v4TargetPos;
	//_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition))
		return;
	//vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

_bool CTransform::Move_Pos_NotNavigateY(const _fvector& _v4TargetPos)
{
	_vector vPosition = _v4TargetPos;
	_float fY = XMVectorGetY(vPosition);
	if (false == Is_Running_Nav(vPosition))
	{
		vPosition = Get_Pos();
		vPosition = XMVectorSetY(vPosition, fY);
		Set_Pos(vPosition);
		return false;

	}
	Set_Pos(vPosition);

	return true;
}

void CTransform::Translate_Pos(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	if (_fLimitDistance <= Compute_Distance(_v4Pos))
	{
		_vector vPosition = Get_Pos();
		_vector vDir = _v4Pos - vPosition;
		vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
		_float fY = 0.f;
		if (false == Is_Running_Nav(vPosition, &fY))
			return;
		vPosition = XMVectorSetY(vPosition, fY);
		Set_Pos(vPosition);
	}
}

void CTransform::Translate_Dir(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	vPosition += XMVector3Normalize(_v4Dir) * (_float)(_dTimeDelta * _fSpeed);
	_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition, &fY))
		return;
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Translate_DirNotNavigation(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	vPosition += _v4Dir * (_float)(_dTimeDelta * _fSpeed);
	//_float fY = 0.f;
	if (false == Is_Running_Nav(vPosition))
		return;
	//vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Translate_Trans(CTransform* _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	if (nullptr == _pTransform)
		return;
	if (_fLimitDistance <= Compute_Distance(_pTransform->Get_Pos()))
	{
		_vector vPosition = Get_Pos();
		_vector vDir = _pTransform->Get_Pos() - vPosition;
		vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
		_float fY = 0.f;
		if (false == Is_Running_Nav(vPosition, &fY))
			return;
		vPosition = XMVectorSetY(vPosition, fY);
		Set_Pos(vPosition);
	}
}

void CTransform::Translate_DirSliding(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_vector vPosition = Get_Pos();
	vPosition += XMVector3Normalize(_v4Dir) * (_float)(_dTimeDelta * _fSpeed);
	_float fY = 0.f;
	_uint iCount = 0;
	while (false == Is_Running_Nav(vPosition, &fY))
	{
		_float3 vSlideDir = m_pNavigation->Compute_SlideVector(Get_Pos(), vPosition);
		vPosition = Get_Pos() + vSlideDir.Get_Vector() * (_float)(_dTimeDelta * _fSpeed);
		if (iCount++ > 5)
			return;
	}
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Pos(vPosition);
}

void CTransform::Rotate_Turn(const _fvector& _v4Axis, const _double& _dTimeDelta, const _float& _fSpeed)
{
	_matrix RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(_v4Axis, (_float)(_dTimeDelta * _fSpeed));

	_vector		vRight = Get_Right();
	_vector		vUp = Get_Up();
	_vector		vLook = Get_Look();

	// XMVector4Transform();
	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotate_Fix(const _fvector& _v4Axis, const _float& _fAngle)
{
	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(_v4Axis, XMConvertToRadians(_fAngle));
	_float3 v3Scale = Get_Scale();
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector vRight = XMLoadFloat3(&v3Right), vUp = XMLoadFloat3(&v3Up), vLook = XMLoadFloat3(&v3Look);

	vRight = XMVector3TransformNormal(vRight * v3Scale.x, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp * v3Scale.y, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook * v3Scale.z, RotationMatrix);
	// XMVector4Transform();
	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);
}

void CTransform::Rotate_Turn(const _float3& _v3Angle)
{
	_float3 v3Angle = _v3Angle;

	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));
	_float3 v3Scale = Get_Scale();
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);
}

void CTransform::Rotate_TurnToNotFix(const _float3& _vAngle)
{
	_float3 v3Angle = _vAngle;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));
	_float3 v3Scale = Get_Scale();
	_float3 v3Right = Get_RightF3().Normalize(), v3Up = Get_UpF3().Normalize(), v3Look = Get_LookF3().Normalize();
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);
}

void CTransform::Chase(const _fvector& _v4TargetPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
	LookAt(_v4TargetPos);

	if (_fLimitDistance <= Compute_Distance(_v4TargetPos))
		Move_Forward(_dTimeDelta, _fSpeed);
}

void CTransform::LookAt(const _fvector& _v4TargetPos)
{
	_vector vPosition = Get_Pos();
	_vector vLook = XMVector3Normalize(_v4TargetPos - vPosition);

	_float3 Look = vLook;

	if (isnan(Look.x) || isnan(Look.y)  || isnan(Look.z))
		return;

	if (CMethod::Is_Vector_Zero(vLook))
		return;

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	_float3 vScale = Get_Scale();

	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);

	Set_Scale(vScale);
}

void CTransform::LookBack(const _fvector& _v4TargetPos)
{
	_vector vPosition = Get_Pos();
	_vector vLook = XMVector3Normalize(vPosition - _v4TargetPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	_float3 vScale = Get_Scale();

	Set_Right(vRight);
	Set_Up(vUp);
	Set_Look(vLook);

	Set_Scale(vScale);
}

_float CTransform::Compute_Distance(const _fvector& _v4TargetPos)
{
	_vector		vPosition = Get_Pos();
	return XMVectorGetX(XMVector3Length(vPosition - _v4TargetPos));
}

// 타겟 위치로 네비게이션 셀 업데이트
_bool CTransform::Update_Navigation_Cell(const _fvector& vTargetPos)
{
	CCell* pCell = m_pNavigation->Find_Cell(vTargetPos);
	if (nullptr == pCell)
		return false;
	_uint iNavi = pCell->Get_Index();
	m_pNavigation->Set_CurIndex(iNavi);
	Set_Pos(vTargetPos);
	return true;
}

_bool CTransform::Is_Running_Nav(_vector& _v4Pos, _float* _fY)
{
	_float fYPos = XMVectorGetY(_v4Pos);
	if (nullptr != m_pNavigation)
	{
		if (false == m_pNavigation->Is_Move(_v4Pos, &m_pCell))
		{
			m_bCollisionNav = true;
			return false;
		}

		if (nullptr != _fY)
		{
			fYPos = m_pNavigation->Compute_Height(_v4Pos);
		}
	}
	if (nullptr != _fY)
		*_fY = fYPos;

	m_bCollisionNav = false;
	return true;
}

CTransform* CTransform::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTransform* pInstance = new CTransform(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("CTransform::Create To Failed");
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* _pArg)
{
	CTransform* pInstance = new CTransform(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("CTransform::Clone To Failed");
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
