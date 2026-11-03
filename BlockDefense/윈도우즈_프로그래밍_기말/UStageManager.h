#pragma once
#include"UManager.h"

class UStageManager : public UManager
{
public:
	UStageManager(){}
	UStageManager(UGame* game) { this->uGame = game; Initialize(); }
	virtual ~UStageManager() { Reset(); }

	void Initialize() override;

	void SettingStage();

	void Update(float deltaSecond);

	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap);

	void Reset()  override { DELETE_MEMORY(clEnemyList); }

	void WinLoseJudge(HWND hWnd);
public:
	CustomList<class AEnemy*>* GetEnemyList() const { return clEnemyList; }

	int GetStageCount() const { return iStageCount; }
	void SetStageCount(int stageCount) { this->iStageCount = stageCount; }

	int GetStageClearGold() const { return iStageClearGold; }
	void SetStageClearGold(int stageClearGold) { this->iStageClearGold = stageClearGold; }

	bool IsStart() const { return bIsStart; }
	void SetStart(bool start) { this->bIsStart = start; }

	void SetWL(HWND hWnd) { this->hWLHWnd = hWnd; }
protected:
	CustomList<class AEnemy*>* clEnemyList = nullptr;
	HWND hWLHWnd = NULL;
	int iStageCount = 1, iStageClearGold = 0;
	bool bIsStart = false;
};

