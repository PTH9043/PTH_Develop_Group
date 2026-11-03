#pragma once
#include"AActor.h"

class ABackground : public AActor
{
public:
	ABackground(){}
	ABackground(class UGame* game, int updateOrder = 1);
	virtual ~ABackground();

	// 액터의 업데이트 함수
	 void Update(float deltaSecond) override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
	void Initialize() override;
	void TowerInRect();
protected:
	// 가상 함수
	virtual void AddPos() {}

	void Clear() override;
public:
	CustomList<Vector2>* GetWayPointList() const { return clWayPointList; }
	CustomList<UPosition*>* GetTowerPosList() const { return clTowerPosList; }
	void SetPutonState(bool putonState) { this->bIsPutonState = putonState; }
	bool IsPutonState() const { return bIsPutonState; }
protected:
	class UBackgroundSpriteComponent* uBackgroundSpriteComp = nullptr;
	CustomList<UPosition*>* clTowerPosList = nullptr;
	CustomList<Vector2>* clWayPointList = nullptr;
	class ATower* aDeleteTower = nullptr;
	bool bIsPutonState = false;
	HBRUSH hCanPutonTowerBrush = NULL;
};

