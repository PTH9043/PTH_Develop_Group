#pragma once
#include"AActor.h"

class AEnemy : public AActor
{
public:
	AEnemy(){}
	AEnemy(UGame* game, int updateOrder = 1);
	virtual ~AEnemy(){}

	void Initialize() override;
	// 업데이트 하는 함수
	void Update(float deltaSecond) override;

	virtual AEnemy* Copy() { return nullptr; }
protected:
	virtual void Move(float deltaSecond);

	void Destory();
public:
	RECT GetCollisionRect() const { return rCollisionRect; }

	double GetHp() const { return dHp; }
	void SetHp(double hp) { this->dHp = hp; }

	double GetSpeed() const { return dSpeed; }
	void SetSpeed(double speed) { this->dSpeed = speed; }

	bool IsDestory() const { return bIsDestory; }
	void SetDestory(bool destory) { this->bIsDestory = destory; }

protected:
	RECT rCollisionRect = RECT{ 0, 0, 0,0 };
	double dHp = 0.0,  dSpeed = 0.0;
	bool bIsArrive = true,  bIsDestory = false;
	int iPosIndex = 0;
};

