#pragma once
#include"AActor.h"

class ATower : public AActor
{
public:
	ATower() {}
	ATower(class UGame* game, int updateOrder = 1);
	virtual ~ATower() { Clear(); }

	virtual ATower* Copy(){ return nullptr;}
	// 초기화 함수
	void Initialize() override;
	// 업데이트 하는 함수
	void Update(float deltaSecond) override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
	// 마우스가 클릭되었을 때
	void ClickMouseButton(Vector2 mousePosition);
	// 마우스 드래그
	void Drag(Vector2 mousePosition);
	// Range
	void UpdateRange();

	void Clear() override;
protected:
	void EnemyInRange(CustomList<class AEnemy*>* enemyList);
public:
	bool IsClicked() const { return bIsClicked; }
	void SetClicked(bool click) { bIsClicked = click; }

	bool IsInit() const { return bIsInit; }
	void SetInit(bool init) { this->bIsInit = init; }

	CustomList<class ABullet*>* GetBulletList() const { return clBulletList; }
	class USimpleSpriteComponent* GetSimpleSpriteComp() const { return uSImpleSpriteComp; }
	void SetTowerStatus(UTowerStatus towerStatus) { this->uTowerStatus = towerStatus; }
	UTowerStatus GetTowerStatus() const { return uTowerStatus; }
	int GetPayGold() const { return iPayGold; }

	int GetPayReinForceGold() const { return iPayReinForceGold; }
	void SetPayReinForceGold(int gold) { this->iPayReinForceGold = gold; }

	int GetPrevReinForceGold() const { return iPrevReinForceGold; }
	void SetPrevReinForceGold(int gold) { this->iPrevReinForceGold = gold; }

	int GetSellGold() const { return iSellGold; }
	void SetSellGold(int gold) { iSellGold = gold; }

	void SetPosIndex(int index) { this->iPosIndex = index; }
	int GetPosIndex() const { return iPosIndex; }
protected:
	CustomList<class ABullet*>* clBulletList = nullptr;
	class USimpleSpriteComponent * uSImpleSpriteComp = nullptr;
	UTowerStatus uTowerStatus;
	HBRUSH hRangeColor = NULL;
	RECT rRangeRect = RECT{ 0, 0, 0, 0 };
	int iPayGold = 0, iPayReinForceGold = 0, iPrevReinForceGold = 0, iSellGold = 0, iPosIndex = 0;
	double dWaitTime = 0.0;
	bool bIsClicked = false;
	bool bIsInit = false;
};

