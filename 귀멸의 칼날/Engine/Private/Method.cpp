#include "Method.h"
#include "GameInstance.h"


 _string CMethod::ConvertWToS(const wstring& var)
{
	string str;
	size_t size;
	str.resize(var.length());
	wcstombs_s(&size, &str[0], str.size() + 1, var.c_str(), var.size());
	return str;
}

 _wstring CMethod::ConvertSToW(const string& var)
{
	_wstring wstr;
	size_t size;
	wstr.resize(var.length());
	mbstowcs_s(&size, &wstr[0], wstr.size() + 1, var.c_str(), var.size());
	return wstr;
}

_bool CMethod::Is_Same_Text(const _string& _strOriginText, const _string& _strFindText)
{
	for (_uint k = 0; k < _strOriginText.length();)
	{
		_bool bIsTrue = false;
		for (_uint i = 0; i < _strFindText.length(); ++i)
		{
			if (false == bIsTrue)
			{
				if (_strFindText[i] == _strOriginText[k])
				{
					bIsTrue = true;
					++k;
				}
				else
					break;
			}
			else
			{
				if (_strFindText[i] == _strOriginText[k])
				{
					++k;
				}
				else
				{
					bIsTrue = false;
					break;
				}
			}
		}
		if (true == bIsTrue)
			return true;
		++k;
	}
	return false;
}
// Find Same Text
_bool CMethod::Is_Same_Text(const _wstring& _strOriginText, const _wstring& _strFindText)
{
	for (_uint k = 0; k < _strOriginText.length();)
	{
		_bool bIsTrue = false;
		for (_uint i = 0; i < _strFindText.length(); ++i)
		{
			if (false == bIsTrue)
			{
				if (_strFindText[i] == _strOriginText[k])
				{
					bIsTrue = true;
					++k;
				}
				else
					break;
			}
			else
			{
				if (_strFindText[i] == _strOriginText[k])
				{
					++k;
				}
				else
				{
					bIsTrue = false;
					break;
				}
			}
		}
		if (true == bIsTrue)
			return true;
		++k;
	}
	return false;
}

string CMethod::Convert_Number_S(const _uint& _iNum)
{
	string changeStr = "";

	_uint iNum = _iNum;
	while (true)
	{
		_uint Remain = iNum % 10;
		_char c = static_cast<_char>(Remain + 48);
		changeStr += c;
		if (iNum <= 9)
			break;
		iNum /= 10;
	}

	if (_iNum < 0)
		changeStr.append("-");

	string str;
	_int iIndex = (_int)changeStr.length() - 1;
	for (_int i = iIndex; i >= 0;)
	{
		str += changeStr[i--];
	}

	return str;
}

_wstring CMethod::Convert_Number_W(const _uint& _iNum)
{
	_wstring changeStr = L"";

	_uint iNum = _iNum;
	while (true)
	{
		_uint Remain = iNum % 10;
		_char c = static_cast<_char>(Remain + 48);
		changeStr += c;
		if (iNum <= 9)
			break;
		iNum /= 10;
	}

	if (_iNum < 0)
		changeStr.append(L"-");

	_wstring str;
	_int iIndex = (_int)changeStr.length() - 1;
	for (_int i = iIndex; i >= 0;)
	{
		str += changeStr[i--];
	}

	return str;
}

_string CMethod::Convert_LNumber_S(const _ulong& _iNum)
{
	string changeStr = "";

	_ulong iNum = _iNum;
	while (true)
	{
		_ulong Remain = iNum % 10;
		_char c = static_cast<_char>(Remain + 48);
		changeStr += c;
		if (iNum <= 9)
			break;
		iNum /= 10;
	}

	if (_iNum < 0)
		changeStr.append("-");

	string str;
	_long iIndex = (_long)changeStr.length() - 1;
	for (_long i = iIndex; i >= 0;)
	{
		str += changeStr[i--];
	}

	return str;
}

_wstring CMethod::Convert_LNumber_W(const _ulong& _iNum)
{
	_wstring changeStr = L"";

	_ulong iNum = _iNum;
	while (true)
	{
		_ulong Remain = iNum % 10;
		_char c = static_cast<_char>(Remain + 48);
		changeStr += c;
		if (iNum <= 9)
			break;
		iNum /= 10;
	}

	if (_iNum < 0)
		changeStr.append(L"-");

	_wstring str;
	_long iIndex = (_long)changeStr.length() - 1;
	for (_long i = iIndex; i >= 0;)
	{
		str += changeStr[i--];
	}

	return str;
}

