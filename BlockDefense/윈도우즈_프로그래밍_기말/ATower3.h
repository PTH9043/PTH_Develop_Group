#pragma once
#include"ATower.h"

class ATower3 : public ATower
{
public:
	ATower3() {}
	ATower3(class UGame* game, int updateOrder = 1);
	virtual ~ATower3() { Clear(); }

	ATower* Copy() override { return new ATower3(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};