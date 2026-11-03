#pragma once 

#pragma region VE3

	struct tagFloat2;
	struct tagFloat3;
	struct tagFloat4;
	struct QUATERNION;

	typedef enum EMatrixRow : unsigned int
	{
		MATROW_RIGHT, MATROW_UP, MATROW_LOOK, MATROW_POS, MATROW_END
	}MATRIXROW;


	typedef struct tagFloat4 : public XMFLOAT4
	{
		tagFloat4() noexcept { XMStoreFloat4(this, XMVectorZero()); }
		tagFloat4(const XMFLOAT4& v4) noexcept { XMStoreFloat4(this, XMLoadFloat4(&v4)); }

		tagFloat4(const XMVECTOR& v4) noexcept { XMStoreFloat4(this, v4); }

		tagFloat4(const tagFloat4&) = default;

		tagFloat4(tagFloat4&&) = default;
		tagFloat4& operator=(tagFloat4&&) = default;


		tagFloat4(const float& _x, const float& _y, const float& _z, const float& _w) {
			XMFLOAT4 v = XMFLOAT4(_x, _y, _z, _w);
			XMStoreFloat4(this, XMLoadFloat4(&v));
		}
		explicit tagFloat4(_In_reads_(4) const float* pArray) {
			x = pArray[0]; y = pArray[1]; z = pArray[2]; w = pArray[3];
		}
#pragma region OPERATOR
		tagFloat4& operator =(const tagFloat4& v) noexcept { XMStoreFloat4(this, XMLoadFloat4(&v));	return *this; }
		tagFloat4& operator =(const XMFLOAT3& v) noexcept { XMStoreFloat4(this, XMLoadFloat3(&v));	return *this; }
		tagFloat4& operator =(const XMFLOAT2& v) noexcept { XMStoreFloat4(this, XMLoadFloat2(&v));	return *this; }

		const bool operator == (const tagFloat4& v) { return XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&v)); }
		const bool operator != (const tagFloat4& v) { return XMVector4NotEqual(XMLoadFloat4(this), XMLoadFloat4(&v)); }

		tagFloat4& operator +(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMVectorAdd(XMLoadFloat4(this), XMLoadFloat4(&v))); return *this;
		}

		tagFloat4& operator -(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&v))); return *this;
			return *this;
		}

		tagFloat4& operator *(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMVectorMultiply(XMLoadFloat4(this), XMLoadFloat4(&v))); return *this;
			return *this;
		}

		tagFloat4& operator /(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) / XMLoadFloat4(&v));
			return *this;
		}

		tagFloat4& operator += (const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) + XMLoadFloat4(&v));
			return *this;
		}

		tagFloat4& operator  -=(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) - XMLoadFloat4(&v));
			return *this;
		}

		tagFloat4& operator *=(const  tagFloat4& v)noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) * XMLoadFloat4(&v));
			return *this;
		}

		tagFloat4& operator /=(const  tagFloat4& v) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) / XMLoadFloat4(&v));
			return *this;
		}

		tagFloat4& operator *(const float& F1) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) * F1);
			return *this;
		}

		tagFloat4& operator /(const float& F1) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) / F1);
			return *this;
		}

		tagFloat4& operator *=(const float& F1) noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) * F1);
			return *this;
		}

		tagFloat4& operator /=(const float& F1)  noexcept {
			XMStoreFloat4(this, XMLoadFloat4(this) / F1);
			return *this;
		}

		tagFloat4& operator()(const tagFloat4& Value) noexcept {
			*this = Value;
			return *this;
		}

		// Unary operators
		tagFloat4 operator+ () const noexcept { return *this; }
		tagFloat4 operator- () const noexcept {
			tagFloat4 R;
			XMStoreFloat4(&R, XMVectorNegate(XMLoadFloat4(this)));
			return R;
		}

#pragma endregion OPERATOR
#pragma region ECT
		const XMFLOAT3  Get_Float3() noexcept { return XMFLOAT3(x, y, z); }

		XMVECTOR Get_Vector() const { return XMLoadFloat4(this); }
		void Set_Vector(const XMVECTOR& _v) { XMStoreFloat4(this, _v); }

		// Vector operations
		bool InBounds(const tagFloat4& _Bounds) const noexcept {
			return XMVector4InBounds(XMLoadFloat4(this), XMLoadFloat4(&_Bounds));
		}
		bool InBounds(const FXMVECTOR& _Bounds) const noexcept {
			return XMVector4InBounds(XMLoadFloat4(this), _Bounds);
		}

		float Length() const noexcept {
			return tagFloat4::Length(*this);
		}
		float LengthSquared() const noexcept {
			return tagFloat4::LengthSquared(*this);
		}

		static float Length(const tagFloat4& _v) {
			tagFloat4 vValue;
			XMStoreFloat4(&vValue, XMVector4Length(XMLoadFloat4(&_v)));
			return vValue.x;
		}
		static float Length(const FXMVECTOR& _v) {
			tagFloat4 vValue;
			XMStoreFloat4(&vValue, XMVector4Length(_v));
			return vValue.x;
		}

		static float LengthSquared(const tagFloat4& _v) {
			tagFloat4 vValue;
			XMStoreFloat4(&vValue, XMVector4LengthSq(XMLoadFloat4(&_v)));
			return vValue.x;
		}
		static float LengthSquared(const FXMVECTOR& _v) {
			tagFloat4 vValue;
			XMStoreFloat4(&vValue, XMVector4LengthSq(_v));
			return vValue.x;
		}

		float Dot(const tagFloat4& _v) const noexcept {
			tagFloat4 vValue{};
			XMStoreFloat4(&vValue, XMVector4Dot(XMLoadFloat4(this), XMLoadFloat4(&_v)));
			return vValue.x;
		}
		float Dot(const FXMVECTOR& _v) {
			tagFloat4 vValue{};
			XMStoreFloat4(&vValue, XMVector4Dot(XMLoadFloat4(this), _v));
			return vValue.x;
		}

		static float Dot(const tagFloat4& _v1, const tagFloat4& _v2) {
			tagFloat4 vValue{};
			XMStoreFloat4(&vValue, XMVector4Dot(XMLoadFloat4(&_v1), XMLoadFloat4(&_v2)));
			return vValue.x;
		}

		static float Dot(const FXMVECTOR& _v1, const FXMVECTOR& _v2) {
			tagFloat4 vValue{};
			XMStoreFloat4(&vValue, XMVector4Dot(_v1, _v2));
			return vValue.x;
		}

#pragma endregion ECT
#pragma region CROSS
		void CrossSelf(const tagFloat4& v2, const tagFloat4& v3) {
			tagFloat4 value = Cross(*this, v2, v3);
			XMStoreFloat4(this, XMLoadFloat4(&value));
		}
		static void Cross(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3, tagFloat4& result)  noexcept {
			result = Cross(v1, v2, v3);
		}
		static tagFloat4 Cross(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3) noexcept {
			tagFloat4 result;
			XMStoreFloat4(&result, XMVector4Cross(XMLoadFloat4(&v1), XMLoadFloat4(&v2), XMLoadFloat4(&v3)));
			return result;
		}
		void CrossVecSelf(const FXMVECTOR& v2, const FXMVECTOR& v3) {
			FXMVECTOR value = Cross(XMLoadFloat4(this), v2, v3);
			XMStoreFloat4(this, value);
		}
		static void Cross(const FXMVECTOR& v1, const FXMVECTOR& v2, const XMVECTOR& v3, XMVECTOR& result)  noexcept {
			result = Cross(v1, v2, v3);
		}
		static FXMVECTOR Cross(const FXMVECTOR& v1, const FXMVECTOR& v2, const XMVECTOR& v3)  noexcept {
			XMVECTOR result;
			result = XMVector4Cross(v1, v2, v3);
			return result;
		}
#pragma endregion CROSS
#pragma region NORMALIZE
		void NormalizeSelf() noexcept {
			tagFloat4 value;
			Normalize(value);
			XMStoreFloat4(this, XMLoadFloat4(&value));
		}
		tagFloat4 Normalize() {
			tagFloat4 value;
			XMStoreFloat4(this, XMLoadFloat4(&value));
			return value;
		}
		void Normalize(tagFloat4& result) const noexcept {
			XMStoreFloat4(&result, XMVector4Normalize(XMLoadFloat4(this)));
		}
		XMVECTOR NormalizeVec() {
			XMVECTOR value;
			Normalize(value);
			return value;
		}
		void Normalize(XMVECTOR& result) const noexcept {
			result = XMVector4Normalize(XMLoadFloat4(this));
		}

		static void NormalizeValue(const tagFloat4& _v1, tagFloat4& _result) {
			XMStoreFloat4(&_result, XMVector4Normalize(XMLoadFloat4(&_v1)));
		}
		static tagFloat4 NormalizeValue(const tagFloat4& _v1) {
			tagFloat4 value;
			NormalizeValue(_v1, value);
			return value;
		}
#pragma endregion NORMALIZE
#pragma region CLAMP
		void Clamp(const tagFloat4& vmin, const tagFloat4& vmax) noexcept {
			tagFloat4 value;
			Clamp(vmin, vmax, value);
			XMStoreFloat4(this, XMLoadFloat4(&value));
		}
		void Clamp(const tagFloat4& vmin, const tagFloat4& vmax, tagFloat4& result) const noexcept {
			XMStoreFloat4(&result, XMVectorClamp(XMLoadFloat4(this), XMLoadFloat4(&vmin), XMLoadFloat4(&vmax)));
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax) noexcept {
			XMVECTOR value;
			Clamp(vmin, vmax, value);
			XMStoreFloat4(this, value);
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result) const noexcept {
			result = XMVectorClamp(XMLoadFloat4(this), vmin, vmax);
		}
		static void Clamp(const tagFloat4& _value, const tagFloat4& vmin, const tagFloat4& vmax, tagFloat4& result)  noexcept {
			XMStoreFloat4(&result, XMVectorClamp(XMLoadFloat4(&_value), XMLoadFloat4(&vmin), XMLoadFloat4(&vmax)));
		}
		static tagFloat4 Clamp(const tagFloat4& _value, const tagFloat4& vmin, const tagFloat4& vmax) {
			tagFloat4 value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}
		static void Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result)  noexcept {
			result = XMVectorClamp(_value, vmin, vmax);
		}
		static XMVECTOR Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax) {
			XMVECTOR value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}
#pragma endregion CLAMP
#pragma region DISTANCE
		// Static functions
		static float Distance(const tagFloat4& v1, const tagFloat4& v2) noexcept {
			return XMVectorGetX(XMVector4Length(XMVectorSubtract(XMLoadFloat4(&v1), XMLoadFloat4(&v2))));
		}
		static float DistanceSquared(const tagFloat4& v1, const tagFloat4& v2) noexcept {
			return XMVectorGetX(XMVector4LengthSq(XMVectorSubtract(XMLoadFloat4(&v1), XMLoadFloat4(&v2))));
		}
		float Distance(const tagFloat4& v) noexcept {
			return XMVectorGetX(XMVector4Length(XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&v))));
		}
		float DistanceSquared(const tagFloat4& v) noexcept {
			return XMVectorGetX(XMVector4LengthSq(XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&v))));
		}

		static float Distance(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector4Length(XMVectorSubtract(v1, v2)));
		}
		static float DistanceSquared(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector4LengthSq(XMVectorSubtract(v1, v2)));
		}
		float Distance(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector4Length(XMVectorSubtract(XMLoadFloat4(this), v)));
		}
		float DistanceSquared(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector4LengthSq(XMVectorSubtract(XMLoadFloat4(this), v)));
		}
#pragma endregion DISTANCE
#pragma region MIN
		static void Min(const tagFloat4& v1, const tagFloat4& v2, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVectorMin(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
		}
		static tagFloat4 Min(const tagFloat4& v1, const tagFloat4& v2) noexcept {
			tagFloat4 result;
			Min(v1, v2, result);
			return result;
		}
#pragma endregion MIN
#pragma region Max
		static void Max(const tagFloat4& v1, const tagFloat4& v2, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVectorMax(XMLoadFloat4(&v1), XMLoadFloat4(&v2)));
		}
		static tagFloat4 Max(const tagFloat4& v1, const tagFloat4& v2) noexcept {
			tagFloat4 result;
			Max(v1, v2, result);
			return result;
		}
#pragma endregion Max
#pragma region LERP
		static void Lerp(const tagFloat4& v1, const tagFloat4& v2, const float& t, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVectorLerp(XMLoadFloat4(&v1), XMLoadFloat4(&v2), t));
		}
		static tagFloat4 Lerp(const tagFloat4& v1, const tagFloat4& v2, const float& t) noexcept {
			tagFloat4 value;
			Lerp(v1, v2, t, value);
			return value;
		}
#pragma endregion LERP
#pragma region SMOOTHSTEP
		static void SmoothStep(const tagFloat4& v1, const tagFloat4& v2, float t, tagFloat4& result) noexcept {
			t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
			t = t * t * (3.f - 2.f * t);
			XMStoreFloat4(&result, XMVectorLerp(XMLoadFloat4(&v1), XMLoadFloat4(&v2), t));
		}
		static tagFloat4 SmoothStep(const tagFloat4& v1, const tagFloat4& v2, float t) noexcept {
			tagFloat4 value;
			SmoothStep(v1, v2, t, value);
			return value;
		}
#pragma endregion SMOOTHSTEP
#pragma region BARYCENTRIC
		static void Barycentric(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3, const float& f, const float& g, tagFloat4& result) noexcept {
			const XMVECTOR X = XMVectorBaryCentric(XMLoadFloat4(&v1), XMLoadFloat4(&v2), XMLoadFloat4(&v3), f, g);
			XMStoreFloat4(&result, X);
		}
		static tagFloat4 Barycentric(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3, const float& f, const float& g) noexcept {
			tagFloat4 value;
			Barycentric(v1, v2, v3, f, g, value);
			return value;
		}
#pragma endregion BARYCENTRIC
#pragma region CATMULROM
		static void CatmullRom(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3, const tagFloat4& v4, const float& t, tagFloat4& result) noexcept {
			const XMVECTOR X = XMVectorCatmullRom(XMLoadFloat4(&v1), XMLoadFloat4(&v2), XMLoadFloat4(&v3), XMLoadFloat4(&v4), t);
			XMStoreFloat4(&result, X);
		}
		static tagFloat4 CatmullRom(const tagFloat4& v1, const tagFloat4& v2, const tagFloat4& v3, const tagFloat4& v4, const float& t) noexcept {
			tagFloat4 value;
			CatmullRom(v1, v2, v3, v4, t, value);
			return value;
		}
