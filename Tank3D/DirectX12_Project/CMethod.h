#pragma once
#include "Defines.h"

class  CMethod
{
public:
	static _bool	Is_Vector_Zero(const _fvector& _Vector);

	static XMVECTOR SetVector(const _float _fX, const _float _fY, const _float _fZ);

	static _matrix XMMatrixLoadFloat4x4_P(const _float4x4& _Matrix);
	static _float4x4 XMMatrixStoreFloat4x4_P(_matrix _Matrix);
	static _float4x4 XMMatrixIdentity_P();
	static _matrix XMMatrixTranspose_P(const _float4x4& _Matrix);
	static _float4x4 XMMatrixTranspos4x4_P(const _float4x4& _Matrix);
	static _matrix XMMatrixInverse_P(const _float4x4& _Matirx);
	static _float4x4 XMMatrixInverse4x4_P(const _float4x4& _Matrix);
	static _matrix XMMatrixInverse_P(_matrix _Matrix);
	static _float4x4 XMMatrixInverse4x4_P(_matrix _Matrix);
	static _matrix XMMatrixMultiple_P(MATRIXES4X4 Matrixes);
	static _float4x4 XMMatrixMultiple4x4_P(MATRIXES4X4 Matrixes);
	static _matrix XMMatrixMultiple_P(MATRIXES Matrixes);
	static _float4x4 XMMatrixMultiple4x4_P(MATRIXES Matrixes);

	static _float4 XMMatrix4x4GetFloat4(const _float4x4& _Matrix, const _uint& _iIndex);
	static _float4 XMMatrixGetFloat4(_matrix _Matrix, const _uint& _iIndex);

	static _float3 XMMatrix4x4GetFloat3(const _float4x4& _Matrix, const _uint& _iIndex);
	static _float3 XMMatrixGetFloat3(_matrix _Matrix, const _uint& _iIndex);

	static _vector XMMatrix4x4GetVector(const _float4x4& _Matrix, const _uint& _iIndex);
	static _vector XMMatrixGetVector(_matrix _Matrix, const _uint& _iIndex);

