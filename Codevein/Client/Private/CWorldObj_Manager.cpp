#include "CMainView.h"
#include "CImGui_Manager.h"
#include "json.h"
#include <fstream>
#include "CWorldObj_Manager.h"
#include "CImGui_Manager.h"
#include "CGameObject_Client.h"
#include "CGameInstance.h"
#include "CData_Manager.h"

CWorldObj_Manager::CWorldObj_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CManager(_pDevice, _pContext)
{

}

void CWorldObj_Manager::Release_Data()
{
	for (auto& iter : m_Stage1WorldModels)
		Safe_Release(iter);

	m_Stage1WorldModels.clear();
	m_WallObjList.clear();
}

CModel* CWorldObj_Manager::Find_WorldObjsModel(const _wstring& _strFindName)
{
	list<CModel*>::iterator it = find_if(m_Stage1WorldModels.begin(), m_Stage1WorldModels.end(), [&](auto& Pair)->bool
	{
		if (!lstrcmp(Pair->Get_FileData()->strFileName.c_str(), _strFindName.c_str()))
			return true;

		return false;
	});

	if (m_Stage1WorldModels.end() == it)
		return nullptr;

	return *it;
}

HRESULT CWorldObj_Manager::NativeConstruct()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		pGameInstance->Add_Thread(Start_InitThread, this);
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_Stage1()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{		
		pGameInstance->End_Threads();
		pGameInstance->Add_Thread(Start_Thread_Stage1, this);
		pGameInstance->Join_Thread();
		pGameInstance->End_Threads();
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

void CWorldObj_Manager::Start_InitThread(void* _pArg)
{
	CWorldObj_Manager* _pManager = static_cast<CWorldObj_Manager*>(_pArg);
	if (nullptr != _pManager)
		_pManager->Run_InitThread();
}

void CWorldObj_Manager::Run_InitThread()
{
	if (0 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_Init_Data();
	}
}

void CWorldObj_Manager::Start_Thread_Stage1(void* _pArg)
{
	CWorldObj_Manager* _pManager = static_cast<CWorldObj_Manager*>(_pArg);
	if (nullptr != _pManager)
		_pManager->Run_ThreadStage1();
}

void CWorldObj_Manager::Run_ThreadStage1()
{
	if (0 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_Stage1Data();
	}
}

HRESULT CWorldObj_Manager::Load_Stage2()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		pGameInstance->End_Threads();
		pGameInstance->Add_Thread(Start_Thread_Stage2, this);
		pGameInstance->Join_Thread();
		pGameInstance->End_Threads();
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

void CWorldObj_Manager::Start_Thread_Stage2(void* _pArg)
{
	CWorldObj_Manager* _pManager = static_cast<CWorldObj_Manager*>(_pArg);
	if (nullptr != _pManager)
		_pManager->Run_ThreadStage2();
}

void CWorldObj_Manager::Run_ThreadStage2()
{
	if (0 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_Stage2Data();
	}
}

HRESULT CWorldObj_Manager::Load_Init_Data()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_matrix TransformationMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
		PATHS Path{ L"Game", L"Environment", L"CheckPoint", L"FBX_Converter" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

		for (auto& iter : pFileGroup->vecFileDataList)
		{
			m_CheckPointsModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
		}
	}
	END_SINGLETON(pGameInstance);

	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		pDataManager->Plus_Init_PlusThreadCount();
	}
	END_SINGLETON(pDataManager);

	m_iThreadCount = 0;
	return E_NOTIMPL;
}

HRESULT CWorldObj_Manager::Load_Stage1Data()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_matrix TransformationMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
		PATHS Path{ L"Game", L"Environment", L"Stage1", L"FBX_Converter" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

		for (auto& iter : pFileGroup->vecFileDataList)
		{
			CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
			m_Stage1WorldModels.push_back(pModel);
		}
	}
	END_SINGLETON(pGameInstance);
	m_iThreadCount = 0;
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_Stage2Data()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PATHS Path{ L"Game", L"Environment", L"Stage12", L"FBX_Converter" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

		for (auto& iter : pFileGroup->vecFileDataList)
		{
			CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
			m_Stage1WorldModels.push_back(pModel);
		}
	}
	END_SINGLETON(pGameInstance);
	m_iThreadCount = 0;
	return S_OK;
}

