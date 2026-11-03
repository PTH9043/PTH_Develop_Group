#pragma once 


typedef unsigned char		_ubyte;
typedef signed char			_byte;
typedef char							_char;

typedef unsigned short		_ushort;
typedef signed short			_short;

typedef unsigned int			_uint;
typedef signed int				_int;

typedef unsigned long		_ulong;
typedef signed long				_long;

typedef float							_float;
typedef double						_double;

typedef bool							_bool;
typedef wchar_t					_tchar;

typedef FLOAT2					_float2;
typedef FLOAT3					_float3;
typedef FLOAT4					_float4;
typedef XMINT3					_int3;
typedef XMINT2					_int2;
typedef XMUINT3					_uint3;
typedef XMUINT2					_uint2;

typedef string						_string;
typedef wstring					_wstring;

typedef XMVECTOR				_vector;
// 1 ~ 3
typedef FXMVECTOR				_fvector;
//4
typedef GXMVECTOR				_gvector;
// 5 ~ 6
typedef HXMVECTOR			_hvector;
// ³ª¸ÓÁö
typedef CXMVECTOR				_cvector;


typedef XMFLOAT4X4			_float4x4;
typedef XMMATRIX				_matrix;

typedef vector<_float4x4> MATRIXES4X4;
typedef vector<_matrix>     MATRIXES;

typedef vector<void*>		VOIDDATAS;