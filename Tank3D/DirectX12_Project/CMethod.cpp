#include "CMethod.h"

_bool CMethod::Is_Vector_Zero(const _fvector& _Vector)
{
	if (0 == XMVectorGetX(_Vector) && 0 == XMVectorGetY(_Vector) && 0 == XMVectorGetZ(_Vector))
		return true;

	return false;
}

XMVECTOR CMethod::SetVector(const _float _fX, const _float _fY, const _float _fZ)
{
	return XMVectorSet(_fX, _fY, _fZ, 0);
}

_matrix CMethod::XMMatrixLoadFloat4x4_P(const _float4x4& _Matrix)
{
	return XMLoadFloat4x4(&_Matrix);
}

_float4x4 CMethod::XMMatrixStoreFloat4x4_P(_matrix _Matrix)
{
	_float4x4 Value;
	XMStoreFloat4x4(&Value, _Matrix);
	return Value;
}

_float4x4 CMethod::XMMatrixIdentity_P()
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixIdentity());
	return Matrix;
}

_matrix CMethod::XMMatrixTranspose_P(const _float4x4& _Matrix)
{
	return XMMatrixTranspose(XMLoadFloat4x4(&_Matrix));
}

_float4x4 CMethod::XMMatrixTranspos4x4_P(const _float4x4& _Matrix)
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixTranspose(XMLoadFloat4x4(&_Matrix)));
	return Matrix;
}

_matrix CMethod::XMMatrixInverse_P(const _float4x4& _Matirx)
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&_Matirx));
}

_float4x4 CMethod::XMMatrixInverse4x4_P(const _float4x4& _Matrix)
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&_Matrix)));
	return Matrix;
}

_matrix CMethod::XMMatrixInverse_P(_matrix _Matrix)
{
	return XMMatrixInverse(nullptr, _Matrix);
}

_float4x4 CMethod::XMMatrixInverse4x4_P(_matrix _Matrix)
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixInverse(nullptr, _Matrix));
	return Matrix;
}

_matrix CMethod::XMMatrixMultiple_P(MATRIXES4X4 Matrixes)
{
	if (Matrixes.size() <= 0)
		return XMMatrixIdentity();

	if (Matrixes.size() < 1)
		return XMLoadFloat4x4(&Matrixes[0]);

	MATRIXES M;
	for (auto& iter : Matrixes)
		M.push_back(XMLoadFloat4x4(&iter));

	_matrix Matrix = M[0];
	for (_uint i = 1; i < M.size(); ++i)
		Matrix *= M[i];

	return Matrix;
}

_float4x4 CMethod::XMMatrixMultiple4x4_P(MATRIXES4X4 Matrixes)
{
	if (Matrixes.size() <= 0)
		return XMMatrixIdentity_P();

	if (Matrixes.size() < 1)
		return Matrixes[0];

	MATRIXES M;
	for (auto& iter : Matrixes)
		M.push_back(XMLoadFloat4x4(&iter));

	_matrix Matrix = M[0];
	for (_uint i = 1; i < M.size(); ++i)
		Matrix *= M[i];

	_float4x4 Convert;
	XMStoreFloat4x4(&Convert, Matrix);
	return Convert;
}

_matrix CMethod::XMMatrixMultiple_P(MATRIXES Matrixes)
{
	if (Matrixes.size() <= 0)
		return XMMatrixIdentity();

	if (Matrixes.size() < 1)
		return Matrixes[0];

	_matrix Matrix = Matrixes[0];
	for (_uint i = 1; i < Matrixes.size(); ++i)
		Matrix *= Matrixes[i];

	return Matrix;
}

_float4x4 CMethod::XMMatrixMultiple4x4_P(MATRIXES Matrixes)
{
	_float4x4 M;
	_matrix Matrix = XMMatrixMultiple_P(Matrixes);
	XMStoreFloat4x4(&M, Matrix);
	return M;
}

