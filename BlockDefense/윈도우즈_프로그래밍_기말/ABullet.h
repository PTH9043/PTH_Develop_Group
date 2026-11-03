#pragma once
#include"AActor.h"

class ABullet : public AActor
{
public:
	ABullet(){}
	ABullet(UGame* game, class ATower * tower, class AEnemy * enemy, int updateOrder = 1);
	~ABullet() {}

	void Initialize() override;

	void Update(float deltaTime) override;

	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;

	void Clear() override{ }
protected:
	void Move(float deltaTime);
	void CollisionEnemy();
public:
	void SetDamage(double damage) { this->dDamage = damage; }
	double GetDamage() const { return dDamage; }

	bool IsDestory() const { return bIsDestory; }
	void SetDestory(bool destory) { this->bIsDestory = destory; }

protected:
	class USimpleSpriteComponent* uSimpleSpriteComp = nullptr;
	class ATower* aTower = nullptr;
	class AEnemy* aTargetEnemy = nullptr;
	RECT rCollisioRect = RECT{ 0, 0, 0, 0 };
	bool bIsDestory = false;
	double dDamage = 0.0;
};