#pragma endregion CATMULROM
#pragma region HERMITE
		static void Hermite(const tagFloat4& v1, const tagFloat4& t1, const tagFloat4& v2, const tagFloat4& t2, const float& t, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVectorHermite(XMLoadFloat4(&v1), XMLoadFloat4(&t1), XMLoadFloat4(&v2), XMLoadFloat4(&t2), t));
		}
		static tagFloat4 Hermite(const tagFloat4& v1, const tagFloat4& t1, const tagFloat4& v2, const tagFloat4& t2, const float& t) noexcept {
			tagFloat4 value;
			Hermite(v1, v2, t1, t2, t, value);
			return value;
		}
#pragma endregion HERMITE
#pragma region REFLECT
		static void Reflect(const tagFloat4& ivec, const tagFloat4& nvec, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVector4Reflect(XMLoadFloat4(&ivec), XMLoadFloat4(&nvec)));
		}
		static tagFloat4 Reflect(const tagFloat4& ivec, const tagFloat4& nvec) noexcept {
			tagFloat4 value;
			Reflect(ivec, nvec, value);
			return value;
		}
		static void Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec, XMVECTOR& result) noexcept {
			result = XMVector4Reflect(ivec, nvec);
		}
		static XMVECTOR Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec) noexcept {
			XMVECTOR value;
			Reflect(ivec, nvec, value);
			return value;
		}
#pragma endregion REFLECT
#pragma region REFRACT
		static void Refract(const tagFloat4& ivec, const tagFloat4& nvec, const float& refractionIndex, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVector4Refract(XMLoadFloat4(&ivec), XMLoadFloat4(&nvec), refractionIndex));
		}
		static tagFloat4 Refract(const tagFloat4& ivec, const tagFloat4& nvec, const float& refractionIndex) noexcept {
			tagFloat4 value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
		static void Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex, XMVECTOR& result) noexcept {
			result = XMVector4Refract(ivec, nvec, refractionIndex);
		}
		static XMVECTOR Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex) noexcept {
			XMVECTOR value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
#pragma endregion REFRACT
#pragma region TRANSFORM_FLOAT4
		static void Transform(const XMFLOAT2& v, const XMFLOAT4& quat, tagFloat4& result) noexcept {
			XMStoreFloat4(&result, XMVectorSelect(g_XMIdentityR3, XMVector3Rotate(XMLoadFloat2(&v), XMLoadFloat4(&quat)), g_XMSelect1110));
		}
		static tagFloat4 Transform(const XMFLOAT2& v, const XMFLOAT4& quat) noexcept {
			tagFloat4 value;
			Transform(v, quat, value);
			return value;
		}

		static void Transform(const XMFLOAT3& v, const XMFLOAT4& quat, tagFloat4& result) noexcept {
			XMVECTOR X = XMVector3Rotate(XMLoadFloat3(&v), XMLoadFloat4(&quat));
			X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110); // result.w = 1.f
			XMStoreFloat4(&result, X);
		}
		static tagFloat4 Transform(const XMFLOAT3& v, const XMFLOAT4& quat) noexcept {
			tagFloat4 value;
			Transform(v, quat, value);
			return value;
		}

		static void Transform(const tagFloat4 v, const XMFLOAT4& quat, tagFloat4& result) noexcept {
			const XMVECTOR v1 = XMLoadFloat4(&v);
			const XMVECTOR q = XMLoadFloat4(&quat);
			XMVECTOR X = XMVector3Rotate(v1, q);
			X = XMVectorSelect(v1, X, g_XMSelect1110); // result.w = v.w
			XMStoreFloat4(&result, X);
		}
		static tagFloat4 Transform(const tagFloat4& v, const XMFLOAT4& quat) noexcept {
			tagFloat4 value;
			Transform(v, quat, value);
			return value;
		}
#pragma endregion TRANSFORM_FLOAT4
#pragma region TRANSFORM_MATRIX

		static void Transform(const tagFloat4& v, const XMFLOAT4X4& m, tagFloat4& result) noexcept {
			const XMVECTOR v1 = XMLoadFloat4(&v);
			const XMMATRIX M = XMLoadFloat4x4(&m);
			const XMVECTOR X = XMVector4Transform(v1, M);
			XMStoreFloat4(&result, X);
		}
		static tagFloat4 Transform(const tagFloat4& v, const XMFLOAT4X4& m) noexcept {
			tagFloat4 value;
			Transform(v, m, value);
			return value;
		}
		static void Transform(const FXMVECTOR& v, const XMMATRIX& m, XMVECTOR& result) noexcept {
			const XMVECTOR v1 = v;
			const XMMATRIX M = m;
			result = XMVector4Transform(v1, M);
		}
		static XMVECTOR Transform(const FXMVECTOR& v, const XMMATRIX& m) noexcept {
			XMVECTOR value;
			Transform(v, m, value);
			return value;
		}
		static void Transform(_In_reads_(count) const tagFloat4* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat4* resultArray) noexcept {
			const XMMATRIX M = XMLoadFloat4x4(&m);
			XMVector4TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT4), count, M);
		}
#pragma endregion TRNASFORM_MATRIX
#pragma region SPLINECURVE
		static void SplineCurve(const tagFloat4& _prevPos, const tagFloat4& _vStandrardPos, const tagFloat4& _vNextPos1,
			const tagFloat4& _vNextPos2, const float& _fValue, tagFloat4& _result)
		{
			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat4(&_prevPos);
			XMVECTOR y1 = XMLoadFloat4(&_vStandrardPos);
			XMVECTOR y2 = XMLoadFloat4(&_vNextPos1);
			XMVECTOR y3 = XMLoadFloat4(&_vNextPos2);

			XMVECTOR A0 = y3 - y2 - y1 + y1;
			XMVECTOR A1 = y0 - y1 - A0;
			XMVECTOR A2 = y2 - y0;
			XMVECTOR A3 = y1;

			float m2 = fValue * fValue;

			_result = (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
		}

		static tagFloat4 SplineCurve(const tagFloat4& _prevPos, const tagFloat4& _vStandrardPos, const tagFloat4& _vNextPos1,
			const tagFloat4& _vNextPos2, const float& _fValue)
		{
			tagFloat4 value;
			SplineCurve(_prevPos, _vStandrardPos, _vNextPos1, _vNextPos2, _fValue, value);
			return value;
		}
#pragma endregion SPLINECURVE
#pragma region BEZIER
		static void BezierCurve(const tagFloat4& _vStandard1, const tagFloat4& _vControl1, const tagFloat4& _vStandard2,
			const tagFloat4& _vControl2, const float& _fValue, tagFloat4& result) {

			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat4(&_vStandard1);
			XMVECTOR y1 = XMLoadFloat4(&_vControl1);
			XMVECTOR y2 = XMLoadFloat4(&_vStandard2);
			XMVECTOR y3 = XMLoadFloat4(&_vControl2);

			float t = fValue;
			float u = 1.f - fValue;
			float tt = t * t;
			float uu = u * u;
			float uuu = uu * u;
			float ttt = tt * t;

			XMVECTOR vPos = (uuu * y0);
			vPos += (3 * uu * t * y1);
			vPos += (3 * u * tt * y2);
			vPos += (ttt * y3);

			XMStoreFloat4(&result, vPos);
		}

		static tagFloat4 BezierCurve(const tagFloat4& _vStandard1, const tagFloat4& _vControl1, const tagFloat4& _vStandard2,
			const tagFloat4& _vControl2, const float& _fValue)
		{
			tagFloat4 value;
			BezierCurve(_vStandard1, _vControl1, _vStandard2, _vControl2, _fValue, value);
			return value;
		}
#pragma endregion BEZIER

	}FLOAT4;

	typedef struct tagFloat3 : public XMFLOAT3
	{
		tagFloat3() noexcept { XMStoreFloat3(this, XMVectorZero()); }
		tagFloat3(const XMFLOAT3& v) noexcept { XMStoreFloat3(this, XMLoadFloat3(&v)); }

		tagFloat3(const XMVECTOR& v) noexcept { XMStoreFloat3(this, v); }

		tagFloat3(const tagFloat3&) = default;

		tagFloat3(tagFloat3&&) = default;
		tagFloat3& operator=(tagFloat3&&) = default;

		tagFloat3(const float& _x, const float& _y, const float& _z) {
			this->x = _x; this->y = _y; this->z = _z;
		}
		explicit tagFloat3(_In_reads_(3) const float* pArray) {
			x = pArray[0]; y = pArray[1]; z = pArray[2];
		}
#pragma region OPERATOR
		tagFloat3& operator =(const tagFloat4& v) noexcept { XMStoreFloat3(this, XMLoadFloat4(&v));	return *this; }
		tagFloat3& operator =(const tagFloat3& v) noexcept { XMStoreFloat3(this, XMLoadFloat3(&v));	return *this; }
		tagFloat3& operator =(const XMFLOAT2& v) noexcept { XMStoreFloat3(this, XMLoadFloat2(&v));	return *this; }

		const bool operator == (const tagFloat3& v) { return XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&v)); }
		const bool operator != (const tagFloat3& v) { return XMVector3NotEqual(XMLoadFloat3(this), XMLoadFloat3(&v)); }

		tagFloat3& operator +(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMVectorAdd(XMLoadFloat3(this), XMLoadFloat3(&v))); return *this;
		}

		tagFloat3& operator -(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&v))); return *this;
			return *this;
		}

		tagFloat3& operator *(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMVectorMultiply(XMLoadFloat3(this), XMLoadFloat3(&v))); return *this;
			return *this;
		}

		tagFloat3& operator /(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) / XMLoadFloat3(&v));
			return *this;
		}

		tagFloat3& operator += (const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) + XMLoadFloat3(&v));
			return *this;
		}

		tagFloat3& operator  -=(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) - XMLoadFloat3(&v));
			return *this;
		}

		tagFloat3& operator *=(const  tagFloat3& v)noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) * XMLoadFloat3(&v));
			return *this;
		}

		tagFloat3& operator /=(const  tagFloat3& v) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) / XMLoadFloat3(&v));
			return *this;
		}

		tagFloat3& operator *(const float& F1) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) * F1);
			return *this;
		}

		tagFloat3& operator /(const float& F1) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) / F1);
			return *this;
		}

		tagFloat3& operator *=(const float& F1) noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) * F1);
			return *this;
		}

		tagFloat3& operator /=(const float& F1)  noexcept {
			XMStoreFloat3(this, XMLoadFloat3(this) / F1);
			return *this;
		}

		tagFloat3& operator()(const tagFloat3& Value) noexcept {
			*this = Value;
			return *this;
		}

		// Unary operators
		tagFloat3 operator+ () const noexcept { return *this; }
		tagFloat3 operator- () const noexcept {
			tagFloat3 R;
			XMStoreFloat3(&R, XMVectorNegate(XMLoadFloat3(this)));
			return R;
		}
#pragma endregion OPERATOR
#pragma region ECT
		const XMFLOAT2  Get_Float2() noexcept { return XMFLOAT2(x, y); }
		const XMFLOAT4  Get_Pos() noexcept { return XMFLOAT4(x, y, z, 1); }
		const XMFLOAT4  Get_Dir() noexcept { return XMFLOAT4(x, y, z, 0); }

		XMVECTOR Get_Vector() const { return XMLoadFloat3(this); }
		void Set_Vector(const XMVECTOR& _v) { XMStoreFloat3(this, _v); }

		// Vector operations
		bool InBounds(const tagFloat3& _Bounds) const noexcept {
			return XMVector3InBounds(XMLoadFloat3(this), XMLoadFloat3(&_Bounds));
		}
		bool InBounds(const FXMVECTOR& _Bounds) const noexcept {
			return XMVector3InBounds(XMLoadFloat3(this), _Bounds);
		}

		float Length() const noexcept {
			return XMVectorGetX(XMVector3Length(XMLoadFloat3(this)));
		}
		float LengthSquared() const noexcept {
			return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(this)));
		}

		static float Length(const tagFloat3& _v) {
			return XMVectorGetX(XMVector3Length(XMLoadFloat3(&_v)));
		}
		static float Length(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector3Length(_v));
		}

		static float LengthSquared(const tagFloat3& _v) {
			return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&_v)));
		}
		static float LengthSquared(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector3LengthSq(_v));
		}


		float Dot(const tagFloat3& _v) const noexcept {
			return XMVectorGetX(XMVector3Dot(XMLoadFloat3(this), XMLoadFloat3(&_v)));
		}
		float Dot(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector3Dot(XMLoadFloat3(this), _v));
		}

		static float Dot(const tagFloat3& _v1, const tagFloat3& _v2) {
			return XMVectorGetX(XMVector3Dot(XMLoadFloat3(&_v1), XMLoadFloat3(&_v2)));
		}

		static float Dot(const FXMVECTOR& _v1, const FXMVECTOR& _v2) {
			return XMVectorGetX(XMVector3Dot(_v1, _v2));
		}

#pragma endregion ECT
#pragma region CROSS
		void CrossSelf(const tagFloat3& v2) {
			tagFloat3 value = Cross(*this, v2);
			XMStoreFloat3(this, XMLoadFloat3(&value));
		}
		static void Cross(const tagFloat3& v1, const tagFloat3& v2, tagFloat3& result)  noexcept {
			result = Cross(v1, v2);
		}
		static tagFloat3 Cross(const tagFloat3& v1, const tagFloat3& v2)  noexcept {
			tagFloat3 result;
			XMStoreFloat3(&result, XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
			return result;
		}
		void CrossVecSelf(const FXMVECTOR& v2) {
			XMVECTOR value = Cross(XMLoadFloat3(this), v2);
			XMStoreFloat3(this, value);
		}
		static void Cross(const FXMVECTOR& v1, const FXMVECTOR& v2, XMVECTOR& result)  noexcept {
			result = Cross(v1, v2);
		}
		static XMVECTOR Cross(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			XMVECTOR result;
			result = XMVector3Cross(v1, v2);
			return result;
		}
#pragma endregion CROSS
#pragma region NORMALIZE
		void NormalizeSelf() noexcept {
			tagFloat3 value;
			Normalize(value);
			XMStoreFloat3(this, XMLoadFloat3(&value));
		}
		tagFloat3 Normalize() {
			tagFloat3 value;
			XMStoreFloat3(this, XMLoadFloat3(&value));
			return value;
		}
		void Normalize(tagFloat3& result) const noexcept {
			XMStoreFloat3(&result, XMVector3Normalize(XMLoadFloat3(this)));
		}
		XMVECTOR NormalizeVec() {
			XMVECTOR value;
			Normalize(value);
			return value;
		}
		void Normalize(XMVECTOR& result) const noexcept {
			result = XMVector3Normalize(XMLoadFloat3(this));
		}

		static void NormalizeValue(const tagFloat3& _v1, tagFloat3& _result) {
			XMStoreFloat3(&_result, XMVector3Normalize(XMLoadFloat3(&_v1)));
		}
		static tagFloat3 NormalizeValue(const tagFloat3& _v1) {
			tagFloat3 value;
			NormalizeValue(_v1, value);
			return value;
		}
#pragma endregion NORMALIZE
#pragma region CLAMP
		void Clamp(const tagFloat3& vmin, const tagFloat3& vmax) noexcept {
			tagFloat3 value;
			Clamp(vmin, vmax, value);
			XMStoreFloat3(this, XMLoadFloat3(&value));
		}
		void Clamp(const tagFloat3& vmin, const tagFloat3& vmax, tagFloat3& result) const noexcept {
			XMStoreFloat3(&result, XMVectorClamp(XMLoadFloat3(this), XMLoadFloat3(&vmin), XMLoadFloat3(&vmax)));
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax) noexcept {
			XMVECTOR value;
			Clamp(vmin, vmax, value);
			XMStoreFloat3(this, value);
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result) const noexcept {
			result = XMVectorClamp(XMLoadFloat3(this), vmin, vmax);
		}

		static void Clamp(const tagFloat3& _value, const tagFloat3& vmin, const tagFloat3& vmax, tagFloat3& result)  noexcept {
			XMStoreFloat3(&result, XMVectorClamp(XMLoadFloat3(&_value), XMLoadFloat3(&vmin), XMLoadFloat3(&vmax)));
		}
		static tagFloat3 Clamp(const tagFloat3& _value, const tagFloat3& vmin, const tagFloat3& vmax) {
			tagFloat3 value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}
		static void Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result)  noexcept {
			result = XMVectorClamp(_value, vmin, vmax);
		}
		static XMVECTOR Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax) {
			XMVECTOR value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}

