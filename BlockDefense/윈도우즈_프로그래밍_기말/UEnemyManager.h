#pragma once
#include"UManager.h"

class UEnemyManager : public UManager
{
public:
	UEnemyManager(){}
	UEnemyManager(UGame* game) { this->uGame = game; Initialize(); }
	virtual ~UEnemyManager() { Reset(); }

	void Initialize() override;

	void Reset() override;
protected:
	void MobInit();
	void BossInit();
public:
	CustomList<class AEnemy*>* GetMobList() const { return clMobList; }
	CustomList<class AEnemy*>* GetBossList() const { return clBossList; }
protected:
	CustomList<class AEnemy*>* clMobList = nullptr;
	CustomList<class AEnemy*>* clBossList = nullptr;
};

