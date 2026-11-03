#pragma once
#include<math.h>
#include<atlimage.h>
#include"resource.h"


#define WINDOW_START_POS_X 0
#define WINDOW_START_POS_Y 0
#define WINDOW_START_WIDTH 1000
#define WINDOW_START_HEIGHT 800
#define CHILD_START_POS_X 10
#define CHILD_START_POS_Y 10
#define CHILD_START_WIDTH 800
#define CHILD_START_HEIGHT 600
#define RF_START_POS_X 100
#define RF_START_POS_Y 100
#define RF_START_WIDTH 600
#define RF_START_HEIGHT 600
#define DRAW_SUPPLEMENT 0.5f
#define DELETE_MEMORY(x) if(x != nullptr) {delete x; x = nullptr;}
#define DELETE_MEMORY_ARR(x) if(x != nullptr) {delete[] x; x= nullptr;}
#define DELTA_TIME 10

#define MW_ID_TEXT_TURN 10800
#define MW_ID_TEXT_SURVIVENUM 10801
#define MW_ID_TEXT_HAVEGOLD 10802
#define MW_ID_BUTTON_START 10900
#define MW_ID_BUTTON_SHOP 10901
#define MW_ID_BUTTON_INTENSTY 10902
#define MW_ID_BUTTON_INFO 10903
#define MW_ID_BUTTON_CHEAT 10904

#define RFCW_ID_TEXT_GOLD 11000
#define RFCW_ID_LIST_EXPLAIN 11001
#define RFCW_ID_BUTTON_REINFORCE 11002
#define RFCW_ID_BUTTON_EXIT 11003
#define RFCW_ID_PICTURE_SHOW 11004

#define IFCW_ID_TEXT_GOLD 12000
#define IFCW_ID_LIST_EXPLAIN 12001
#define IFCW_ID_BUTTON_SELL 12002
#define IFCW_ID_BUTTON_EXIT 12003
#define IFCW_ID_PICTURE_SHOW 12004

#define COMP_ID_SPRITE_SIMPLE 10101
#define COMP_ID_SPRITE_ANIM 10102

// 위치에 대한 클래스
class Vector2
{
public:
	Vector2() { x = 0.0; y = 0.0; }
	Vector2(double x, double y) { this->x = x; this->y = y; }

	Vector2 operator+(Vector2 v) { return Vector2{ x + v.x, y + v.y }; }
	Vector2 operator+=(Vector2 v) { return Vector2{ x + v.x, y + v.y }; }
	Vector2 operator-(Vector2 v) { return Vector2{ x - v.x, y - v.y }; }
	Vector2 operator-=(Vector2 v) { return Vector2{ x - v.x, y - v.y }; }
	Vector2 operator *(Vector2 v) { return Vector2{ x * v.x, y * v.y }; }
	Vector2 operator *=(Vector2 v) { return Vector2{ x * v.x, y * v.y }; }
	Vector2 operator *(double r) { return Vector2{ x * r, y * r }; }
	Vector2 operator *(int r) { return Vector2{ x * r, y * r }; }
	double x = 0.0, y = 0.0;

