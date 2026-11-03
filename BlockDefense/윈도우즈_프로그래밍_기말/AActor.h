#pragma once
#include"UGame.h"

class AActor
{
public:
	AActor(){}
	AActor(class UGame* game, int updateOrder = 1);
	virtual ~AActor();

	// 초기화 함수
	virtual void Initialize(){}
	// 업데이트 하는 함수
	virtual void Update(float deltaSecond) {  }
	// 그리는 함수
	virtual void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap){}

protected:
public:
	// 안에 들어왔는지 확인
	bool IsInPos(Vector2 pos1, double width1, double height1, Vector2 pos2, int len = 3);
	// 회전
	void Rotate(HDC memDC1);
	// 회전된 월드축 초기화
	void ResetRotate(HDC memDC1);
	// 데이터를 초기화 하는 함수
	virtual void Clear() { DELETE_MEMORY(uGame); }

	//  GetSet함수
	Vector2 GetPos() const { return v2Pos; }
	void SetPos(Vector2 pos) { this->v2Pos = pos; }
	Vector2 GetDir() const { return v2Dir; }
	void SetDir(Vector2 dir) { this->v2Dir = dir; }
	Vector2 GetScale() const { return v2Scale; }
	void SetScale(Vector2 scale) { this->v2Scale = scale; }
	double GetAngle() const { return dAngle; }
	void SetAngle(double angle) { this->dAngle = angle; }
	double GetWidth() const { return dWidth; }
	void SetWidth(double width) { this->dWidth = width; }
	double GetHeight() const { return dHeight; }
	void SetHeight(double height) { this->dHeight = height; }

	int GetUpdateOrder() const { return iUpdateOrder; }
protected:
	class UGame* uGame = nullptr;
	Vector2 v2Pos, v2Dir, v2Scale;
	double dAngle = 0.0, dWidth = 0.0, dHeight = 0.0;
	int iUpdateOrder = 0;
};