_float4 CMethod::XMMatrix4x4GetFloat4(const _float4x4& _Matrix, const _uint& _iIndex)
{
	_float4 vVector;
	vVector.Set_Vector(XMLoadFloat4((_float4*)&_Matrix.m[_iIndex][0]));
	return vVector;
}

_float4 CMethod::XMMatrixGetFloat4(_matrix _Matrix, const _uint& _iIndex)
{
	_float4 vVector;
	_float4x4 Value;
	XMStoreFloat4x4(&Value, _Matrix);
	vVector.Set_Vector(XMLoadFloat4((_float4*)&Value.m[_iIndex][0]));
	return vVector;
}

_float3 CMethod::XMMatrix4x4GetFloat3(const _float4x4& _Matrix, const _uint& _iIndex)
{
	_float3 vVector;
	vVector.Set_Vector(XMLoadFloat3((_float3*)&_Matrix.m[_iIndex][0]));
	return vVector;
}
_float3 CMethod::XMMatrixGetFloat3(_matrix _Matrix, const _uint& _iIndex)
{
	_float3 vVector;
	_float4x4 Value;
	XMStoreFloat4x4(&Value, _Matrix);
	vVector.Set_Vector(XMLoadFloat3((_float3*)&Value.m[_iIndex][0]));
	return vVector;
}

_vector CMethod::XMMatrix4x4GetVector(const _float4x4& _Matrix, const _uint& _iIndex)
{
	return XMLoadFloat4((_float4*)&_Matrix.m[_iIndex][0]);
}

_vector CMethod::XMMatrixGetVector(_matrix _Matrix, const _uint& _iIndex)
{
	_float4x4 Value;
	XMStoreFloat4x4(&Value, _Matrix);
	return XMLoadFloat4((_float4*)&Value.m[_iIndex][0]);
}

_matrix CMethod::XMMatrixSetVector(_vector _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], _vSetVector);
	return XMLoadFloat4x4(&Matrix);
}

_matrix CMethod::XMMatrixSetVector(_vector _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix)
{
	_float4x4 Matrix = _Matrix;
	XMStoreFloat3((_float3*)&_Matrix.m[_iIndex][0], _vSetVector);
	return XMLoadFloat4x4(&Matrix);
}

_float4x4 CMethod::XMMatrix4x4SetVector(_vector _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], _vSetVector);
	return Matrix;
}

_float4x4  CMethod::XMMatrix4x4SetVector(_vector _vSetVector, const _uint& _iIndex, _float4x4  _Matrix)
{
	XMStoreFloat3((_float3*)&_Matrix.m[_iIndex][0], _vSetVector);
	return _Matrix;
}

_matrix CMethod::XMMatrixSetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float4 vVector = _vSetVector;
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat4((_float4*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return XMLoadFloat4x4(&Matrix);
}
_matrix CMethod::XMMatrixSetFloat4(const _float4& _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix)
{
	_float4 vVector = _vSetVector;
	_float4x4 Matrix = _Matrix;
	XMStoreFloat4((_float4*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return XMLoadFloat4x4(&Matrix);
}

_float4x4 CMethod::XMMatrix4x4SetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float4 vVector = _vSetVector;
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat4((_float4*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return Matrix;
}
_float4x4  CMethod::XMMatrix4x4SetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _float4x4  _Matrix)
{
	_float4 vVector = _vSetVector;
	_float4x4 Matrix = _Matrix;
	XMStoreFloat4((_float4*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return Matrix;
}

_matrix CMethod::XMMatrixSetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float3 vVector = _vSetVector;
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return XMLoadFloat4x4(&Matrix);
}
_matrix CMethod::XMMatrixSetFloat3(const _float3& _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix)
{
	_float3 vVector = _vSetVector;
	_float4x4 Matrix = _Matrix;
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return XMLoadFloat4x4(&Matrix);
}

_float4x4 CMethod::XMMatrix4x4SetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _matrix  _Matrix)
{
	_float3 vVector = _vSetVector;
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, _Matrix);
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return Matrix;
}
_float4x4  CMethod::XMMatrix4x4SetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _float4x4  _Matrix)
{
	_float3 vVector = _vSetVector;
	_float4x4 Matrix = _Matrix;
	XMStoreFloat3((_float3*)&Matrix.m[_iIndex][0], vVector.Get_Vector());
	return Matrix;
}

_float4x4 CMethod::XMMatrix4x4SetScaling(const _float3& _v3Scaling, _matrix _Matrix)
{
	_vector		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);

	vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
	vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
	vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

	_float4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
	Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
	Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
	return Matrix;
}