#pragma endregion CLAMP
#pragma region DISTANCE
		// Static functions
		static float Distance(const tagFloat3& v1, const tagFloat3& v2) noexcept {
			return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v2))));
		}
		static float DistanceSquared(const tagFloat3& v1, const tagFloat3& v2) noexcept {
			return XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v2))));
		}
		float Distance(const tagFloat3& v) noexcept {
			return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&v))));
		}
		float DistanceSquared(const tagFloat3& v) noexcept {
			return XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&v))));
		}
		static float Distance(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector3Length(XMVectorSubtract(v1, v2)));
		}
		static float DistanceSquared(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(v1, v2)));
		}
		float Distance(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(this), v)));
		}
		float DistanceSquared(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(this), v)));
		}
#pragma endregion DISTANCE
#pragma region MIN
		static void Min(const tagFloat3& v1, const tagFloat3& v2, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVectorMin(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
		}
		static tagFloat3 Min(const tagFloat3& v1, const tagFloat3& v2) noexcept {
			tagFloat3 result;
			Min(v1, v2, result);
			return result;
		}
#pragma endregion MIN
#pragma region Max
		static void Max(const tagFloat3& v1, const tagFloat3& v2, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVectorMax(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
		}
		static tagFloat3 Max(const tagFloat3& v1, const tagFloat3& v2) noexcept {
			tagFloat3 result;
			Max(v1, v2, result);
			return result;
		}
#pragma endregion Max
#pragma region LERP
		static void Lerp(const tagFloat3& v1, const tagFloat3& v2, const float& t, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVectorLerp(XMLoadFloat3(&v1), XMLoadFloat3(&v2), t));
		}
		static tagFloat3 Lerp(const tagFloat3& v1, const tagFloat3& v2, const float& t) noexcept {
			tagFloat3 value;
			Lerp(v1, v2, t, value);
			return value;
		}
#pragma endregion LERP
#pragma region SMOOTHSTEP
		static void SmoothStep(const tagFloat3& v1, const tagFloat3& v2, float t, tagFloat3& result) noexcept {
			t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
			t = t * t * (3.f - 2.f * t);
			XMStoreFloat3(&result, XMVectorLerp(XMLoadFloat3(&v1), XMLoadFloat3(&v2), t));
		}
		static tagFloat3 SmoothStep(const tagFloat3& v1, const tagFloat3& v2, float t) noexcept {
			tagFloat3 value;
			SmoothStep(v1, v2, t, value);
			return value;
		}
#pragma endregion SMOOTHSTEP
#pragma region BARYCENTRIC
		static void Barycentric(const tagFloat3& v1, const tagFloat3& v2, const tagFloat3& v3, const float& f, const float& g, tagFloat3& result) noexcept {
			const XMVECTOR X = XMVectorBaryCentric(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3), f, g);
			XMStoreFloat3(&result, X);
		}
		static tagFloat3 Barycentric(const tagFloat3& v1, const tagFloat3& v2, const tagFloat3& v3, const float& f, const float& g) noexcept {
			tagFloat3 value;
			Barycentric(v1, v2, v3, f, g, value);
			return value;
		}
#pragma endregion BARYCENTRIC
#pragma region CATMULROM
		static void CatmullRom(const tagFloat3& v1, const tagFloat3& v2, const tagFloat3& v3, const tagFloat3& v4, const float& t, tagFloat3& result) noexcept {
			const XMVECTOR X = XMVectorCatmullRom(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3), XMLoadFloat3(&v4), t);
			XMStoreFloat3(&result, X);
		}
		static tagFloat3 CatmullRom(const tagFloat3& v1, const tagFloat3& v2, const tagFloat3& v3, const tagFloat3& v4, const float& t) noexcept {
			tagFloat3 value;
			CatmullRom(v1, v2, v3, v4, t, value);
			return value;
		}
#pragma endregion CATMULROM
#pragma region HERMITE
		static void Hermite(const tagFloat3& v1, const tagFloat3& t1, const tagFloat3& v2, const tagFloat3& t2, const float& t, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVectorHermite(XMLoadFloat3(&v1), XMLoadFloat3(&t1), XMLoadFloat3(&v2), XMLoadFloat3(&t2), t));
		}
		static tagFloat3 Hermite(const tagFloat3& v1, const tagFloat3& t1, const tagFloat3& v2, const tagFloat3& t2, const float& t) noexcept {
			tagFloat3 value;
			Hermite(v1, v2, t1, t2, t, value);
			return value;
		}
#pragma endregion HERMITE
#pragma region REFLECT
		static void Reflect(const tagFloat3& ivec, const tagFloat3& nvec, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVector3Reflect(XMLoadFloat3(&ivec), XMLoadFloat3(&nvec)));
		}
		static tagFloat3 Reflect(const tagFloat3& ivec, const tagFloat3& nvec) noexcept {
			tagFloat3 value;
			Reflect(ivec, nvec, value);
			return value;
		}
		static void Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec, XMVECTOR& result) noexcept {
			result = XMVector3Reflect(ivec, nvec);
		}
		static XMVECTOR Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec) noexcept {
			XMVECTOR value;
			Reflect(ivec, nvec, value);
			return value;
		}
#pragma endregion REFLECT
#pragma region REFRACT
		static void Refract(const tagFloat3& ivec, const tagFloat3& nvec, const float& refractionIndex, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVector3Refract(XMLoadFloat3(&ivec), XMLoadFloat3(&nvec), refractionIndex));
		}
		static tagFloat3 Refract(const tagFloat3& ivec, const tagFloat3& nvec, const float& refractionIndex) noexcept {
			tagFloat3 value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
		static void Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex, XMVECTOR& result) noexcept {
			result = XMVector3Refract(ivec, nvec, refractionIndex);
		}
		static XMVECTOR Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex) noexcept {
			XMVECTOR value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
#pragma endregion REFRACT
#pragma region TRANSFORM_FLOAT3

		static void Transform(const tagFloat3& v, const XMFLOAT4& quat, tagFloat3& result) noexcept {
			XMStoreFloat3(&result, XMVector3Rotate(XMLoadFloat3(&v), XMLoadFloat4(&quat)));
		}
		static tagFloat3 Transform(const tagFloat3& v, const XMFLOAT4& quat) noexcept {
			tagFloat3 value;
			Transform(v, quat, value);
			return value;
		}
#pragma endregion TRANSFORM_FLOAT3
#pragma region TRANSFORM_MATRIX
		static void Transform(_In_reads_(count) const tagFloat3* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat3* resultArray) noexcept {
			XMVector3TransformCoordStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, XMLoadFloat4x4(&m));
		}
		static void Transform(_In_reads_(count) const tagFloat3* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat4* resultArray) noexcept {
			XMVector3TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT3), count, XMLoadFloat4x4(&m));
		}
		static void TransformNormal(_In_reads_(count) const tagFloat3* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat3* resultArray) noexcept {
			XMVector3TransformNormalStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, XMLoadFloat4x4(&m));
		}
		static void Transform(_In_reads_(count) const tagFloat3* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat3* resultArray) noexcept {
			XMVector3TransformCoordStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, m);
		}
		static void Transform(_In_reads_(count) const tagFloat3* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat4* resultArray) noexcept {
			XMVector3TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT3), count, m);
		}
		static void TransformNormal(_In_reads_(count) const tagFloat3* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat3* resultArray) noexcept {
			XMVector3TransformNormalStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, m);
		}
#pragma endregion TRNASFORM_MATRIX
#pragma region SPLINECURVE
		static void SplineCurve(const tagFloat3& _prevPos, const tagFloat3& _vStandrardPos, const tagFloat3& _vNextPos1,
			const tagFloat3& _vNextPos2, const float& _fValue, tagFloat3& _result)
		{
			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat3(&_prevPos);
			XMVECTOR y1 = XMLoadFloat3(&_vStandrardPos);
			XMVECTOR y2 = XMLoadFloat3(&_vNextPos1);
			XMVECTOR y3 = XMLoadFloat3(&_vNextPos2);

			XMVECTOR A0 = y3 - y2 - y1 + y1;
			XMVECTOR A1 = y0 - y1 - A0;
			XMVECTOR A2 = y2 - y0;
			XMVECTOR A3 = y1;

			float m2 = fValue * fValue;

			_result = (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
		}

		static tagFloat3 SplineCurve(const tagFloat3& _prevPos, const tagFloat3& _vStandrardPos, const tagFloat3& _vNextPos1,
			const tagFloat3& _vNextPos2, const float& _fValue)
		{
			tagFloat3 value;
			SplineCurve(_prevPos, _vStandrardPos, _vNextPos1, _vNextPos2, _fValue, value);
			return value;
		}
#pragma endregion SPLINECURVE
#pragma region BEZIER
		static void BezierCurve(const tagFloat3& _vStandard1, const tagFloat3& _vControl1, const tagFloat3& _vStandard2,
			const tagFloat3& _vControl2, const float& _fValue, tagFloat3& result) {

			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat3(&_vStandard1);
			XMVECTOR y1 = XMLoadFloat3(&_vControl1);
			XMVECTOR y2 = XMLoadFloat3(&_vStandard2);
			XMVECTOR y3 = XMLoadFloat3(&_vControl2);

			float t = fValue;
			float u = 1.f - fValue;
			float tt = t * t;
			float uu = u * u;
			float uuu = uu * u;
			float ttt = tt * t;

			XMVECTOR vPos = (uuu * y0);
			vPos += (3 * uu * t * y1);
			vPos += (3 * u * tt * y2);
			vPos += (ttt * y3);

			XMStoreFloat3(&result, vPos);
		}

		static tagFloat3 BezierCurve(const tagFloat3& _vStandard1, const tagFloat3& _vControl1, const tagFloat3& _vStandard2,
			const tagFloat3& _vControl2, const float& _fValue)
		{
			tagFloat3 value;
			BezierCurve(_vStandard1, _vControl1, _vStandard2, _vControl2, _fValue, value);
			return value;
		}
#pragma endregion BEZIER
#pragma region COORD 
		tagFloat3 Coord4x4(const XMFLOAT4X4& _Matrix) {
			tagFloat3 value;
			XMStoreFloat3(&value, XMVector3TransformCoord(XMLoadFloat3(this), XMLoadFloat4x4(&_Matrix)));
			return value;
		}
		tagFloat3 CoordMatrix(const XMMATRIX& _Matrix) {
			tagFloat3 value;
			XMStoreFloat3(&value, XMVector3TransformCoord(XMLoadFloat3(this), _Matrix));
			return value;
		}
		tagFloat3 Normal4x4(const XMFLOAT4X4& _Matrix) {
			tagFloat3 value;
			XMStoreFloat3(&value, XMVector3TransformNormal(XMLoadFloat3(this), XMLoadFloat4x4(&_Matrix)));
			return value;
		}
		tagFloat3 NormalMatrix(const XMMATRIX& _Matrix) {
			tagFloat3 value;
			XMStoreFloat3(&value, XMVector3TransformNormal(XMLoadFloat3(this), _Matrix));
			return value;
		}
		void Coord4x4_Self(const XMFLOAT4X4& _Matrix) { XMStoreFloat3(this, XMVector3TransformCoord(XMLoadFloat3(this), XMLoadFloat4x4(&_Matrix))); }
		void CoordMatrix_Self(const XMMATRIX& _Matrix) { XMStoreFloat3(this, XMVector3TransformCoord(XMLoadFloat3(this), _Matrix)); }
		void Normal4x4_Self(const XMFLOAT4X4& _Matrix) { XMStoreFloat3(this, XMVector3TransformNormal(XMLoadFloat3(this), XMLoadFloat4x4(&_Matrix))); }
		void NormalMatrix_Self(const XMMATRIX& _Matrix) { XMStoreFloat3(this, XMVector3TransformNormal(XMLoadFloat3(this), _Matrix)); }
#pragma endregion COORD
	}FLOAT3;

	typedef struct tagFloat2 : public XMFLOAT2
	{
		tagFloat2() noexcept { XMStoreFloat2(this, XMVectorZero()); }
		tagFloat2(const XMFLOAT2& v) noexcept { XMStoreFloat2(this, XMLoadFloat2(&v)); }

		tagFloat2(const XMVECTOR& v) noexcept { XMStoreFloat2(this, v); }

		tagFloat2(const tagFloat2&) = default;

		tagFloat2(tagFloat2&&) = default;
		tagFloat2& operator=(tagFloat2&&) = default;

		tagFloat2(const float& _x, const float& _y) {
			this->x = _x; this->y = _y;
		}
		explicit tagFloat2(_In_reads_(2) const float* pArray) { x = pArray[0]; y = pArray[1]; }
#pragma region OPERATOR
		tagFloat2& operator =(const XMFLOAT2& v) noexcept { XMStoreFloat2(this, XMLoadFloat2(&v));	return *this; }
		tagFloat2& operator =(const tagFloat2& v) noexcept { XMStoreFloat2(this, XMLoadFloat2(&v));	return *this; }

		const bool operator == (const tagFloat2& v) { return XMVector2Equal(XMLoadFloat2(this), XMLoadFloat2(&v)); }
		const bool operator != (const tagFloat2& v) { return XMVector2NotEqual(XMLoadFloat2(this), XMLoadFloat2(&v)); }

		tagFloat2& operator +(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMVectorAdd(XMLoadFloat2(this), XMLoadFloat2(&v))); return *this;
		}

		tagFloat2& operator -(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&v))); return *this;
			return *this;
		}

		tagFloat2& operator *(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMVectorMultiply(XMLoadFloat2(this), XMLoadFloat2(&v))); return *this;
			return *this;
		}

		tagFloat2& operator /(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) / XMLoadFloat2(&v));
			return *this;
		}

		tagFloat2& operator += (const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) + XMLoadFloat2(&v));
			return *this;
		}

		tagFloat2& operator  -=(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) - XMLoadFloat2(&v));
			return *this;
		}

		tagFloat2& operator *=(const  tagFloat2& v)noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) * XMLoadFloat2(&v));
			return *this;
		}

		tagFloat2& operator /=(const  tagFloat2& v) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) / XMLoadFloat2(&v));
			return *this;
		}

		tagFloat2& operator *(const float& F1) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) * F1);
			return *this;
		}

		tagFloat2& operator /(const float& F1) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) / F1);
			return *this;
		}

		tagFloat2& operator *=(const float& F1) noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) * F1);
			return *this;
		}

		tagFloat2& operator /=(const float& F1)  noexcept {
			XMStoreFloat2(this, XMLoadFloat2(this) / F1);
			return *this;
		}

		tagFloat2& operator()(const tagFloat2& Value) noexcept {
			*this = Value;
			return *this;
		}

		// Unary operators
		tagFloat2 operator+ () const noexcept { return *this; }
		tagFloat2 operator- () const noexcept {
			tagFloat2 R;
			XMStoreFloat2(&R, XMVectorNegate(XMLoadFloat2(this)));
			return R;
		}
