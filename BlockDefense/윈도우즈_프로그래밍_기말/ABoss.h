#pragma once
#include"AEnemy.h"

class ABoss : public AEnemy
{
public:
	ABoss() {}

	ABoss(UGame* game, double hp, HBRUSH color, int updateOrder = 1);

	AEnemy* Copy()
	{
		AEnemy* enemy = new ABoss(uGame, dHp, hColor);
		enemy->SetSpeed(dSpeed);
		return enemy;
	}
	void Initialize() override { AEnemy::Initialize();  dWidth = 96; dHeight = 96; }
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:
	HBRUSH hColor = NULL;
};

