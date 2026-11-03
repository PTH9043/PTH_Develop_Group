#pragma once
#include"UManager.h"

class UPlayerManager : public UManager
{
public:
	UPlayerManager() {}
	UPlayerManager(class UGame* game);
	virtual ~UPlayerManager() { Reset(); }

	void AddTower(class ATower* tower) { clTowerList->Insert(tower); }
	void DeleteTower(class ATower* tower) { clTowerList->DeleteToData(tower); }

	void Initialize() override;
	// 업데이트 하는 함수
	void Update(float deltaSecond);
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap);
	void ClickLeftMouserButton(Vector2 mousePosition);
	void MouseDrag(Vector2 mousePosition);

	void Clear();
protected:
	void Reset() override { DELETE_MEMORY(clTowerList); }
public:
	CustomList<class ATower*>* GetTowerList() const { return clTowerList; }

	void SetSelectedTower(class ATower* tower) { this->aSelectedTower = tower; }
	class ATower* GetSelectedTower() const { return aSelectedTower; }

	void SetGold(int gold) { this->iGold = gold; }
	int GetGold() const { return iGold; }

	void SetSurviveCount(int survive) { this->iSurviveCount = survive; }
	int GetSurviveCount() const { return iSurviveCount; }
protected:
	CustomList<class ATower*>* clTowerList = nullptr;
	class ATower* aSelectedTower = nullptr;
	int iGold = 0, iSurviveCount = 20;
};

