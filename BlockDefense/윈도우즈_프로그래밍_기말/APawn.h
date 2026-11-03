#pragma once
#include"AActor.h"

class APawn : public AActor
{
public:
	APawn() {}
	APawn(class UGame* game, int updateOrder = 1);
	virtual ~APawn();

	// 액터의 업데이트 함수
	virtual  void Update(float deltaSecond) override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;

protected:

};

