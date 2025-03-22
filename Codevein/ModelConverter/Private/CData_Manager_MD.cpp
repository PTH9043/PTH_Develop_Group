#include "json.h"
#include <fstream>
#include <direct.h>
#include "CData_Manager_MD.h"
#include "CGameInstance.h"
#include "DirectXTK/SimpleMath.h"


IMPLEMENT_SINGLETON(CData_Manager_MD);


CData_Manager_MD::CData_Manager_MD()
{
}

HRESULT CData_Manager_MD::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    this->m_pDevice = _pDevice;
    this->m_pContext = _pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    return S_OK;
}

HRESULT CData_Manager_MD::Save_Data_NonAnim()
{
    for (auto& iter : m_AddModelList)
    {
        iter->Save_NonAnimModel();
    }
    return S_OK;
}

HRESULT CData_Manager_MD::Save_Data_Anim()
{
    for (auto& iter : m_AddAnimModelList)
    {
        iter->Save_AnimModel();
    }
    return S_OK;
}

HRESULT CData_Manager_MD::Load_Data_FBX()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
    //     Load_ModelData_1();
     Load_ModelData_2();
         //     Load_ModelData_3();
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}


HRESULT CData_Manager_MD::Load_Data_NonAnim()
{ 
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        _vector vScale = XMLoadFloat3(&_float3(0.5f, 0.5f, 0.5f));
        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

        ULONGLONG dTime = GetTickCount64();
        //{
        //    PATHS Path{ L"Game", L"Environment",  L"ShoppingMall" , L"Floors", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Environment", L"ShoppingMall" ,  L"Walls", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Environment",  L"ShoppingMall" ,  L"Ceil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Environment", L"ShoppingMall" ,  L"Objects", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowModelList.push_back(pModel);
        //    }
        //}


        {
            PATHS Path{ L"Game", L"Equip", L"Weapon" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            for (auto& iter : pFileGroup->lsUnderList)
            {
                FILEGROUP* pFiles = iter->Find_Group(L"FBX_Converter");
                if (nullptr != pFiles)
                {
                    for (_uint i = 0; i < (_uint)pFiles->vecFileDataList.size(); ++i)
                    {
                        CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                            pFiles, &pFiles->vecFileDataList[i], TransformationMatrix);
                        if (nullptr != pModel)
                            m_ShowModelList.push_back(pModel);
                        else
                        {
                            cout << pFiles[i].strUppderName.c_str() << endl;
                        }
                    }
                }
            }
        }
        ULONGLONG dEndTime = GetTickCount64();
        cout << "Load_Bin " << dEndTime - dTime << endl;
    }
    END_SINGLETON(pGameInstance)
    return E_FAIL;
}

HRESULT CData_Manager_MD::Load_Data_Anim()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        ULONGLONG dTime = GetTickCount64();
        //{
        //    PATHS Path{ L"Game", L"Character", L"AncientHannibal", L"FBX_Converter"};
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for(auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);

        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        {
            PATHS Path{ L"Game", L"Character", L"GateKeeperWoman", L"FBX_Converter" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
            _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
            _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
            _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
            for (auto& iter : pFileGroup->vecFileDataList)
            {
                CModel* pModel = CModel::Create(m_pDevice, m_pContext,
                    CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
                m_ShowAnimModelList.push_back(pModel);
            }
        }

     //   {
     //       PATHS Path{ L"Game", L"Character", L"Player", L"Io", L"FBX_Converter" };
     //       FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
     //       for (auto& iter : pFileGroup->vecFileDataList)
     //       {
     //           CModel* pModel = CModel::Create(m_pDevice, m_pContext,
     //               CModel::TYPE_ANIM, pFileGroup, &iter);
     ////           PATHS Anim{ L"Game", L"Character", L"Player", L"Io",  L"Animations_Converter"};
     //  //         FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
     ////           pModel->Load_Animation(pAnim);
     //           m_ShowAnimModelList.push_back(pModel);
     //       }
     //   }

        //{
        //    PATHS Path{ L"Game", L"Character", L"GiantWhiteDevil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"MonkeyDevil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"SlimeDevil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"UrchinDevil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"WolfDevil", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //    _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //    _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //PATHS Path{ L"Game", L"Character", L"GiantDevil", L"FBX_Converter" };
        //FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //_vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //_vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //_vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //for (auto& iter : pFileGroup->vecFileDataList)
        //{
        //    CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //        CModel::TYPE_ANIM, pFileGroup, &iter, TransformationMatrix);
        //    m_ShowAnimModelList.push_back(pModel);
        //}
        //{
        //    PATHS Path{ L"Game", L"Character", L"Player", L"Jack", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter);
        //        m_ShowAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"Player", L"Mia", L"FBX_Converter" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    for (auto& iter : pFileGroup->vecFileDataList)
        //    {
        //        CModel* pModel = CModel::Create(m_pDevice, m_pContext,
        //            CModel::TYPE_ANIM, pFileGroup, &iter);
        //        m_ShowAnimModelList.push_back(pModel);
        //        PATHS Anim{ L"Game", L"Character", L"Player",  L"Animations_Converter" };
        //        FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
        //        pModel->Load_Animation(pAnim);
        //    }
        //}

        ULONGLONG dEndTime = GetTickCount64();
        cout << "Load_Anim " << dEndTime - dTime << endl;
    }
    END_SINGLETON(pGameInstance)
    return S_OK;
}

