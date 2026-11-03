#include "UEnemyManager.h"
#include"AEnemy.h"
#include"AMob.h"
#include"ABoss.h"

void UEnemyManager::Initialize()
{
	MobInit(); BossInit();
}

void UEnemyManager::Reset()
{
	DELETE_MEMORY(clMobList); DELETE_MEMORY(clBossList);
}

void UEnemyManager::MobInit()
{
	clMobList = new CustomList<AEnemy*>();
	clMobList->Insert(new AMob(uGame, 1, CreateSolidBrush(RGB(255, 000, 000))));
	clMobList->Insert(new AMob(uGame, 5, CreateSolidBrush(RGB(153, 204, 102))));
	clMobList->Insert(new AMob(uGame, 10, CreateSolidBrush(RGB(102, 204, 051))));
	clMobList->Insert(new AMob(uGame, 17, CreateSolidBrush(RGB(051, 204, 000))));
	clMobList->Insert(new AMob(uGame, 20, CreateSolidBrush(RGB(000, 204, 255))));
	clMobList->Insert(new AMob(uGame, 25, CreateSolidBrush(RGB(051, 255, 102))));
	clMobList->Insert(new AMob(uGame, 30, CreateSolidBrush(RGB(102, 000, 153))));
	clMobList->Insert(new AMob(uGame, 70, CreateSolidBrush(RGB(153, 051, 255))));
	clMobList->Insert(new AMob(uGame, 120, CreateSolidBrush(RGB(204, 102, 051))));
	clMobList->Insert(new AMob(uGame, 200, CreateSolidBrush(RGB(255, 102, 051))));
	clMobList->Insert(new AMob(uGame, 300, CreateSolidBrush(RGB(204, 000, 255))));
	clMobList->Insert(new AMob(uGame, 500, CreateSolidBrush(RGB(153, 000, 255))));
	clMobList->Insert(new AMob(uGame, 700, CreateSolidBrush(RGB(102, 255, 000))));
	clMobList->Insert(new AMob(uGame,1000, CreateSolidBrush(RGB(051, 153, 255))));
	clMobList->Insert(new AMob(uGame, 1500, CreateSolidBrush(RGB(051, 153, 204))));
	clMobList->Insert(new AMob(uGame, 2000, CreateSolidBrush(RGB(000, 000, 102))));
	clMobList->Insert(new AMob(uGame, 2700, CreateSolidBrush(RGB(051, 051, 051))));
	clMobList->Insert(new AMob(uGame, 3500, CreateSolidBrush(RGB(000, 051, 000))));
	clMobList->Insert(new AMob(uGame, 4300, CreateSolidBrush(RGB(000, 051, 000))));
	clMobList->Insert(new AMob(uGame, 5000, CreateSolidBrush(RGB(000, 000, 000))));
}

void UEnemyManager::BossInit()
{
	clBossList = new CustomList<AEnemy*>();
	clBossList->Insert(new ABoss(uGame, 10, CreateSolidBrush(RGB(255, 000, 000))));
	clBossList->Insert(new ABoss(uGame, 50, CreateSolidBrush(RGB(153, 204, 102))));
	clBossList->Insert(new ABoss(uGame, 100, CreateSolidBrush(RGB(102, 204, 051))));
	clBossList->Insert(new ABoss(uGame, 170, CreateSolidBrush(RGB(051, 204, 000))));
	clBossList->Insert(new ABoss(uGame, 200, CreateSolidBrush(RGB(000, 204, 255))));
	clBossList->Insert(new ABoss(uGame, 250, CreateSolidBrush(RGB(051, 255, 102))));
	clBossList->Insert(new ABoss(uGame, 300, CreateSolidBrush(RGB(102, 000, 153))));
	clBossList->Insert(new ABoss(uGame, 700, CreateSolidBrush(RGB(153, 051, 255))));
	clBossList->Insert(new ABoss(uGame, 1200, CreateSolidBrush(RGB(204, 102, 051))));
	clBossList->Insert(new ABoss(uGame, 2000, CreateSolidBrush(RGB(255, 102, 051))));
	clBossList->Insert(new ABoss(uGame, 3000, CreateSolidBrush(RGB(204, 000, 255))));
	clBossList->Insert(new ABoss(uGame, 5000, CreateSolidBrush(RGB(153, 000, 255))));
	clBossList->Insert(new ABoss(uGame, 7000, CreateSolidBrush(RGB(102, 255, 000))));
	clBossList->Insert(new ABoss(uGame, 10000, CreateSolidBrush(RGB(051, 153, 255))));
	clBossList->Insert(new ABoss(uGame, 15000, CreateSolidBrush(RGB(051, 153, 204))));
	clBossList->Insert(new ABoss(uGame, 20000, CreateSolidBrush(RGB(000, 000, 102))));
	clBossList->Insert(new ABoss(uGame, 27000, CreateSolidBrush(RGB(051, 051, 051))));
	clBossList->Insert(new ABoss(uGame, 35000, CreateSolidBrush(RGB(000, 051, 000))));
	clBossList->Insert(new ABoss(uGame, 43000, CreateSolidBrush(RGB(000, 051, 000))));
	clBossList->Insert(new ABoss(uGame, 50000, CreateSolidBrush(RGB(000, 000, 000))));
}
