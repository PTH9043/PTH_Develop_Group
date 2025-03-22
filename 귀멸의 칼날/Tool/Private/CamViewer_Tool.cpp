#include "CamViewer_Tool.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Camera_Cinematic_Tool.h"
#include "Camera_Free_Tool.h"

CCamViewer_Tool::CCamViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CCamViewer_Tool::NativeConstruct()
{
    m_pName = "CamViewer";

    // Main
    m_tMainView = DOCKDESC("CamView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
        , ImVec2(1.f, 1.f), ImVec2(1000.f, 1000.f), true);
    // Select View
    m_tSelectView = DOCKDESC("CamSelectView", ImGuiWindowFlags_NoFocusOnAppearing);
    // Cam View
    m_tCamView = DOCKDESC("CamShowView", ImGuiWindowFlags_NoFocusOnAppearing);

    // Bascic Value
    CVIBuffer_Terrain::TERRAINDESC tDesc;
    tDesc.iNumVerticiesX = 100;
    tDesc.iNumVerticiesZ = 100;
    m_pTerrainData = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, tDesc);
    return S_OK;
}

HRESULT CCamViewer_Tool::NativeConstruct_InnerValue()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        // Cam List
        list<CGameObject*> CamList = pGameInstance->Find_GameObjectList(LEVEL_TOOL, LAYER_CAM);
        m_AssignCamList.reserve(CamList.size());
        for (auto& iter : CamList)
        {
            m_AssignCamList.push_back(static_cast<CCamera*>(iter));
            iter->Set_Active(false);
        }
        m_AssignCamList.front()->Set_Active(true);

        m_pShaderCom = (CShader*)pGameInstance->Clone_Component(LEVEL_STATIC,
            PROTO_COMP_SHADER_VTXNORTEX);

        m_pTransformComp = CTransform::Create(m_pDevice, m_pContext);
        m_pTransformComp->Set_PosF3(_float3(-50.f, 0.f, -50.f));
        m_pTransformComp->Set_Scale(_float3(3.f, 3.f, 3.f));
        CCamera::CAMDESC CameraDesc;
        ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

        CameraDesc.v3Eye = _float3(0.f, 0.f, 0.0f);
        CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
        CameraDesc.fSpeed = 3.f;
        CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
        CameraDesc.fFovy = XMConvertToRadians(60.0f);
        CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
        CameraDesc.fNear = 0.2f;
        CameraDesc.fFar = 5000;

        m_pCinematicCamera = (CCamera_Cinematic_Tool*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
            LAYER_CAM, PROTO_GOB_CINEMATIC_CAMREA_TOOL, &CameraDesc);
        Safe_AddRef(m_pCinematicCamera);
    }
    RELEASE_INSTANCE(CGameInstance);
    return S_OK;
}

HRESULT CCamViewer_Tool::Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Tick(_dTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Late_Tick(_dTimeDelta)))
        return E_FAIL;


    Show_Window();
    return S_OK;
}

HRESULT CCamViewer_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    ModeTo_Render();

    return S_OK;
}

HRESULT CCamViewer_Tool::Release_InnerValue()
{
    m_AssignCamList.clear();

    Safe_Release(m_pTerrainData);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTransformComp);
    Safe_Release(m_pCinematicCamera);
    return S_OK;
}

void CCamViewer_Tool::Run_Thread()
{
}

void CCamViewer_Tool::Show_Window()
{
    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + 10.f, 5.f + imVec2.y + 20.f), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(500.f, 500.f), ImGuiCond_Appearing);
    ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
    {
        Select_Menu();
        {
            m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
            if (false == m_bInitSetting)
            {
                ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
                ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

                ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                ImGuiID dock_id_Select= ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.3f, NULL, &dock_main_id);
                ImGuiID dock_id_Cam = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.7f, NULL, &dock_main_id);

                m_tSelectView.iID = dock_id_Select;
                m_tCamView.iID = dock_id_Cam;
                // Perspective , Heirecky
                ImGui::DockBuilderDockWindow(m_tSelectView.strName.c_str(), dock_id_Select);
                ImGui::DockBuilderDockWindow(m_tCamView.strName.c_str(), dock_id_Cam);
                ImGui::DockBuilderFinish(m_tMainView.iID);

                m_bInitSetting = true;
            }
            ImGui::DockSpace(m_tMainView.iID);
        }
        if (0 == m_iMode)
        {
            // Path View
            {
                // Path View
                ImGui::Begin(m_tSelectView.strName.c_str(), &m_tSelectView.bEnable, m_tSelectView.imGuiWindowFlags);
                {
                    Select_View();
                }
                ImGui::End();
            }
            // Folder View
            {
                // Folder View
                ImGui::Begin(m_tCamView.strName.c_str(), &m_tCamView.bEnable, m_tCamView.imGuiWindowFlags);
                {
                    Cam_View();
                }
                ImGui::End();
            }
        }
        else if (1 == m_iMode)
        {
            // Path View
            {
                // Path View
                ImGui::Begin(m_tSelectView.strName.c_str(), &m_tSelectView.bEnable, m_tSelectView.imGuiWindowFlags);
                {
                    Move_TerrainPosition();
                }
                ImGui::End();
            }
            // Folder View
            {
                // Folder View
                ImGui::Begin(m_tCamView.strName.c_str(), &m_tCamView.bEnable, m_tCamView.imGuiWindowFlags);
                {
                    Show_TerrainData();
                }
                ImGui::End();
            }
        }
    }
    ImGui::End();
}