	void Normalize()
	{
		double L = static_cast<double>(sqrtl(x * x + y * y));
		x = x / L;
		y = y / L;
	}
	void Set(double x, double y) { this->x = x; this->y = y; }
};
// 커스텀 리스트
template<typename T>
class CustomList
{
	template<typename T>
	struct STNode
	{
	public:
		STNode<T>* next;
		STNode<T>* prev;
		T data;
		int index;
	};
public:
	CustomList()
	{
		stHead = new STNode<T>();
		stHead->next = stHead;
		stHead->prev = stHead;
	}
	~CustomList()
	{
		Clear();
		DELETE_MEMORY(stHead);
	}
	void Insert(T data)
	{
		STNode<T>* newNode = new STNode<T>();
		newNode->data = data;
		if (stHead->next == stHead)
		{
			stHead->next = newNode; stHead->prev = newNode;
			newNode->next = stHead; newNode->prev = stHead;
			newNode->index = 0;
		}
		else
		{
			newNode->next = stHead; newNode->prev = stHead->prev;
			newNode->index = stHead->prev->index + 1;
			stHead->prev->next = newNode; stHead->prev = newNode;
		}
	}
	int GetLength() const
	{
		if (stHead->next == stHead) return 0;
		return stHead->prev->index + 1;
	}
	T GetData(int index)
	{
		T data = T();
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->index == index)
				{
					data = findNode->data;
					break;
				}
			}
		}
		return data;
	}
	void DeleteToIndex(int index)
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->index == index)
				{
					findNode->prev->next = findNode->next;
					findNode->next->prev = findNode->prev;
					DELETE_MEMORY(findNode);
					findNode = stHead->next;
					int index = 0;
					for (; findNode != stHead; findNode = findNode->next)
						findNode->index = index++;
					break;
				}
			}
		}
	}
	void DeleteToData(T data)
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->data == data)
				{
					findNode->prev->next = findNode->next;
					findNode->next->prev = findNode->prev;
					DELETE_MEMORY(findNode);
					findNode = stHead->next;
					int index = 0;
					for (; findNode != stHead; findNode = findNode->next)
						findNode->index = index++;

					break;
				}
			}
		}
	}
	void Clear()
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			while (findNode != stHead)
			{
STNode<T>* tmp = findNode;
findNode = findNode->next;
tmp->prev->next = tmp->next;
DELETE_MEMORY(tmp);
			}
			stHead->next = stHead;
			stHead->prev = stHead;
		}
	}
protected:
	STNode<T>* stHead;
};
// 그림
class  UPicture
{
public:
	UPicture() { hBitmap = NULL;  iID = 0; iWidth = 0; iHeight = 0; }
	UPicture(HBITMAP bitmap, int id, int width, int height)
	{
		this->hBitmap = bitmap;
		this->iID = id;
		this->iWidth = width; this->iHeight = height;
		lpName = L"";
	}
	UPicture(HBITMAP bitmap, const TCHAR* name, int width, int height)
	{
		this->hBitmap = bitmap;
		this->iID = 0;
		this->iWidth = width; this->iHeight = height;
		lpName = name;
	}
	~UPicture() { DeleteObject(hBitmap); }

	HBITMAP GetBitmap() const { return hBitmap; }
	const TCHAR* GetName() const { return lpName; }
	int GetID() const { return iID; }
	int GetWidth() const { return iWidth; }
	int GetHeight() const { return iHeight; }
protected:
	HBITMAP hBitmap;
	const TCHAR* lpName = NULL;
	int iID = 0, iWidth = 0, iHeight = 0;
};
// 포지션
class UPosition
{
public:
	UPosition() {}
	UPosition(Vector2 pos)
	{
		this->v2Pos = pos;
		rCollisionRect = RECT{ static_cast<int>(pos.x - iLength / 2), static_cast<int>(pos.y - iLength / 2),
			static_cast<int>(pos.x + iLength / 2), static_cast<int>(pos.y + iLength / 2) };
	}
	~UPosition() {}

	RECT GetCollisionRect() const { return rCollisionRect; }
	Vector2 GetPos() const { return v2Pos; }
	bool IsDraw() const { return bIsDraw; }
	void SetDraw(bool draw) { this->bIsDraw = draw; }
	bool IsPutonTower() const { return bIsPutonTower; }
	void SetPutonTower(bool putOnTower) { this->bIsPutonTower = putOnTower; }
protected:
	RECT rCollisionRect = RECT{ 0, 0, 0, 0 };
	Vector2 v2Pos;
	bool bIsDraw = true, bIsPutonTower = false;
	int iLength = 47;
};

