#pragma once
#include"ATower.h"

class ATower9 : public ATower
{
public:
	ATower9() {}
	ATower9(class UGame* game, int updateOrder = 1);
	virtual ~ATower9() { Clear(); }

	ATower* Copy() override { return new ATower9(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};