void CCamViewer_Tool::Select_View()
{
    _uint iIndex = 0;
    for (auto& iter : m_AssignCamList)
    {
        _string str = CMethod::Combine_Int(iIndex++, iter->Get_CamName());
        if (ImGui::Selectable(str.c_str()))
        {
            m_pSelectCamera = iter;
            m_strSelectCamName = str;
            m_bSelectCamera = true;
            break;
        }
    }
}

void CCamViewer_Tool::Cam_View()
{
    if (nullptr == m_pSelectCamera)
        return;

    if (true == m_bSelectCamera)
    {
        if (nullptr != m_pSelectCamera)
        {
            for (auto& iter : m_AssignCamList)
            {
                iter->Set_Active(false);
            }
            m_pSelectCamera->Set_Active(true);
        }
        m_bSelectCamera = false;
    }
    else
    {
        CCamera::CAMDESC* pDesc = nullptr;
        m_pSelectCamera->Out_CameraDesc(pDesc);

        if (nullptr == pDesc)
            return;

        ImGui::TextWrapped(m_strSelectCamName.c_str());
        ImGui::DragFloat("Cam Speed", &pDesc->fSpeed, 0.f, 1000.f);
        _float fRot = XMConvertToDegrees(pDesc->fRotSpeed);
        ImGui::DragFloat("Cam RotSpeed", &fRot, 0.f, 1000.f);
        pDesc->fRotSpeed = XMConvertToRadians(fRot);
    }
}

void CCamViewer_Tool::Move_TerrainPosition()
{
    {
        _float3 v3Position = m_vTerrainPositionControl;
        _float fArrValue[3] = { v3Position.x, v3Position.y, v3Position.z };
        ImGui::DragFloat3("TerrainPositionControl", fArrValue);
        m_vTerrainPositionControl = _float3(fArrValue[0], fArrValue[1], fArrValue[2]);
        m_pTransformComp->Set_PosF3(_float3(fArrValue[0] - 50.f * m_pTransformComp->Get_Scale().x, fArrValue[1],
            fArrValue[2] - 50.f * m_pTransformComp->Get_Scale().z));
    }
    if (ImGui::Button("Add_Position"))
    {
        if (nullptr != m_pCinematicCamera)
        {
            if (nullptr != m_pSelectCamera)
            {
                CTransform* pTransform = m_pSelectCamera->Get_Transform();
 
                _vector vPos, vRot, vScale;
                XMMatrixDecompose(&vScale, &vRot, &vPos, pTransform->Get_WorldMatrix());
                _float4 Rotation;
                Rotation.Set_Vector(vRot);

                CINEMATIC_CAMDATA tData = CINEMATIC_CAMDATA(pTransform->Get_Pos(), Rotation);
                m_pCinematicCamera->Insert_Data(tData);
            }
        }
    }

    if (ImGui::Button("Start Camera Move"))
    {
        for (auto& iter : m_AssignCamList)
        {
            iter->Set_Active(false);
        }

        m_pCinematicCamera->Play_CameraMove();
    }

    if (ImGui::Button("Stop Camera Move"))
    {
        m_pCinematicCamera->Stop_CameraMove();
    }

    ImGui::NewLine();
    
    {
        _char value[512] = "";
        strcpy_s(value, m_strSaveName.c_str());
        ImGui::InputText("Save Cin File", value, sizeof(_char) * 512);
        m_strSaveName = value;
        ImGui::SameLine();
        if (ImGui::Button("Save File"))
        {
            if (nullptr == m_pCinematicCamera)
                return;

            m_pCinematicCamera->Save_Data(L"Camera_Cinematic", CMethod::ConvertSToW(m_strSaveName));
        }
    }

    {
        _char value[512] = "";
        strcpy_s(value, m_strLoadName.c_str());
        ImGui::InputText("Load Cin File", value, sizeof(_char) * 512);
        m_strLoadName = value;
        ImGui::SameLine();
        if (ImGui::Button("Load File"))
        {
            if (nullptr == m_pCinematicCamera)
                return;

            m_pCinematicCamera->Load_Data(L"Camera_Cinematic", CMethod::ConvertSToW(m_strLoadName));
        }
    }
}

