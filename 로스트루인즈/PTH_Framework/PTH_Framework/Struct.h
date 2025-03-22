#pragma once
#include "pch.h"

typedef struct tagVector2
{
public:
	float x;
	float y;
public:
	tagVector2() : x(0.f), y(0.f) {}
	tagVector2(float _x, float _y) : x(_x), y(_y) {}
	// Vector Vector
	tagVector2 operator +(const tagVector2& v2)
	{
		return VECTOR2(x + v2.x, y + v2.y);
	}
	tagVector2 operator -(const tagVector2& v2)
	{
		return VECTOR2(x - v2.x, y - v2.y);
	}
	tagVector2 operator *(const tagVector2& v2)
	{
		return VECTOR2(x * v2.x, y * v2.y);
	}
	tagVector2 operator /(const tagVector2& v2)
	{
		return VECTOR2(x / v2.x, y / v2.y);
	}
	tagVector2 operator +=(const tagVector2& v2)
	{
		*this = *this + v2;
		return (*this);
	}
	tagVector2 operator -=(const tagVector2& v2)
	{
		*this = *this - v2;
		return (*this);
	}
	tagVector2 operator *=(const tagVector2& v2)
	{
		*this = *this * v2;
		return (*this);
	}
	tagVector2 operator /=(const tagVector2& v2)
	{
		*this = *this / v2;
		return (*this);
	}
	// Vector float 
	tagVector2 operator* (const float& f)
	{
		return VECTOR2(x * f, y * f);
	}
	tagVector2 operator/ (const float& f)
	{
		return VECTOR2(x / f, y / f);
	}
	tagVector2 operator *= (const float& f)
	{
		*this = *this * f;
		return (*this);
	}
	tagVector2 operator /= (const float& f)
	{
		*this = *this / f;
		return (*this);
	}
	// Normalize
	void Normalize()
	{
		float fLength = Get_Length_Sqrt();
		if(x != 0)
		{
			x /= fLength;
		}
		if (y != 0)
		{
			y /= fLength;
		}
	}
	float Get_Normalize()
	{
		VECTOR2 v2 = VECTOR2(x, y);
		float fLength = v2.Get_Length_Sqrt();
		if (x != 0)
		{
			v2.x /= fLength;
		}
		if (y != 0)
		{
			v2.y /= fLength;
		}
		return fLength;
	}
	void Reset()
	{
		this->x = 0.f;
		this->y = 0.f;
	}
	// x*x + y*y
	float Get_Length()
	{
		if (x != 0 && y != 0)
			return ((x * x) + (y * y));
		else if (x == 0 && y != 0.f)
			return (y * y);
		else if (y == 0 && x != 0.f)
			return (x * x);
		return 0.f;
	}
	// sqrtf(x*x  + y*y)
	float Get_Length_Sqrt()
	{
		if (x != 0 && y != 0)
			return sqrtf((x * x) + (y * y));
		else if (x == 0 && y != 0.f)
			return sqrtf(y * y);
		else if (y == 0 && x != 0.f)
			return sqrtf(x * x);
		return 0.f;
	}
}VECTOR2;
typedef struct tagLineInfo
{
public:
	VECTOR2		v2LeftPos;
	VECTOR2		v2RightPos;
	float				fInclination;
public:
	tagLineInfo()
	{
		ZeroMemory(&v2LeftPos, sizeof(VECTOR2));
		ZeroMemory(&v2RightPos, sizeof(VECTOR2));
		fInclination = 0.f;
	}
}LINEINFO;

typedef struct tagTimer
{
	DWORD dwTime;
	int		   iPassTime;

	tagTimer()
	{
		dwTime = GetTickCount();
		iPassTime = 0;
	}
	tagTimer(const int& _iPassTime)
	{
		dwTime = GetTickCount();
		iPassTime = _iPassTime;
	}

	bool Is_PassTime()
	{
		if (iPassTime + dwTime < GetTickCount())
			return true;

		return false;
	}
}TIMER;

typedef struct tagStatus
{
public:
	tagStatus()
	{
		iCurHp = 0;
		iMaxHp = 0;
		iCurMp = 0;
		iMaxMp = 0;
		iAtkPower = 0;
		iAtkSpeed = 0;
		iMoveSpeed = 0;
	}
	int iCurHp;
	int iMaxHp;
	int iCurMp;
	int iMaxMp;
	int iAtkPower;
	int iAtkSpeed;
	int iMoveSpeed;
}STATUS;

typedef struct tagRGB
{
	int iR;
	int iG;
	int iB;

	tagRGB() { iR = 0; iG = 0; iB = 0; }
	tagRGB(const int& _iR, const int& _iG, const int& _iB)
	{
		this->iR = _iR;
		this->iG = _iG;
		this->iB = _iB;
	}
}RGBINFO;