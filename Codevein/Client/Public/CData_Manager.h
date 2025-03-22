#pragma once
#include "CManager.h"
#include "CWorldObj_Manager.h"
#include "CPlayer_Manager.h"
#include "CMonster_Manager.h"
#include "CItem_Manager.h"
#include "CVariable_Manager.h"
#include "CStageMananger.h"

BEGIN(Client)

class CData_Manager final : public CManager
{
	DECLARE_SINGLETON(CData_Manager)
private:
	explicit CData_Manager();
	virtual ~CData_Manager() = default;
public:
	CWorldObj_Manager* Get_WorldMgr() { return m_pWorldManager; }
	CPlayer_Manager* Get_PlayerMgr() { return m_pPlayerManager; }
	CMonster_Manager* Get_MonsterMgr() { return m_pMonsterManager; }
	CItem_Manager* Get_ItemMgr() { return m_pItemManager; }
	CVariable_Manager* Get_VariableMgr() { return m_pVarialbeManager; }
	CStageManager* Get_StageMgr() { return m_pStageManager; }
public:
	void Set_InitLoadData(const _bool& _bIsInitLoadData) { this->m_bInitLoadData = _bIsInitLoadData; }
	const _bool& Is_InitLoadData() const { return m_bInitLoadData; }

	void Plus_Init_PlusThreadCount() {++m_iInit_ThreadCount;}
	_bool Is_InitLoad_End();
public:
	// NativeConstruct
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
public:
	HRESULT Save_Data(const string& _strFileName);
	HRESULT Load_Data(const string& _strFilePath, const LEVEL& _eLevel = LEVEL_TOOL);
	void Reset_Data();
private:
	HRESULT Clone_Objects(const LEVEL& _eLevel);

public:
	void Delete_WorldManager() { Safe_Release(m_pWorldManager); }
	void Delete_PlayerManager() { Safe_Release(m_pPlayerManager); }
private:
	const char* FOLDER_PATH = "..\\..\\Resource\\Json";

private:
	CWorldObj_Manager*		m_pWorldManager			= nullptr;
	CPlayer_Manager*				m_pPlayerManager			= nullptr;
	CMonster_Manager*			m_pMonsterManager		= nullptr;
	CItem_Manager*					m_pItemManager				= nullptr;
	CVariable_Manager*			m_pVarialbeManager		= nullptr;
	CStageManager*				m_pStageManager				=	nullptr;

private:
	// Init Load Data
	_bool										m_bInitLoadData = false;
	_uint										m_iInit_ThreadCount = 0;
public:
	virtual void Free() override;
};

END