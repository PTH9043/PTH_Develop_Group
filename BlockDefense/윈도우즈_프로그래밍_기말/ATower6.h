#pragma once
#include"ATower.h"

class ATower6 : public ATower
{
public:
	ATower6() {}
	ATower6(class UGame* game, int updateOrder = 1);
	virtual ~ATower6() { Clear(); }

	ATower* Copy() override { return new ATower6(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};
