#pragma once
#include"ATower.h"

class ATower8 : public ATower
{
public:
	ATower8() {}
	ATower8(class UGame* game, int updateOrder = 1);
	virtual ~ATower8() { Clear(); }

	ATower* Copy() override { return new ATower8(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};