void CData_Manager_MD::Load_ModelData_1()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        ULONGLONG dTime = GetTickCount64();
        {
            PATHS Path{ L"Game", L"Environment", L"CheckPoint" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
            for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
            {
                _matrix matWorld = XMMatrixScaling(0.000001f, 0.000001f, 0.000001f);

                CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
                    CModel_FBX::TYPE_NONANIM, pFileGroup, i, matWorld);

                if (nullptr == pModel)
                    continue;
                m_AddModelList.push_back(pModel);
            }
        }
        ULONGLONG dEndTime = GetTickCount64();
        cout << "Load_FBX " << dEndTime - dTime << endl;
    }
    END_SINGLETON(pGameInstance)
}

void CData_Manager_MD::Load_ModelData_2()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        ULONGLONG dTime = GetTickCount64();
        {
           /* PATHS Path{ L"Game", L"Character", L"AncientHannibal" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
            for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
            {
                _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
                _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
                _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
                _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
                CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
                    CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);
                m_AddAnimModelList.push_back(pModel);
            }*/
        }
        {
            PATHS Path{ L"Game", L"Character", L"Player", L"Io" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
                CModel_FBX::TYPE_ANIM, pFileGroup, 0);

            FILEGROUP* pAnimations = pFileGroup->Find_Group(L"Animations");
            for (auto& iter : pAnimations->vecFileDataList)
            {
                string strPath;
                strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
                pModel->Create_Animation(pFileGroup, &iter);
            }

            m_AddAnimModelList.push_back(pModel);
        }

        //{
        //    PATHS Path{ L"Game", L"Character", L"Player", L"Io" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //        CModel_FBX::TYPE_NONANIM, pFileGroup, 1);

        //    m_AddModelList.push_back(pModel);
        //}


        //{
        //    PATHS Path{ L"Game", L"Character", L"GiantWhiteDevil" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);
        //     /*   FILEGROUP* pAnimations = pFileGroup->Find_Group(L"Animations");
        //        for (auto& iter : pAnimations->vecFileDataList)
        //        {
        //            string strPath;
        //            strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
        //            pModel->Create_Animation(pFileGroup, &iter);
        //        }*/
        //        m_AddAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"MonkeyDevil" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);
        //     /*   FILEGROUP* pAnimations = pFileGroup->Find_Group(L"Animations");
        //        for (auto& iter : pAnimations->vecFileDataList)
        //        {
        //            string strPath;
        //            strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
        //            pModel->Create_Animation(pFileGroup, &iter);
        //        }*/
        //        m_AddAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"SlimeDevil" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);

       

        //        m_AddAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"UrchinDevil" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);

        //  

        //        m_AddAnimModelList.push_back(pModel);
        //    }
        //}

        //{
        //    PATHS Path{ L"Game", L"Character", L"WolfDevil" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
        //        _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
        //        _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
        //        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddAnimModelList.push_back(pModel);
        //    }
        //}

        {
     /*       PATHS Path{ L"Game", L"Character", L"GiantDevil" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
            for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
            {
                _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
                _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
                _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
                _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
                CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
                    CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);

                FILEGROUP* pAnimations = pFileGroup->Find_Group(L"Animations");
                for (auto& iter : pAnimations->vecFileDataList)
                {
                    string strPath;
                    strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
                    pModel->Create_Animation(pFileGroup, &iter);
                }

                m_AddAnimModelList.push_back(pModel);
            }*/


 //           {
 //               PATHS Path{ L"Game", L"Character", L"GateKeeperWoman" };
 //               FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
 //               FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
 //               for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
 //               {
 //                   _vector vScale = XMLoadFloat3(&_float3(0.1f, 0.1f, 0.1f));
 //                   _vector vRotation = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 0.f));
 //                   _vector vPosition = XMLoadFloat4(&_float4(0.0f, 0.0f, 0.f, 1.f));
 //                   _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
 //                   CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
 //                       CModel_FBX::TYPE_ANIM, pFileGroup, i, TransformationMatrix);
 ///*                      FILEGROUP* pAnimations = pFileGroup->Find_Group(L"Animations");
 //                      for (auto& iter : pAnimations->vecFileDataList)
 //                      {
 //                          string strPath;
 //                          strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
 //                          pModel->Create_Animation(pFileGroup, &iter);
 //                      }*/
 //                   m_AddAnimModelList.push_back(pModel);
 //               }
 //           }
        }

        ULONGLONG dEndTime = GetTickCount64();
        cout << "Load_Anim_FBX " << dEndTime - dTime << endl;
    }
    END_SINGLETON(pGameInstance)
}

