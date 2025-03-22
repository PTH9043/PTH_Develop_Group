#include "CWorldObj_Manager.h"
#include "CImGui_Manager.h"
#include "CMonster_Manager.h"
#include "CGameInstance.h"
#include <direct.h>

CMonster_Manager::CMonster_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CManager(_pDevice, _pContext)
{
}

HRESULT CMonster_Manager::NativeConstruct()
{
	return S_OK;
}

HRESULT CMonster_Manager::Save_Data(const string& _strPath)
{
	string strMonsterPath = _strPath;
	strMonsterPath.append("\\");
	strMonsterPath.append("Monster");
	_mkdir(strMonsterPath.c_str());

	Save_MonekyDevil(strMonsterPath);
	Save_WolfDevil(strMonsterPath);
	Save_UrchinDevil(strMonsterPath);
	Save_GWhiteDevil(strMonsterPath);
	Save_AncientHannibal(strMonsterPath);
	Save_GateKeeperWoman(strMonsterPath);

	m_GiantDevilList.clear();
	m_MonkeyDevilSaveList.clear();
	m_WolfDevilSaveList.clear();
	m_UrchinSaves.clear();
	m_GWhiteDevilSaves.clear();
	m_AncHannibalSaves.clear();
	m_GateKeepSaves.clear();
	return S_OK;
}

HRESULT CMonster_Manager::Load_Data(const string& _strPath)
{
	string strMonsterPath = _strPath;
	strMonsterPath.append("\\");
	strMonsterPath.append("Monster");

	Load_MonkeyDevil(strMonsterPath);
	Load_WolfDevil(strMonsterPath);
	Load_UrchinDevil(strMonsterPath);
	Load_GWhiteDevil(strMonsterPath);
	Load_AncientHannibal(strMonsterPath);
	Load_GateKeeperWoman(strMonsterPath);

	return S_OK;
}

HRESULT CMonster_Manager::Clone_Objects(const LEVEL& _eLevel)
{
	MonkeyDevil_Clone(_eLevel);
	WolfDevil_Clone(_eLevel);
	Urchin_Clone(_eLevel);
	GWhiteDevil_Clone(_eLevel);
	AncHannibalClone(_eLevel);
	GateKeeperWomanClone(_eLevel);

	m_GiantDevilList.clear();
	m_MonkeyDevilSaveList.clear();
	m_WolfDevilSaveList.clear();
	m_UrchinSaves.clear();
	m_GWhiteDevilSaves.clear();
	m_AncHannibalSaves.clear();
	m_GateKeepSaves.clear();
	return S_OK;
}

