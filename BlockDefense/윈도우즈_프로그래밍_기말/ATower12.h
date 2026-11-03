#pragma once
#include"ATower.h"

class ATower12 : public ATower
{
public:
	ATower12() {}
	ATower12(class UGame* game, int updateOrder = 1);
	virtual ~ATower12() { Clear(); }

	ATower* Copy() override { return new ATower12(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};