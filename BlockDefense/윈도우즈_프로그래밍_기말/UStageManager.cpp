#include "UStageManager.h"
#include"UEnemyManager.h"
#include"UPlayerManager.h"
#include"AEnemy.h"
#include"ABackground.h"

void UStageManager::Initialize()
{
	clEnemyList = new CustomList<AEnemy*>();
	
}

void UStageManager::SettingStage()
{
	if (bIsStart == false)
	{
		if (iStageCount == 1)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 10;
			bIsStart = true;
		}
		else if (iStageCount == 2)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 20;
			bIsStart = true;
		}
		else if (iStageCount == 3)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 30;
			bIsStart = true;
		}
		else if (iStageCount == 4)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 40;
			bIsStart = true;
		}
		else if (iStageCount == 5)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(0);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 1; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 100;
			bIsStart = true;
		}
		else if (iStageCount == 6)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 100;
			bIsStart = true;
		}
		else if (iStageCount == 7)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 150;
			bIsStart = true;
		}
		else if (iStageCount == 8)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 200;
			bIsStart = true;
		}
		else if (iStageCount == 9)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(0);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 250;
			bIsStart = true;
		}
		else if (iStageCount == 10)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(1);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 1; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 1000;
			bIsStart = true;
		}
		else if (iStageCount == 11)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 600;
			bIsStart = true;
		}
		else if (iStageCount == 12)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 700;
			bIsStart = true;
		}
		else if (iStageCount == 13)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 800;
			bIsStart = true;
		}
		else if (iStageCount == 14)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(1);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 900;
			bIsStart = true;
		}
		else if (iStageCount == 15)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(2);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 1; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 3000;
			bIsStart = true;
		}
		else if (iStageCount == 16)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 1750;
			bIsStart = true;
		}
		else if (iStageCount == 17)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 2000;
			bIsStart = true;
		}
		else if (iStageCount == 18)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 2250;
			bIsStart = true;
		}
		else if (iStageCount == 19)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(2);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 2500;
			bIsStart = true;
		}
		else if (iStageCount == 20)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(3);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 1; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 10000;
			bIsStart = true;
		}
		else if (iStageCount == 21)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 6000;
			bIsStart = true;
		}
		else if (iStageCount == 22)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 7000;
			bIsStart = true;
		}
		else if (iStageCount == 23)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 8000;
			bIsStart = true;
		}
		else if (iStageCount == 24)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(3);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 9000;
			bIsStart = true;
		}
		else if (iStageCount == 25)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(4);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 1; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 20000;
			bIsStart = true;
		}
		else if (iStageCount == 26)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 25; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 5; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 16000;
			bIsStart = true;
		}
		else if (iStageCount == 27)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 17000;
			bIsStart = true;
		}
		else if (iStageCount == 28)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 15; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 18000;
			bIsStart = true;
		}
		else if (iStageCount == 29)
		{
			AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(4);
			AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
			Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
			for (int i = 0; i < 10; i++)
			{
				AEnemy* enemy = enemy1->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			for (int i = 0; i < 20; i++)
			{
				AEnemy* enemy = enemy2->Copy();
				enemy->SetPos(startPos); startPos.y -= 75;
				clEnemyList->Insert(enemy);
			}
			iStageClearGold = 19000;
			bIsStart = true;
		}
		else if (iStageCount == 30)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(5);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 30000;
		bIsStart = true;
		}
		else if (iStageCount == 31)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 21000;
		bIsStart = true;
		}
		else if (iStageCount == 32)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 22000;
		bIsStart = true;
		}
		else if (iStageCount == 33)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 23000;
		bIsStart = true;
		}
		else if (iStageCount == 34)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(5);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 24000;
		bIsStart = true;
		}
		else if (iStageCount == 35)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(6);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 50000;
		bIsStart = true;
		}
		else if (iStageCount == 36)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 36000;
		bIsStart = true;
		}
		else if (iStageCount == 37)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 37000;
		bIsStart = true;
		}
		else if (iStageCount == 38)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 38000;
		bIsStart = true;
		}
		else if (iStageCount == 39)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(6);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 39000;
		bIsStart = true;
		}
		else if (iStageCount == 40)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(7);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 75000;
		bIsStart = true;
		}
		else if (iStageCount == 41)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 51000;
		bIsStart = true;
		}
		else if (iStageCount == 42)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 52000;
		bIsStart = true;
		}
		else if (iStageCount == 43)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 53000;
		bIsStart = true;
		}
		else if (iStageCount == 44)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(7);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 54000;
		bIsStart = true;
		}
		else if (iStageCount == 45)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(8);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 75000;
		bIsStart = true;
		}
		else if (iStageCount == 46)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 51000;
		bIsStart = true;
		}
		else if (iStageCount == 47)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 52000;
		bIsStart = true;
		}
		else if (iStageCount == 48)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 53000;
		bIsStart = true;
		}
		else if (iStageCount == 49)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(8);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 54000;
		bIsStart = true;
		}
		else if (iStageCount == 50)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(9);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 75000;
		bIsStart = true;
		}
		else if (iStageCount == 51)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 51000;
		bIsStart = true;
		}
		else if (iStageCount == 52)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 52000;
		bIsStart = true;
		}
		else if (iStageCount == 53)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 53000;
		bIsStart = true;
		}
		else if (iStageCount == 54)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(9);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 54000;
		bIsStart = true;
		}
		else if (iStageCount == 55)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(10);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 100000;
		bIsStart = true;
		}
		else if (iStageCount == 56)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 76000;
		bIsStart = true;
		}
		else if (iStageCount == 57)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 77000;
		bIsStart = true;
		}
		else if (iStageCount == 58)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 78000;
		bIsStart = true;
		}
		else if (iStageCount == 59)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(10);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 79000;
		bIsStart = true;
		}
		else if (iStageCount == 60)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(11);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 125000;
		bIsStart = true;
		}
		else if (iStageCount == 61)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 110000;
		bIsStart = true;
		}
		else if (iStageCount == 62)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 120000;
		bIsStart = true;
		}
		else if (iStageCount == 63)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 130000;
		bIsStart = true;
		}
		else if (iStageCount == 64)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(11);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 140000;
		bIsStart = true;
		}
		else if (iStageCount == 65)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(12);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 175000;
		bIsStart = true;
		}
		else if (iStageCount == 66)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 151000;
		bIsStart = true;
		}
		else if (iStageCount == 67)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 152000;
		bIsStart = true;
		}
		else if (iStageCount == 68)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 153000;
		bIsStart = true;
		}
		else if (iStageCount == 69)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(12);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 154000;
		bIsStart = true;
		}
		else if (iStageCount == 70)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(13);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 200000;
		bIsStart = true;
		}
		else if (iStageCount == 71)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 176000;
		bIsStart = true;
		}
		else if (iStageCount == 72)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 177000;
		bIsStart = true;
		}
		else if (iStageCount == 73)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 178000;
		bIsStart = true;
		}
		else if (iStageCount == 74)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(13);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 179000;
		bIsStart = true;
		}
		else if (iStageCount == 75)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(14);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 200000;
		bIsStart = true;
		}
		else if (iStageCount == 76)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 176000;
		bIsStart = true;
		}
		else if (iStageCount == 77)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 177000;
		bIsStart = true;
		}
		else if (iStageCount == 78)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 178000;
		bIsStart = true;
		}
		else if (iStageCount == 79)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(14);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 179000;
		bIsStart = true;
		}
		else if (iStageCount == 80)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(15);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 250000;
		bIsStart = true;
		}
		else if (iStageCount == 81)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 210000;
		bIsStart = true;
		}
		else if (iStageCount == 82)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 220000;
		bIsStart = true;
		}
		else if (iStageCount == 83)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 230000;
		bIsStart = true;
		}
		else if (iStageCount == 84)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(15);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 240000;
		bIsStart = true;
		}
		else if (iStageCount == 85)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(16);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 400000;
		bIsStart = true;
		}
		else if (iStageCount == 86)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 310000;
		bIsStart = true;
		}
		else if (iStageCount == 87)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 320000;
		bIsStart = true;
		}
		else if (iStageCount == 88)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 330000;
		bIsStart = true;
		}
		else if (iStageCount == 89)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(16);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 340000;
		bIsStart = true;
		}
		else if (iStageCount == 90)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(17);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 700000;
		bIsStart = true;
		}
		else if (iStageCount == 91)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 510000;
		bIsStart = true;
		}
		else if (iStageCount == 92)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 520000;
		bIsStart = true;
		}
		else if (iStageCount == 93)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 530000;
		bIsStart = true;
		}
		else if (iStageCount == 94)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(17);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 540000;
		bIsStart = true;
		}
		else if (iStageCount == 95)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(18);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 800000;
		bIsStart = true;
		}
		else if (iStageCount == 96)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(19);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 5; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 510000;
		bIsStart = true;
		}
		else if (iStageCount == 97)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(19);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 520000;
		bIsStart = true;
		}
		else if (iStageCount == 98)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(19);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 15; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 530000;
		bIsStart = true;
		}
		else if (iStageCount == 99)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(18);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetMobList()->GetData(19);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 10; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 20; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 540000;
		bIsStart = true;
		}
		else if (iStageCount == 100)
		{
		AEnemy* enemy1 = uGame->GetEnemyManager()->GetMobList()->GetData(19);
		AEnemy* enemy2 = uGame->GetEnemyManager()->GetBossList()->GetData(19);
		Vector2 startPos = uGame->GetBackground()->GetWayPointList()->GetData(0);
		for (int i = 0; i < 25; i++)
		{
			AEnemy* enemy = enemy1->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		for (int i = 0; i < 1; i++)
		{
			AEnemy* enemy = enemy2->Copy();
			enemy->SetPos(startPos); startPos.y -= 75;
			clEnemyList->Insert(enemy);
		}
		iStageClearGold = 800000;
		bIsStart = true;
		}
	}
}