_string CWorldObj_Manager::Research_Model(_string _strModelName)
{
	_string strModelName = _strModelName;
	char TEXT[MAX_PATH] = "";
	strcpy_s(TEXT, m_strFindName.c_str());
	_bool bIsTrue = false;
	bIsTrue = ImGui::InputText("Find", TEXT, sizeof(char) * MAX_PATH);
	m_strFindName = TEXT;
	if (!strcmp(TEXT, ""))
	{
		for (auto& iter : m_Stage1WorldModels)
		{
			FILEDATA* pFileData = iter->Get_FileData();
			if (nullptr != pFileData)
			{
				string str;
				str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
				if (ImGui::Selectable(str.c_str()))
				{
					strModelName = str;
					BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
					{
						pImGuiManager->Set_DebugObject_Model(iter);
					}
					END_SINGLETON(pImGuiManager);
					ImGui::SetItemDefaultFocus();
				}
			}
		}
	}
	else
	{
		if (true == bIsTrue)
		{
			m_ShowModels.clear();
			for (auto& iter : m_Stage1WorldModels)
			{
				FILEDATA* pFileData = iter->Get_FileData();
				if (nullptr != pFileData)
				{
					string str;
					str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());

					if (true == CFuncClass::Same_Text(TEXT, str))
					{
						m_ShowModels.push_back(iter);
					}
				}
			}
		}

		for (auto& iter : m_ShowModels)
		{
			FILEDATA* pFileData = iter->Get_FileData();
			if (nullptr != pFileData)
			{
				string str;
				str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
				if (ImGui::Selectable(str.c_str()))
				{
					strModelName = str;
					BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
					{
						pImGuiManager->Set_DebugObject_Model(iter);
					}
					END_SINGLETON(pImGuiManager);
					ImGui::SetItemDefaultFocus();
				}
			}
		}
	}
	return strModelName;
}

void CWorldObj_Manager::Setting_CheckPointModel()
{
	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
	{
		pImGuiManager->Set_DebugObject_Model(m_CheckPointsModel);
	}
	END_SINGLETON(pImGuiManager);
}

