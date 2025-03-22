#include "FilePathViewer_Tool.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager_Tool.h"
#include "Model_Tool.h"

CFilePathViewer_Tool::CFilePathViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CFilePathViewer_Tool::NativeConstruct()
{
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
        {
            m_pFileGroup = pGameInstance->Get_FileGroup();
        }
        RELEASE_INSTANCE(CGameInstance);
    }

    m_pName = "FilePathViewer";
    // Main
    m_tMainView = DOCKDESC("MainView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar , ImVec2(1.f, 1.f), ImVec2(1000.f, 1000.f), true);
    // Path View
    m_tPathView = DOCKDESC("PathView", ImGuiWindowFlags_NoFocusOnAppearing);
    // FolderView
    m_tFolderView = DOCKDESC("FolderView", ImGuiWindowFlags_NoFocusOnAppearing);
    // FolderView
    m_tFileView = DOCKDESC("FileView", ImGuiWindowFlags_NoFocusOnAppearing);

    
    Create_Bin(m_pFileGroup, L"AnimationModel", L"Model", L"FBX", L"FBX_Converter", L"Textures");

    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
        {
            pGameInstance->Load_FirstFolder(L"..\\..\\Resources");
            m_pFileGroup = pGameInstance->Get_FileGroup();
        }
        RELEASE_INSTANCE(CGameInstance);
    }

    return S_OK;
}

HRESULT CFilePathViewer_Tool::Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Tick(_dTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFilePathViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Late_Tick(_dTimeDelta)))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

void CFilePathViewer_Tool::Show_Window()
{
    if (nullptr == m_pFileGroup)
        return;

    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + 10.f, 5.f + imVec2.y + 20.f), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(500.f, 500.f), ImGuiCond_Appearing);
    ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
    {
        {
            m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
            if (false == m_bInitSetting)
            {
                ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
                ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

                ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                ImGuiID dock_id_Path = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);

                ImGuiID dock_id_Foloder = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f,
                    &dock_main_id, &dock_main_id);

                ImGuiID dock_id_File = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down , 0.5f,
                    &dock_id_Foloder, &dock_main_id);
              
                m_tPathView.iID = dock_id_Path;
                m_tFolderView.iID = dock_id_Foloder;
                m_tFileView.iID = dock_id_File;
                // Perspective , Heirecky
                ImGui::DockBuilderDockWindow(m_tPathView.strName.c_str(), dock_id_Path);
                ImGui::DockBuilderDockWindow(m_tFolderView.strName.c_str(), dock_id_Foloder);
                ImGui::DockBuilderDockWindow(m_tFileView.strName.c_str(), dock_id_File);
                ImGui::DockBuilderFinish(m_tMainView.iID);

                m_bInitSetting = true;
            }
            ImGui::DockSpace(m_tMainView.iID);
        }
        // Path View
        {
            // Path View
            ImGui::Begin(m_tPathView.strName.c_str(), &m_tPathView.bEnable, m_tPathView.imGuiWindowFlags);
            {
                PathView();
            }
            ImGui::End();
        }
        // Folder View
        {
            // Folder View
            ImGui::Begin(m_tFolderView.strName.c_str(), &m_tFolderView.bEnable, m_tFolderView.imGuiWindowFlags);
            {
                FolderView();
            }
            ImGui::End();
        }
        // File View
        {
            // File View
            ImGui::Begin(m_tFileView.strName.c_str(), &m_tFileView.bEnable, m_tFileView.imGuiWindowFlags);
            {
                FileView();
            }
            ImGui::End();
        }
    }
    ImGui::End();
}