void UStageManager::Update(float deltaSecond)
{
	if (bIsStart)
	{
		int len = clEnemyList->GetLength();
		for (int i = 0; i < len; i++)
		{
			AEnemy* enemy = clEnemyList->GetData(i);
			enemy->Update(deltaSecond);
		}

		int deadCount = 0, destoryCount = 0;
		for (int i = 0; i < len; i++)
		{
			AEnemy* enemy = clEnemyList->GetData(i);
			if (enemy->GetHp() <= 0) deadCount++;
			if (enemy->IsDestory()) destoryCount++;
		}
		if (destoryCount == len && deadCount != len)
		{
			clEnemyList->Clear();
			bIsStart = false; iStageCount++;
			uGame->GetPlayerManager()->SetSurviveCount(uGame->GetPlayerManager()->GetSurviveCount() - (len - deadCount));
			uGame->GetPlayerManager()->SetGold(uGame->GetPlayerManager()->GetGold() + iStageClearGold);
			uGame->GetPlayerManager()->Clear();
			uGame->UpdateTurnText(); uGame->UpdateSurviveNum(); uGame->UpdateHaveGold();
			if (uGame->GetPlayerManager()->GetSurviveCount() <= 0)
			{
				DialogBox(uGame->GetInst(), MAKEINTRESOURCE(IDD_WINLOSE), uGame->GetGameHWND(), uGame->GetWLDialog());
			}
		}
		else	if (destoryCount == len && deadCount == len)
		{
			clEnemyList->Clear();
			bIsStart = false; iStageCount++;
			uGame->GetPlayerManager()->SetGold(uGame->GetPlayerManager()->GetGold() + iStageClearGold);
			uGame->GetPlayerManager()->Clear();
			uGame->UpdateTurnText(); uGame->UpdateSurviveNum(); uGame->UpdateHaveGold();
			if (iStageCount > 100)
			{
				DialogBox(uGame->GetInst(), MAKEINTRESOURCE(IDD_WINLOSE), uGame->GetGameHWND(), uGame->GetWLDialog());
			}
		}
	}
}

void UStageManager::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	if (bIsStart)
	{
		int len = clEnemyList->GetLength();
		for (int i = 0; i < len; i++)
		{
			AEnemy* enemy = clEnemyList->GetData(i);
			enemy->Draw(memDC1, memDC2, bitmap);
		}
	}
}

void UStageManager::WinLoseJudge(HWND hWnd)
{
	if (uGame->GetPlayerManager()->GetSurviveCount() <= 0)
	{
		HWND list = GetDlgItem(hWLHWnd, WLDL_LIST);
		SendMessage(list, LB_DELETESTRING, 0, (LPARAM)0);
		SendMessage(list, LB_ADDSTRING, 0, (LPARAM)L"ÆÐ¹è");
	}
	else	if (iStageCount > 100)
	{
		HWND list = GetDlgItem(hWLHWnd, WLDL_LIST);
		SendMessage(list, LB_DELETESTRING, 0, (LPARAM)0);
		SendMessage(list, LB_ADDSTRING, 0, (LPARAM)L"½Â¸®");
	}
}
