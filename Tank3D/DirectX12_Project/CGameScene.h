#pragma once
#include "CScene.h"

class CCamera;
class CTank;
class CFloor;
class CMainCamera;
class CCristal;

class CMonster;
class CCharacter;
enum MONTYPE;

class CText;
class CTextButton;

typedef list<shared_ptr<CMonster>> MONACTIVE;
typedef vector<shared_ptr<CMonster>> MONVECTOR;
typedef unordered_map< MONTYPE, MONVECTOR> MONCONTAINER;

class CGameScene : public CScene {
protected:
protected:
	explicit CGameScene() : CScene(SCENE_GAME){}
	CGameScene(const CGameScene& _rhs) : CScene(_rhs) {};
	virtual ~CGameScene() = default;

public:
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT LoadSceneData() override;

	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);

	void DeleteMonster(const shared_ptr<CCharacter>& _pCharcter);
protected:
	// MonsterRandomSpawn
	void MonsterRadomsSpawn();
	// Add 
	void Add_MonList(const MONTYPE _eType, const _uint _iMonsterNum, const _uint& _iMobLevel);
	// Check Died Monster
	void CheckDiedMonster();
	// Create Monster
	void CreateMonster(const VOIDDATAS& _tDatas, const MONTYPE _eType, 
		const _uint _iMonsterNum, const _wstring& _strProtoType);
public:
	// Get Floor
	const shared_ptr<CFloor> Get_Floor() const { return m_pFloor; }

	const MONACTIVE& GetMonsterActiveList() const { return m_MonActiveList; }
	const shared_ptr<CTank>& Get_Tank() const { return m_pTank; }
private:
	shared_ptr< CMainCamera>				m_pCamera{  };
	shared_ptr< CTank>							m_pTank{  };
	shared_ptr< CFloor>							m_pFloor{};
	shared_ptr<CCristal>							m_pCristal{};
	// MonActiveList
	MONACTIVE											m_MonActiveList{};
	// Random Spawn Time 
	_float														m_fRandomSpawnTime{ 0.f };
	// RoundValue
	_uint														m_iRoundValue{ 1 };
	// Text
	shared_ptr< CText>								m_pRemainLifeText = nullptr;
	shared_ptr< CText>								m_pShockText = nullptr;
	shared_ptr< CText>								m_pShockingText = nullptr;
	shared_ptr<CText>								m_pNextRoundText = nullptr;
	shared_ptr<CText>								m_pDamgeShowText = nullptr;
	shared_ptr<CText>								m_pShootSpeedShowText = nullptr;
	static const int										SPAWN_CNT = 100;
	vector<_float3>									m_SpawnPosList{};
	_bool														m_bNextRound{ false };
	_double													m_dNextRoundTime{ 0 };
	const		_double									NEXT_ROUND_TIME{ 3 };

};