void CCamViewer_Tool::Show_TerrainData()
{
    if (nullptr == m_pCinematicCamera)
        return;

    if (nullptr == m_pCinematicCamera->Get_CinematicData())
        return;

    _string str = "CInemaic Data Index : ";
    _uint iIndex = 0;
    vector<CINEMATIC_CAMDATA>* pData = m_pCinematicCamera->Get_CinematicData();
    _uint i = 0;
    CINEMATIC_CAMDATA* it = nullptr;
    for (auto& iter : *pData)
    {
        _string Value = CMethod::Combine_Int(iIndex++, str);
        if (ImGui::TreeNode(Value.c_str()))
        {
            {
                _float3 vValue = iter.vCamPos;
                _float fArrValue[3] = { vValue.x, vValue.y, vValue.z };
                ImGui::DragFloat3("Cam Position", fArrValue);
                iter.vCamPos = _float3(fArrValue[0], fArrValue[1], fArrValue[2]);
            }
            {
                _float4 vValue = iter.vRotValue;
                _float fArrValue[4] = { vValue.x, vValue.y, vValue.z, vValue.w };
                ImGui::DragFloat4("Cam Rotation", fArrValue);
                iter.vRotValue = _float4(fArrValue[0], fArrValue[1], fArrValue[2], fArrValue[3]);
            }
            {
                _float Value = iter.fSpeed;
                ImGui::DragFloat("Cam Speed", &Value);
                iter.fSpeed = Value;
            }

            if (ImGui::Button("Remove"))
            {
                it = &iter;
                ImGui::TreePop();
                break;
            }

            ImGui::TreePop();
        }
        ++i;
    }

    if (nullptr != it)
    {
        for (vector<CINEMATIC_CAMDATA>::iterator value = m_pCinematicCamera->Get_CinematicData()->begin(); 
            value != m_pCinematicCamera->Get_CinematicData()->end();)
        {
            if (&(*value) == it)
            {
                m_pCinematicCamera->Get_CinematicData()->erase(value);
                break;
            }
            else
                ++value;
        }
    }
}

void CCamViewer_Tool::Show_BoundingBox(const _float3& _v3Size, const _float3& _v3Pos, CGameInstance* _pGameInstance)
{
#ifdef _DEBUG
    BoundingBox bBox1 = BoundingBox(_v3Pos, _v3Size);
    _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D);
#endif // _DEBUG
}

void CCamViewer_Tool::Select_Menu()
{
    ImGui::BeginMenuBar();
    {
        if (ImGui::BeginMenu("Select"))
        {
            if (ImGui::MenuItem("Camera Select"))
            {
                m_iMode = 0;
            }
            else if (ImGui::MenuItem("Cinematic Cam"))
            {
                m_iMode = 1;
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMenuBar();
}

void CCamViewer_Tool::ModeTo_Render()
{
    if (1 == m_iMode)
    {
        if (nullptr == m_pShaderCom)
            return;

        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
        {
            _float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
            WorldMatrix = m_pTransformComp->Get_WorldFloat4x4_TP();
            ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
            ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

            if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
            {
                RELEASE_INSTANCE(CGameInstance);
                return;
            }
            if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
            {
                RELEASE_INSTANCE(CGameInstance);
                return;
            }
            if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
            {
                RELEASE_INSTANCE(CGameInstance);
                return;
            }

            m_pShaderCom->Begin(1);
            m_pTerrainData->Render();

            if (nullptr != m_pCinematicCamera)
            {
                for (auto& iter : *m_pCinematicCamera->Get_CinematicData())
                {
                    Show_BoundingBox(_float3(1.f, 1.f, 1.f), iter.vCamPos, pGameInstance);
                }
            }
        }
        RELEASE_INSTANCE(CGameInstance);
    }
}

CCamViewer_Tool* CCamViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamViewer_Tool* pInstance = new CCamViewer_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamViewer_Tool::Create To failed");
#endif
    }
    return pInstance;
}

void CCamViewer_Tool::Free()
{
    Safe_Release(m_pTerrainData);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTransformComp);
    Safe_Release(m_pCinematicCamera);
    __super::Free();
}