_float4x4 CMethod::XMMatrix4x4SetScaling(const _float3& _v3Scaling, _float4x4 _Matrix)
{
	_vector		vRight = XMMatrix4x4GetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrix4x4GetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrix4x4GetVector(_Matrix, MATROW_LOOK);

	vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
	vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
	vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

	_float4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
	Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
	Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
	return Matrix;
}

_float4x4 CMethod::XMMatrix4x4SetRotationFix(const _float3& _vRotation, _matrix _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_Matrix = CMethod::XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);

	return XMMatrixStoreFloat4x4_P(_Matrix);
}

_float4x4 CMethod::XMMatrix4x4SetRotationFix(const _float3& _vRotation, const _float4x4& _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_float4x4 Matrix = _Matrix;
	Matrix = CMethod::XMMatrix4x4SetFloat3(vRight, MATROW_RIGHT, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vUp, MATROW_UP, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vLook, MATROW_LOOK, Matrix);

	return Matrix;
}

_float4x4 CMethod::XMMatrix4x4SetRotationTurn(const _float3& _vRotation, _matrix _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = XMMatrixGetFloat3(_Matrix, MATROW_RIGHT), v3Up = XMMatrixGetFloat3(_Matrix, MATROW_UP),
		v3Look = XMMatrixGetFloat3(_Matrix, MATROW_LOOK);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_Matrix = CMethod::XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);

	return XMMatrixStoreFloat4x4_P(_Matrix);
}

_float4x4 CMethod::XMMatrix4x4SetRotationTurn(const _float3& _vRotation, const _float4x4& _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = XMMatrix4x4GetFloat3(_Matrix, MATROW_RIGHT), v3Up = XMMatrix4x4GetFloat3(_Matrix, MATROW_UP),
		v3Look = XMMatrix4x4GetFloat3(_Matrix, MATROW_LOOK);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_float4x4 Matrix = _Matrix;
	Matrix = CMethod::XMMatrix4x4SetFloat3(vRight, MATROW_RIGHT, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vUp, MATROW_UP, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vLook, MATROW_LOOK, Matrix);

	return Matrix;
}

_matrix CMethod::XMMatrixSetRotationFix(const _float3& _vRotation, _matrix _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_Matrix = CMethod::XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);

	return _Matrix;
}

_matrix CMethod::XMMatrixSetRotationFix(const _float3& _vRotation, const _float4x4& _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);


	_float4x4 Matrix = _Matrix;
	Matrix = CMethod::XMMatrix4x4SetFloat3(vRight, MATROW_RIGHT, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vUp, MATROW_UP, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vLook, MATROW_LOOK, Matrix);

	return XMMatrixLoadFloat4x4_P(Matrix);
}

_matrix CMethod::XMMatrixSetRotationTurn(const _float3& _vRotation, _matrix _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = XMMatrixGetFloat3(_Matrix, MATROW_RIGHT), v3Up = XMMatrixGetFloat3(_Matrix, MATROW_UP),
		v3Look = XMMatrixGetFloat3(_Matrix, MATROW_LOOK);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_Matrix = CMethod::XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
	_Matrix = CMethod::XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);

	return _Matrix;
}