#pragma endregion OPERATOR
#pragma region ECT
		const XMVECTOR Get_Vector() const { return XMLoadFloat2(this); }
		void Set_Vector(const XMVECTOR& _v) { XMStoreFloat2(this, _v); }

		// Vector operations
		bool InBounds(const tagFloat2& _Bounds) const noexcept {
			return XMVector2InBounds(XMLoadFloat2(this), XMLoadFloat2(&_Bounds));
		}
		bool InBounds(const FXMVECTOR& _Bounds) const noexcept {
			return XMVector2InBounds(XMLoadFloat2(this), _Bounds);
		}

		float Length() const noexcept {
			return XMVectorGetX(XMVector2Length(XMLoadFloat2(this)));
		}
		float LengthSquared() const noexcept {
			return XMVectorGetX(XMVector2LengthSq(XMLoadFloat2(this)));
		}

		static float Length(const tagFloat2& _v) {
			return XMVectorGetX(XMVector2Length(XMLoadFloat2(&_v)));
		}
		static float Length(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector2Length(_v));
		}

		static float LengthSquared(const tagFloat2& _v) {
			return XMVectorGetX(XMVector2LengthSq(XMLoadFloat2(&_v)));
		}
		static float LengthSquared(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector2LengthSq(_v));
		}
		float Dot(const tagFloat2& _v) const noexcept {
			return XMVectorGetX(XMVector2Dot(XMLoadFloat2(this), XMLoadFloat2(&_v)));
		}
		float Dot(const FXMVECTOR& _v) {
			return XMVectorGetX(XMVector2Dot(XMLoadFloat2(this), _v));
		}

		static float Dot(const tagFloat2& _v1, const tagFloat2& _v2) {
			return XMVectorGetX(XMVector2Dot(XMLoadFloat2(&_v1), XMLoadFloat2(&_v2)));
		}

		static float Dot(const FXMVECTOR& _v1, const FXMVECTOR& _v2) {
			return XMVectorGetX(XMVector2Dot(_v1, _v2));
		}

#pragma endregion ECT
#pragma region CROSS
		void CrossSelf(const tagFloat2& v2) {
			tagFloat2 value = Cross(*this, v2);
			XMStoreFloat2(this, XMLoadFloat2(&value));
		}
		static void Cross(const tagFloat2& v1, const tagFloat2& v2, tagFloat2& result)  noexcept {
			result = Cross(v1, v2);
		}
		static tagFloat2 Cross(const tagFloat2& v1, const tagFloat2& v2)  noexcept {
			tagFloat2 result;
			XMStoreFloat2(&result, XMVector2Cross(XMLoadFloat2(&v1), XMLoadFloat2(&v2)));
			return result;
		}
		void CrossVecSelf(const FXMVECTOR& v2) {
			XMVECTOR value = Cross(XMLoadFloat2(this), v2);
			XMStoreFloat2(this, value);
		}
		static void Cross(const FXMVECTOR& v1, const FXMVECTOR& v2, XMVECTOR& result)  noexcept {
			result = Cross(v1, v2);
		}
		static XMVECTOR Cross(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			XMVECTOR result;
			result = XMVector2Cross(v1, v2);
			return result;
		}
#pragma endregion CROSS
#pragma region NORMALIZE
		void NormalizeSelf() noexcept {
			tagFloat2 value;
			Normalize(value);
			XMStoreFloat2(this, XMLoadFloat2(&value));
		}
		tagFloat2 Normalize() {
			tagFloat2 value;
			XMStoreFloat2(this, XMLoadFloat2(&value));
			return value;
		}
		void Normalize(tagFloat2& result) const noexcept {
			XMStoreFloat2(&result, XMVector2Normalize(XMLoadFloat2(this)));
		}
		XMVECTOR NormalizeVec() {
			XMVECTOR value;
			Normalize(value);
			return value;
		}
		void Normalize(XMVECTOR& result) const noexcept {
			result = XMVector2Normalize(XMLoadFloat2(this));
		}

		static void NormalizeValue(const tagFloat2& _v1, tagFloat2& _result) {
			XMStoreFloat2(&_result, XMVector2Normalize(XMLoadFloat2(&_v1)));
		}
		static tagFloat2 NormalizeValue(const tagFloat2& _v1) {
			tagFloat2 value;
			NormalizeValue(_v1, value);
			return value;
		}
#pragma endregion NORMALIZE
#pragma region CLAMP
		void Clamp(const tagFloat2& vmin, const tagFloat2& vmax) noexcept {
			tagFloat2 value;
			Clamp(vmin, vmax, value);
			XMStoreFloat2(this, XMLoadFloat2(&value));
		}
		void Clamp(const tagFloat2& vmin, const tagFloat2& vmax, tagFloat2& result) const noexcept {
			XMStoreFloat2(&result, XMVectorClamp(XMLoadFloat2(this), XMLoadFloat2(&vmin), XMLoadFloat2(&vmax)));
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax) noexcept {
			XMVECTOR value;
			Clamp(vmin, vmax, value);
			XMStoreFloat2(this, value);
		}
		void Clamp(const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result) const noexcept {
			result = XMVectorClamp(XMLoadFloat2(this), vmin, vmax);
		}

		static void Clamp(const tagFloat2& _value, const tagFloat2& vmin, const tagFloat2& vmax, tagFloat2& result)  noexcept {
			XMStoreFloat2(&result, XMVectorClamp(XMLoadFloat2(&_value), XMLoadFloat2(&vmin), XMLoadFloat2(&vmax)));
		}
		static tagFloat2 Clamp(const tagFloat2& _value, const tagFloat2& vmin, const tagFloat2& vmax) {
			tagFloat2 value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}
		static void Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax, XMVECTOR& result)  noexcept {
			result = XMVectorClamp(_value, vmin, vmax);
		}
		static XMVECTOR Clamp(const FXMVECTOR& _value, const FXMVECTOR& vmin, const FXMVECTOR& vmax) {
			XMVECTOR value;
			Clamp(_value, vmin, vmax, value);
			return value;
		}

#pragma endregion CLAMP
#pragma region DISTANCE
		// Static functions
		static float Distance(const tagFloat2& v1, const tagFloat2& v2) noexcept {
			return XMVectorGetX(XMVector2Length(XMVectorSubtract(XMLoadFloat2(&v1), XMLoadFloat2(&v2))));
		}
		static float DistanceSquared(const tagFloat2& v1, const tagFloat2& v2) noexcept {
			return XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMLoadFloat2(&v1), XMLoadFloat2(&v2))));
		}
		float Distance(const tagFloat2& v) noexcept {
			return XMVectorGetX(XMVector2Length(XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&v))));
		}
		float DistanceSquared(const tagFloat2& v) noexcept {
			return XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&v))));
		}
		static float Distance(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector2Length(XMVectorSubtract(v1, v2)));
		}
		static float DistanceSquared(const FXMVECTOR& v1, const FXMVECTOR& v2) noexcept {
			return XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(v1, v2)));
		}
		float Distance(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector2Length(XMVectorSubtract(XMLoadFloat2(this), v)));
		}
		float DistanceSquared(const FXMVECTOR& v) noexcept {
			return XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(XMLoadFloat2(this), v)));
		}
#pragma endregion DISTANCE
#pragma region MIN
		static void Min(const tagFloat2& v1, const tagFloat2& v2, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVectorMin(XMLoadFloat2(&v1), XMLoadFloat2(&v2)));
		}
		static tagFloat2 Min(const tagFloat2& v1, const tagFloat2& v2) noexcept {
			tagFloat2 result;
			Min(v1, v2, result);
			return result;
		}
#pragma endregion MIN
#pragma region Max
		static void Max(const tagFloat2& v1, const tagFloat2& v2, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVectorMax(XMLoadFloat2(&v1), XMLoadFloat2(&v2)));
		}
		static tagFloat2 Max(const tagFloat2& v1, const tagFloat2& v2) noexcept {
			tagFloat2 result;
			Max(v1, v2, result);
			return result;
		}
#pragma endregion Max
#pragma region LERP
		static void Lerp(const tagFloat2& v1, const tagFloat2& v2, const float& t, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVectorLerp(XMLoadFloat2(&v1), XMLoadFloat2(&v2), t));
		}
		static tagFloat2 Lerp(const tagFloat2& v1, const tagFloat2& v2, const float& t) noexcept {
			tagFloat2 value;
			Lerp(v1, v2, t, value);
			return value;
		}
#pragma endregion LERP
#pragma region SMOOTHSTEP
		static void SmoothStep(const tagFloat2& v1, const tagFloat2& v2, float t, tagFloat2& result) noexcept {
			t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
			t = t * t * (3.f - 2.f * t);
			XMStoreFloat2(&result, XMVectorLerp(XMLoadFloat2(&v1), XMLoadFloat2(&v2), t));
		}
		static tagFloat2 SmoothStep(const tagFloat2& v1, const tagFloat2& v2, float t) noexcept {
			tagFloat2 value;
			SmoothStep(v1, v2, t, value);
			return value;
		}
#pragma endregion SMOOTHSTEP
#pragma region BARYCENTRIC
		static void Barycentric(const tagFloat2& v1, const tagFloat2& v2, const tagFloat2& v3, const float& f, const float& g, tagFloat2& result) noexcept {
			const XMVECTOR X = XMVectorBaryCentric(XMLoadFloat2(&v1), XMLoadFloat2(&v2), XMLoadFloat2(&v3), f, g);
			XMStoreFloat2(&result, X);
		}
		static tagFloat2 Barycentric(const tagFloat2& v1, const tagFloat2& v2, const tagFloat2& v3, const float& f, const float& g) noexcept {
			tagFloat2 value;
			Barycentric(v1, v2, v3, f, g, value);
			return value;
		}
#pragma endregion BARYCENTRIC
#pragma region CATMULROM
		static void CatmullRom(const tagFloat2& v1, const tagFloat2& v2, const tagFloat2& v3, const tagFloat2& v4, const float& t, tagFloat2& result) noexcept {
			const XMVECTOR X = XMVectorCatmullRom(XMLoadFloat2(&v1), XMLoadFloat2(&v2), XMLoadFloat2(&v3), XMLoadFloat2(&v4), t);
			XMStoreFloat2(&result, X);
		}
		static tagFloat2 CatmullRom(const tagFloat2& v1, const tagFloat2& v2, const tagFloat2& v3, const tagFloat2& v4, const float& t) noexcept {
			tagFloat2 value;
			CatmullRom(v1, v2, v3, v4, t, value);
			return value;
		}
#pragma endregion CATMULROM
#pragma region HERMITE
		static void Hermite(const tagFloat2& v1, const tagFloat2& t1, const tagFloat2& v2, const tagFloat2& t2, const float& t, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVectorHermite(XMLoadFloat2(&v1), XMLoadFloat2(&t1), XMLoadFloat2(&v2), XMLoadFloat2(&t2), t));
		}
		static tagFloat2 Hermite(const tagFloat2& v1, const tagFloat2& t1, const tagFloat2& v2, const tagFloat2& t2, const float& t) noexcept {
			tagFloat2 value;
			Hermite(v1, v2, t1, t2, t, value);
			return value;
		}
#pragma endregion HERMITE
#pragma region REFLECT
		static void Reflect(const tagFloat2& ivec, const tagFloat2& nvec, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVector2Reflect(XMLoadFloat2(&ivec), XMLoadFloat2(&nvec)));
		}
		static tagFloat2 Reflect(const tagFloat2& ivec, const tagFloat2& nvec) noexcept {
			tagFloat2 value;
			Reflect(ivec, nvec, value);
			return value;
		}
		static void Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec, XMVECTOR& result) noexcept {
			result = XMVector2Reflect(ivec, nvec);
		}
		static XMVECTOR Reflect(const FXMVECTOR& ivec, const FXMVECTOR& nvec) noexcept {
			XMVECTOR value;
			Reflect(ivec, nvec, value);
			return value;
		}
#pragma endregion REFLECT
#pragma region REFRACT
		static void Refract(const tagFloat2& ivec, const tagFloat2& nvec, const float& refractionIndex, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVector2Refract(XMLoadFloat2(&ivec), XMLoadFloat2(&nvec), refractionIndex));
		}
		static tagFloat2 Refract(const tagFloat2& ivec, const tagFloat2& nvec, const float& refractionIndex) noexcept {
			tagFloat2 value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
		static void Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex, XMVECTOR& result) noexcept {
			result = XMVector2Refract(ivec, nvec, refractionIndex);
		}
		static XMVECTOR Refract(const FXMVECTOR& ivec, const FXMVECTOR& nvec, const float& refractionIndex) noexcept {
			XMVECTOR value;
			Refract(ivec, nvec, refractionIndex, value);
			return value;
		}
#pragma endregion REFRACT
#pragma region TRANSFORM_FLOAT2

		static void Transform(const tagFloat2& v, const XMFLOAT4& quat, tagFloat2& result) noexcept {
			XMStoreFloat2(&result, XMVector3Rotate(XMLoadFloat2(&v), XMLoadFloat4(&quat)));
		}
		static tagFloat2 Transform(const tagFloat2& v, const XMFLOAT4& quat) noexcept {
			tagFloat2 value;
			Transform(v, quat, value);
			return value;
		}
