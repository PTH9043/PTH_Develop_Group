#pragma once
#include"ATower.h"

class ATower4 : public ATower
{
public:
	ATower4() {}
	ATower4(class UGame* game, int updateOrder = 1);
	virtual ~ATower4() { Clear(); }

	ATower* Copy() override { return new ATower4(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};