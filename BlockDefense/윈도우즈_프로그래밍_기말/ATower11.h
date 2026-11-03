#pragma once
#include"ATower.h"

class ATower11 : public ATower
{
public:
	ATower11() {}
	ATower11(class UGame* game, int updateOrder = 1);
	virtual ~ATower11() { Clear(); }

	ATower* Copy() override { return new ATower11(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};