#pragma endregion TRANSFORM_FLOAT2
#pragma region TRANSFORM_MATRIX
		static void Transform(_In_reads_(count) const tagFloat2* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat2* resultArray) noexcept {
			XMVector2TransformCoordStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, XMLoadFloat4x4(&m));
		}
		static void Transform(_In_reads_(count) const tagFloat2* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat4* resultArray) noexcept {
			XMVector2TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT2), count, XMLoadFloat4x4(&m));
		}
		static void TransformNormal(_In_reads_(count) const tagFloat2* varray, size_t count, const XMFLOAT4X4& m, _Out_writes_(count) tagFloat2* resultArray) noexcept {
			XMVector2TransformNormalStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, XMLoadFloat4x4(&m));
		}
		static void Transform(_In_reads_(count) const tagFloat2* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat2* resultArray) noexcept {
			XMVector2TransformCoordStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, m);
		}
		static void Transform(_In_reads_(count) const tagFloat2* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat4* resultArray) noexcept {
			XMVector2TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT2), count, m);
		}
		static void TransformNormal(_In_reads_(count) const tagFloat2* varray, size_t count, const XMMATRIX& m, _Out_writes_(count) tagFloat2* resultArray) noexcept {
			XMVector2TransformNormalStream(resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, m);
		}
#pragma endregion TRNASFORM_MATRIX
#pragma region SPLINECURVE
		static void SplineCurve(const tagFloat2& _prevPos, const tagFloat2& _vStandrardPos, const tagFloat2& _vNextPos1,
			const tagFloat2& _vNextPos2, const float& _fValue, tagFloat2& _result)
		{
			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat2(&_prevPos);
			XMVECTOR y1 = XMLoadFloat2(&_vStandrardPos);
			XMVECTOR y2 = XMLoadFloat2(&_vNextPos1);
			XMVECTOR y3 = XMLoadFloat2(&_vNextPos2);

			XMVECTOR A0 = y3 - y2 - y1 + y1;
			XMVECTOR A1 = y0 - y1 - A0;
			XMVECTOR A2 = y2 - y0;
			XMVECTOR A3 = y1;

			float m2 = fValue * fValue;

			_result = (A0 * fValue * m2 + A1 * m2 + A2 * fValue + A3);
		}

		static tagFloat2 SplineCurve(const tagFloat2& _prevPos, const tagFloat2& _vStandrardPos, const tagFloat2& _vNextPos1,
			const tagFloat2& _vNextPos2, const float& _fValue)
		{
			tagFloat2 value;
			SplineCurve(_prevPos, _vStandrardPos, _vNextPos1, _vNextPos2, _fValue, value);
			return value;
		}
#pragma endregion SPLINECURVE
#pragma region BEZIER
		static void BezierCurve(const tagFloat2& _vStandard1, const tagFloat2& _vControl1, const tagFloat2& _vStandard2,
			const tagFloat2& _vControl2, const float& _fValue, tagFloat2& result) {

			float fValue = _fValue;
			if (fValue < 0.f) fValue = 0.f;
			else if (fValue > 1.f) fValue = 1.f;

			XMVECTOR y0 = XMLoadFloat2(&_vStandard1);
			XMVECTOR y1 = XMLoadFloat2(&_vControl1);
			XMVECTOR y2 = XMLoadFloat2(&_vStandard2);
			XMVECTOR y3 = XMLoadFloat2(&_vControl2);

			float t = fValue;
			float u = 1.f - fValue;
			float tt = t * t;
			float uu = u * u;
			float uuu = uu * u;
			float ttt = tt * t;

			XMVECTOR vPos = (uuu * y0);
			vPos += (3 * uu * t * y1);
			vPos += (3 * u * tt * y2);
			vPos += (ttt * y3);

			XMStoreFloat2(&result, vPos);
		}

		static tagFloat2 BezierCurve(const tagFloat2& _vStandard1, const tagFloat2& _vControl1, const tagFloat2& _vStandard2,
			const tagFloat2& _vControl2, const float& _fValue)
		{
			tagFloat2 value;
			BezierCurve(_vStandard1, _vControl1, _vStandard2, _vControl2, _fValue, value);
			return value;
		}
#pragma endregion BEZIER
#pragma region COORD 
		tagFloat2 Coord4x4(const XMFLOAT4X4& _Matrix) {
			tagFloat2 value;
			XMStoreFloat2(&value, XMVector2TransformCoord(XMLoadFloat2(this), XMLoadFloat4x4(&_Matrix)));
			return value;
		}
		tagFloat2 CoordMatrix(const XMMATRIX& _Matrix) {
			tagFloat2 value;
			XMStoreFloat2(&value, XMVector2TransformCoord(XMLoadFloat2(this), _Matrix));
			return value;
		}
		tagFloat2 Normal4x4(const XMFLOAT4X4& _Matrix) {
			tagFloat2 value;
			XMStoreFloat2(&value, XMVector2TransformNormal(XMLoadFloat2(this), XMLoadFloat4x4(&_Matrix)));
			return value;
		}
		tagFloat2 NormalMatrix(const XMMATRIX& _Matrix) {
			tagFloat2 value;
			XMStoreFloat2(&value, XMVector2TransformNormal(XMLoadFloat2(this), _Matrix));
			return value;
		}
		void Coord4x4_Self(const XMFLOAT4X4& _Matrix) { XMStoreFloat2(this, XMVector2TransformCoord(XMLoadFloat2(this), XMLoadFloat4x4(&_Matrix))); }
		void CoordMatrix_Self(const XMMATRIX& _Matrix) { XMStoreFloat2(this, XMVector2TransformCoord(XMLoadFloat2(this), _Matrix)); }
		void Normal4x4_Self(const XMFLOAT4X4& _Matrix) { XMStoreFloat2(this, XMVector2TransformNormal(XMLoadFloat2(this), XMLoadFloat4x4(&_Matrix))); }
		void NormalMatrix_Self(const XMMATRIX& _Matrix) { XMStoreFloat2(this, XMVector2TransformNormal(XMLoadFloat2(this), _Matrix)); }
#pragma endregion COORD
	}FLOAT2;

#pragma endregion VE3

#pragma region OUTMATRIX
	typedef struct tagOutWorldMatrix
	{
		FLOAT3 vPos = FLOAT3(0.f, 0.f, 0.f);
		FLOAT3 vScale = FLOAT3(0.f, 0.f, 0.f);
		FLOAT3 vRot = FLOAT3(0.f, 0.f, 0.f);
		FLOAT4 vQuaternionRot = FLOAT4(0.f, 0.f, 0.f, 0.f);
	}OUTMATRIX;
#pragma endregion OUTMATRIX

