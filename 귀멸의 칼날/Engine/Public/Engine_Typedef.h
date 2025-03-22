#pragma once

namespace Engine
{

	typedef unsigned char		_ubyte;
	typedef signed char			_byte;
	typedef char			_char;

	typedef unsigned short		_ushort;
	typedef signed short			_short;

	typedef unsigned int			_uint;
	typedef signed int				_int;

	typedef unsigned long		_ulong;
	typedef signed long			_long;

	typedef float						_float;
	typedef double					_double;

	typedef bool							_bool;
	typedef wchar_t					_tchar;

	typedef XMFLOAT2_P				_float2;
	typedef XMFLOAT3_P				_float3;
	typedef XMFLOAT4_P				_float4;
	typedef XMINT3						_int3;
	typedef XMINT2						_int2;
	typedef XMUINT3					_uint3;
	typedef XMUINT2					_uint2;

	typedef XMVECTOR				_vector;
	// 1 ~ 3
	typedef FXMVECTOR				_fvector;
	// 4 th Up
	typedef GXMVECTOR				_gvector;
	// 5 ~ 6
	typedef HXMVECTOR			_hvector;
	// 7 th Up
	typedef CXMVECTOR				_cvector;

	typedef XMFLOAT4X4			_float4x4;
	typedef XMMATRIX				_matrix;
	typedef FXMMATRIX				_fmatrix;

	typedef wstring					_wstring;
	typedef string						_string;

	typedef vector<_wstring> PATHS;
	typedef vector<_string> EXPLAINS;

	typedef vector<_string> DEBUG_EXPLAINES;

	typedef vector<_float4x4> MATRIXES4X4;
	typedef vector<_matrix>     MATRIXES;

	typedef vector<class CTransform*> TRANSFORMVECTOR;
	typedef vector<_float4> F4POSVECTOR;

	typedef vector<void*> VOIDDATAS;
	typedef vector<class CCollider*> COLLIDERS;
	typedef vector<class CBonesCollider*> BONECOLLIDERS;

	//	template<class T>
	//	typedef list<T>::iterator 
}