_string CMethod::Convert_Number_S(const _double& _fNum, const _int& _iDemicalNum)
{
	_string changeStr = to_string(_fNum);

	_uint iEnd = (_uint)changeStr.find(".", 0) + _iDemicalNum + 1;

	changeStr = changeStr.substr(0, iEnd);

	return changeStr;
}
_wstring CMethod::Convert_Number_W(const _double& _fNum, const _int& _iDemicalNum)
{
	_wstring changeStr = to_wstring(_fNum);

	_uint iEnd = (_uint)changeStr.find(L".", 0) + _iDemicalNum + 1;

	changeStr = changeStr.substr(0, iEnd);

	return changeStr;
}
// Combine String To Int
string CMethod::Combine_Int(const _uint& _iIndex, const string& strName)
{
	_string str = strName;
	string s = Convert_Number_S(_iIndex);
	str.append(s);
	return str;
}
 _wstring CMethod::Combine_Int(const _uint& _iIndex, const _wstring& strName)
{
	_wstring str = strName;
	_wstring s = Convert_Number_W(_iIndex);
	str.append(s);
	return str;
}

string CMethod::Find_Extension(const _string& _str)
{
	_string str = _str;
	_uint iIndex = (_uint)str.find_last_of(".");

	_string returnStr = str.substr(iIndex + 1, str.length());
	return returnStr;
}

_wstring CMethod::Find_Extension(const _wstring& _str)
{
	_wstring str = _str;
	_uint iIndex = (_uint)str.find_last_of(L".");

	_wstring returnStr = str.substr(iIndex + 1, str.length());
	return returnStr;
}

PATHS CMethod::Split_Paths(const _wstring& _str, const _wstring& _strFirstFolderName)
{
	size_t iIndex = _str.find(_strFirstFolderName);
	_wstring strPath = _str.substr(iIndex + _strFirstFolderName.length(), _str.length());

	PATHS FolderPath;
	int i = 0;
	int j = 2;
	while (true)
	{
		i = (int)strPath.find(L"\\");
		if (-1 == i)
			break;

		strPath = strPath.substr(1, strPath.length());
		j = (_int)strPath.find(L"\\");
		if (j == -1)
			j = (_int)strPath.length();
		_wstring str = strPath.substr(i, j);
		strPath = strPath.substr(j, strPath.length());

		FolderPath.push_back(str);
	}
	return FolderPath;
}

CUSRGB CMethod::HexToRGB(const _ulong& _hexValue)
{
	CUSRGB rgb;
	//rgb= (_hexValue & 255) >> 24;
	rgb.r = (_hexValue & 0x000000ff);
	rgb.g= (_hexValue & 0x0000ff00) >> 8;
	rgb.b = (_hexValue & 0x00ff0000) >> 16;
	rgb.a = (_hexValue & 0xff000000) >> 24;
	return rgb;
}

_int CMethod::HexadecimalToDecimal(string hex)
{
	_int hexLength = (_int)hex.length();
	double dec = 0;

	for (_int i = 0; i < hexLength; ++i)
	{
		char b = hex[i];

		if (b >= 48 && b <= 57)
			b -= 48;
		else if (b >= 65 && b <= 70)
			b -= 55;

		dec += b * pow(16, ((hexLength - i) - 1));
	}

	return (_int)dec;
}

HANDLE CMethod::Begin_Save(const _tchar* _pPath)
{
	HANDLE hFile = CreateFile(_pPath, GENERIC_WRITE,
		NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return NULL;
	}
	return hFile;
}

HANDLE CMethod::Begin_Load(const _tchar* _pPath)
{
	HANDLE hFile = CreateFile(_pPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return NULL;
	}
	return hFile;
}
void CMethod::Save_File_WString(HANDLE& _hHandle, DWORD& _dwByte, const _wstring& _str)
{
	_uint Length = (_uint)lstrlen(_str.c_str()) + 1;
	WriteFile(_hHandle, &Length, sizeof(_uint), &_dwByte, 0);
	WriteFile(_hHandle, _str.c_str(), sizeof(_tchar) * Length, &_dwByte, 0);
}

void CMethod::Save_File_String(HANDLE& _hHandle, DWORD& _dwByte, const _string& _str)
{
	_uint Length = (_uint)strlen(_str.c_str()) + 1;
	WriteFile(_hHandle, &Length, sizeof(_uint), &_dwByte, 0);
	WriteFile(_hHandle, _str.c_str(), sizeof(_char) * Length, &_dwByte, 0);
}

_wstring CMethod::Load_File_WString(HANDLE& _hHandle, DWORD& _dwByte)
{
	_wstring Value = L"";
	_uint iLength = 0;
	ReadFile(_hHandle, &iLength, sizeof(_uint), &_dwByte, nullptr);
	_tchar* pProtoName = new _tchar[iLength];
	ReadFile(_hHandle, pProtoName, sizeof(_tchar) * iLength, &_dwByte, nullptr);
	Value = pProtoName;
	Safe_Delete_Array(pProtoName);
	return Value;
}

