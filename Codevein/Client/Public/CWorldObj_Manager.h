#pragma once
#include "CManager.h"
#include "CTerrain.h"
#include "CWorldObj.h"
#include "CWOEffect.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CWorldObj_Manager final : public CManager
{
private:
	explicit CWorldObj_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CWorldObj_Manager() = default;
public:
	void Release_Data();
public:
	void Insert_TerrainData(const CTerrain::TERRIANSAVEDESC& tData) { m_SaveTerrianList.push_back(tData); }
	// Get Floor Model 
	list<class CModel*> Get_WorldObjsModel() { return m_Stage1WorldModels; }
	// Insert Floor Data
	void Insert_WorldObjs(const CWorldObj::WORLDSAVES& _tData) { m_SaveFloorList.push_back(_tData); }
	// Find Floor Model
	CModel* Find_WorldObjsModel(const _wstring& _strFindName);

	void Insert_WOEffectList(const CWOEffect::WODEFFECTDESC& _tDesc) { m_SaveWordEffectList.push_back(_tDesc); }

	const _string& Get_Name() { return m_strFindName; }
	void Set_Name(const _string& _strModelName) { this->m_strFindName = _strModelName; }

	CModel* Get_CheckPoints_Model() { return m_CheckPointsModel; }

	list<CWorldObj*> Get_WallObjList() { return m_WallObjList; }
public:
	HRESULT NativeConstruct();
	// Load Floor Model
	HRESULT Load_Stage1();
	// Start Init Thread
	static void Start_InitThread(void* _pArg);
	// Init Thread
	void Run_InitThread();
	// Start Thread
	static void Start_Thread_Stage1(void* _pArg);
	// Run Thread 
	void Run_ThreadStage1();
	// Load Stage2 
	HRESULT Load_Stage2();
	// Start Thread Stage2
	static void Start_Thread_Stage2(void* _pArg);
	// Run Thread Stage2 
	void Run_ThreadStage2();
private:
	// Load Init Data
	HRESULT Load_Init_Data();
	// Load FloorData 1
	HRESULT Load_Stage1Data();
	// Load Stage 2 
	HRESULT Load_Stage2Data();
public:
	string Research_Model(_string _strModelName);
	void Setting_CheckPointModel();
	// Data Manager
	HRESULT Save_Data(const string& _strFolderName);
	HRESULT Load_Data(const string& _strPath);
	HRESULT Clone_Objects(const LEVEL& _eLevel = LEVEL_TOOL);
private:
	/* Save */
	HRESULT Save_TerrianData(const string& _strFolderPath);
	HRESULT  Save_FloorData(const _string& _strFolderPath);
	HRESULT Save_WOEffectData(const _string& _strFolderPath);
	// Load
	HRESULT Load_TerrianData(const string& _strPath);
	HRESULT  Load_FloorData(const _string& _strPath);
	HRESULT Load_WOEffectData(const _string& _strPath);
	// Create
	HRESULT Clone_Terrain(const LEVEL& _eLevel = LEVEL_TOOL);
	HRESULT  Clone_Floor(const LEVEL& _eLevel = LEVEL_TOOL);
	HRESULT Clone_WOEffect(const LEVEL& _eLevel = LEVEL_TOOL);
private:
	HRESULT Create_Datas(const LEVEL& _eLevel = LEVEL_TOOL);
private:
	const char* SAVE_NAME_TERRAIN = "Terrain.dat";
	const char* SAVE_FLOOR_NAME = "Floor.dat";
	const char* SAVE_WOEFFECT_NAME = "WOEffect.dat";
private:
	_uint										m_iThreadCount = 0;
private:
	typedef list<CTerrain::TERRIANSAVEDESC> TERRIANSAVES;
	TERRIANSAVES					m_SaveTerrianList;
private:
	// Floor Model List
	list<class CModel*>			m_Stage1WorldModels;
	// Save Data 
	typedef list<CWorldObj::WORLDSAVES> FLOORSAVES;
	FLOORSAVES						m_SaveFloorList;
private:
	typedef list <CWOEffect::WODEFFECTDESC> WORDEFFDESC;
	WORDEFFDESC					m_SaveWordEffectList;
private:
	// Show List
	list<CModel*>					m_ShowModels;
	CModel*							m_CheckPointsModel = nullptr;
	_string								m_strFindName;
private:
	// Wall Obj List
	list<CWorldObj*>			m_WallObjList;
public:
	static CWorldObj_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END