_matrix CMethod::XMMatrixSetRotationTurn(const _float3& _vRotation, const _float4x4& _Matrix)
{
	_float3 v3Angle = _vRotation;
	v3Angle.x = XMConvertToRadians(v3Angle.x);
	v3Angle.y = XMConvertToRadians(v3Angle.y);
	v3Angle.z = XMConvertToRadians(v3Angle.z);
	_matrix RotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

	OUTMATRIX OutMatrix = Out_Matrix(_Matrix);

	_float3 v3Scale = OutMatrix.vScale;
	_float3 v3Right = XMMatrix4x4GetFloat3(_Matrix, MATROW_RIGHT), v3Up = XMMatrix4x4GetFloat3(_Matrix, MATROW_UP),
		v3Look = XMMatrix4x4GetFloat3(_Matrix, MATROW_LOOK);
	_vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
		vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_float4x4 Matrix = _Matrix;
	Matrix = CMethod::XMMatrix4x4SetFloat3(vRight, MATROW_RIGHT, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vUp, MATROW_UP, Matrix);
	Matrix = CMethod::XMMatrix4x4SetFloat3(vLook, MATROW_LOOK, Matrix);

	return XMMatrixLoadFloat4x4_P(Matrix);
}

_matrix CMethod::XMMatrixSetScaling(const _float3& _v3Scaling, _matrix _Matrix)
{
	_vector		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);

	vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
	vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
	vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

	_float4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
	Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
	Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
	return XMMatrixLoadFloat4x4_P(Matrix);
}
_matrix CMethod::XMMatrixSetScaling(const _float3& _v3Scaling, _float4x4 _Matrix)
{
	_vector		vRight = XMMatrix4x4GetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrix4x4GetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrix4x4GetVector(_Matrix, MATROW_LOOK);

	vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
	vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
	vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

	_float4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
	Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
	Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
	return XMMatrixLoadFloat4x4_P(Matrix);
}

_float3 CMethod::Get_Scaling(_matrix _Matrix)
{
	_float3 v3Scale;
	_vector		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);

	v3Scale.x = XMVectorGetX(XMVector3Length(vRight));
	v3Scale.y = XMVectorGetY(XMVector3Length(vUp));
	v3Scale.z = XMVectorGetZ(XMVector3Length(vLook));

	return v3Scale;

}
_float3 CMethod::Get_Scaling4x4(const _float4x4& _Matrix)
{
	_float3 v3Scale;
	_vector		vRight = XMMatrix4x4GetVector(_Matrix, MATROW_RIGHT);
	_vector		vUp = XMMatrix4x4GetVector(_Matrix, MATROW_UP);
	_vector		vLook = XMMatrix4x4GetVector(_Matrix, MATROW_LOOK);

	v3Scale.x = XMVectorGetX(XMVector3Length(vRight));
	v3Scale.y = XMVectorGetY(XMVector3Length(vUp));
	v3Scale.z = XMVectorGetZ(XMVector3Length(vLook));

	return v3Scale;
}

_float4x4 CMethod::XMMatrixMultipleFloat(const _float& _vValue, const _float4x4& _WorldMatrix)
{
	_matrix Mat = XMLoadFloat4x4(&_WorldMatrix);
	Mat *= _vValue;
	return XMMatrixStoreFloat4x4_P(Mat);
}

_matrix CMethod::XMMatrixMultipleFloat(const _float& _vValue, const _matrix& _WorldMatrix)
{
	_matrix Mat = _WorldMatrix;
	Mat *= _vValue;
	return Mat;
}

OUTMATRIX CMethod::Out_Matrix(const _float4x4& _World)
{
	OUTMATRIX OutMatrix;
	_vector vPos, vRot, vScale;
	XMMatrixDecompose(&vScale, &vRot, &vPos, XMMatrixLoadFloat4x4_P(_World));
	OutMatrix.vPos.Set_Vector(vPos);
	OutMatrix.vQuaternionRot.Set_Vector(vRot);
	OutMatrix.vScale.Set_Vector(vScale);

	_float sqw = OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.w;
	_float sqx = OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.x;
	_float sqy = OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.y;
	_float sqz = OutMatrix.vQuaternionRot.z * OutMatrix.vQuaternionRot.z;

	OutMatrix.vRot.x = XMConvertToDegrees(asinf(2.f * (OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.x -
		OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.z)));
	OutMatrix.vRot.y = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.z +
		OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.y), (-sqx - sqy + sqz + sqw)));
	OutMatrix.vRot.z = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.y +
		OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.z), (-sqx + sqy - sqz + sqw)));

	return OutMatrix;
}