void CData_Manager_MD::Load_ModelData_3()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        _matrix TransformationMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"HannibalSword" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MardukBayonet" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MardukGreatSword" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MardukHalberd" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MilitaryBayonet" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    for (_uint i = 0; i < (_uint)(pFiles->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MilitaryCatWoman" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    for (_uint i = 0; i < (_uint)(pFileGroup->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}
        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"MilitaryGreatSword" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    for (_uint i = 0; i < (_uint)(pFileGroup->vecFileDataList.size()); ++i)
        //    {
        //        CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //            CModel_FBX::TYPE_NONANIM, pFileGroup, i);
        //        m_AddModelList.push_back(pModel);
        //    }
        //}


        //{
        //    PATHS Path{ L"Game", L"Equip", L"Weapon", L"AnceintHannibal_Weapon" };
        //    FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
        //    FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
        //    if (nullptr != pFiles)
        //    {
        //        for (_uint i = 0; i < (_uint)pFiles->vecFileDataList.size(); ++i)
        //        {
        //            CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
        //                CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
        //            if (nullptr != pModel)
        //                m_AddModelList.push_back(pModel);
        //            else
        //            {
        //                cout << pFiles[i].strUppderName.c_str() << endl;
        //            }
        //        }
        //    }
        //}
             PATHS Path{ L"Game", L"Effect"};
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            FILEGROUP* pFiles = pFileGroup->Find_Group(L"FBX");
            if (nullptr != pFiles)
            {
                for (_uint i = 0; i < (_uint)pFiles->vecFileDataList.size(); ++i)
                {
                    CModel_FBX* pModel = CModel_FBX::Create(m_pDevice, m_pContext,
                        CModel_FBX::TYPE_NONANIM, pFileGroup, i, TransformationMatrix);
                    if (nullptr != pModel)
                        m_AddModelList.push_back(pModel);
                    else
                    {
                        cout << pFiles[i].strUppderName.c_str() << endl;
                    }
                }
            }
    }
    END_SINGLETON(pGameInstance)
}

void CData_Manager_MD::Free()
{
    for (auto& iter : m_ShowModelList)
        Safe_Release(iter);

    for (auto& iter : m_AddModelList)
        Safe_Release(iter);

    for (auto& iter : m_ShowAnimModelList)
        Safe_Release(iter);

    for (auto& iter : m_AddAnimModelList)
        Safe_Release(iter);

    __super::Free();
}