#pragma region MONKEYDEVIL
HRESULT CMonster_Manager::Save_MonekyDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_MONKEYDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_MonekyDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_MonkeyDevilSaveList.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_MonkeyDevilSaveList)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eWeaponType, sizeof(CMonkeyDevil::EQUIP_WEAPONTYPE), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_MonkeyDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_MONKEYDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_MonekyDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CMonkeyDevil::MONKEYDEVILDESC tMonkeyDevilDesc;
		ReadFile(hFile, &tMonkeyDevilDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tMonkeyDevilDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tMonkeyDevilDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tMonkeyDevilDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tMonkeyDevilDesc.eWeaponType, sizeof(CMonkeyDevil::EQUIP_WEAPONTYPE), &dwByte, nullptr);
		ReadFile(hFile, &tMonkeyDevilDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_MonkeyDevilSaveList.push_back(tMonkeyDevilDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::MonkeyDevil_Clone(const LEVEL& _eLevel)
{
	if (m_MonkeyDevilSaveList.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_MONKEYDEVIL))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_MonkeyDevilSaveList)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion MONKEYDEVIL

#pragma region WOLFDEVIL
HRESULT CMonster_Manager::Save_WolfDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_WOLFDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_WolfDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_WolfDevilSaveList.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_WolfDevilSaveList)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_WolfDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_WOLFDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Load_WolfDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif

		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CWolfDevil::WOLFDEVILDESC tDesc;
		ReadFile(hFile, &tDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_WolfDevilSaveList.push_back(tDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::WolfDevil_Clone(const LEVEL& _eLevel)
{
	if (m_WolfDevilSaveList.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_WOLFDEVIL))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_WolfDevilSaveList)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion WOLFDEVIL

#pragma region URCHINDEVIL
HRESULT CMonster_Manager::Save_UrchinDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_URCHINDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_UrchinDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_UrchinSaves.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_UrchinSaves)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_UrchinDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_URCHINDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		cout << strFileName.c_str() << endl;
#endif
		return S_OK;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CUrchinDevil::URCHINDEVILDESC tDesc;
		ReadFile(hFile, &tDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_UrchinSaves.push_back(tDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Urchin_Clone(const LEVEL& _eLevel)
{
	if (m_UrchinSaves.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_URCHINDEVI))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_UrchinSaves)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion URCHINDEVIL

#pragma region GWHITEDEVIL
HRESULT CMonster_Manager::Save_GWhiteDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_GWHITEDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_GWhiteDevil to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_GWhiteDevilSaves.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_GWhiteDevilSaves)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_GWhiteDevil(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_GWHITEDEVIL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		cout << strFileName.c_str() << endl;
#endif
		return S_OK;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CGiantWhiteDevil::GWHITEDEVILDESC tDesc;
		ReadFile(hFile, &tDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_GWhiteDevilSaves.push_back(tDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::GWhiteDevil_Clone(const LEVEL& _eLevel)
{
	if (m_GWhiteDevilSaves.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_GIANTWHITEDEVIL))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_GWhiteDevilSaves)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion GWHITEDEVIL

#pragma region ANCHANNIBAL
HRESULT CMonster_Manager::Save_AncientHannibal(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_ANCHANNIBAL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_AncientHannibal to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_AncHannibalSaves.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_AncHannibalSaves)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_AncientHannibal(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_ANCHANNIBAL_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		cout << strFileName.c_str() << endl;
#endif
		return S_OK;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CAncientHannibal::ANCHANNIBALDESC tDesc;
		ReadFile(hFile, &tDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_AncHannibalSaves.push_back(tDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::AncHannibalClone(const LEVEL& _eLevel)
{
	if (m_AncHannibalSaves.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_ANCIENTHANNIBAL))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_AncHannibalSaves)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion ANCHANNIBAL


#pragma region GATEKEEPERWOMAN
HRESULT CMonster_Manager::Save_GateKeeperWoman(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_GATEKEEP_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CMonster_Manager::Save_GateKeeperWoman to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_GateKeepSaves.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);

	for (auto& iter : m_GateKeepSaves)
	{
		WriteFile(hFile, &iter.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint Length = (_uint)lstrlen(iter.tMonsterDesc.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.tMonsterDesc.strModelName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::Load_GateKeeperWoman(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_GATEKEEP_NAME);
	wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());

	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		cout << strFileName.c_str() << endl;
#endif
		return S_OK;
	}

	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CGateKeeperWoman::GATEKEEPERWOMANDESC tDesc;
		ReadFile(hFile, &tDesc.tGobDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.tPawnStatus, sizeof(CPawn::PAWNSTATUS), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureName = new _tchar[iLength];
			ReadFile(hFile, pTextureName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tDesc.tMonsterDesc.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		ReadFile(hFile, &tDesc.tMonsterDesc.tSightDesc, sizeof(CMonster::SIGHTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tDesc.eInitAnimType, sizeof(CMonkeyDevil::INIT_ANIMTYPE), &dwByte, nullptr);
		m_GateKeepSaves.push_back(tDesc);
	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CMonster_Manager::GateKeeperWomanClone(const LEVEL& _eLevel)
{
	if (m_GateKeepSaves.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_GATEKEEPERWOMAN))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_GateKeepSaves)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}
#pragma endregion GATEKEEPERWOMAN
CMonster_Manager* CMonster_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMonster_Manager* pInstance = new CMonster_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMonster_Manager::Create to Failed");
	}
	return pInstance;
}

void CMonster_Manager::Free()
{
	__super::Free();
}
