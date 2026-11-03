#pragma once
#include"ATower.h"

class ATower5 : public ATower
{
public:
	ATower5() {}
	ATower5(class UGame* game, int updateOrder = 1);
	virtual ~ATower5() { Clear(); }

	ATower* Copy() override { return new ATower5(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};