#pragma region MATRICIES

	typedef struct tagFloat4x4 : public XMFLOAT4X4
	{
		tagFloat4x4() = default;

		tagFloat4x4(const tagFloat4x4&) = default;
		tagFloat4x4& operator=(const tagFloat4x4&) = default;

		tagFloat4x4(tagFloat4x4&&) = default;
		tagFloat4x4& operator=(tagFloat4x4&&) = default;

		tagFloat4x4(const XMFLOAT4X4& _tMatrix) { *this = _tMatrix; }

		tagFloat4x4(const XMFLOAT3X3& _Matrix)
		{
			_11 = _Matrix._11; _12 = _Matrix._12; _13 = _Matrix._13; _14 = 0.f;
			_21 = _Matrix._21; _22 = _Matrix._22; _23 = _Matrix._23; _24 = 0.f;
			_31 = _Matrix._31; _32 = _Matrix._32; _33 = _Matrix._33; _34 = 0.f;
			_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
		}
		tagFloat4x4(const XMFLOAT4X3& _Matrix)
		{
			_11 = _Matrix._11; _12 = _Matrix._12; _13 = _Matrix._13; _14 = 0.f;
			_21 = _Matrix._21; _22 = _Matrix._22; _23 = _Matrix._23; _24 = 0.f;
			_31 = _Matrix._31; _32 = _Matrix._32; _33 = _Matrix._33; _34 = 0.f;
			_41 = _Matrix._41; _42 = _Matrix._42; _43 = _Matrix._43; _44 = 1.f;
		}

		//------------------------------------------------------------------------------
		// Comparision operators
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		// Assignment operators
		//------------------------------------------------------------------------------

		tagFloat4x4& operator =(const XMFLOAT3X3& _Matrix)
		{
			*this = tagFloat4x4(_Matrix);
			return *this;
		}
		tagFloat4x4& operator =(const XMFLOAT4X3& _Matrix)
		{
			*this = tagFloat4x4(_Matrix);
			return *this;
		}

		tagFloat4x4& operator += (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator -= (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator *= (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator /=(const XMFLOAT4X4& _Matrix)
		{
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._11));
			const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._21));
			const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._31));
			const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._41));

			x1 = XMVectorDivide(x1, y1);
			x2 = XMVectorDivide(x2, y2);
			x3 = XMVectorDivide(x3, y3);
			x4 = XMVectorDivide(x4, y4);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

		tagFloat4x4& operator + (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator - (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator * (const XMFLOAT4X4& _Matrix)
		{
			XMStoreFloat4x4(this, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_Matrix));
			return *this;
		}

		tagFloat4x4& operator /(const XMFLOAT4X4& _Matrix)
		{
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._11));
			const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._21));
			const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._31));
			const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_Matrix._41));

			x1 = XMVectorDivide(x1, y1);
			x2 = XMVectorDivide(x2, y2);
			x3 = XMVectorDivide(x3, y3);
			x4 = XMVectorDivide(x4, y4);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

		tagFloat4x4& operator *= (const float& _Value)
		{
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			x1 = XMVectorScale(x1, _Value);
			x2 = XMVectorScale(x2, _Value);
			x3 = XMVectorScale(x3, _Value);
			x4 = XMVectorScale(x4, _Value);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

		tagFloat4x4& operator /= (const float& _Value)
		{
			assert(_Value != 0.f);
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			const float rs = 1.f / _Value;

			x1 = XMVectorScale(x1, rs);
			x2 = XMVectorScale(x2, rs);
			x3 = XMVectorScale(x3, rs);
			x4 = XMVectorScale(x4, rs);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

		tagFloat4x4& operator * (const float& _Value)
		{
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			x1 = XMVectorScale(x1, _Value);
			x2 = XMVectorScale(x2, _Value);
			x3 = XMVectorScale(x3, _Value);
			x4 = XMVectorScale(x4, _Value);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

		tagFloat4x4& operator / (const float& _Value)
		{
			assert(_Value != 0.f);
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

			const float rs = 1.f / _Value;

			x1 = XMVectorScale(x1, rs);
			x2 = XMVectorScale(x2, rs);
			x3 = XMVectorScale(x3, rs);
			x4 = XMVectorScale(x4, rs);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
			return *this;
		}

#pragma region MATRIX_COMFORT

		static tagFloat4x4 Idetify() { tagFloat4x4 value; XMStoreFloat4x4(&value, XMMatrixIdentity()); return value; }

		XMMATRIX Get_Matrix() { return XMLoadFloat4x4(this); }
		static XMMATRIX Get_Matrix(const tagFloat4x4& _value) { return XMLoadFloat4x4(&_value); }

		void Set_Matrix(const XMMATRIX& _value) { XMStoreFloat4x4(this, _value); }
		static tagFloat4x4 Store_Matrix(const XMMATRIX& _value) { tagFloat4x4 value; value.Set_Matrix(_value); return value; }

		static XMMATRIX Load_Matrix(const tagFloat4x4& _value) { return XMLoadFloat4x4(&_value); }

		static XMMATRIX InverseMatrix(const tagFloat4x4& _value) { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&_value)); }
		static XMMATRIX InverseMatrix(const XMMATRIX& _value) { return XMMatrixInverse(nullptr, _value); }

		static tagFloat4x4 Inverse4X4(const tagFloat4x4& _value) { tagFloat4x4 value; XMStoreFloat4x4(&value, InverseMatrix(_value)); return value; }
		static tagFloat4x4 Inverse4X4(const XMMATRIX& _value) { tagFloat4x4 value; XMStoreFloat4x4(&value, InverseMatrix(_value)); return value; }

		static XMMATRIX TransposMatrix(const tagFloat4x4& _value) { return XMMatrixTranspose(XMLoadFloat4x4(&_value)); }
		static XMMATRIX TransposMatrix(const XMMATRIX& _value) { return XMMatrixTranspose(_value); }

		static tagFloat4x4 Transpos4X4(const tagFloat4x4& _value) { tagFloat4x4 value; XMStoreFloat4x4(&value, TransposMatrix(_value)); return value; }
		static tagFloat4x4 Transpos4X4(const XMMATRIX& _value) { tagFloat4x4 value; XMStoreFloat4x4(&value, TransposMatrix(_value)); return value; }
#pragma endregion MATRIX_COMFORT

		//------------------------------------------------------------------------------
		// Binary operators
		//------------------------------------------------------------------------------
#pragma region MATRIX_DEFAULT
		FLOAT3 Up() const { return FLOAT3(_21, _22, _23); }
		void Up(const FLOAT3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

		FLOAT3 Down() const { return FLOAT3(-_21, -_22, -_23); }
		void Down(const FLOAT3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

		FLOAT3 Right() const { return  FLOAT3(_11, _12, _13); }
		void Right(const FLOAT3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

		FLOAT3 Left() const { return FLOAT3(-_11, -_12, -_13); }
		void Left(const FLOAT3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

		FLOAT3 Forward() const { return FLOAT3(_31, _32, _33); }
		void Forward(const FLOAT3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

		FLOAT3 Backward() const { return FLOAT3(-_31, -_32, -_33); }
		void Backward(const FLOAT3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

		FLOAT3 Translation() const { return FLOAT3(_41, _42, _43); }
		void Translation(const FLOAT3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

		bool Decompose(FLOAT3& scale, XMFLOAT4& rotation, FLOAT3& translation) {
			XMVECTOR s, r, t;
			if (!XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(this)))
				return false;

			XMStoreFloat3(&scale, s);
			XMStoreFloat4(&rotation, r);
			XMStoreFloat3(&translation, t);
			return true;
		}

		tagFloat4x4 Transpose() const {
			const XMMATRIX M = XMLoadFloat4x4(this);
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixTranspose(M));
			return R;
		}
		void Transpose(tagFloat4x4& result) const {
			result = Transpose();
		}

		tagFloat4x4 Inverse() const {
			const XMMATRIX M = XMLoadFloat4x4(this);
			FLOAT4X4 R;
			XMVECTOR det;
			XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
			return R;
		}
		void Inverse(tagFloat4x4& result) const {
			result = Inverse();
		}

		float Determinant() const {
			const XMMATRIX M = XMLoadFloat4x4(this);
			return XMVectorGetX(XMMatrixDeterminant(M));
		}
		// Computes rotation about y-axis (y), then x-axis (x), then z-axis (z)
		FLOAT3 ToEuler() const {
			const float cy = sqrtf(_33 * _33 + _31 * _31);
			const float cx = atan2f(-_32, cy);
			if (cy > 16.f * FLT_EPSILON)
				return FLOAT3(cx, atan2f(_31, _33), atan2f(_12, _22));
			else
				return FLOAT3(cx, 0.f, atan2f(-_21, _11));
		}

		// Static functions
		static tagFloat4x4 CreateBillboard(
			const FLOAT3& object, const FLOAT3& cameraPosition, const FLOAT3& cameraUp, _In_opt_ const FLOAT3* cameraForward = nullptr) {

			const XMVECTOR O = XMLoadFloat3(&object);
			const XMVECTOR C = XMLoadFloat3(&cameraPosition);
			XMVECTOR Z = XMVectorSubtract(O, C);
			const XMVECTOR N = XMVector3LengthSq(Z);
			if (XMVector3Less(N, g_XMEpsilon))
			{
				if (cameraForward)
				{
					const XMVECTOR F = XMLoadFloat3(cameraForward);
					Z = XMVectorNegate(F);
				}
				else
					Z = g_XMNegIdentityR2;
			}
			else
				Z = XMVector3Normalize(Z);

			const XMVECTOR up = XMLoadFloat3(&cameraUp);
			XMVECTOR X = XMVector3Cross(up, Z);
			X = XMVector3Normalize(X);

			const XMVECTOR Y = XMVector3Cross(Z, X);

			XMMATRIX M;
			M.r[0] = X;
			M.r[1] = Y;
			M.r[2] = Z;
			M.r[3] = XMVectorSetW(O, 1.f);

			FLOAT4X4 R;
			XMStoreFloat4x4(&R, M);
			return R;
		}

		static tagFloat4x4 CreateConstrainedBillboard(
			const FLOAT3& object, const FLOAT3& cameraPosition, const FLOAT3& rotateAxis,
			_In_opt_ const FLOAT3* cameraForward = nullptr, _In_opt_ const FLOAT3* objectForward = nullptr) {
			static const XMVECTORF32 s_minAngle = { { { 0.99825467075f, 0.99825467075f, 0.99825467075f, 0.99825467075f } } }; // 1.0 - XMConvertToRadians( 0.1f );

			const XMVECTOR O = XMLoadFloat3(&object);
			const XMVECTOR C = XMLoadFloat3(&cameraPosition);
			XMVECTOR faceDir = XMVectorSubtract(O, C);
			const XMVECTOR N = XMVector3LengthSq(faceDir);
			if (XMVector3Less(N, g_XMEpsilon))
			{
				if (cameraForward)
				{
					const XMVECTOR F = XMLoadFloat3(cameraForward);
					faceDir = XMVectorNegate(F);
				}
				else
					faceDir = g_XMNegIdentityR2;
			}
			else
				faceDir = XMVector3Normalize(faceDir);

			const XMVECTOR Y = XMLoadFloat3(&rotateAxis);
			XMVECTOR X, Z;

			XMVECTOR dot = XMVectorAbs(XMVector3Dot(Y, faceDir));
			if (XMVector3Greater(dot, s_minAngle))
			{
				if (objectForward)
				{
					Z = XMLoadFloat3(objectForward);
					dot = XMVectorAbs(XMVector3Dot(Y, Z));
					if (XMVector3Greater(dot, s_minAngle))
					{
						dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
						Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
					}
				}
				else
				{
					dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
					Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
				}

				X = XMVector3Cross(Y, Z);
				X = XMVector3Normalize(X);

				Z = XMVector3Cross(X, Y);
				Z = XMVector3Normalize(Z);
			}
			else
			{
				X = XMVector3Cross(Y, faceDir);
				X = XMVector3Normalize(X);

				Z = XMVector3Cross(X, Y);
				Z = XMVector3Normalize(Z);
			}

			XMMATRIX M;
			M.r[0] = X;
			M.r[1] = Y;
			M.r[2] = Z;
			M.r[3] = XMVectorSetW(O, 1.f);

			tagFloat4x4 R;
			XMStoreFloat4x4(&R, M);
			return R;
		}

		static tagFloat4x4 CreateTranslation(const FLOAT3& position) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixTranslation(position.x, position.y, position.z));
			return R;
		}
		static tagFloat4x4 CreateTranslation(const float& x, const float& y, const float& z) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
			return R;
		}

		static tagFloat4x4 CreateScale(const FLOAT3& scales) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixScaling(scales.x, scales.y, scales.z));
			return R;
		}
		static tagFloat4x4 CreateScale(const float& xs, const float& ys, const float& zs) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
			return R;
		}
		static tagFloat4x4 CreateScale(const float& scale) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixScaling(scale, scale, scale));
			return R;
		}

		static tagFloat4x4 CreateRotationX(const float& radians) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationX(radians));
			return R;
		}
		static tagFloat4x4 CreateRotationY(const float& radians) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationY(radians));
			return R;
		}
		static tagFloat4x4 CreateRotationZ(const float& radians) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
			return R;
		}

		static tagFloat4x4 CreateFromAxisAngle(const FLOAT3& axis, const float& angle) {
			tagFloat4x4 R;
			const XMVECTOR a = XMLoadFloat3(&axis);
			XMStoreFloat4x4(&R, XMMatrixRotationAxis(a, angle));
			return R;
		}

		static tagFloat4x4 CreatePerspectiveFieldOfView(const float& fov, const float& aspectRatio, const float& nearPlane, const float& farPlane) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
			return R;
		}
		static tagFloat4x4 CreatePerspective(const float& width, const float& height, const float& nearPlane, const float& farPlane) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixPerspectiveRH(width, height, nearPlane, farPlane));
			return R;
		}
		static tagFloat4x4 CreatePerspectiveOffCenter(const float& left, const float& right, const float& bottom, const float& top, const float& nearPlane, const float& farPlane) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixPerspectiveOffCenterRH(left, right, bottom, top, nearPlane, farPlane));
			return R;
		}
		static tagFloat4x4 CreateOrthographic(const float& width, const float& height, const float& zNearPlane, const float& zFarPlane) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixOrthographicRH(width, height, zNearPlane, zFarPlane));
			return R;
		}
		static tagFloat4x4 CreateOrthographicOffCenter(const float& left, const float& right, const float& bottom, const float& top, const float& zNearPlane, const float& zFarPlane) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane));
			return R;
		}

		static tagFloat4x4 CreateLookAt(const FLOAT3& eye, const FLOAT3& target, const FLOAT3& up) {
			tagFloat4x4 R;
			const XMVECTOR eyev = XMLoadFloat3(&eye);
			const XMVECTOR targetv = XMLoadFloat3(&target);
			const XMVECTOR upv = XMLoadFloat3(&up);
			XMStoreFloat4x4(&R, XMMatrixLookAtRH(eyev, targetv, upv));
			return R;
		}
		static tagFloat4x4 CreateWorld(const FLOAT3& position, const FLOAT3& forward, const FLOAT3& up) {
			const XMVECTOR zaxis = XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&forward)));
			XMVECTOR yaxis = XMLoadFloat3(&up);
			const XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
			yaxis = XMVector3Cross(zaxis, xaxis);

			tagFloat4x4 R;
			XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
			XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
			XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
			R._14 = R._24 = R._34 = 0.f;
			R._41 = position.x; R._42 = position.y; R._43 = position.z;
			R._44 = 1.f;
			return R;
		}

		static tagFloat4x4 CreateFromQuaternion(const XMFLOAT4& rotation) {
			const XMVECTOR quatv = XMLoadFloat4(&rotation);
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationQuaternion(quatv));
			return R;
		}

		// Rotates about y-axis (yaw), then x-axis (pitch), then z-axis (roll)
		static tagFloat4x4 CreateFromYawPitchRoll(const float& yaw, const float& pitch, const float& roll) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
			return R;
		}

		// Rotates about y-axis (angles.y), then x-axis (angles.x), then z-axis (angles.z)
		static tagFloat4x4 CreateFromYawPitchRoll(const FLOAT3& angles) {
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&angles)));
			return R;
		}

		static tagFloat4x4 CreateShadow(const FLOAT3& lightDir, const FLOAT4& plane) {
			const XMVECTOR light = XMLoadFloat3(&lightDir);
			const XMVECTOR planev = XMLoadFloat4(&plane);
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixShadow(planev, light));
			return R;
		}

		static tagFloat4x4 CreateReflection(const FLOAT4& plane) {
			const XMVECTOR planev = XMLoadFloat4(&plane);
			tagFloat4x4 R;
			XMStoreFloat4x4(&R, XMMatrixReflect(planev));
			return R;
		}

		static void Lerp(const tagFloat4x4& M1, const tagFloat4x4& M2, float t, tagFloat4x4& result) {
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

			const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
			const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
			const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
			const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

			x1 = XMVectorLerp(x1, y1, t);
			x2 = XMVectorLerp(x2, y2, t);
			x3 = XMVectorLerp(x3, y3, t);
			x4 = XMVectorLerp(x4, y4, t);

			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
		}
		static tagFloat4x4 Lerp(const tagFloat4x4& M1, const tagFloat4x4& M2, float t) {
			XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
			XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
			XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
			XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

			const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
			const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
			const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
			const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

			x1 = XMVectorLerp(x1, y1, t);
			x2 = XMVectorLerp(x2, y2, t);
			x3 = XMVectorLerp(x3, y3, t);
			x4 = XMVectorLerp(x4, y4, t);

			tagFloat4x4 result;
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
			return result;
		}

		static void Transform(const tagFloat4x4& M, const XMFLOAT4& rotation, tagFloat4x4& result) {
			XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(&M), XMMatrixRotationQuaternion(XMLoadFloat4(&rotation))));
		}
		static tagFloat4x4 Transform(const tagFloat4x4& M, const XMFLOAT4& rotation) {
			tagFloat4x4 result;
			XMStoreFloat4x4(&result, XMMatrixMultiply(XMLoadFloat4x4(&M), XMMatrixRotationQuaternion(XMLoadFloat4(&rotation))));
			return result;
		}