_string CMethod::Load_File_String(HANDLE& _hHandle, DWORD& _dwByte)
{
	_string Value = "";
	_uint iLength = 0;
	ReadFile(_hHandle, &iLength, sizeof(_uint), &_dwByte, nullptr);
	_char* pProtoName = new _char[iLength];
	ReadFile(_hHandle, pProtoName, sizeof(_char) * iLength, &_dwByte, nullptr);
	Value = pProtoName;
	Safe_Delete_Array(pProtoName);
	return Value;
}

_bool CMethod::Is_Vector_Zero(const _fvector& _Vector)
{	
	if (0 == XMVectorGetX(_Vector) && 0 == XMVectorGetY(_Vector) && 0 == XMVectorGetZ(_Vector))
		return true;

	return false;
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
	_matrix Matrix =  XMMatrixMultiple_P(Matrixes);
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

_vector CMethod::XMMatrixGetVector( _matrix _Matrix, const _uint& _iIndex)
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
	_float4x4 Matrix = _Matrix;
	XMStoreFloat3((_float3*)&_Matrix.m[_iIndex][0], _vSetVector);
	return Matrix;
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

_float CMethod::Compute_Radian_XZ_Plane(const _float3& _vSrcVector, const _float3& _vDstVector)
{
	_float3 vSrcNor = _float3(_vSrcVector.x, 0.f, _vSrcVector.z).Normalize();
	_float3 vDstNor = _float3(_vDstVector.x, 0.f, _vDstVector.z).Normalize();

	_float fRadian = XMVectorGetX(XMVector3AngleBetweenNormals(vSrcNor.Get_Vector(), vDstNor.Get_Vector()));

	if (XMVectorGetY(XMVector3Cross(vSrcNor.Get_Vector(), vDstNor.Get_Vector())) < 0.f)
		fRadian *= -1.f;

	return fRadian;
}

_float CMethod::Compute_Radian_XY_Plane(const _float3& _vSrcVector, const _float3& _vDstVector)
{
	_float3 vSrcNor = _float3(_vSrcVector.x, _vSrcVector.y, 0.f).Normalize();
	_float3 vDstNor = _float3(_vDstVector.x, _vDstVector.y, 0.f).Normalize();

	_float fRadian = XMVectorGetX(XMVector3AngleBetweenNormals(vSrcNor.Get_Vector(), vDstNor.Get_Vector()));

	if (XMVectorGetZ(XMVector3Cross(vSrcNor.Get_Vector(), vDstNor.Get_Vector())) > 0.f)
		fRadian *= -1.f;

	return fRadian;
}

_float CMethod::Compute_Degree_XZ_Plane(const _float3& _vSrcVector, const _float3& _vDstVector)
{
	_float fRadian = Compute_Radian_XZ_Plane(_vSrcVector, _vDstVector);

	return XMConvertToDegrees(fRadian);
}

_float CMethod::Compute_Degree_XY_Plane(const _float3& _vSrcVector, const _float3& _vDstVector)
{
	_float fRadian = Compute_Radian_XY_Plane(_vSrcVector, _vDstVector);

	return XMConvertToDegrees(fRadian);
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
	_float3 vScale = _vScale;
	_float3 vPos = _vPos;
	_float4 vRotation = _vRotation;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * vScale.z;
	Rot.r[3] = vPos.Get_Vector();

	_float4x4 MATRIX = XMMatrixStoreFloat4x4_P(Rot);
	return MATRIX;
}

_matrix CMethod::Combine_Matrix(const _float3& _vScale, const _float3& _vPos, const _float4& _vRotation)
{
	_float3 vScale = _vScale;
	_float3 vPos = _vPos;
	_float4 vRotation = _vRotation;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * vScale.z;
	Rot.r[3] = vPos.Get_Vector();

	return Rot;
}

_float4x4 CMethod::Combine_Matrix4x4(const OUTMATRIX& _OutMatrix)
{
	_float3 vScale = _OutMatrix.vScale;
	_float3 vPos = _OutMatrix.vPos;
	_float4 vRotation = _OutMatrix.vQuaternionRot;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * vScale.z;
	Rot.r[3] = vPos.Get_Vector();

	_float4x4 MATRIX = XMMatrixStoreFloat4x4_P(Rot);
	return MATRIX;
}

_matrix CMethod::Combine_Matrix(const OUTMATRIX& _OutMatrix)
{
	_float3 vScale = _OutMatrix.vScale;
	_float3 vPos = _OutMatrix.vPos;
	_float4 vRotation = _OutMatrix.vQuaternionRot;
	_matrix Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

	Rot.r[0] = XMVector3Normalize(Rot.r[0]) * vScale.x;
	Rot.r[1] = XMVector3Normalize(Rot.r[1]) * vScale.y;
	Rot.r[2] = XMVector3Normalize(Rot.r[2]) * vScale.z;
	Rot.r[3] = vPos.Get_Vector();

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
