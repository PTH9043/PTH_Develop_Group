#pragma once
#include "CManager.h"
#include "CGiantDevil.h"
#include "CMonkeyDevil.h"
#include "CWolfDevil.h"
#include "CUrchinDevil.h"
#include "CGiantWhiteDevil.h"
#include "CAncientHannibal.h"
#include "CGateKeeperWoman.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)


class CMonster_Manager : public CManager
{
private:
	explicit CMonster_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMonster_Manager() = default;
public:
	HRESULT NativeConstruct();

public:
	void Insert_GiantDevilData(const CGiantDevil::GIANTDEVILDESC& _tData) { m_GiantDevilList.push_back(_tData); }
	void Insert_MonkeyDevilData(const CMonkeyDevil::MONKEYDEVILDESC& _tData) { m_MonkeyDevilSaveList.push_back(_tData); }
	void Insert_WolfDevilData(const CWolfDevil::WOLFDEVILDESC& _tData) { m_WolfDevilSaveList.push_back(_tData); }
	void Insert_UrchinDevilData(const CUrchinDevil::URCHINDEVILDESC& _tData) { m_UrchinSaves.push_back(_tData); }
	void Insert_GWhiteDevilData(const CGiantWhiteDevil::GWHITEDEVILDESC& _tData) { m_GWhiteDevilSaves.push_back(_tData);  }
	void Insert_AncHannibal(const CAncientHannibal::ANCHANNIBALDESC& _tData) { m_AncHannibalSaves.push_back(_tData); }
	void Insert_GateKeepWoman(const CGateKeeperWoman::GATEKEEPERWOMANDESC& _tData) { m_GateKeepSaves.push_back(_tData); }

	// Data Manager
	HRESULT Save_Data(const string& _strPath);
	HRESULT Load_Data(const string& _strPath);
	HRESULT Clone_Objects(const LEVEL& _eLevel);
private:
	HRESULT Save_MonekyDevil(const string& _strPath);
	HRESULT Load_MonkeyDevil(const string& _strPath);
	HRESULT MonkeyDevil_Clone(const LEVEL& _eLevel);
private:
	HRESULT Save_WolfDevil(const string& _strPath);
	HRESULT Load_WolfDevil(const string& _strPath);
	HRESULT WolfDevil_Clone(const LEVEL& _eLevel);
private:
	HRESULT Save_UrchinDevil(const string& _strPath);
	HRESULT Load_UrchinDevil(const string& _strPath);
	HRESULT Urchin_Clone(const LEVEL& _eLevel);
private:
	HRESULT Save_GWhiteDevil(const string& _strPath);
	HRESULT Load_GWhiteDevil(const string& _strPath);
	HRESULT GWhiteDevil_Clone(const LEVEL& _eLevel);
private:
	HRESULT Save_AncientHannibal(const string& _strPath);
	HRESULT Load_AncientHannibal(const string& _strPath);
	HRESULT AncHannibalClone(const LEVEL& _eLevel);
private:
	HRESULT Save_GateKeeperWoman(const string& _strPath);
	HRESULT Load_GateKeeperWoman(const string& _strPath);
	HRESULT GateKeeperWomanClone(const LEVEL& _eLevel);
private:
	const char* SAVE_MONKEYDEVIL_NAME = "MonekyDevil.dat";
	const char* SAVE_WOLFDEVIL_NAME = "WolfDevil.dat";
	const char* SAVE_URCHINDEVIL_NAME = "UrchinDevil.dat";
	const char* SAVE_GWHITEDEVIL_NAME = "GWhiteDevil.dat";
	const char* SAVE_ANCHANNIBAL_NAME = "AncHannibal.dat";
	const char* SAVE_GATEKEEP_NAME =  "GateKeeperWoman.dat";
private:
	typedef list<CGiantDevil::GIANTDEVILDESC> GIANTDEVILSAVES;
	GIANTDEVILSAVES m_GiantDevilList;

	typedef list< CMonkeyDevil::MONKEYDEVILDESC> MONKEYDEVILSAVES;
	MONKEYDEVILSAVES m_MonkeyDevilSaveList;

	typedef list<CWolfDevil::WOLFDEVILDESC> WOLFDEVILSAVES;
	WOLFDEVILSAVES m_WolfDevilSaveList;

	typedef list<CUrchinDevil::URCHINDEVILDESC> URCHINSAVES;
	URCHINSAVES m_UrchinSaves;

	typedef list<CGiantWhiteDevil::GWHITEDEVILDESC> GWHITEDEVILSAVES;
	GWHITEDEVILSAVES m_GWhiteDevilSaves;

	typedef list<CAncientHannibal::ANCHANNIBALDESC> ANCHANNIBALSAVES;
	ANCHANNIBALSAVES m_AncHannibalSaves;

	typedef list< CGateKeeperWoman::GATEKEEPERWOMANDESC> GATEKEEPSAVES;
	GATEKEEPSAVES m_GateKeepSaves;
public:
	static CMonster_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};


END