#pragma endregion MATRIX_DEFAULT
#pragma region MATRIX_SETGET
		OUTMATRIX Get_OutMatrix() {
			OUTMATRIX OutMatrix = Get_OutMatrix(XMLoadFloat4x4(this));
			return OutMatrix;
		}
		static OUTMATRIX Get_OutMatrix(const tagFloat4x4& _matrix) {
			return Get_OutMatrix(XMLoadFloat4x4(&_matrix));
		}
		static OUTMATRIX Get_OutMatrix(const XMMATRIX& _matrix) {
			OUTMATRIX OutMatrix;
			XMVECTOR vPos, vRot, vScale;
			XMMatrixDecompose(&vScale, &vRot, &vPos, _matrix);
			OutMatrix.vPos.Set_Vector(vPos);
			OutMatrix.vQuaternionRot = vRot;
			OutMatrix.vScale.Set_Vector(vScale);

			float sqw = OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.w;
			float sqx = OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.x;
			float sqy = OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.y;
			float sqz = OutMatrix.vQuaternionRot.z * OutMatrix.vQuaternionRot.z;

			OutMatrix.vRot.x = XMConvertToDegrees(asinf(2.f * (OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.x -
				OutMatrix.vQuaternionRot.y * OutMatrix.vQuaternionRot.z)));
			OutMatrix.vRot.y = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.z +
				OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.y), (-sqx - sqy + sqz + sqw)));
			OutMatrix.vRot.z = XMConvertToDegrees(atan2(2.f * (OutMatrix.vQuaternionRot.x * OutMatrix.vQuaternionRot.y +
				OutMatrix.vQuaternionRot.w * OutMatrix.vQuaternionRot.z), (-sqx + sqy - sqz + sqw)));

			return OutMatrix;
		}

		static tagFloat4x4 Identity() { tagFloat4x4 value; XMStoreFloat4x4(&value, XMMatrixIdentity()); return value; }

		// _4x4에서 Right, Up, Look, Pos의 값 중 하나를 _float4형태로 가지고 옴
		static const FLOAT4 XMMatrix4x4GetFloat4(const tagFloat4x4& _Matrix, const MATRIXROW& _eMatrixRow) {
			FLOAT4 vVector = XMLoadFloat4((FLOAT4*)&_Matrix.m[_eMatrixRow][0]);
			return vVector;
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나를 _float4형태로 가지고 옴
		static const FLOAT4 XMMatrixGetFloat4(const XMMATRIX& _Matrix, const MATRIXROW& _eMatrixRow) {
			tagFloat4x4 Value;
			XMStoreFloat4x4(&Value, _Matrix);
			FLOAT4 vVector = XMLoadFloat4((FLOAT4*)&Value.m[_eMatrixRow][0]);
			return vVector;
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나를 _float3형태로 가지고 옴
		static const FLOAT3 XMMatrix4x4GetFloat3(const tagFloat4x4& _Matrix, const MATRIXROW& _eMatrixRow) {
			FLOAT3 vVector;
			vVector.Set_Vector(XMLoadFloat3((FLOAT3*)&_Matrix.m[_eMatrixRow][0]));
			return vVector;
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나를 _float3형태로 가지고 옴
		static const FLOAT3 XMMatrixGetFloat3(const XMMATRIX& _Matrix, const MATRIXROW& _eMatrixRow) {
			FLOAT3 vVector;
			tagFloat4x4 Value;
			XMStoreFloat4x4(&Value, _Matrix);
			vVector.Set_Vector(XMLoadFloat3((FLOAT3*)&Value.m[_eMatrixRow][0]));
			return vVector;
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나를 _vector형태로 가지고 옴
		static const XMVECTOR XMMatrix4x4GetVector(const tagFloat4x4& _Matrix, const MATRIXROW& _eMatrixRow) {
			return XMLoadFloat4((FLOAT4*)&_Matrix.m[_eMatrixRow][0]);
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나를 _vector형태로 가지고 옴
		static const XMVECTOR XMMatrixGetVector(const XMMATRIX& _Matrix, const MATRIXROW& _eMatrixRow) {
			tagFloat4x4 Value;
			XMStoreFloat4x4(&Value, _Matrix);
			return XMLoadFloat4((FLOAT4*)&Value.m[_eMatrixRow][0]);
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const XMMATRIX XMMatrixSetVector(const XMVECTOR& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], _vSetVector);
			return XMLoadFloat4x4(&Matrix);
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const XMMATRIX XMMatrixSetVector(const XMVECTOR& _vSetVector, const MATRIXROW& _eMatrixRow, const tagFloat4x4& _Matrix) {
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat3((FLOAT3*)&_Matrix.m[_eMatrixRow][0], _vSetVector);
			return XMLoadFloat4x4(&Matrix);
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _4x4를 리턴
		static const tagFloat4x4 XMMatrix4x4SetVector(const XMVECTOR& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], _vSetVector);
			return Matrix;
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const tagFloat4x4  XMMatrix4x4SetVector(const XMVECTOR& _vSetVector, const MATRIXROW& _eMatrixRow, tagFloat4x4  _Matrix) {
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat3((FLOAT3*)&_Matrix.m[_eMatrixRow][0], _vSetVector);
			return Matrix;
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const XMMATRIX XMMatrixSetFloat4(const FLOAT4& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			FLOAT4 vVector = _vSetVector;
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat4((FLOAT4*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return XMLoadFloat4x4(&Matrix);
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const XMMATRIX XMMatrixSetFloat4(const FLOAT4& _vSetVector, const MATRIXROW& _eMatrixRow, const tagFloat4x4& _Matrix) {
			FLOAT4 vVector = _vSetVector;
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat4((FLOAT4*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return XMLoadFloat4x4(&Matrix);
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _4x4를 리턴
		static const tagFloat4x4 XMMatrix4x4SetFloat4(const FLOAT4& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			FLOAT4 vVector = _vSetVector;
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat4((FLOAT4*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return Matrix;
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _float4x4를 리턴
		static const  tagFloat4x4  XMMatrix4x4SetFloat4(const FLOAT4& _vSetVector, const MATRIXROW& _eMatrixRow, tagFloat4x4  _Matrix) {
			FLOAT4 vVector = _vSetVector;
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat4((FLOAT4*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return Matrix;
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const  XMMATRIX XMMatrixSetFloat3(const FLOAT3& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			FLOAT3 vVector = _vSetVector;
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return XMLoadFloat4x4(&Matrix);
		}
		// _matrix에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _float4x4를 리턴
		static const  XMMATRIX XMMatrixSetFloat3(const FLOAT3& _vSetVector, const MATRIXROW& _eMatrixRow, const tagFloat4x4& _Matrix) {
			FLOAT3 vVector = _vSetVector;
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return XMLoadFloat4x4(&Matrix);
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _matrix를 리턴
		static const  tagFloat4x4 XMMatrix4x4SetFloat3(const FLOAT3& _vSetVector, const MATRIXROW& _eMatrixRow, const XMMATRIX& _Matrix) {
			FLOAT3 vVector = _vSetVector;
			tagFloat4x4 Matrix;
			XMStoreFloat4x4(&Matrix, _Matrix);
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return Matrix;
		}
		// _4x4에서 Right, Up, Look, Pos의 값 중 하나에 값을 저장하여 _float4x4를 리턴
		static const  tagFloat4x4  XMMatrix4x4SetFloat3(const FLOAT3& _vSetVector, const MATRIXROW& _eMatrixRow, const tagFloat4x4& _Matrix) {
			FLOAT3 vVector = _vSetVector;
			tagFloat4x4 Matrix = _Matrix;
			XMStoreFloat3((FLOAT3*)&Matrix.m[_eMatrixRow][0], vVector.Get_Vector());
			return Matrix;
		}
		// 행렬에 Scale 값을 저장하여 _4x4 값을 리턴
		static const  tagFloat4x4 XMMatrix4x4SetScaling(const FLOAT3& _v3Scaling, const XMMATRIX& _Matrix) {
			XMVECTOR		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
			XMVECTOR		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
			XMVECTOR		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);

			vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
			vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
			vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

			tagFloat4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
			Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
			Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
			return Matrix;
		}
		// 행렬에 Scale 값을 저장하여 _4x4 값을 리턴
		static const  tagFloat4x4 XMMatrix4x4SetScaling(const FLOAT3& _v3Scaling, const tagFloat4x4& _Matrix) {
			XMVECTOR		vRight = XMMatrix4x4GetVector(_Matrix, MATROW_RIGHT);
			XMVECTOR		vUp = XMMatrix4x4GetVector(_Matrix, MATROW_UP);
			XMVECTOR		vLook = XMMatrix4x4GetVector(_Matrix, MATROW_LOOK);

			vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
			vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
			vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

			tagFloat4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
			Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
			Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
			return Matrix;
		}

		// 행렬에 Rotation 값을 적용하여 _4x4 값을 리턴(회전 값을 0으로 초기화 후 돌림)
		static const  tagFloat4x4 XMMatrix4x4SetRotationFix(const FLOAT3& _vRotation, const XMMATRIX& _Matrix) {
			FLOAT3 v3Angle = _vRotation;
			v3Angle.x = XMConvertToRadians(v3Angle.x);
			v3Angle.y = XMConvertToRadians(v3Angle.y);
			v3Angle.z = XMConvertToRadians(v3Angle.z);
			XMMATRIX RotationMatrix =
				XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

			OUTMATRIX OutMatrix = Get_OutMatrix(_Matrix);

			FLOAT3 v3Scale = OutMatrix.vScale;
			FLOAT3 v3Right = FLOAT3(1.f, 0.f, 0.f), v3Up = FLOAT3(0.f, 1.f, 0.f), v3Look = FLOAT3(0.f, 0.f, 1.f);
			XMVECTOR		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
				vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

			vRight = XMVector3TransformNormal(vRight, RotationMatrix);
			vUp = XMVector3TransformNormal(vUp, RotationMatrix);
			vLook = XMVector3TransformNormal(vLook, RotationMatrix);

			XMMATRIX Matrix = _Matrix;
			Matrix = XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
			Matrix = XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
			Matrix = XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);

			return Store_Matrix(Matrix);
		}
		// 행렬에 Rotation 값을 적용하여  _4x4 값을 리턴 (회전 값을 0으로 초기화 후 돌림)
		static  const tagFloat4x4 XMMatrix4x4SetRotationFix(const FLOAT3& _vRotation, const tagFloat4x4& _Matrix) {
			return XMMatrix4x4SetRotationFix(_vRotation, XMLoadFloat4x4(&_Matrix));
		}
		// 행렬에 Rotation 값을 적용하여 _4x4 값을 리턴(본래 회전 값을 적용하여 돌림)
		static const  tagFloat4x4 XMMatrix4x4SetRotationTurn(const FLOAT3& _vRotation, const XMMATRIX& _Matrix) {
			FLOAT3 v3Angle = _vRotation;
			v3Angle.x = XMConvertToRadians(v3Angle.x);
			v3Angle.y = XMConvertToRadians(v3Angle.y);
			v3Angle.z = XMConvertToRadians(v3Angle.z);
			XMMATRIX RotationMatrix =
				XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));

			OUTMATRIX OutMatrix = Get_OutMatrix(_Matrix);

			FLOAT3 v3Scale = OutMatrix.vScale;
			FLOAT3 v3Right = XMMatrixGetFloat3(_Matrix, MATROW_RIGHT), v3Up = XMMatrixGetFloat3(_Matrix, MATROW_UP),
				v3Look = XMMatrixGetFloat3(_Matrix, MATROW_LOOK);
			XMVECTOR		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
				vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

			vRight = XMVector3TransformNormal(vRight, RotationMatrix);
			vUp = XMVector3TransformNormal(vUp, RotationMatrix);
			vLook = XMVector3TransformNormal(vLook, RotationMatrix);

			XMMATRIX Matrix = _Matrix;
			Matrix = XMMatrixSetFloat3(vRight, MATROW_RIGHT, _Matrix);
			Matrix = XMMatrixSetFloat3(vUp, MATROW_UP, _Matrix);
			Matrix = XMMatrixSetFloat3(vLook, MATROW_LOOK, _Matrix);
			return Store_Matrix(Matrix);
		}
		// 행렬에 Rotation 값을 적용하여 _4x4 값을 리턴(본래 회전 값을 적용하여 돌림)
		static  const tagFloat4x4 XMMatrix4x4SetRotationTurn(const FLOAT3& _vRotation, const tagFloat4x4& _Matrix) {
			return XMMatrix4x4SetRotationTurn(_vRotation, XMLoadFloat4x4(&_Matrix));
		}

		// 행렬에 Rotation 값을 적용하여 _matrix 값을 리턴(회전 값을 0으로 초기화 후 돌림)
		static  const XMMATRIX XMMatrixSetRotationFix(const FLOAT3& _vRotation, const XMMATRIX& _Matrix) {
			tagFloat4x4 value = XMMatrix4x4SetRotationFix(_vRotation, _Matrix);
			return XMLoadFloat4x4(&value);
		}
		// 행렬에 Rotation 값을 적용하여  _matrix 값을 리턴 (회전 값을 0으로 초기화 후 돌림)
		static const  XMMATRIX XMMatrixSetRotationFix(const FLOAT3& _vRotation, const tagFloat4x4& _Matrix) {
			return XMMatrixSetRotationFix(_vRotation, XMLoadFloat4x4(&_Matrix));
		}
		// 행렬에 Rotation 값을 적용하여 _matrix 값을 리턴(본래 회전 값을 적용하여 돌림)
		static  const XMMATRIX XMMatrixSetRotationTurn(const FLOAT3& _vRotation, const XMMATRIX& _Matrix) {
			tagFloat4x4 value = XMMatrix4x4SetRotationTurn(_vRotation, _Matrix);
			return XMLoadFloat4x4(&value);
		}
		// 행렬에 Rotation 값을 적용하여 _matrix 값을 리턴(본래 회전 값을 적용하여 돌림)
		static  const XMMATRIX XMMatrixSetRotationTurn(const FLOAT3& _vRotation, const tagFloat4x4& _Matrix) {
			return XMMatrixSetRotationTurn(_vRotation, XMLoadFloat4x4(&_Matrix));
		}

		// 행렬에 Scale 값을 저장하여 _matrix 값을 리턴
		static  const XMMATRIX XMMatrixSetScaling(const FLOAT3& _v3Scaling, const XMMATRIX& _Matrix) {
			XMVECTOR		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
			XMVECTOR		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
			XMVECTOR		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);

			vRight = XMVector3Normalize(vRight) * _v3Scaling.x;
			vUp = XMVector3Normalize(vUp) * _v3Scaling.y;
			vLook = XMVector3Normalize(vLook) * _v3Scaling.z;

			tagFloat4x4 Matrix = XMMatrix4x4SetVector(vRight, MATROW_RIGHT, _Matrix);
			Matrix = XMMatrix4x4SetVector(vUp, MATROW_UP, Matrix);
			Matrix = XMMatrix4x4SetVector(vLook, MATROW_LOOK, Matrix);
			return XMLoadFloat4x4(&Matrix);
		}
		// 행렬에 Scale 값을 저장하여 _matrix 값을 리턴
		static const  XMMATRIX XMMatrixSetScaling(const FLOAT3& _v3Scaling, const tagFloat4x4& _Matrix) {
			return XMMatrixSetScaling(_v3Scaling, Load_Matrix(_Matrix));
		}

		static const tagFloat4x4 XMMatrix4x4Scaling(const FLOAT3& _vScaling, const XMMATRIX& _Matrix) {
			return Store_Matrix(XMMatrixSetScaling(_vScaling, _Matrix));
		}

		static const tagFloat4x4 XMMatrix4x4Scaling(const FLOAT3& _vScaling, const tagFloat4x4& _Matrix) {
			return Store_Matrix(XMMatrixSetScaling(_vScaling, _Matrix));
		}

		// _matrix에 Scale 값을 추출하여 리턴
		static  const FLOAT3 Get_Scaling(const XMMATRIX& _Matrix) {
			FLOAT3 v3Scale;
			XMVECTOR		vRight = XMMatrixGetVector(_Matrix, MATROW_RIGHT);
			XMVECTOR		vUp = XMMatrixGetVector(_Matrix, MATROW_UP);
			XMVECTOR		vLook = XMMatrixGetVector(_Matrix, MATROW_LOOK);
			v3Scale.x = XMVectorGetX(XMVector3Length(vRight));
			v3Scale.y = XMVectorGetY(XMVector3Length(vUp));
			v3Scale.z = XMVectorGetZ(XMVector3Length(vLook));
			return v3Scale;
		}
		// _float4x4에 Scale 값을 추출하여 리턴
		static const  FLOAT3 Get_Scaling4x4(const tagFloat4x4& _Matrix) {
			return Get_Scaling(Load_Matrix(_Matrix));
		}

		// Scale, Pos, Rotation 값을 조합하여 행렬로 리턴
		static const tagFloat4x4 Combine_Matrix4x4(const FLOAT3& _vScale, const FLOAT3& _vPos, const FLOAT4& _vRotation) {
			FLOAT3 vScale = _vScale;
			FLOAT3 vPos = _vPos;
			FLOAT4 vRotation = _vRotation;
			XMMATRIX Rot = XMMatrixRotationQuaternion(vRotation.Get_Vector());

			Rot.r[0] = XMVector3Normalize(Rot.r[0]) * vScale.x;
			Rot.r[1] = XMVector3Normalize(Rot.r[1]) * vScale.y;
			Rot.r[2] = XMVector3Normalize(Rot.r[2]) * vScale.z;
			Rot.r[3] = vPos.Get_Vector();

			tagFloat4x4 MATRIX = tagFloat4x4::Store_Matrix(Rot);
			return MATRIX;
		}
		// Scale, Pos, Rotation 값을 조합하여 행렬로 리턴
		static const XMMATRIX Combine_Matrix(const FLOAT3& _vScale, const FLOAT3& _vPos, const FLOAT4& _vRotation) {
			return Load_Matrix(Combine_Matrix4x4(_vScale, _vPos, _vRotation));
		}
		// OutMatrix 값을 받아 행렬로 만들어 리턴
		static const tagFloat4x4 Combine_Matrix4x4(const OUTMATRIX& _OutMatrix) {
			return Store_Matrix(Combine_Matrix(_OutMatrix.vScale, _OutMatrix.vPos, _OutMatrix.vQuaternionRot));
		}
		// OutMatrix 값을 받아 행렬로 만들어 리턴
		static const XMMATRIX Combine_Matrix(const OUTMATRIX& _OutMatrix) {
			return Combine_Matrix(_OutMatrix.vScale, _OutMatrix.vPos, _OutMatrix.vQuaternionRot);
		}

#pragma endregion MATRIX_SETGET

	}FLOAT4X4;

	//// Quaternion
	struct QUATERNION : public XMFLOAT4
	{
		QUATERNION() : XMFLOAT4(0, 0, 0, 1.f) {}
		constexpr QUATERNION(const float& ix, const float& iy, const float& iz, const float& iw) : XMFLOAT4(ix, iy, iz, iw) {}
		QUATERNION(const XMFLOAT3& v, const float& scalar) : XMFLOAT4(v.x, v.y, v.z, scalar) {}
		explicit QUATERNION(const XMFLOAT4& v) : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit QUATERNION(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		QUATERNION(FXMVECTOR V) { XMStoreFloat4(this, V); }
		explicit QUATERNION(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		QUATERNION(const QUATERNION&) = default;
		QUATERNION& operator=(const QUATERNION&) = default;

		QUATERNION(QUATERNION&&) = default;
		QUATERNION& operator=(QUATERNION&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const QUATERNION& q) const {
			return XMQuaternionEqual(XMLoadFloat4(this), XMLoadFloat4(&q));
		}
		bool operator != (const QUATERNION& q) const {
			return XMQuaternionNotEqual(XMLoadFloat4(this), XMLoadFloat4(&q));
		}

		// Assignment operators
		QUATERNION& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		QUATERNION& operator+= (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) + XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator-= (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) - XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator*= (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) * XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator*= (const float& S) {
			XMStoreFloat4(this, XMLoadFloat4(this) * S);
			return *this;
		}
		QUATERNION& operator/= (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) / XMLoadFloat4(&q));
			return *this;
		}

		QUATERNION& operator+ (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) + XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator- (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) - XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator* (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) * XMLoadFloat4(&q));
			return *this;
		}
		QUATERNION& operator/ (const QUATERNION& q) {
			XMStoreFloat4(this, XMLoadFloat4(this) / XMLoadFloat4(&q));
			return *this;
		}

		// Unary operators
		QUATERNION operator+ () const  noexcept { return *this; }
		QUATERNION operator- () const noexcept {
			QUATERNION R;
			XMStoreFloat4(&R, XMVectorNegate(XMLoadFloat4(this)));
			return R;
		}

		// Quaternion operations
		float Length() const noexcept {
			return XMVectorGetX(XMVector4Length(XMLoadFloat4(this)));
		}
		float LengthSquared() const noexcept {
			return XMVectorGetX(XMVector4LengthSq(XMLoadFloat4(this)));
		}

		void Normalize() noexcept {
			XMStoreFloat4(this, XMVector4Normalize(XMLoadFloat4(this)));
		}
		void Normalize(QUATERNION& result) const noexcept {
			XMStoreFloat4(&result, XMVector4Normalize(XMLoadFloat4(&result)));
		}

		void Conjugate() noexcept {
			XMStoreFloat4(this, XMQuaternionConjugate(XMLoadFloat4(this)));
		}
		void Conjugate(QUATERNION& result) const noexcept {
			XMStoreFloat4(&result, XMQuaternionConjugate(XMLoadFloat4(&result)));
		}

		void Inverse(QUATERNION& result) const noexcept {
			XMStoreFloat4(&result, XMQuaternionInverse(XMLoadFloat4(&result)));
		}

		float Dot(const QUATERNION& Q) const noexcept {
			return XMVectorGetX(XMQuaternionDot(XMLoadFloat4(this), XMLoadFloat4(&Q)));
		}

		void RotateTowards(const QUATERNION& target, const float& maxAngle) noexcept {
			RotateTowards(target, maxAngle, *this);
		}

		void __cdecl RotateTowards(const QUATERNION& target, const float& maxAngle, QUATERNION& result) const noexcept {

		}

		// Computes rotation about y-axis (y), then x-axis (x), then z-axis (z)
		FLOAT3 ToEuler() const noexcept {
			const float xx = x * x;
			const float yy = y * y;
			const float zz = z * z;

			const float m31 = 2.f * x * z + 2.f * y * w;
			const float m32 = 2.f * y * z - 2.f * x * w;
			const float m33 = 1.f - 2.f * xx - 2.f * yy;

			const float cy = sqrtf(m33 * m33 + m31 * m31);
			const float cx = atan2f(-m32, cy);
			if (cy > 16.f * FLT_EPSILON)
			{
				const float m12 = 2.f * x * y + 2.f * z * w;
				const float m22 = 1.f - 2.f * xx - 2.f * zz;

				return FLOAT3(cx, atan2f(m31, m33), atan2f(m12, m22));
			}
			else
			{
				const float m11 = 1.f - 2.f * yy - 2.f * zz;
				const float m21 = 2.f * x * y - 2.f * z * w;

				return FLOAT3(cx, 0.f, atan2f(-m21, m11));
			}
		}
		// Static functions
		static QUATERNION CreateFromAxisAngle(const FLOAT3& axis, const float& angle) noexcept {
			QUATERNION R;
			XMStoreFloat4(&R, XMQuaternionRotationAxis(XMLoadFloat3(&axis), angle));
			return R;
		}
		// Rotates about y-axis (yaw), then x-axis (pitch), then z-axis (roll)
		static QUATERNION CreateFromYawPitchRoll(const float& yaw, const float& pitch, const float& roll) noexcept {
			QUATERNION R;
			XMStoreFloat4(&R, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
			return R;
		}
		// Rotates about y-axis (angles.y), then x-axis (angles.x), then z-axis (angles.z)
		static QUATERNION CreateFromYawPitchRoll(const FLOAT3& angles) noexcept {
			QUATERNION R;
			XMStoreFloat4(&R, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&angles)));
			return R;
		}
		static QUATERNION CreateFromRotationMatrix(const FLOAT4X4& M) noexcept {
			QUATERNION R;
			XMStoreFloat4(&R, XMQuaternionRotationMatrix(XMLoadFloat4x4(&M)));
			return R;
		}
		// Lerp
		static void Lerp(const QUATERNION& q1, const QUATERNION& q2, const float& t, QUATERNION& result) noexcept {
			const XMVECTOR Q0 = XMLoadFloat4(&q1);
			const XMVECTOR Q1 = XMLoadFloat4(&q2);

			const XMVECTOR dot = XMVector4Dot(Q0, Q1);

			XMVECTOR R;
			if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
			{
				R = XMVectorLerp(Q0, Q1, t);
			}
			else
			{
				const XMVECTOR tv = XMVectorReplicate(t);
				const XMVECTOR t1v = XMVectorReplicate(1.f - t);
				const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
				const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
				R = XMVectorSubtract(X0, X1);
			}

			XMStoreFloat4(&result, XMQuaternionNormalize(R));
		}
		static QUATERNION Lerp(const QUATERNION& q1, const QUATERNION& q2, const float& t) noexcept {
			using namespace DirectX;
			const XMVECTOR Q0 = XMLoadFloat4(&q1);
			const XMVECTOR Q1 = XMLoadFloat4(&q2);

			const XMVECTOR dot = XMVector4Dot(Q0, Q1);

			XMVECTOR R;
			if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
			{
				R = XMVectorLerp(Q0, Q1, t);
			}
			else
			{
				const XMVECTOR tv = XMVectorReplicate(t);
				const XMVECTOR t1v = XMVectorReplicate(1.f - t);
				const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
				const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
				R = XMVectorSubtract(X0, X1);
			}

			QUATERNION result;
			XMStoreFloat4(&result, XMQuaternionNormalize(R));
			return result;
		}
		// Slerp
		static void Slerp(const QUATERNION& q1, const QUATERNION& q2, const float& t, QUATERNION& result) noexcept {
			XMStoreFloat4(&result, XMQuaternionSlerp(XMLoadFloat4(&q1), XMLoadFloat4(&q2), t));
		}
		static QUATERNION Slerp(const QUATERNION& q1, const QUATERNION& q2, float t) noexcept {
			QUATERNION result;
			XMStoreFloat4(&result, XMQuaternionSlerp(XMLoadFloat4(&q1), XMLoadFloat4(&q2), t));
			return result;
		}
		// ConCaternate
		static void Concatenate(const QUATERNION& q1, const QUATERNION& q2, QUATERNION& result) noexcept {
			XMStoreFloat4(&result, XMQuaternionMultiply(XMLoadFloat4(&q1), XMLoadFloat4(&q2)));
		}
		static QUATERNION Concatenate(const QUATERNION& q1, const QUATERNION& q2) noexcept {
			QUATERNION result;
			XMStoreFloat4(&result, XMQuaternionMultiply(XMLoadFloat4(&q1), XMLoadFloat4(&q2)));
			return result;
		}
		// From To Rotation
		static void __cdecl FromToRotation(const FLOAT3& fromDir, const FLOAT3& toDir, QUATERNION& result) noexcept {
			result = FromToRotation(fromDir, toDir);
		}
		static QUATERNION FromToRotation(const FLOAT3& fromDir, const FLOAT3& toDir) noexcept {
			QUATERNION result;
			FromToRotation(fromDir, toDir, result);
			return result;
		}
		// Look Rotation 
		static void __cdecl LookRotation(const FLOAT3& forward, const FLOAT3& up, QUATERNION& result) noexcept {
			result = LookRotation(forward, up);
		}
		static QUATERNION LookRotation(const FLOAT3& forward, const FLOAT3& up) noexcept {
			QUATERNION result;
			LookRotation(forward, up, result);
			return result;
		}
		// Angle
		static float Angle(const QUATERNION& q1, const QUATERNION& q2) noexcept
		{
			const XMVECTOR Q0 = XMLoadFloat4(&q1);
			const XMVECTOR Q1 = XMLoadFloat4(&q2);

			// We can use the conjugate here instead of inverse assuming q1 & q2 are normalized.
			XMVECTOR R = XMQuaternionMultiply(XMQuaternionConjugate(Q0), Q1);

			const float rs = XMVectorGetW(R);
			R = XMVector3Length(R);
			return 2.f * atan2f(XMVectorGetX(R), rs);
		}

		// Qot 값을 _float3 Rotation 값으로 리턴
		static const FLOAT3 Convert_QuaternionToRotation(const FLOAT4& _vQuarter) {
			float sqw = _vQuarter.w * _vQuarter.w;
			float sqx = _vQuarter.x * _vQuarter.x;
			float sqy = _vQuarter.y * _vQuarter.y;
			float sqz = _vQuarter.z * _vQuarter.z;

			FLOAT3 vQuat = FLOAT3(0.f, 0.f, 0.f);
			vQuat.x = XMConvertToDegrees(asinf(2.f * (_vQuarter.w * _vQuarter.x -
				_vQuarter.y * _vQuarter.z)));
			vQuat.y = XMConvertToDegrees(atan2(2.f * (_vQuarter.x * _vQuarter.z +
				_vQuarter.w * _vQuarter.y), (-sqx - sqy + sqz + sqw)));
			vQuat.z = XMConvertToDegrees(atan2(2.f * (_vQuarter.x * _vQuarter.y +
				_vQuarter.w * _vQuarter.z), (-sqx + sqy - sqz + sqw)));

			return vQuat;
		}
		// _float3 Rotation 값을 Qot으로 리턴
		static const FLOAT4 Convert_RotationToQuaternion(const FLOAT3& _vRotation) {
			FLOAT3 v3Rotation = _vRotation;
			v3Rotation.x = XMConvertToRadians(v3Rotation.x);
			v3Rotation.y = XMConvertToRadians(v3Rotation.y);
			v3Rotation.z = XMConvertToRadians(v3Rotation.z);
			FLOAT4 vQuaternion = XMQuaternionRotationRollPitchYawFromVector(v3Rotation.Get_Vector());

			return vQuaternion;
		}
	};

	//-----------------------------------------------------------------------------
	 // Plane
	struct PLANE : public XMFLOAT4
	{
		PLANE() : XMFLOAT4(0.f, 1.f, 0.f, 0.f) {}
		constexpr PLANE(float ix, float iy, float iz, float iw) : XMFLOAT4(ix, iy, iz, iw) {}
		PLANE(const XMFLOAT3& normal, float d) : XMFLOAT4(normal.x, normal.y, normal.z, d) {}
		PLANE(const XMFLOAT3& point1, const XMFLOAT3& point2, const XMFLOAT3& point3) {
			XMStoreFloat4(this, XMPlaneFromPoints(XMLoadFloat3(&point1), XMLoadFloat3(&point2), XMLoadFloat3(&point3)));
		}
		PLANE(const XMFLOAT3& point, const XMFLOAT3& normal) {
			XMStoreFloat4(this, XMPlaneFromPointNormal(XMLoadFloat3(&point), XMLoadFloat3(&normal)));
		}
		explicit PLANE(const XMFLOAT4& v) : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit PLANE(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		PLANE(FXMVECTOR V) { XMStoreFloat4(this, V); }
		explicit PLANE(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		PLANE(const PLANE&) = default;
		PLANE& operator=(const PLANE&) = default;

		PLANE(PLANE&&) = default;
		PLANE& operator=(PLANE&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const PLANE& p) const {
			return XMPlaneEqual(XMLoadFloat4(this), XMLoadFloat4(&p));
		}
		bool operator != (const PLANE& p) const {
			return XMPlaneNotEqual(XMLoadFloat4(this), XMLoadFloat4(&p));
		}
		// Assignment operators
		PLANE& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }

		// Properties
		FLOAT3 Normal() const { return FLOAT3(x, y, z); }
		void Normal(const FLOAT3& normal) { x = normal.x; y = normal.y; z = normal.z; }

		float D() const { return w; }
		void D(float d) { w = d; }

		// Plane operations
		void Normalize() {
			XMStoreFloat4(this, XMPlaneNormalize(XMLoadFloat4(this)));
		}
		void Normalize(PLANE& result) const {
			XMStoreFloat4(&result, XMPlaneNormalize(XMLoadFloat4(this)));
		}

		float Dot(const FLOAT4& v) const {
			return XMVectorGetX(XMPlaneDot(XMLoadFloat4(this), XMLoadFloat4(&v)));
		}
		float DotCoordinate(const FLOAT3& position) const {
			return XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(this), XMLoadFloat3(&position)));
		}
		float DotNormal(const FLOAT3& normal) const {
			return XMVectorGetX(XMPlaneDotNormal(XMLoadFloat4(this), XMLoadFloat3(&normal)));
		}

		// Static functions
		static void Transform(const PLANE& plane, const FLOAT4X4& M, PLANE& _result) {
			const XMVECTOR p = XMLoadFloat4(&plane);
			const XMMATRIX m0 = XMLoadFloat4x4(&M);
			XMStoreFloat4(&_result, XMPlaneTransform(p, m0));
		}
		static PLANE Transform(const PLANE& plane, const FLOAT4X4& M) {
			PLANE t;
			Transform(plane, M, t);
			return t;
		}

		static void Transform(const PLANE& plane, const QUATERNION& rotation, PLANE& result) {
			const XMVECTOR p = XMLoadFloat4(&plane);
			const XMVECTOR q = XMLoadFloat4(&rotation);
			XMVECTOR X = XMVector3Rotate(p, q);
			X = XMVectorSelect(p, X, g_XMSelect1110); // result.d = plane.d
			XMStoreFloat4(&result, X);
		}
		static PLANE Transform(const PLANE& plane, const QUATERNION& rotation) {
			PLANE t;
			Transform(plane, rotation, t);
			return t;
		}
		// Input quaternion must be the inverse transpose of the transformation
	};

#pragma endregion MATRICES
