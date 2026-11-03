#pragma once
#include"AEnemy.h"

class AMob : public AEnemy
{
public:
	AMob(){}

	AMob(UGame* game, double hp, HBRUSH color, int updateOrder = 1);

	AEnemy* Copy()
	{ 
		AEnemy* enemy = new AMob(uGame, dHp, hColor);
		enemy->SetSpeed(dSpeed);
		return enemy;
	}

	void Initialize() override { AEnemy::Initialize(); dWidth = 48; dHeight = 48; }
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:
	HBRUSH hColor = NULL;
};

