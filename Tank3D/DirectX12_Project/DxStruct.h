#pragma once

typedef struct tagVertex
{
	tagVertex() = default;
	tagVertex(const _float& _x, const _float& _y, const _float& _z)
		: vPosition(_float3(_x, _y, _z))
	{}
	tagVertex(const _float3& _vPos) 
		: vPosition(_vPos)
	{}

	_float3 vPosition = _float3(0.f, 0.f, 0.f);
}VERTEX;

typedef struct tagIndexBuffDesc {

	tagIndexBuffDesc() = default;
	tagIndexBuffDesc(const _ushort& _a, const _ushort& _b, const _ushort& _c)
		: _1(_a), _2(_b), _3(_c){}

	_ushort _1{ 0 }, _2{ 0 }, _3{ 0 };
}INDEXBUFDESC;

typedef struct tagRgbInfo {
	// rgb
	_float r{ 0.f }, g{ 0.f }, b{ 0.f };
}RGBINFO;