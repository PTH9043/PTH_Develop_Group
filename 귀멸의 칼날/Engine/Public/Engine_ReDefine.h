#pragma once

namespace Engine
{

#pragma region VE3

	typedef struct tagXMFloat4_P : public XMFLOAT4
	{
		tagXMFloat4_P()
		{
			x = 0.f;
			y = 0.f;
			z = 0.f;
		}

		tagXMFloat4_P(const XMFLOAT4& v3) {
			x = v3.x; y = v3.y; z = v3.z; w = v3.w;
		}

		tagXMFloat4_P operator =(const tagXMFloat4_P& v3) {
			x = v3.x; y = v3.y; z = v3.z; w = v3.w;
			return tagXMFloat4_P(x, y, z, w);
		}

		tagXMFloat4_P operator =(const XMFLOAT3& v3) {
			x = v3.x; y = v3.y; z = v3.z; ;
			return tagXMFloat4_P(x, y, z, w);
		}
		tagXMFloat4_P operator =(const XMFLOAT2& v3) {
			x = v3.x; y = v3.y;
			return tagXMFloat4_P(x, y, z, w);
		}

		tagXMFloat4_P operator +(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator -(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator *(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator /(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator +=(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator -=(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator *=(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator /=(const  tagXMFloat4_P& v3) {
			XMVECTOR _Value1 = XMLoadFloat4(&v3);
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value2 = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat4_P operator *(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator /(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator *=(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator /=(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x, y, z, w);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator +=(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x + F1, y + F1, z + F1, w + F1);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator +(const float& F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x + F1, y + F1, z + F1, w + F1);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value);
			return ReturnValue;
		}

		tagXMFloat4_P operator -=(float F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x - F1, y - F1, z - F1, w - F1);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat4_P operator -(float F1) {
			XMFLOAT4 VALUE3 = XMFLOAT4(x - F1, y - F1, z - F1, w - F1);
			XMVECTOR _Value = XMLoadFloat4(&VALUE3);
			tagXMFloat4_P ReturnValue;
			XMStoreFloat4(&ReturnValue, _Value);
			return ReturnValue;
		}

		tagXMFloat4_P operator()(const tagXMFloat4_P& Value)
		{
			this->x = Value.x;
			this->y = Value.y;
			this->z = Value.z;
			this->w = Value.w;
			return Value;
		}

		tagXMFloat4_P(const tagXMFloat4_P&) = default;

		tagXMFloat4_P(tagXMFloat4_P&&) = default;
		tagXMFloat4_P& operator=(tagXMFloat4_P&&) = default;


		tagXMFloat4_P(float _x, float _y, float _z, float _w)
		{
			x = _x; y = _y; z = _z; w = _w;
		}
		explicit tagXMFloat4_P(_In_reads_(4) const float* pArray)
		{
			x = pArray[0]; y = pArray[1]; z = pArray[2]; z = pArray[3];
		}

		tagXMFloat4_P Normalize()
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			XMStoreFloat4(&p, XMVector4Normalize(XMLoadFloat4(&p)));
			return p;
		}

		float Length()
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			return XMVectorGetX(XMVector4Length(XMLoadFloat4(&p)));
		}

		float Dot(const tagXMFloat4_P& _tData)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			return XMVectorGetX(XMVector4Dot(XMLoadFloat4(&p), XMLoadFloat4(&_tData)));
		}

		XMVECTOR Get_Vector()
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			return XMLoadFloat4(&p);
		}

		tagXMFloat4_P Transformation(XMMATRIX _matrix)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			tagXMFloat4_P value;
			XMStoreFloat4(&value, XMVector4Transform(XMLoadFloat4(&p), _matrix));
			return value;
		}

		tagXMFloat4_P Transformation4x4(const XMFLOAT4X4& _value) {
			return Transformation(XMLoadFloat4x4(&_value));
		}

		tagXMFloat4_P operator += (const XMFLOAT3& _V3)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			p.x += _V3.x;
			p.y += _V3.y;
			p.z += _V3.z;
			return p;
		}

		tagXMFloat4_P operator + (const XMFLOAT3& _V3)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			p.x += _V3.x;
			p.y += _V3.y;
			p.z += _V3.z;
			return p;
		}

		tagXMFloat4_P operator -= (const XMFLOAT3& _V3)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			p.x -= _V3.x;
			p.y -= _V3.y;
			p.z -= _V3.z;
			return p;
		}

		tagXMFloat4_P operator - (const XMFLOAT3& _V3)
		{
			tagXMFloat4_P p = tagXMFloat4_P(x, y, z, w);
			p.x -= _V3.x;
			p.y -= _V3.y;
			p.z -= _V3.z;
			return p;
		}

		// 플러스
		tagXMFloat4_P PVectorV4(XMVECTOR v)
		{
			tagXMFloat4_P p;
			XMStoreFloat4(&p, Get_Vector() + v);
			return p;
		}
		// 마이너스
		tagXMFloat4_P MVectorV4(XMVECTOR v)
		{
			tagXMFloat4_P p;
			XMStoreFloat4(&p, Get_Vector() - v);
			return p;
		}
		// 나누기 
		tagXMFloat4_P DVectorV4(XMVECTOR v)
		{
			tagXMFloat4_P p;
			XMStoreFloat4(&p, Get_Vector() / v);
			return p;
		}
		// 곱
		tagXMFloat4_P MMVectorV4(XMVECTOR v)
		{
			tagXMFloat4_P p;
			XMStoreFloat4(&p, Get_Vector() * v);
			return p;
		}
		// 플러스
		XMVECTOR PVector(const XMFLOAT4& v)
		{
			return Get_Vector() + XMLoadFloat4(&v);
		}
		// 마이너스
		XMVECTOR MVector(const XMFLOAT4& v)
		{
			return Get_Vector() - XMLoadFloat4(&v);
		}
		// 나누기
		XMVECTOR DVector(const XMFLOAT4& v)
		{
			return Get_Vector() * XMLoadFloat4(&v);
		}
		// 곱
		XMVECTOR MMVector(const XMFLOAT4& v)
		{
			return Get_Vector() * XMLoadFloat4(&v);
		}

		XMFLOAT3 Get_Float3() { return XMFLOAT3(x, y, z); }

		tagXMFloat4_P Set_Vector(const XMVECTOR& _Vector)
		{
			XMFLOAT4 v4;
			XMStoreFloat4(&v4, _Vector);
			x = v4.x;
			y = v4.y;
			z = v4.z;
			w = v4.w;
			return v4;
		}

		tagXMFloat4_P operator = (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x = v4.x;
			y = v4.y;
			z = v4.z;
			w = v4.w;
			return v4;
		}

		tagXMFloat4_P operator += (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x += v4.x;
			y += v4.y;
			z += v4.z;
			w += v4.w;
			return v4;
		}

		tagXMFloat4_P operator + (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x += v4.x;
			y += v4.y;
			z += v4.z;
			w += v4.w;
			return v4;
		}

		tagXMFloat4_P operator -= (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			z -= v4.z;
			w -= v4.w;
			return v4;
		}

		tagXMFloat4_P operator - (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			z -= v4.z;
			w -= v4.w;
			return v4;
		}

		tagXMFloat4_P operator /= (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			z /= v4.z;
			w /= v4.w;
			return v4;
		}

		tagXMFloat4_P operator / (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			z /= v4.z;
			w /= v4.w;
			return v4;
		}

		tagXMFloat4_P operator *= (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			z *= v4.z;
			w *= v4.w;
			return v4;
		}

		tagXMFloat4_P operator * (const XMVECTOR& _vector)
		{
			tagXMFloat4_P v4;
			XMStoreFloat4(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			z *= v4.z;
			w *= v4.w;
			return v4;
		}

	}XMFLOAT4_P;

	typedef struct tagXMFloat3_P : public XMFLOAT3
	{
		tagXMFloat3_P()
		{
			x = 0.f;
			y = 0.f;
			z = 0.f;
		}

		tagXMFloat3_P(const XMFLOAT3& v3) {
			x = v3.x; y = v3.y; z = v3.z;
		}

		tagXMFloat3_P operator =(const tagXMFloat3_P& v3) {
			x = v3.x; y = v3.y; z = v3.z;;
			return tagXMFloat3_P(x, y, z);
		}

		tagXMFloat3_P operator +(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator -(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator *(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator /(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator +=(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator -=(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator *=(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator /=(const XMFLOAT3& v3) {
			XMVECTOR _Value1 = XMLoadFloat3(&v3);
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value2 = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat3_P operator *(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator /(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator *=(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x, y, z);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator +=(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x + F1, y + F1, z + F1);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator +(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x + F1, y + F1, z + F1);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator -=(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x - F1, y - F1, z - F1);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator -(const float& F1) {
			XMFLOAT3 VALUE3 = XMFLOAT3(x - F1, y - F1, z - F1);
			XMVECTOR _Value = XMLoadFloat3(&VALUE3);
			tagXMFloat3_P ReturnValue;
			XMStoreFloat3(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat3_P operator()(const tagXMFloat3_P& Value)
		{
			this->x = Value.x;
			this->y = Value.y;
			this->z = Value.z;
			return Value;
		}

		tagXMFloat3_P(const tagXMFloat3_P&) = default;

		tagXMFloat3_P(tagXMFloat3_P&&) = default;
		tagXMFloat3_P& operator=(tagXMFloat3_P&&) = default;
		tagXMFloat3_P(const XMVECTOR& _vValue)
		{
			tagXMFloat3_P vValue;
			XMStoreFloat3(&vValue, _vValue);
			x = vValue.x;
			y = vValue.y;
			z = vValue.z;
		}


		tagXMFloat3_P(float _x, float _y, float _z)
		{
			x = _x; y = _y; z = _z;
		}
		explicit tagXMFloat3_P(_In_reads_(3) const float* pArray)
		{
			x = pArray[0]; y = pArray[1]; z = pArray[2];
		}

		tagXMFloat3_P Normalize() {
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			XMStoreFloat3(&p, XMVector3Normalize(XMLoadFloat3(&p)));
			return p;
		}

		float Length() {
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p)));
		}

		float Dot(const tagXMFloat3_P& _tData) {
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			return XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&p)), XMLoadFloat3(&_tData)));
		}

		float Dot(XMVECTOR vValue) {
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			return XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&p)), vValue));
		}


		XMVECTOR Get_Vector()
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			return XMLoadFloat3(&p);
		}

		tagXMFloat3_P Coord(XMMATRIX Value)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			tagXMFloat3_P value;
			XMStoreFloat3(&value, XMVector3TransformCoord(XMLoadFloat3(&p), Value));
			return value;
		}

		tagXMFloat3_P Coord4x4(XMFLOAT4X4 _value) {
			XMMATRIX Value = XMLoadFloat4x4(&_value);
			return Coord(Value);
		}

		tagXMFloat3_P Normal(XMMATRIX Value)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			tagXMFloat3_P value;
			XMStoreFloat3(&value, XMVector3TransformNormal(XMLoadFloat3(&p), Value));
			return value;
		}

		tagXMFloat3_P Normal4x4(XMFLOAT4X4 _value) {
			return Normal(XMLoadFloat4x4(&_value));
		}

		tagXMFloat3_P CatCutmullRom(tagXMFloat3_P _v1, tagXMFloat3_P _v2, tagXMFloat3_P _v3, tagXMFloat3_P _v4, const float& _fWeight)
		{
			tagXMFloat3_P v3Pos;
			v3Pos.Set_Vector(XMVectorCatmullRom(_v1.Get_Vector(), _v2.Get_Vector(), _v3.Get_Vector(), _v4.Get_Vector(), _fWeight));
			return v3Pos;
		}

		tagXMFloat3_P operator += (const XMFLOAT4& _V3)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			p.x += _V3.x;
			p.y += _V3.y;
			p.z += _V3.z;
			return p;
		}

