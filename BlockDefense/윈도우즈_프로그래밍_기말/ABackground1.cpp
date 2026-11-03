#include "ABackground1.h"
#include "UBackgroundSpriteComponent.h"

ABackground1::ABackground1(UGame* game, int updateOrder)
{
	this->uGame = game;
	this->iUpdateOrder = updateOrder;
	Initialize();
}

ABackground1::~ABackground1() { Clear(); }

void ABackground1::Initialize()
{
	ABackground::Initialize();
	uBackgroundSpriteComp = new UBackgroundSpriteComponent(uGame, this,uGame->GetTexture(L"background_map_1.bmp"));
}

void ABackground1::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ABackground::Draw(memDC1, memDC2, bitmap);
}

void ABackground1::AddPos()
{
	// 가야할 길을 미리 정해놓음
	clWayPointList->Insert(Vector2{ 575, -5 });
	clWayPointList->Insert(Vector2{ 575, 160 });
	clWayPointList->Insert(Vector2{ 160, 160 });
	clWayPointList->Insert(Vector2{ 160, 390 });
	clWayPointList->Insert(Vector2{ 575, 390 });
	clWayPointList->Insert(Vector2{ 575, 560 });
	// Tower의 위치를 미리 세팅해놓은 것
	// 오른쪽
	// 1
	clTowerPosList->Insert(new UPosition{ Vector2{500, 80} }); 	clTowerPosList->Insert(new UPosition{ Vector2{440, 80} });
	clTowerPosList->Insert(new UPosition{ Vector2{380, 80} }); 	clTowerPosList->Insert(new UPosition{ Vector2{320, 80} });
	clTowerPosList->Insert(new UPosition{ Vector2{260, 80} }); 	clTowerPosList->Insert(new UPosition{ Vector2{200, 80} });
	clTowerPosList->Insert(new UPosition{ Vector2{140, 80} });
	// 2
	clTowerPosList->Insert(new UPosition{ Vector2{75, 150} }); 	clTowerPosList->Insert(new UPosition{ Vector2{75, 210} });
	clTowerPosList->Insert(new UPosition{ Vector2{75, 270} }); 	clTowerPosList->Insert(new UPosition{ Vector2{75, 330} });
	clTowerPosList->Insert(new UPosition{ Vector2{75, 390} });
	// 3
	clTowerPosList->Insert(new UPosition{ Vector2{500, 470} }); 	clTowerPosList->Insert(new UPosition{ Vector2{440, 470} });
	clTowerPosList->Insert(new UPosition{ Vector2{380, 470} }); 	clTowerPosList->Insert(new UPosition{ Vector2{320, 470} });
	clTowerPosList->Insert(new UPosition{ Vector2{260, 470} }); 	clTowerPosList->Insert(new UPosition{ Vector2{200, 470} });
	clTowerPosList->Insert(new UPosition{ Vector2{140, 470} });
	//4
	clTowerPosList->Insert(new UPosition{ Vector2{500, 530} });
	// 왼쪽
	//1 
	clTowerPosList->Insert(new UPosition{ Vector2{650, 60} }); 	clTowerPosList->Insert(new UPosition{ Vector2{650, 120} });
	clTowerPosList->Insert(new UPosition{ Vector2{650, 180} });
	// 2
	clTowerPosList->Insert(new UPosition{ Vector2{595, 245} }); 	clTowerPosList->Insert(new UPosition{ Vector2{535, 245} });
	clTowerPosList->Insert(new UPosition{ Vector2{475, 245} }); 	clTowerPosList->Insert(new UPosition{ Vector2{415, 245} });
	clTowerPosList->Insert(new UPosition{ Vector2{355, 245} }); 	clTowerPosList->Insert(new UPosition{ Vector2{295, 245} });
	clTowerPosList->Insert(new UPosition{ Vector2{235, 245} });
	//3
	clTowerPosList->Insert(new UPosition{ Vector2{595, 300} }); 	clTowerPosList->Insert(new UPosition{ Vector2{535, 300} });
	clTowerPosList->Insert(new UPosition{ Vector2{475, 300} }); 	clTowerPosList->Insert(new UPosition{ Vector2{415, 300} });
	clTowerPosList->Insert(new UPosition{ Vector2{355, 300} }); 	clTowerPosList->Insert(new UPosition{ Vector2{295, 300} });
	clTowerPosList->Insert(new UPosition{ Vector2{235, 300} });
	// 4
	clTowerPosList->Insert(new UPosition{ Vector2{650, 370} }); 	clTowerPosList->Insert(new UPosition{ Vector2{650, 430} });
	clTowerPosList->Insert(new UPosition{ Vector2{650, 490} }); 	clTowerPosList->Insert(new UPosition{ Vector2{650, 550} });
}