	static _matrix XMMatrixSetVector(_vector _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _matrix XMMatrixSetVector(_vector _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix);

	static _float4x4 XMMatrix4x4SetVector(_vector _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _float4x4  XMMatrix4x4SetVector(_vector _vSetVector, const _uint& _iIndex, _float4x4  _Matrix);

	static _matrix XMMatrixSetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _matrix XMMatrixSetFloat4(const _float4& _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix);

	static _float4x4 XMMatrix4x4SetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _float4x4  XMMatrix4x4SetFloat4(const _float4& _vSetVector, const _uint& _iIndex, _float4x4  _Matrix);

	static _matrix XMMatrixSetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _matrix XMMatrixSetFloat3(const _float3& _vSetVector, const _uint& _iIndex, const _float4x4& _Matrix);

	static _float4x4 XMMatrix4x4SetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _matrix  _Matrix);
	static _float4x4  XMMatrix4x4SetFloat3(const _float3& _vSetVector, const _uint& _iIndex, _float4x4  _Matrix);

	static _float4x4 XMMatrix4x4SetScaling(const _float3& _v3Scaling, _matrix _Matrix);
	static _float4x4 XMMatrix4x4SetScaling(const _float3& _v3Scaling, _float4x4 _Matrix);

	static _float4x4 XMMatrix4x4SetRotationFix(const _float3& _vRotation, _matrix _Matrix);
	static _float4x4 XMMatrix4x4SetRotationFix(const _float3& _vRotation, const _float4x4& _Matrix);

	static _float4x4 XMMatrix4x4SetRotationTurn(const _float3& _vRotation, _matrix _Matrix);
	static _float4x4 XMMatrix4x4SetRotationTurn(const _float3& _vRotation, const _float4x4& _Matrix);

	static _matrix XMMatrixSetRotationFix(const _float3& _vRotation, _matrix _Matrix);
	static _matrix XMMatrixSetRotationFix(const _float3& _vRotation, const _float4x4& _Matrix);

	static _matrix XMMatrixSetRotationTurn(const _float3& _vRotation, _matrix _Matrix);
	static _matrix XMMatrixSetRotationTurn(const _float3& _vRotation, const _float4x4& _Matrix);

	static _matrix XMMatrixSetScaling(const _float3& _v3Scaling, _matrix _Matrix);
	static _matrix XMMatrixSetScaling(const _float3& _v3Scaling, _float4x4 _Matrix);

	static _float3 Get_Scaling(_matrix _Matrix);
	static _float3 Get_Scaling4x4(const _float4x4& _Matrix);

	static _float4x4 XMMatrixMultipleFloat(const _float& _vValue, const _float4x4& _WorldMatrix);
	static _matrix XMMatrixMultipleFloat(const _float& _vValue, const _matrix& _WorldMatrix);

	template<typename T>
	static T ConvertTemplate_PopBack(VOIDDATAS* _pDatas);

	template<typename T>
	static T ConvertTemplate(void* _pData);

	template<typename T>
	static T ConvertTemplate_Index(VOIDDATAS* _pDatas, const _uint& _iIndex);

	static OUTMATRIX Out_Matrix(const _float4x4& _World);
	static OUTMATRIX Out_Matrix(_matrix _World);
	// Combine Matrix
	static _float4x4 Combine_Matrix4x4(const _float3& _vScale, const _float3& _vPos, const _float4& _vRotation);
	static _matrix Combine_Matrix(const _float3& _vScale, const _float3& _vPos, const _float4& _vRotation);

	static _float4x4 Combine_Matrix4x4(const OUTMATRIX& _OutMatrix);
	static _matrix Combine_Matrix(const OUTMATRIX& _OutMatrix);
	static _float4x4 Combine_Matrix4x4(const _float3& _vPos, const _float3& _vScale, const _float3& _vRot);
	static _matrix Combine_Matrix(const _float3& _vPos, const _float3& _vScale, const _float3& _vRot);

	static _float3 Convert_QuaternionToRotation(const _float4& _vQuarter);
	static _float4 Convert_RotationToQuaternion(const _float3& _vRotation);

	static _float3 Compute_SplineCurveF3(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue);
	static _float3 Compute_SplineCurveF3(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue);

	static _vector Compute_SplineCurve(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue);
	static _vector Compute_SplineCurve(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue);

	static _float3 Compute_BezierCurveF3(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue);
	static _float3 Compute_BezierCurveF3(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue);

	static _vector Compute_BezierCurve(const _float3& _v3Pos1, const _float3& _v3Pos2, const _float3& _v3Pos3, const _float3& _v3Pos4, const _float& _fValue);
	static _vector Compute_BezierCurve(const _vector& _v3Pos1, const _vector& _v3Pos2, const _vector& _v3Pos3, const _vector& _v3Pos4, const _float& _fValue);

	static _float Clamp(const _float& _fValue);
};

template<typename T>
inline T CMethod::ConvertTemplate_PopBack(VOIDDATAS* _pDatas)
{
	if (nullptr == _pDatas)
		return T();

	if (_pDatas->size() <= 0)
		return T();

	T* data = static_cast<T*>(_pDatas->back());
	_pDatas->pop_back();
	return *data;
}

template<typename T>
inline T CMethod::ConvertTemplate(void* _pData)
{
	T data = static_cast<T>(_pData);
	return data;
}

template<typename T>
inline T CMethod::ConvertTemplate_Index(VOIDDATAS* _pDatas, const _uint& _iIndex)
{
	if (nullptr == _pDatas)
		return T();

	if (_pDatas->size() <= 0)
		return T();

	if (_pDatas->size() <= _iIndex)
		return T();

	T* data = static_cast<T*>((*_pDatas)[_iIndex]);
	return *data;
}
