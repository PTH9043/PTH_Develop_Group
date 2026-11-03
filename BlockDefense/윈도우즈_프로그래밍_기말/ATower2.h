#pragma once
#include"ATower.h"

class ATower2 : public ATower
{
public:
	ATower2() {}
	ATower2(class UGame* game, int updateOrder = 1);
	virtual ~ATower2() { Clear(); }

	ATower* Copy() override { return new ATower2(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};