HRESULT CWorldObj_Manager::Save_Data(const string& _strFolderName)
{
	if (FAILED(Save_FloorData(_strFolderName)))
		return E_FAIL;

	if (FAILED(Save_WOEffectData(_strFolderName)))
		return E_FAIL;

	m_SaveFloorList.clear();
	m_SaveTerrianList.clear();
	m_SaveWordEffectList.clear();
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_Data(const string& _strPath)
{
	m_SaveFloorList.clear();
	m_SaveTerrianList.clear();
	m_SaveWordEffectList.clear();
	Load_FloorData(_strPath);
	Load_WOEffectData(_strPath);

	return S_OK;
}

HRESULT CWorldObj_Manager::Clone_Objects(const LEVEL& _eLevel)
{
	Clone_Floor(_eLevel);
	Clone_WOEffect(_eLevel);

	m_SaveFloorList.clear();
	m_SaveTerrianList.clear();
	m_SaveWordEffectList.clear();
	return S_OK;
}

HRESULT CWorldObj_Manager::Save_TerrianData(const string& _strFolderPath)
{
	string strFileName = _strFolderPath;
	strFileName.append("\\");
	strFileName.append(SAVE_NAME_TERRAIN);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Save_TerrianData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = (_uint)m_SaveTerrianList.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (auto& iter : m_SaveTerrianList)
	{
		WriteFile(hFile, &iter.tGobData, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.iNavIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &iter.iNumVertices, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &iter.iNumVerticesX, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &iter.iNumVerticesZ, sizeof(_uint), &dwByte, nullptr);
		for (_uint i = 0; i < iter.iNumVertices; ++i)
			WriteFile(hFile, &iter.pVertices[i], sizeof(_float3), &dwByte, nullptr);

		if (nullptr != iter.pNavigation)
		{
			string Path = _strFolderPath;
			_wstring str;
			str.assign(Path.begin(), Path.end());
			iter.pNavigation->Set_CurIndex(iter.iNavIndex);
			iter.pNavigation->Save_File(str.c_str(), iter.iNavIndex);
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Save_WOEffectData(const _string& _strFolderPath)
{
	string strFileName = _strFolderPath;
	strFileName.append("\\");
	strFileName.append(SAVE_WOEFFECT_NAME);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Save_WOEffectData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = (_uint)m_SaveWordEffectList.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (auto& iter : m_SaveWordEffectList)
	{
		WriteFile(hFile, &iter.tGobClientDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		WriteFile(hFile, &iter.iProtoSelectIndex, sizeof(_uint), &dwByte, 0);
		{
			_uint Length = (_uint)lstrlen(iter.strPrototype.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.strPrototype.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Save_FloorData(const string& _strFolderPath)
{
	string strFileName = _strFolderPath;
	strFileName.append("\\");
	strFileName.append(SAVE_FLOOR_NAME);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Save_FloorData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = (_uint)m_SaveFloorList.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (auto& iter : m_SaveFloorList)
	{
		WriteFile(hFile, &iter.tGobClientDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		{
			_uint Length = (_uint)strlen(iter.strModelName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.strModelName.c_str(), sizeof(_char) * Length, &dwByte, 0);
		}
		{
			_uint Length = (_uint)lstrlen(iter.strPrototype.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.strPrototype.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		WriteFile(hFile, &iter.tColliderDesc, sizeof(CWorldObj::COLLIDERDESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_TerrianData(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_NAME_TERRAIN);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Load_TerrianData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CTerrain::TERRIANSAVEDESC tData;
		ReadFile(hFile, &tData.tGobData, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tData.iNavIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tData.iNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tData.iNumVerticesX, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &tData.iNumVerticesZ, sizeof(_uint), &dwByte, nullptr);
		tData.pVertices = new _float3[tData.iNumVertices];
		ReadFile(hFile, tData.pVertices, sizeof(_float3) * tData.iNumVertices, &dwByte, nullptr);
		{
			string str = _strPath;
			_wstring Path;
			Path.assign(str.begin(), str.end());
			Path.append(L"\\");
			Path.append(L"Nav");
			Path.append(L"\\");
			_tchar NAME[100];
			wsprintf(NAME, L"%d", tData.iNavIndex);
			Path.append(NAME);
			Path.append(L".dat");
			tData.strPath = Path;
		}
		m_SaveTerrianList.push_back(tData);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_WOEffectData(const _string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_WOEFFECT_NAME);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Load_WOEffectData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CWOEffect::WODEFFECTDESC tData;
		ReadFile(hFile, &tData.tGobClientDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		ReadFile(hFile, &tData.iProtoSelectIndex, sizeof(_uint), &dwByte, 0);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pProtoName = new _tchar[iLength];
			ReadFile(hFile, pProtoName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tData.strPrototype = pProtoName;
			Safe_Delete_Array(pProtoName);
		}
		m_SaveWordEffectList.push_back(tData);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Load_FloorData(const string& _strPath)
{
	string strFileName = _strPath;
	strFileName.append("\\");
	strFileName.append(SAVE_FLOOR_NAME);

	_wstring strPath;
	strPath.assign(strFileName.begin(), strFileName.end());
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
#ifdef _DEBUG
		MSG_BOX("CWorldObj_Manager::Load_FloorData to Failed");
		cout << strFileName.c_str() << endl;
#endif
		return E_FAIL;
	}
	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		CWorldObj::WORLDSAVES tData;
		ReadFile(hFile, &tData.tGobClientDesc, sizeof(CGameObject_Client::GOBCLIENTDESC), &dwByte, nullptr);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_char* pTextureName = new _char[iLength];
			ReadFile(hFile, pTextureName, sizeof(_char) * iLength, &dwByte, nullptr);
			tData.strModelName = pTextureName;
			Safe_Delete_Array(pTextureName);
		}
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pProtoName = new _tchar[iLength];
			ReadFile(hFile, pProtoName, sizeof(_tchar) * iLength, &dwByte, nullptr);
			tData.strPrototype = pProtoName;
			Safe_Delete_Array(pProtoName);
		}
		ReadFile(hFile, &tData.tColliderDesc, sizeof(CWorldObj::COLLIDERDESC), &dwByte, nullptr);
		m_SaveFloorList.push_back(tData);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWorldObj_Manager::Clone_Terrain(const LEVEL& _eLevel)
{
	if (m_SaveTerrianList.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_TERRAIN))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_SaveTerrianList)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}

HRESULT CWorldObj_Manager::Clone_Floor(const LEVEL& _eLevel)
{
	if (m_SaveFloorList.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;

		for (auto& iter : m_SaveFloorList)
		{
			for (auto& NAME : pImGuiManager->Get_ProtoObjMap())
			{
				if (nullptr == pCloneObject)
				{
					if (!lstrcmp(NAME.first, iter.strPrototype.c_str()))
					{
						pCloneObject = NAME.second;
						break;
					}
				}
				else 
				{
					if (!lstrcmp(pCloneObject->Get_Proto(), iter.strPrototype.c_str()))
					{
						break;
					}
					else
					{
						if (!lstrcmp(NAME.first, iter.strPrototype.c_str()))
						{
							pCloneObject = NAME.second;
							break;
						}
					}
				}
			}
			if (nullptr != pCloneObject)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);

				if (true == CFuncClass::Same_Text("Wall", iter.strModelName))
					m_WallObjList.push_back(static_cast<CWorldObj*>(pGameObject));
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}

HRESULT CWorldObj_Manager::Clone_WOEffect(const LEVEL& _eLevel)
{
	if (m_SaveWordEffectList.size() <= 0)
		return E_FAIL;

	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_WOEFFECT))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_SaveWordEffectList)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return S_OK;
}

CWorldObj_Manager* CWorldObj_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWorldObj_Manager* pInstance = new CWorldObj_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CWorldObj_Manager::Create To Failed");
	}
	return pInstance;
}

void CWorldObj_Manager::Free()
{
	for (auto& iter : m_Stage1WorldModels)
		Safe_Release(iter);


	Safe_Release(m_CheckPointsModel);

	__super::Free();
}


/*
	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CGameObject_Client* pCloneObject = nullptr;

		for (auto& iter : m_SaveFloorList)
		{
			for (auto& NAME : pImGuiManager->Get_ProtoObjMap())
			{
				if (nullptr == pCloneObject)
				{
					if (!lstrcmp(NAME.first, PROTO_GOB_WORLDOBJ))
					{
						pCloneObject = NAME.second;
						break;
					}
				}
				else
				{
					if (!lstrcmp(pCloneObject->Get_Proto(), iter.strPrototype.c_str()))
					{
						break;
					}
					else
					{
						if (!lstrcmp(NAME.first, PROTO_GOB_WORLDOBJ))
						{
							pCloneObject = NAME.second;
							break;
						}
					}
				}
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
	}
	END_SINGLETON(pImGuiManager);
*/


/*
CGameObject_Client* pCloneObject = nullptr;
		for (auto& iter : pImGuiManager->Get_ProtoObjMap())
		{
			if (!lstrcmp(iter.first, PROTO_GOB_WORLDOBJ))
			{
				pCloneObject = iter.second;
				break;
			}
		}
		if (nullptr != pCloneObject)
		{
			for (auto& iter : m_SaveFloorList)
			{
				CGameObject* pGameObject = nullptr;
				pCloneObject->Load_Data(&pGameObject, _eLevel, &iter);
			}
		}
*/