		tagXMFloat3_P operator + (const XMFLOAT4& _V3)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			p.x += _V3.x;
			p.y += _V3.y;
			p.z += _V3.z;
			return p;
		}

		tagXMFloat3_P operator - (const XMFLOAT4& _V3)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			p.x -= _V3.x;
			p.y -= _V3.y;
			p.z -= _V3.z;
			return p;
		}

		tagXMFloat3_P operator -= (const XMFLOAT4& _V3)
		{
			tagXMFloat3_P p = tagXMFloat3_P(x, y, z);
			p.x -= _V3.x;
			p.y -= _V3.y;
			p.z -= _V3.z;
			return p;
		}

		tagXMFloat3_P PVectorV3(XMVECTOR v)
		{
			tagXMFloat3_P p;
			XMStoreFloat3(&p, Get_Vector() + v);
			return p;
		}

		tagXMFloat3_P MVectorV3(XMVECTOR v)
		{
			tagXMFloat3_P p;
			XMStoreFloat3(&p, Get_Vector() - v);
			return p;
		}

		tagXMFloat3_P DVectorV3(XMVECTOR v)
		{
			tagXMFloat3_P p;
			XMStoreFloat3(&p, Get_Vector() / v);
			return p;
		}

		tagXMFloat3_P MMVectorV3(XMVECTOR v)
		{
			tagXMFloat3_P p;
			XMStoreFloat3(&p, Get_Vector() * v);
			return p;
		}

		XMVECTOR PVector(const XMFLOAT3& v)
		{

			return Get_Vector() + XMLoadFloat3(&v);
		}

		XMVECTOR MVector(const XMFLOAT3& v)
		{
			return Get_Vector() - XMLoadFloat3(&v);
		}

		XMVECTOR DVector(const XMFLOAT3& v)
		{
			return Get_Vector() * XMLoadFloat3(&v);
		}

		XMVECTOR MMVector(const XMFLOAT3& v)
		{
			return Get_Vector() * XMLoadFloat3(&v);
		}

		XMFLOAT4_P Convert_PosF4() { return XMFLOAT4_P(x, y, z, 1.f); }
		XMFLOAT4_P Convert_DirF4() { return XMFLOAT4_P(x, y, z, 0.f); }

		tagXMFloat3_P Set_Vector(const XMVECTOR& _Vector)
		{
			XMFLOAT3 v4;
			XMStoreFloat3(&v4, _Vector);
			x = v4.x;
			y = v4.y;
			z = v4.z;
			return v4;
		}

		tagXMFloat3_P operator = (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x = v4.x;
			y = v4.y;
			z = v4.z;
			return v4;
		}

		tagXMFloat3_P operator += (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x += v4.x;
			y += v4.y;
			z += v4.z;
			return v4;
		}

		tagXMFloat3_P operator + (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x += v4.x;
			y += v4.y;
			z += v4.z;
			return v4;
		}

		tagXMFloat3_P operator -= (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			z -= v4.z;
			return v4;
		}

		tagXMFloat3_P operator - (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			z -= v4.z;
			return v4;
		}

		tagXMFloat3_P operator /= (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			z /= v4.z;
			return v4;
		}

		tagXMFloat3_P operator / (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			z /= v4.z;
			return v4;
		}

		tagXMFloat3_P operator *= (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			z *= v4.z;
			return v4;
		}

		tagXMFloat3_P operator * (const XMVECTOR& _vector)
		{
			tagXMFloat3_P v4;
			XMStoreFloat3(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			z *= v4.z;
			return v4;
		}

		tagXMFloat3_P operator =(const XMFLOAT4& v3) {
			x = v3.x; y = v3.y; z = v3.z;;
			return tagXMFloat3_P(x, y, z);
		}

		tagXMFloat3_P operator =(const XMFLOAT2& v3) {
			x = v3.x; y = v3.y;;
			return tagXMFloat3_P(x, y, z);
		}

	}XMFLOAT3_P;

	typedef struct tagXMFloat2_P : public XMFLOAT2
	{
		tagXMFloat2_P()
		{
			x = 0.f;
			y = 0.f;
		}

		tagXMFloat2_P(const XMFLOAT2& v2) {
			x = v2.x; y = v2.y;
		}

		tagXMFloat2_P operator =(const tagXMFloat2_P& v3) {
			x = v3.x; y = v3.y;
			return tagXMFloat2_P(x, y);
		}

		tagXMFloat2_P operator =(const XMFLOAT4& v3) {
			x = v3.x; y = v3.y;
			return tagXMFloat2_P(x, y);
		}

		tagXMFloat2_P operator =(const XMFLOAT3& v3) {
			x = v3.x; y = v3.y;
			return tagXMFloat2_P(x, y);
		}

		tagXMFloat2_P operator +(const XMFLOAT2& v2) {
			XMVECTOR _Value1 = XMLoadFloat2(&v2);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator -(const XMFLOAT2& v2) {
			XMVECTOR _Value1 = XMLoadFloat2(&v2);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator *(const XMFLOAT2& v2) {
			XMVECTOR _Value1 = XMLoadFloat2(&v2);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator /(const XMFLOAT2& v3) {
			XMVECTOR _Value1 = XMLoadFloat2(&v3);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator +=(const XMFLOAT2& v3) {
			XMVECTOR _Value1 = XMLoadFloat2(&v3);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 + _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator -=(const XMFLOAT2& v3) {
			XMVECTOR _Value1 = XMLoadFloat2(&v3);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 - _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator *=(const XMFLOAT2& v2) {
			XMVECTOR _Value1 = XMLoadFloat2(&v2);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 * _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator /=(const XMFLOAT2& v2) {
			XMVECTOR _Value1 = XMLoadFloat2(&v2);
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value2 = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value1 / _Value2);
			return ReturnValue;
		}

		tagXMFloat2_P operator *(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator /(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator *=(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator /=(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x, y);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator +(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x + F1, y + F1);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value);
			return ReturnValue;
		}

		tagXMFloat2_P operator +=(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x + F1, y + F1);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator -(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x - F1, y - F1);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value * F1);
			return ReturnValue;
		}

		tagXMFloat2_P operator -=(const float& F1) {
			XMFLOAT2 VALUE2 = XMFLOAT2(x - F1, y - F1);
			XMVECTOR _Value = XMLoadFloat2(&VALUE2);
			tagXMFloat2_P ReturnValue;
			XMStoreFloat2(&ReturnValue, _Value / F1);
			return ReturnValue;
		}

		tagXMFloat2_P(const tagXMFloat2_P&) = default;

		tagXMFloat2_P(tagXMFloat2_P&&) = default;
		tagXMFloat2_P& operator=(tagXMFloat2_P&&) = default;

		tagXMFloat2_P(float _x, float _y)
		{
			x = _x; y = _y;
		}
		tagXMFloat2_P(_In_reads_(2) const float* pArray)
		{
			x = pArray[0];
			y = pArray[1];
		}

		tagXMFloat2_P Normalize()
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			XMStoreFloat2(&p, XMVector2Normalize(XMLoadFloat2(&p)));
			return p;
		}

		float Length()
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			return XMVectorGetX(XMVector2Length(XMLoadFloat2(&p)));
		}

		float Dot(const tagXMFloat2_P& _tData)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			return XMVectorGetX(XMVector2Dot(XMLoadFloat2(&p), XMLoadFloat2(&_tData)));
		}

		XMVECTOR Get_Vector()
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			return XMLoadFloat2(&p);
		}

		tagXMFloat2_P Coord(XMMATRIX Value)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			tagXMFloat2_P value;
			XMStoreFloat2(&value, XMVector2TransformCoord(XMLoadFloat2(&p), Value));
			return value;
		}

		tagXMFloat2_P Coord4x4(XMFLOAT4X4 _value) {
			XMMATRIX Value = XMLoadFloat4x4(&_value);
			return Coord(Value);
		}

		tagXMFloat2_P Normal(XMMATRIX Value)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			tagXMFloat2_P value;
			XMStoreFloat2(&value, XMVector2TransformNormal(XMLoadFloat2(&p), Value));
			return value;
		}

		tagXMFloat2_P Normal4x4(XMFLOAT4X4 _value) {
			return Normal(XMLoadFloat4x4(&_value));
		}

		tagXMFloat2_P operator += (const XMFLOAT4& _V3)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			p.x += _V3.x;
			p.y += _V3.y;
			return p;
		}

		tagXMFloat2_P operator + (const XMFLOAT4& _V3)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			p.x += _V3.x;
			p.y += _V3.y;
			return p;
		}

		tagXMFloat2_P operator -= (const XMFLOAT4& _V3)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			p.x -= _V3.x;
			p.y -= _V3.y;
			return p;
		}

		tagXMFloat2_P operator -(const XMFLOAT4& _V3)
		{
			tagXMFloat2_P p = tagXMFloat2_P(x, y);
			p.x -= _V3.x;
			p.y -= _V3.y;
			return p;
		}

		tagXMFloat2_P operator += (XMVECTOR v)
		{
			tagXMFloat2_P p;
			XMStoreFloat2(&p, Get_Vector() + v);
			return p;
		}
		// 플러스
		tagXMFloat2_P PVectorV2(XMVECTOR v)
		{
			tagXMFloat2_P p;
			XMStoreFloat2(&p, Get_Vector() + v);
			return p;
		}
		// 마이너스
		tagXMFloat2_P MVectorV2(XMVECTOR v)
		{
			tagXMFloat2_P p;
			XMStoreFloat2(&p, Get_Vector() - v);
			return p;
		}
		// 나누기 
		tagXMFloat2_P DVectorV2(XMVECTOR v)
		{
			tagXMFloat2_P p;
			XMStoreFloat2(&p, Get_Vector() / v);
			return p;
		}
		// 곱
		tagXMFloat2_P MMVectorV2(XMVECTOR v)
		{
			tagXMFloat2_P p;
			XMStoreFloat2(&p, Get_Vector() * v);
			return p;
		}
		// 플러스
		XMVECTOR PVector(const XMFLOAT2& v)
		{
			;
			return Get_Vector() + XMLoadFloat2(&v);
		}
		// 마이너스
		XMVECTOR MVector(const XMFLOAT2& v)
		{
			return Get_Vector() - XMLoadFloat2(&v);
		}
		// 나누기
		XMVECTOR DVector(const XMFLOAT2& v)
		{
			return Get_Vector() * XMLoadFloat2(&v);
		}
		// 곱
		XMVECTOR MMVector(const XMFLOAT2& v)
		{
			return Get_Vector() * XMLoadFloat2(&v);
		}

		tagXMFloat2_P Set_Vector(const XMVECTOR& _Vector)
		{
			XMFLOAT2 v4;
			XMStoreFloat2(&v4, _Vector);
			x = v4.x;
			y = v4.y;
			return v4;
		}

		tagXMFloat2_P operator = (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x = v4.x;
			y = v4.y;
			return v4;
		}

		tagXMFloat2_P operator += (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x += v4.x;
			y += v4.y;
			return v4;
		}

		tagXMFloat2_P operator + (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x += v4.x;
			y += v4.y;
			return v4;
		}

		tagXMFloat2_P operator -= (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			return v4;
		}

		tagXMFloat2_P operator - (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x -= v4.x;
			y -= v4.y;
			return v4;
		}

		tagXMFloat2_P operator /= (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			return v4;
		}

		tagXMFloat2_P operator / (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x /= v4.x;
			y /= v4.y;
			return v4;
		}

		tagXMFloat2_P operator *= (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			return v4;
		}

		tagXMFloat2_P operator * (const XMVECTOR& _vector)
		{
			tagXMFloat2_P v4;
			XMStoreFloat2(&v4, _vector);
			x *= v4.x;
			y *= v4.y;
			return v4;
		}

	}XMFLOAT2_P;

#pragma endregion VE3
}