void CFilePathViewer_Tool::Create_Bin(FILEGROUP* _pFileGroup, const _wstring& _strAnimModelFolderName, const _wstring& _strModelFolderName,
    const _wstring& _FBXModelFolderName, const _wstring& _strConvertModelFolderName, const _wstring& _strTextureName)
{
    if (nullptr == _pFileGroup)
        return;

    _bool IsFBXFolder = false;
    _bool IsBin = false;
    FILEGROUP* pFBX = nullptr;
    for (auto& iter : _pFileGroup->lsUnderList)
    {
        if (!lstrcmp(iter->strUppderName.c_str(), _FBXModelFolderName.c_str()))
        {
            IsFBXFolder = true;
            pFBX = iter;
            break;
        }
    }

    if (true == IsFBXFolder)
    {
        for (auto& iter : _pFileGroup->lsUnderList)
        {
            if (!lstrcmp(iter->strUppderName.c_str(), _strConvertModelFolderName.c_str()))
            {
                IsBin = true;
                break;
            }
        }

        if (false == IsBin)
        {
            FILEGROUP* pParentsGroup = _pFileGroup;
            _uint MODEL = 0;
            while (true)
            {
                if (nullptr == pParentsGroup)
                    break;

                if (!lstrcmp(_strModelFolderName.c_str(), pParentsGroup->strUppderName.c_str()))
                {
                    MODEL = 1;
                    break;
                }
                else  if (!lstrcmp(_strAnimModelFolderName.c_str(), pParentsGroup->strUppderName.c_str()))
                {
                    MODEL = 2;
                    break;
                }

                pParentsGroup = pParentsGroup->pParentsUpper;
            }

            CModel_Tool::TYPE iType = CModel_Tool::TYPE_END;
            if (1 == MODEL)
            {
                iType = CModel_Tool::TYPE_NONANIM;
            }
            else if (2 == MODEL)
            {
                iType = CModel_Tool::TYPE_ANIM;
            }

            list<CModel_Tool*> LIST;
            for (auto& iter : pFBX->vecFileDataList)
            {
                CModel_Tool* _pModelTool = CModel_Tool::Create(m_pDevice, m_pContext, iType,
                    _pFileGroup, &iter, _strTextureName.c_str());
                LIST.push_back(_pModelTool);
            }

            if (iType == CModel_Tool::TYPE_NONANIM)
            {
                for (auto& iter : LIST)
                    iter->Save_NonAnimModel();
            }
            else
            {
                for (auto& iter : LIST)
                    iter->Save_AnimModel();
            }

            for (auto& iter : LIST)
                Safe_Release(iter);
        }
    }

    for (auto& iter : _pFileGroup->lsUnderList)
    {
        Create_Bin(iter, _strAnimModelFolderName, _strModelFolderName, _FBXModelFolderName, _strConvertModelFolderName, 
            _strTextureName);
    }
}

void CFilePathViewer_Tool::PathView()
{
    {
        _char chName[MAX_PATH] = { "" };
        strcpy_s(chName, m_strFindFolderFileName.c_str());
       _bool bIsInput =  ImGui::InputText("Find Folder", chName, sizeof(_char) * MAX_PATH);
        m_strFindFolderFileName = chName;

        const char* pData[] = { "Folder", "File" };
        ImGui::ListBox("Select Folder Or File", (_int*)&m_iSelectIndex, pData, 2);
        if (!strcmp("", m_strFindFolderFileName.c_str()))
        {
            _uint iIndex = 0;
            _string str = CMethod::ConvertWToS(m_pFileGroup->strUppderName);
            if (ImGui::TreeNode(str.c_str()))
            {
                for (auto& iter : m_pFileGroup->lsUnderList)
                {
                    Show_FolderAndFile_NotFind(iter, &iIndex);
                }
                ImGui::TreePop();
            }
        }
        else
        {
            if (0 == m_iSelectIndex)
            {
                if (true == bIsInput)
                {
                    m_FindFolderList.clear();
                    Find_Folder(m_pFileGroup);
                }
                _uint iIndex = 0;
                for (auto& iter : m_FindFolderList)
                {
                    _string Name = CMethod::ConvertWToS(iter->strUppderName);
                    if (ImGui::Selectable(Name.c_str()))
                    {
                        m_pSelectFileGroup = iter;
                    }
                }
            }
            else if (1 == m_iSelectIndex)
            {
                if (true == bIsInput)
                {
                    m_FindFileDataList.clear();
                    Find_FileData(m_pFileGroup);
                }

                for (auto& iter : m_FindFileDataList)
                {
                    _string Name = CMethod::ConvertWToS(iter->strFileName);
                    if (ImGui::Selectable(Name.c_str()))
                    {
                        m_pSelectFileData = iter;
                    }
                }
            }
        }
    }
}

void CFilePathViewer_Tool::FolderView()
{
    if (nullptr == m_pSelectFileGroup)
        return;

    _string Name = CMethod::ConvertWToS(m_pSelectFileGroup->strUppderName);
    _string Path = CMethod::ConvertWToS(m_pSelectFileGroup->strPath);

    _string strParetns = "NULL";
    if (nullptr != m_pSelectFileGroup->pParentsUpper)
    {
        strParetns = CMethod::ConvertWToS(m_pSelectFileGroup->strUppderName);
    }

    _uint iIndex = (_uint)Path.find("Resources");

    _string NameView = "Name: ";
    NameView.append(Name);

    _string PathRelative = Path.substr(iIndex, Path.length());

    _string PathRelativeView = "<Path>";
    PathRelativeView.append(PathRelative);

    _string ParrentsView = "ParentsName: ";
    ParrentsView.append(strParetns);

   ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), NameView.c_str());
   ImGui::NewLine();
   ImGui::TextWrapped(PathRelativeView.c_str());
   ImGui::NewLine();
   ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), ParrentsView.c_str());
   ImGui::NewLine();

    for (auto& iter : m_pSelectFileGroup->lsUnderList)
    {
        Show_FolderAndFile_NotFind(iter, &iIndex);
    }
    
    if (nullptr != m_pSelectFileGroup)
    {
        if (ImGui::Button("Open Folder"))
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
            {
                pGameInstance->Open_FolderSelectRoot(m_pSelectFileGroup->strPath, L"Tool", nullptr);
            }
            RELEASE_INSTANCE(CGameInstance);
        }
    }
}

