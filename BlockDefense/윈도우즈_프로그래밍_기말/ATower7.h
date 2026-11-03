#pragma once
#include"ATower.h"

class ATower7 : public ATower
{
public:
	ATower7() {}
	ATower7(class UGame* game, int updateOrder = 1);
	virtual ~ATower7() { Clear(); }

	ATower* Copy() override { return new ATower7(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};