class UTowerStatus
{
public:
	UTowerStatus(){}
	UTowerStatus(double power, double range, double speed, double increaseAttackPower, double increaseAttackSpeed)
	{
		this->dAttackPower = power;
		this->dAttackRange = range;
		this->dAttackSpeed = speed;
		this->dIncreaseAttackPower = increaseAttackPower;
		this->dIncreaseAttackSpeed = increaseAttackSpeed;
	}

	void ReinforceTower()
	{
		dPlusAttackPower = (dIncreaseAttackPower * (iReinforceGrade));
		dPlusAttackSpeed = (dIncreaseAttackSpeed * (iReinforceGrade));
		iReinforceGrade++;
	}

	double GetAttackPower() const { return (dAttackPower + dPlusAttackPower); }
	double GetAttackSpeed() const { return (dAttackSpeed + dPlusAttackSpeed); }
	double GetAttackRange() const { return dAttackRange; }
	double GetIncreaseAttackPower() const { return dIncreaseAttackPower; }
	double GetIncreaseAttackSpeed() const { return dIncreaseAttackSpeed; }

	int GetReinforceGrade() const { return iReinforceGrade; }
protected:
	double dAttackPower = 0.0, dAttackRange = 0.0, dAttackSpeed = 0.0;
	double dIncreaseAttackPower = 0.0, dIncreaseAttackSpeed = 0.0;
	double dPlusAttackPower = 0.0, dPlusAttackSpeed = 0.0;
	int iReinforceGrade = 1;
};

class UCollision
{
public:
	static bool PointInCircle(Vector2 cPos, double width, double height, Vector2 p)
	{
		double x = (cPos.x - p.x), y = (cPos.y - p.y);
		width = width / 2; height = height / 2;
		x = x * x; y = y * y;
		if (width * width >= x && height * height >= y) return true;
		return false;
	}

	static bool PointInRect(RECT r, Vector2 p)
	{
		if((r.left < p.x && p.x < r.right) && (r.top < p.y && p.y < r.bottom))	return true;
		return false;
	}
	static bool CircleToRectangle(Vector2 cPos, double cWidth, double cHeight, const RECT rRect)
	{
		int nCenterX = static_cast<int>(cPos.x);
		int nCenterY = static_cast<int>(cPos.y);

		// 원의 중심이 사각형 안에 있는지 파악
		if ((rRect.left <= nCenterX && nCenterX <= rRect.right) &&
			(rRect.top <= nCenterY && nCenterY <= rRect.bottom))
		{
			RECT rcTmp = RECT
			{
				static_cast<int>(rRect.left - cWidth / 2),
				static_cast<int>(rRect.top - cHeight / 2),
				static_cast<int>(rRect.right + cWidth / 2),
				static_cast<int>(rRect.bottom + cHeight / 2)
			};

			if ((rcTmp.left < nCenterX && nCenterX < rcTmp.right) &&
				(rcTmp.top < nCenterY && nCenterY < rcTmp.bottom))
				return TRUE;
		}
		else
		{
			// 각 꼭짓점들 중 하나라도 원 안에 들어가 있으면 충돌
			Vector2 pPos = Vector2{ static_cast<double>(rRect.left), static_cast<double>(rRect.top) };
			if (PointInCircle(cPos, cWidth, cHeight, pPos) == TRUE) return true;
			pPos = Vector2{ static_cast<double>(rRect.left), static_cast<double>(rRect.bottom) };
			if (PointInCircle(cPos, cWidth, cHeight, pPos) == TRUE) return true;
			pPos = Vector2{ static_cast<double>(rRect.right), static_cast<double>(rRect.bottom) };
			if (PointInCircle(cPos, cWidth, cHeight, pPos) == TRUE) return true;
			pPos = Vector2{ static_cast<double>(rRect.right), static_cast<double>(rRect.bottom) };
			if (PointInCircle(cPos, cWidth, cHeight, pPos) == TRUE) return true;
		}

		return false;
	}


};