void CFilePathViewer_Tool::FileView()
{
    if (nullptr == m_pSelectFileData)
        return;

    _string Name = CMethod::ConvertWToS(m_pSelectFileData->strFileName);
    _string Path = CMethod::ConvertWToS(m_pSelectFileData->strFilePath);
    _uint iIndex = (_uint)Path.find("Resources");

    _string NameView = "Name: ";
    NameView.append(Name);

    _string PathRelative = Path.substr(iIndex, Path.length());

    _string PathRelativeView = "<Path>";
    PathRelativeView.append(PathRelative);

   _string Num = "";
   Num = CMethod::Combine_Int(m_pSelectFileData->iIndex, Num);

   _string NumView = "Num: ";
   NumView.append(Num);

   ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), NameView.c_str());
   ImGui::NewLine();
   ImGui::TextWrapped(PathRelativeView.c_str());
   ImGui::NewLine();
   ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), NumView.c_str());

   if (nullptr != m_pSelectFileData)
   {
       if (ImGui::Button("Open File"))
       {
           CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
           {
               pGameInstance->Open_FolderSelectRoot(m_pSelectFileData->strFilePath, L"Tool", nullptr);
           }
           RELEASE_INSTANCE(CGameInstance);
       }
   }
}

void CFilePathViewer_Tool::Show_FolderAndFile_NotFind(FILEGROUP* _pFileGroup, _uint* _pIndex, const _bool& _bFindFolder)
{
    if (nullptr == _pFileGroup)
        return;

    _string Name = CMethod::ConvertWToS(_pFileGroup->strUppderName);
    if (nullptr != _pIndex)
    {
        Name.append(" - ");
        Name = CMethod::CMethod::Combine_Int(*_pIndex, Name);

        *_pIndex = *_pIndex + 1;
    }
    if (ImGui::TreeNode(Name.c_str()))
    {
        ImGui::SameLine();
        if(ImGui::Button(Name.c_str()))
            m_pSelectFileGroup = _pFileGroup;

        for (auto& iter : _pFileGroup->lsUnderList)
            Show_FolderAndFile_NotFind(iter);

                if (false == _bFindFolder)
        {
            for (auto& FILE : _pFileGroup->vecFileDataList)
            {
                _string str = CMethod::ConvertWToS(FILE.strFileName);
                if (ImGui::Selectable(str.c_str()))
                {
                    m_pSelectFileData = &FILE;
                }
            }
        }

        ImGui::TreePop();
    }
}

void CFilePathViewer_Tool::Find_Folder(FILEGROUP* _pFileGroup)
{
    if (nullptr == _pFileGroup)
        return;

    _string Name = CMethod::ConvertWToS(_pFileGroup->strUppderName);
    if (true == CMethod::Is_Same_Text(Name.c_str(), m_strFindFolderFileName.c_str()))
    {
        m_FindFolderList.push_back(_pFileGroup);
    }

    for (auto& iter : _pFileGroup->lsUnderList)
    {
        Find_Folder(iter);
    }
}

void CFilePathViewer_Tool::Find_FileData(FILEGROUP* _pFileGroup)
{
    if (nullptr == _pFileGroup)
        return;

    for (auto& iter : _pFileGroup->lsUnderList)
    {
        Find_FileData(iter);
    }

    for (auto& iter : _pFileGroup->vecFileDataList)
    {
        _string Name = CMethod::ConvertWToS(iter.strFileName);

        if (true == CMethod::Is_Same_Text(Name.c_str(), m_strFindFolderFileName.c_str()))
        {
            m_FindFileDataList.push_back(&iter);
        }
    }
}

void CFilePathViewer_Tool::Open_Folder()
{
}

void CFilePathViewer_Tool::Open_File()
{
}

CFilePathViewer_Tool* CFilePathViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFilePathViewer_Tool* pInstance = new CFilePathViewer_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CFilePathViewer_Tool::Create To Failed");
#endif
    }
    return pInstance;
}

void CFilePathViewer_Tool::Free()
{
    __super::Free();
}