OUTMATRIX CMethod::Out_Matrix(_matrix _World)
{
	OUTMATRIX OutMatrix;
	_vector vPos, vRot, vScale;
	XMMatrixDecompose(&vScale, &vRot, &vPos, _World);
	OutMatrix.vPos.Set_Vector(vPos);
	OutMatrix.vQuaternionRot.Set_Vector(vRot);
	OutMatrix.vScale.Set_Vector(vScale);

	_float sqw = OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.w;
	_float sqx = OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.x;
	_float sqy = OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.y;
	_float sqz = OutMatrix.vQuaternionRot.z * OutMatrix.vQuaternionRot.z;

	OutMatrix.vRot.x = XMConvertToDegrees(asinf(2.f * (OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.x -
		OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.z)));
	OutMatrix.vRot.y = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.z +
		OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.y), (-sqx - sqy + sqz + sqw)));
	OutMatrix.vRot.z = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.y +
		OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.z), (-sqx + sqy - sqz + sqw)));

	return OutMatrix;
}

_float4x4 CMethod::Combine_Matrix4x4(const _float3& _vScale, const _float3& _vPos, const _float4& _vRotation)
{
	_matrix Rot = XMMatrixRotationQuaternion(_vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _vScale.z;
	Rot.r[3] = _vPos.Get_Vector() + XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_float4x4 MATRIX = XMMatrixStoreFloat4x4_P(Rot);
	return MATRIX;
}

_matrix CMethod::Combine_Matrix(const _float3& _vScale, const _float3& _vPos, const _float4& _vRotation)
{
	_matrix Rot = XMMatrixRotationQuaternion(_vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _vScale.z;
	Rot.r[3] = _vPos.Get_Vector() + XMVectorSet(0.f, 0.f, 0.f, 1.f);

	return Rot;
}

_float4x4 CMethod::Combine_Matrix4x4(const OUTMATRIX& _OutMatrix)
{
	_float4 vRotation = _OutMatrix.vQuaternionRot;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _OutMatrix.vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _OutMatrix.vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _OutMatrix.vScale.z;
	Rot.r[3] = _OutMatrix.vPos.Get_Vector();

	_float4x4 MATRIX = XMMatrixStoreFloat4x4_P(Rot);
	return MATRIX;
}

_matrix CMethod::Combine_Matrix(const OUTMATRIX& _OutMatrix)
{
	_float4 vRotation = _OutMatrix.vQuaternionRot;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _OutMatrix.vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _OutMatrix.vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _OutMatrix.vScale.z;
	Rot.r[3] = _OutMatrix.vPos.Get_Vector();

	return Rot;
}

_float4x4 CMethod::Combine_Matrix4x4(const _float3& _vPos, const _float3& _vScale, const _float3& _vRot)
{
	_float4 vRot = CMethod::Convert_RotationToQuaternion(_vRot);
	_matrix Rot = XMMatrixRotationQuaternion(vRot.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _vScale.z;
	Rot.r[3] = _vPos.Get_Vector();

	_float4x4 RotMatrix;
	XMStoreFloat4x4(&RotMatrix, Rot);
	return RotMatrix;
}

_matrix CMethod::Combine_Matrix(const _float3& _vPos, const _float3& _vScale, const _float3& _vRot)
{
	_float4 vRot = CMethod::Convert_RotationToQuaternion(_vRot);
	_matrix Rot = XMMatrixRotationQuaternion(vRot.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * _vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * _vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * _vScale.z;
	Rot.r[3] = _vPos.Get_Vector();

	return Rot;
}

_float3 CMethod::Convert_QuaternionToRotation(const _float4& _vQuarter)
{
	_float sqw = _vQuarter.w * _vQuarter.w;
	_float sqx = _vQuarter.x * _vQuarter.x;
	_float sqy = _vQuarter.y * _vQuarter.y;
	_float sqz = _vQuarter.z * _vQuarter.z;

	_float3 vQuat = _float3(0.f, 0.f, 0.f);
	vQuat.x = XMConvertToDegrees(asinf(2.f * (_vQuarter.w * _vQuarter.x -
		_vQuarter.y * _vQuarter.z)));
	vQuat.y = XMConvertToDegrees(atan2(2.f * (_vQuarter.x * _vQuarter.z +
		_vQuarter.w * _vQuarter.y), (-sqx - sqy + sqz + sqw)));
	vQuat.z = XMConvertToDegrees(atan2(2.f * (_vQuarter.x * _vQuarter.y +
		_vQuarter.w * _vQuarter.z), (-sqx + sqy - sqz + sqw)));

	return vQuat;
}

_float4 CMethod::Convert_RotationToQuaternion(const _float3& _vRotation)
{
	_float3 v3Rotation = _vRotation;
	v3Rotation.x = XMConvertToRadians(v3Rotation.x);
	v3Rotation.y = XMConvertToRadians(v3Rotation.y);
	v3Rotation.z = XMConvertToRadians(v3Rotation.z);
	_float4 vQuaternion;
	vQuaternion.Set_Vector(XMQuaternionRotationRollPitchYawFromVector(v3Rotation.Get_Vector()));

	return vQuaternion;
}

_float3 CMethod::Compute_SplineCurveF3(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = XMLoadFloat3(&_v3Pos1);
	_vector y1 = XMLoadFloat3(&_v3Pos2);
	_vector y2 = XMLoadFloat3(&_v3Pos3);
	_vector y3 = XMLoadFloat3(&_v3Pos4);

	_vector A0 = y3 - y2 - y1 + y1;
	_vector A1 = y0 - y1 - A0;
	_vector A2 = y2 - y0;
	_vector A3 = y1;

	_float m2 = fValue * fValue;

	return (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
}

_float3 CMethod::Compute_SplineCurveF3(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = _v3Pos1;
	_vector y1 = _v3Pos2;
	_vector y2 = _v3Pos3;
	_vector y3 = _v3Pos4;

	_vector A0 = y3 - y2 - y1 + y1;
	_vector A1 = y0 - y1 - A0;
	_vector A2 = y2 - y0;
	_vector A3 = y1;

	_float m2 = fValue * fValue;

	return (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
}

_vector CMethod::Compute_SplineCurve(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue)
{
	//float fValue = Clamp(_fValue);

	//_vector vPrevPos = XMLoadFloat3(&_v3Pos1);
	//_vector vPos = XMLoadFloat3(&_v3Pos2);
	//_vector vNext1 = XMLoadFloat3(&_v3Pos3);
	//_vector vNext2 = XMLoadFloat3(&_v3Pos4);

	//_vector Pos = 0.5f * ((2.f * vPos) + (-1.f * vPrevPos + vNext1) * fValue +
	//	(2.f * vPrevPos - 5.f * vPos + 4.f * vNext1 - vNext2) * fValue * fValue +
	//	(-1.f * vPrevPos + 3.f * vPos - 3.f * vNext1 + vNext2) * fValue * fValue * fValue);

	float fValue = Clamp(_fValue);

	_vector y0 = XMLoadFloat3(&_v3Pos1);
	_vector y1 = XMLoadFloat3(&_v3Pos2);
	_vector y2 = XMLoadFloat3(&_v3Pos3);
	_vector y3 = XMLoadFloat3(&_v3Pos4);

	_vector A0 = y3 - y2 - y1 + y1;
	_vector A1 = y0 - y1 - A0;
	_vector A2 = y2 - y0;
	_vector A3 = y1;

	_float m2 = fValue * fValue;

	return (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
}

_vector CMethod::Compute_SplineCurve(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = _v3Pos1;
	_vector y1 = _v3Pos2;
	_vector y2 = _v3Pos3;
	_vector y3 = _v3Pos4;

	//_vector Pos = 0.5f * ((2.f * vPos) + (-1.f * vPrevPos + vNext1) * _fValue +
	//	(2.f * vPrevPos - 5.f * vPos + 4.f * vNext1 - vNext2) * _fValue * _fValue +
	//	(-1.f * vPrevPos + 3.f * vPos - 3.f * vNext1 + vNext2) * _fValue * _fValue * _fValue);

	_vector A0 = y3 - y2 - y1 + y1;
	_vector A1 = y0 - y1 - A0;
	_vector A2 = y2 - y0;
	_vector A3 = y1;

	_float m2 = fValue * fValue;

	return (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
}

_float3 CMethod::Compute_BezierCurveF3(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = XMLoadFloat3(&_v3Pos1);
	_vector y1 = XMLoadFloat3(&_v3Pos2);
	_vector y2 = XMLoadFloat3(&_v3Pos3);
	_vector y3 = XMLoadFloat3(&_v3Pos4);

	_float t = fValue;
	_float u = 1.f - fValue;
	_float tt = t * t;
	_float uu = u * u;
	_float uuu = uu * u;
	_float ttt = tt * t;

	_vector vPos = (uuu * y0);
	vPos += (3 * uu * t * y1);
	vPos += (3 * u * tt * y2);
	vPos += (ttt * y3);

	return vPos;
}

_float3 CMethod::Compute_BezierCurveF3(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);
	_vector y0 = _v3Pos1;
	_vector y1 = _v3Pos2;
	_vector y2 = _v3Pos3;
	_vector y3 = _v3Pos4;

	_float t = fValue;
	_float u = 1.f - fValue;
	_float tt = t * t;
	_float uu = u * u;
	_float uuu = uu * u;
	_float ttt = tt * t;

	_vector vPos = (uuu * y0);
	vPos += (3 * uu * t * y1);
	vPos += (3 * u * tt * y2);
	vPos += (ttt * y3);

	return vPos;
}

_vector CMethod::Compute_BezierCurve(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = XMLoadFloat3(&_v3Pos1);
	_vector y1 = XMLoadFloat3(&_v3Pos2);
	_vector y2 = XMLoadFloat3(&_v3Pos3);
	_vector y3 = XMLoadFloat3(&_v3Pos4);

	_float t = fValue;
	_float u = 1.f - fValue;
	_float tt = t * t;
	_float uu = u * u;
	_float uuu = uu * u;
	_float ttt = tt * t;

	_vector vPos = (uuu * y0);
	vPos += (3 * uu * t * y1);
	vPos += (3 * u * tt * y2);
	vPos += (ttt * y3);

	return vPos;
}

_vector CMethod::Compute_BezierCurve(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue)
{
	float fValue = Clamp(_fValue);

	_vector y0 = _v3Pos1;
	_vector y1 = _v3Pos2;
	_vector y2 = _v3Pos3;
	_vector y3 = _v3Pos4;

	_float t = fValue;
	_float u = 1.f - fValue;
	_float tt = t * t;
	_float uu = u * u;
	_float uuu = uu * u;
	_float ttt = tt * t;

	_vector vPos = (uuu * y0);
	vPos += (3 * uu * t * y1);
	vPos += (3 * u * tt * y2);
	vPos += (ttt * y3);

	return vPos;
}

_float CMethod::Clamp(const _float& _fValue)
{
	_float fValue = _fValue;
	if (fValue < 0.f) fValue = 0.f;
	else if (fValue > 1.f) fValue = 1.f;

	return fValue;
}
