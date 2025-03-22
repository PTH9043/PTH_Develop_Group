#pragma once
#include "Engine_Defines.h"


BEGIN(Engine)
class CModel;

class ENGINE_DLL CMethod
{
public:
	static _string ConvertWToS(const wstring& var);

	static _wstring ConvertSToW(const string& var);

	static _bool Is_Same_Text(const _string& _strOriginText, const _string& _strFindText);
	// Find Same Text
	static _bool Is_Same_Text(const _wstring& _strOriginText, const _wstring& _strFindText);

	static _string Convert_Number_S(const _uint& _iNum);

	static _wstring Convert_Number_W(const _uint& _iNum);

	static _string Convert_LNumber_S(const _ulong& _iNum);

	static _wstring Convert_LNumber_W(const _ulong& _iNum);

	// Float to String
	static _string Convert_Number_S(const _double& _fNum, const _int& _iDemicalNum);
	// Float to String 
	static _wstring Convert_Number_W(const _double& _fNum, const _int& _iDemicalNum);

	// Combine String To Int
	static string Combine_Int(const _uint& _iIndex, const string& strName);

	static _wstring Combine_Int(const _uint& _iIndex, const _wstring& strName);

	static string Find_Extension(const _string& _str);

	static _wstring Find_Extension(const _wstring& _str);

	static PATHS Split_Paths(const _wstring& _str, const _wstring& _strFirstFolderName);

	static CUSRGB HexToRGB(const _ulong& _hexValue);

	static _int HexadecimalToDecimal(string hex);

	static HANDLE Begin_Save(const _tchar* _pPath);
	static HANDLE Begin_Load(const _tchar* _pPath);

	template<typename T>
	static void Save_File_NotString(HANDLE& _hHandle, DWORD& _dwByte, T _Data);
	static void Save_File_WString(HANDLE& _hHandle, DWORD& _dwByte, const _wstring& _str);
	static void Save_File_String(HANDLE& _hHandle, DWORD& _dwByte, const _string& _str);

	template<typename T>
	static T Load_File_NotString(HANDLE& _hHandle, DWORD& _dwByte, const DWORD& _iSize);
	static _wstring Load_File_WString(HANDLE& _hHandle, DWORD& _dwByte);
	static _string Load_File_String(HANDLE& _hHandle, DWORD& _dwByte);

	static _bool	Is_Vector_Zero(const _fvector& _Vector);

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
	static _float4 XMMatrixGetFloat4( _matrix _Matrix, const _uint& _iIndex);

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

	// _vSrcVector와 _vDstVector 사이의 Radian 각도를 구해준다.
	// _vSrcVector 기준으로 _DstVector가 오른쪽에 있을 경우 양의 값을 왼쪽에 있을 경우 음의 값을 반환한다.
	static _float Compute_Radian_XZ_Plane(const _float3& _vSrcVector, const _float3& _vDstVector);
	static _float Compute_Radian_XY_Plane(const _float3& _vSrcVector, const _float3& _vDstVector);

	// _vSrcVector와 _vDstVector 사이의 Degree 각도를 구해준다.
	// _vSrcVector 기준으로 _DstVector가 오른쪽에 있을 경우 양의 값을 왼쪽에 있을 경우 음의 값을 반환한다.
	static _float Compute_Degree_XZ_Plane(const _float3& _vSrcVector, const _float3& _vDstVector);
	static _float Compute_Degree_XY_Plane(const _float3& _vSrcVector, const _float3& _vDstVector);

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
inline void CMethod::Save_File_NotString(HANDLE& _hHandle, DWORD& _dwByte, T _Data)
{
	WriteFile(_hHandle, &_Data, sizeof(_Data), &_dwByte, nullptr);
}

template<typename T>
inline T CMethod::Load_File_NotString(HANDLE& _hHandle, DWORD& _dwByte, const DWORD& _iSize)
{
	T data;
	ReadFile(_hHandle, &data, _iSize, &_dwByte, nullptr);
	return data;
}

template<typename T>
inline T CMethod::ConvertTemplate_PopBack(VOIDDATAS* _pDatas)
{
	if (nullptr == _pDatas)
		return T();

	if (_pDatas->size() <= 0)
		return T();

	T data = static_cast<T>(_pDatas->back());
	_pDatas->pop_back();
	return data;
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

	T data = static_cast<T>((*_pDatas)[_iIndex]);
	return data;
}


END