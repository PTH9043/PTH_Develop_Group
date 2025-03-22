#include "CutSceneViewer_Tool.h"
#include "GameInstance.h"
#include "CutSceneModel_Tool.h"
#include "Camera.h"
#include "Camera_Cinematic_Tool.h"
#include "Camera_Free_Tool.h"
#include "Camera_Tool.h"
#include "Item_Tool.h"

CCutSceneViewer_Tool::CCutSceneViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CCutSceneViewer_Tool::NativeConstruct()
{
    m_pName = "CutSceneViewer";

    // Main
    m_tMainView = DOCKDESC("CutSceneView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
        , ImVec2(1.f, 1.f), ImVec2(1000.f, 1000.f), true);
    // Select View
    m_tModifyViewer = DOCKDESC("CutScene_Info", ImGuiWindowFlags_NoFocusOnAppearing);
    // Cam View
    m_tCutSceneViewer = DOCKDESC("CutScene_Modify", ImGuiWindowFlags_NoFocusOnAppearing);
    // Anim Clip Viewer
    m_tAnimClipViewer = DOCKDESC("CutScene_AnimClip", ImGuiWindowFlags_NoFocusOnAppearing);
    // Target Player Viewer
    m_tModifyTargetPlayerViewer = DOCKDESC("Target_PlayerViewer", ImGuiWindowFlags_NoFocusOnAppearing);
    // Save Scene Viewer 
    m_tSaveSceneViewer = DOCKDESC("CutScene_Save", ImGuiWindowFlags_NoFocusOnAppearing);
    XMStoreFloat4x4(&m_Texture_WorldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_Texture_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_Texture_ProjMatrix, XMMatrixIdentity());

    XMStoreFloat4x4(&m_Texture_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

    m_Texture_WorldMatrix._11 = g_iWinSizeX;
    m_Texture_WorldMatrix._22 = g_iWinSizeY;
    XMStoreFloat4x4(&m_Texture_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_Texture_WorldMatrix)));
    return S_OK;
}

HRESULT CCutSceneViewer_Tool::NativeConstruct_InnerValue()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        PATHS pPath = { L"AnimationModel", L"PlayerCharacter" };
        FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&pPath);
        if (nullptr != pFileGroup)
        {
            for (auto& iter : pFileGroup->lsUnderList)
            {
                if (nullptr == iter)
                    continue;
                FILEGROUP* pFBX = iter->Find_Group(L"FBX_Converter");
                if (nullptr != pFBX)
                {
                    if (pFBX->vecFileDataList.size() > 0)
                    {
                        CCutSceneModel_Tool* pInstance = (CCutSceneModel_Tool*)(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL,
                            LAYER_DEBUG, PROTO_GOB_CUTSCENEMODEL_TOOL));
                        if (nullptr == pInstance)
                            continue;
                        Safe_AddRef(pInstance);

                        HRESULT RESULT = pInstance->Load_CutSceneModel(pFBX, &pFBX->vecFileDataList.front());
                        if (FAILED(RESULT))
                        {
                            Safe_Release(pInstance);
                            pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_DEBUG, pInstance);
                        }
                        else
                        {
                            m_CutSceneModelList.push_back(pInstance);
                        }
                    }
                }
            }
        }

        // Cam Free 
        {
            list<CGameObject*> CamList = pGameInstance->Find_GameObjectList(LEVEL_TOOL, LAYER_CAM);
            for (auto& iter : CamList)
            {
                CCamera* pCam = static_cast<CCamera*>(iter);
                if (nullptr != pCam)
                {
                    // Find Camera Tool 
                    if ("CCamera_Tool" == pCam->Get_CamName())
                    {
                        m_pCamera_Tool = static_cast<CCamera_Tool*>(pCam);
                        Safe_AddRef(pCam);
                    }
                    // Find Camera Free Tool
                    else if ("CCamera_Free_Tool" == pCam->Get_CamName())
                    {
                        m_pCamera_Free_Tool = static_cast<CCamera_Free_Tool*>(pCam);
                        Safe_AddRef(m_pCamera_Free_Tool);
                    }
                }
            }
        }
        {
            PATHS Path = { L"CutScene", L"TexGroup" };
            FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
            if (nullptr != pFileGroup)
            {
                for (auto& iter : pFileGroup->lsUnderList)
                {
                    m_TextureVector.push_back(CTexture::Create(m_pDevice, m_pContext, iter));
                }
            }
        }
        // Picking To Terrain Value
        {
            CVIBuffer_Terrain::TERRAINDESC tDesc;
            tDesc.iNumVerticiesX = 100;
            tDesc.iNumVerticiesZ = 100;
            m_pVIBuffer_TerrainData = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, tDesc);

            // Clone Terrain Data
            m_pTerrain_ShaderCom = (CShader*)pGameInstance->Clone_Component(LEVEL_STATIC,
                PROTO_COMP_SHADER_VTXNORTEX);

            m_pTerrain_TransformComp = CTransform::Create(m_pDevice, m_pContext);
            m_pTerrain_TransformComp->Set_PosF3(_float3(-250.f, 0.f, -250.f));
            m_pTerrain_TransformComp->Set_Scale(_float3(5.f, 5.f, 5.f));
        }
        // Cut Scene Texture
        {
            // VIBuffer 
            m_VIBuffer_CutSceneTexture = (CVIBuffer_Rect*)pGameInstance->Clone_Component(LEVEL_STATIC,
                PROTO_COMP_VIBUFFER_RECT);

            m_pCutScene_Shader = (CShader*)pGameInstance->Clone_Component(LEVEL_STATIC,
                PROTO_COMP_SHADER_VTXTEX);
        }

        PATHS Path = { L"Data", L"Effect" };
        m_pEffectFileGroup = pGameInstance->Find_Folder(&Path);

        // Get _Item
        {
            Path = { L"Model", L"Item" };
            FILEGROUP* ItemFileGroup = pGameInstance->Find_Folder(&Path);
            for (auto& iter : ItemFileGroup->lsUnderList)
            {
                // 바이너리 폴더를 받아온다.
                FILEGROUP* pBinaryFolder = iter->Find_Group(L"FBX_Converter");
                // 바이너리 폴더가 없다면 continue;
                if (nullptr == pBinaryFolder)
                    continue;

                for (auto& Data : pBinaryFolder->vecFileDataList)
                {
                    CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pBinaryFolder, &Data,
                        XMMatrixScaling(0.1f, 0.1f, 0.1f));
                    m_ItemModelList.push_back(pModel);
                }
            }
        }
        m_pOwnerSword = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
        Safe_AddRef(m_pOwnerSword);
        m_pTargetSword = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
        Safe_AddRef(m_pTargetSword);
    }
    RELEASE_INSTANCE(CGameInstance);
    return S_OK;
}

HRESULT CCutSceneViewer_Tool::Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Tick(_dTimeDelta)))
        return E_FAIL;

    Picking_Terrain();
    _float fOwnerAnimSpeed = 1.f;
    if (m_iCutChar_Index < m_CutCharDataVector.size())
    {
        if (0.f == fOwnerAnimSpeed)
            fOwnerAnimSpeed = 1.f;

        fOwnerAnimSpeed = m_CutCharDataVector[m_iCutChar_Index].fControlTotalUpdateSpeed;
    }
    
    if (true == m_bPlayCutChar)
    {
        if (nullptr != m_pCutScene_OwnerModel)
            m_pCutScene_OwnerModel->Cut_Tick(_dTimeDelta * m_fMultipleValue * m_fUpdateTime * fOwnerAnimSpeed);

        if (nullptr != m_pCutScene_TargetModel)
            m_pCutScene_TargetModel->Cut_Tick(_dTimeDelta * m_fMultipleValue * m_fUpdateTime * fOwnerAnimSpeed);


        if (false == m_bControl_PlayAnimation)
        {
            if (nullptr != m_pCutScene_OwnerModel)
                m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->Update_AnimClip(m_fCurrentCharTime);
            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Update_AnimClip(m_fCurrentCharTime);
        }
        else
        {
            if (nullptr != m_pCutScene_OwnerModel)
            {
                m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
                if (true == m_bRootBone_Owner)
                {
                    m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->RootBoneTransform();
                }
            }
            if (nullptr != m_pCutScene_TargetModel)
            {
                m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
                if (true == m_bRootBone_Target)
                {
                    m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->RootBoneTransform();
                }
            }
            m_bControl_PlayAnimation = false;
        }
    }
    else
    {
        if (true == m_bControl_PlayAnimation)
        {
            if (nullptr != m_pCutScene_OwnerModel)
                m_pCutScene_OwnerModel->Cut_Tick(_dTimeDelta * fOwnerAnimSpeed);

            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Cut_Tick(_dTimeDelta * fOwnerAnimSpeed);

            if (nullptr != m_pCutScene_OwnerModel)
            {
                m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
                if (true == m_bRootBone_Owner)
                {
                    m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->RootBoneTransform();
                }
            }
            if (nullptr != m_pCutScene_TargetModel)
            {
                m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
                if (true == m_bRootBone_Target)
                {
                    m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->RootBoneTransform();
                }
            }
            m_bControl_PlayAnimation = false;
        }
    }

    Update_CutScene(_dTimeDelta * m_fUpdateTime * fOwnerAnimSpeed);
    return S_OK;
}

HRESULT CCutSceneViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
    if (FAILED(__super::Late_Tick(_dTimeDelta)))
        return E_FAIL;

    _float fOwnerAnimSpeed = 1.f;
    if (m_iCutChar_Index < m_CutCharDataVector.size())
    {
        if (0.f == fOwnerAnimSpeed)
            fOwnerAnimSpeed = 1.f;

        fOwnerAnimSpeed = m_CutCharDataVector[m_iCutChar_Index].fControlTotalUpdateSpeed;
    }

    if (nullptr != m_pCutScene_OwnerModel)
        m_pCutScene_OwnerModel->Cut_Late_Tick(_dTimeDelta * fOwnerAnimSpeed);

    if (nullptr != m_pCutScene_TargetModel)
        m_pCutScene_TargetModel->Cut_Late_Tick(_dTimeDelta * fOwnerAnimSpeed);


    LateUpdate_CutScene(_dTimeDelta * m_fUpdateLateTime * fOwnerAnimSpeed);
    Show_Window();
    return S_OK;
}

HRESULT CCutSceneViewer_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    Render_Terrain();
    Render_To_All_Position();
    Render_Texture();
    return S_OK;
}

HRESULT CCutSceneViewer_Tool::Release_InnerValue()
{
    Safe_Release(m_pCamera_Tool);
    return S_OK;
}

void CCutSceneViewer_Tool::Show_Window()
{
    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + 10.f, 5.f + imVec2.y + 20.f), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(750.f, 750.f), ImGuiCond_Appearing);
    ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
    {
        {
            m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
            if (false == m_bInitSetting)
            {
                ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
                ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

                ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                ImGuiID dock_id_Select = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);
                ImGuiID dock_id_Cam = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, &dock_id_Cam, &dock_main_id);
                ImGuiID dock_id_Save = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, &dock_id_Cam, &dock_main_id);
                ImGuiID dock_id_AnimViewer = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, &dock_id_AnimViewer, &dock_main_id);
                ImGuiID dock_id_ModifyViewer = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, &dock_id_AnimViewer, &dock_main_id);

                m_tModifyViewer.iID = dock_id_Select;
                m_tCutSceneViewer.iID = dock_id_Cam;
                m_tAnimClipViewer.iID = dock_id_AnimViewer;
                m_tModifyTargetPlayerViewer.iID = dock_id_ModifyViewer;
                m_tSaveSceneViewer.iID = dock_id_Save;
                // Perspective , Heirecky
                ImGui::DockBuilderDockWindow(m_tModifyViewer.strName.c_str(), dock_id_Select);
                ImGui::DockBuilderDockWindow(m_tCutSceneViewer.strName.c_str(), dock_id_Cam);
                ImGui::DockBuilderDockWindow(m_tAnimClipViewer.strName.c_str(), dock_id_AnimViewer);
                ImGui::DockBuilderDockWindow(m_tModifyTargetPlayerViewer.strName.c_str(), dock_id_ModifyViewer);
                ImGui::DockBuilderDockWindow(m_tSaveSceneViewer.strName.c_str(), dock_id_Save);
                ImGui::DockBuilderFinish(m_tMainView.iID);

                m_bInitSetting = true;
            }
            ImGui::DockSpace(m_tMainView.iID);
        }
        // m_tModifyViewer View
        {
            // m_tModifyViewer View
            ImGui::Begin(m_tModifyViewer.strName.c_str(), &m_tModifyViewer.bEnable, m_tModifyViewer.imGuiWindowFlags);
            {
                First_Modify_CutSceneViewer_Tool();
                Show_Select_Model();
                Modify_Total_Value();
                Control_CutScene();
            }
            ImGui::End();
        }
        // Anim Viewer 
        {
            // m_tCutSceneViewer 
            ImGui::Begin(m_tAnimClipViewer.strName.c_str(), &m_tAnimClipViewer.bEnable, m_tAnimClipViewer.imGuiWindowFlags);
            {
                Show_CutSceneAnimationViewer();
            }
            ImGui::End();
        }
        // Target
        {
            ImGui::Begin(m_tModifyTargetPlayerViewer.strName.c_str(), &m_tModifyTargetPlayerViewer.bEnable, m_tModifyTargetPlayerViewer.imGuiWindowFlags);
            {
                Show_CutCharData();
            }
            ImGui::End();
        }
        // m_tCutSceneViewer 
        {
            // m_tCutSceneViewer 
            ImGui::Begin(m_tCutSceneViewer.strName.c_str(), &m_tCutSceneViewer.bEnable, m_tCutSceneViewer.imGuiWindowFlags);
            {
                Show_And_CutSceneViewer();
            }
            ImGui::End();
        }
        // Save Load
        {
            ImGui::Begin(m_tSaveSceneViewer.strName.c_str(), &m_tSaveSceneViewer.bEnable, m_tCutSceneViewer.imGuiWindowFlags);
            {
                SaveLoad_ControlMethod();
            }
            ImGui::End();
        }
    }
    ImGui::End();
}
// First Modify CutSceneViewer Tool 
void CCutSceneViewer_Tool::First_Modify_CutSceneViewer_Tool()
{
    if (nullptr != m_pTerrain_TransformComp)
    {
        CTransform* pTransform = m_pTerrain_TransformComp;
        _float3 vPos = pTransform->Get_PosF3();
        ImGui::DragFloat3("Terrain Pos", &vPos.x, 0.05f, -10000.f, 10000.f);
        pTransform->Set_Pos(vPos.Get_Vector());
    }
    ImGui::Checkbox("Render Spline Curve", &m_bRender_Spline); ImGui::SameLine();
    ImGui::Checkbox("Render Bezier Curve", &m_bRender_Bezier);
    _bool bIsInput = ImGui::Checkbox("Change_Camera", &m_bChangeCamera);  ImGui::SameLine();
    if (true == bIsInput)
    {
        if (true == m_bChangeCamera)
        {
            if (nullptr != m_pCamera_Free_Tool)
                m_pCamera_Free_Tool->Set_Active(true);

            if (nullptr != m_pCamera_Tool)
                m_pCamera_Tool->Set_Active(false);
        }
        else
        {
            if (nullptr != m_pCamera_Free_Tool)
                m_pCamera_Free_Tool->Set_Active(false);

            if (nullptr != m_pCamera_Tool)
                m_pCamera_Tool->Set_Active(true);
        }
    }
    bIsInput = ImGui::Checkbox("ModelActive", &m_bModelActive);
    if (true == bIsInput)
    {
        if (true == m_bModelActive)
        {
            if (nullptr != m_pCutScene_OwnerModel)
                m_pCutScene_OwnerModel->Set_Active(true);

            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Set_Active(true);
        }
        else
        {
            if (nullptr != m_pCutScene_OwnerModel)
                m_pCutScene_OwnerModel->Set_Active(false);

            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Set_Active(false);
        }
    }
    if (nullptr != m_pCamera_Tool)
    {
        CTransform* pTransform = m_pCamera_Tool->Get_Transform();
        _float3 vPos = pTransform->Get_PosF3();
        ImGui::DragFloat3("Camera Tool Pos", &vPos.x, 0.05f, -10000.f, 10000.f);
        pTransform->Set_Pos(vPos.Get_Vector());

        _float3 vRot;
        OUTMATRIX OutMatrix = CMethod::Out_Matrix(pTransform->Get_WorldFloat4x4());
        vRot = OutMatrix.vRot;
        _bool bIsTrue = ImGui::DragFloat3("Camera Tool Rot", &vRot.x, 0.05f, -10000.f, 10000.f);
        if (true == bIsTrue)
            pTransform->Rotate_Turn(vRot);
        if (nullptr != m_pCamera_Tool)
        {
            if (true == m_pCamera_Tool->Is_Active())
            {
                CCamera::CAMDESC* pDesc = nullptr;
                m_pCamera_Tool->Out_CameraDesc(pDesc);
                if (nullptr != pDesc)
                {
                    ImGui::TextWrapped(m_pCamera_Tool->Get_CamName().c_str());
                    ImGui::DragFloat("CamTool Speed", &pDesc->fSpeed, 0.05f, -10000.f, 10000.f);
                    _float fRot = XMConvertToDegrees(pDesc->fRotSpeed);
                    ImGui::DragFloat("CamTool RotSpeed", &fRot, 0.05f, -10000.f, 10000.f);
                    pDesc->fRotSpeed = XMConvertToRadians(fRot);
                }

                if (m_CutSceneDataVector.size() > 0)
                {
                    ImGui::DragInt("BindCameraPosRot_CutData", &m_iBindCutScene_CameraPosAndRotation_Index);
                    if (ImGui::Button("BindCameraButton"))
                    {
                        if (m_iBindCutScene_CameraPosAndRotation_Index < m_CutSceneDataVector.size())
                        {
                            CUTSCENEDATA tData = m_CutSceneDataVector[m_iBindCutScene_CameraPosAndRotation_Index];
                            CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
                            if (nullptr != pCamTransform)
                            {
                                pCamTransform->Set_Pos(tData.vCamPosition.Get_Vector());
                                pCamTransform->Rotate_Turn(tData.vCamRotation);
                            }
                        }
                        else
                        {
                            m_iBindCutScene_CameraPosAndRotation_Index = (_int)(m_CutSceneDataVector.size());
                        }
                    }
                }
            }
        }
        if (nullptr != m_pCamera_Free_Tool)
        {
            if (true == m_pCamera_Free_Tool->Is_Active())
            {
                CCamera::CAMDESC* pDesc = nullptr;
                m_pCamera_Free_Tool->Out_CameraDesc(pDesc);
                if (nullptr != pDesc)
                {
                    ImGui::TextWrapped(m_pCamera_Free_Tool->Get_CamName().c_str());
                    ImGui::DragFloat("CamFTool Speed", &pDesc->fSpeed, 0.f, 1000.f);
                    _float fRot = XMConvertToDegrees(pDesc->fRotSpeed);
                    ImGui::DragFloat("CamFTool RotSpeed", &fRot, 0.f, 1000.f);
                    pDesc->fRotSpeed = XMConvertToRadians(fRot);
                }
            }
        }
    }
}
// Show Select Model
void CCutSceneViewer_Tool::Show_Select_Model()
{
    // Cut 씬에 들어갈 모델들을 선택, Owner가 컷씬의 주인공, CutSceneViewer Target은 임의의 모델이다.
    if (ImGui::TreeNode("Show_Select_Model"))
    {
        if (ImGui::TreeNodeEx("CutSceneViewer_Owner", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginListBox("OwnerModelList", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
            {
                // Cut Scene Model List
                for (auto& iter : m_CutSceneModelList)
                {
                    FILEDATA* pFileData = iter->Get_FileData();
                    if (ImGui::Selectable(CMethod::ConvertWToS(pFileData->strFileName).c_str()))
                    {
                        // Active
                        if (nullptr != m_pCutScene_OwnerModel)
                            m_pCutScene_OwnerModel->Set_Active(false);
                        m_pCutScene_OwnerModel = iter;
                        m_pCutScene_OwnerModel->Set_Active(true);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("CusSceneViewer_Target", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginListBox("TargetModelList", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
            {
                for (auto& iter : m_CutSceneModelList)
                {
                    FILEDATA* pFileData = iter->Get_FileData();
                    if (ImGui::Selectable(CMethod::ConvertWToS(pFileData->strFileName).c_str()))
                    {
                        if (nullptr != m_pCutScene_TargetModel)
                            m_pCutScene_TargetModel->Set_Active(false);
                        m_pCutScene_TargetModel = iter;
                        m_pCutScene_TargetModel->Set_Active(true);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}
// Modify Total Value
void CCutSceneViewer_Tool::Modify_Total_Value()
{
    if (ImGui::TreeNode("Modify_Total_Value"))
    {
        if (nullptr != m_pCutScene_OwnerModel)
        {
            m_dTotalTime = m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->dTotalPlayTime;
        }
        {
            _bool bIsTrue = false;
            bIsTrue |= ImGui::SliderFloat("CutScene_Time", &m_fDragCamCurTime, 0.f, (_float)m_dTotalTime);
            bIsTrue |= ImGui::DragFloat("CutScene_Time Drag", &m_fDragCamCurTime, 0.05f, 0.f, (_float)m_dTotalTime);

            if (true == bIsTrue)
            {
                m_fCurrentCamTime = m_fDragCamCurTime;
            }
        }

        if (ImGui::TreeNode("Texture Control"))
        {
            ImGui::DragInt("Texture ID", (_int*)&m_iCutScene_Texture_Index);
            if (m_iCutScene_Texture_Index >= m_TextureVector.size())
                m_iCutScene_Texture_Index = (_uint)(m_TextureVector.size() - 1);

            if (0 > m_iCutScene_Texture_Index)
                m_iCutScene_Texture_Index = 0;

            ImGui::TreePop();
        }

        ImGui::Checkbox("Make Effect", &m_bMakeEffect);

        Add_CutSceneViewer();
        {
            _bool bIsTrue = false;
            bIsTrue |= ImGui::SliderFloat("CutChar_Time", &m_fDragCharCurTime, 0.f, (_float)m_dTotalTime);
            bIsTrue |= ImGui::DragFloat("CutChar_Time Drag", &m_fDragCharCurTime, 0.05f, 0.f, (_float)m_dTotalTime);

            if (true == bIsTrue)
            {
                m_fCurrentCharTime = m_fDragCharCurTime;
                m_bControl_PlayAnimation = true;
            }
        }

        if (ImGui::TreeNode("Modify_Info_To_Select_Model"))
        {
            if (nullptr != m_pCutScene_OwnerModel)
            {
                _string str = CMethod::ConvertWToS(m_pCutScene_OwnerModel->Get_FileData()->strFileName);
                str.append("##001");
                if (ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    m_pCutScene_OwnerModel->Extern_HierakeyView(str);
                    ImGui::TreePop();
                }
            }

            if (nullptr != m_pCutScene_TargetModel)
            {
                _string str = CMethod::ConvertWToS(m_pCutScene_TargetModel->Get_FileData()->strFileName);
                str.append("##002");
                if (ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    m_pCutScene_TargetModel->Extern_HierakeyView(str);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Modify Picking Model Position"))
        {
            ImGui::DragFloat3("Owner_FramePosition", &m_v3OwnerPosition.x, 0.05f, -1000000.f, 1000000.f);
            ImGui::DragFloat3("Target_FramePosition", &m_v3TargetPosition.x, 0.05f, -1000000.f, 1000000.f);

            ImGui::Checkbox("Picking Position", &m_bIsPicking);
            if (true == m_bIsPicking)
            {
                const char* pArr[] = { "Owner" , "Target" };
                ImGui::Combo(u8"모델 피킹 선택", (_int*)&m_iSelectTarget, pArr, 2);
            }

            if (ImGui::Button("Bind_Current_Position"))
            {
                if (0 == m_iSelectTarget)
                {
                    m_pCutScene_OwnerModel->Get_Transform()->Set_Pos(m_v3OwnerPosition.Get_Vector());
                }
                else
                {
                    m_pCutScene_TargetModel->Get_Transform()->Set_Pos(m_v3TargetPosition.Get_Vector());
                }
            }
            ImGui::TreePop();
        }
        Add_CutCharInfo();
        ImGui::TreePop();
    }
}
// Control Cut Scene 
void CCutSceneViewer_Tool::Control_CutScene()
{
    if (ImGui::TreeNode("Control_CutScene"))
    {
        if (ImGui::Button("Play"))
        {
            if (m_CutSceneDataVector.size() <= 0)
            {
                ImGui::TreePop();
                return;
            }
            if (m_CutCharDataVector.size() <= 0)
            {
                ImGui::TreePop();
                return;
            }
            Reset_CamData();
            Reset_CharData();

            m_pCamera_Tool->Set_CutSceneMode(true);

            if (nullptr != m_pCutScene_OwnerModel)
                m_pCutScene_OwnerModel->Set_PlayCutScene(true);

            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Set_PlayCutScene(true);

            m_bPlayCutScene = true;
            m_bPlayCutChar = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_bPlayCutScene = true;
            m_bPlayCutChar = true;
            if (nullptr != m_pCamera_Tool)
                m_pCamera_Tool->Set_CutSceneMode(true);
        }

        if (ImGui::Button("Stop"))
        {
            m_bPlayCutScene = false;
            m_bPlayCutChar = false;
            if (nullptr != m_pCamera_Tool)
                m_pCamera_Tool->Set_CutSceneMode(false);
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            Reset_CamData();
            Reset_CharData();
            m_bPlayCutScene = false;
            m_bPlayCutChar = false;
        }
        {
            _bool bControl = ImGui::SliderFloat("Play Char Time", &m_fCurrentCharTime, 0.f, (_float)m_dTotalTime, "%.3f");
            if (true == bControl)
            {
                m_bPlayCutChar = false;
                m_bControl_PlayAnimation = true;
            }
        }
        {
            _bool bControl = ImGui::SliderFloat("Play CamTime", &m_fCurrentCamTime, 0.f, (_float)m_dTotalTime, "%.3f");
            if (true == bControl)
            {
                m_bPlayCutScene = false;
            }
        }
        ImGui::DragFloat("Multiple Time", &m_fMultipleValue, 0.f, 10.f);
        ImGui::TreePop();
    }
}
// Add Cut Scene Viewer 
void CCutSceneViewer_Tool::Add_CutSceneViewer()
{
    if (nullptr == m_pCamera_Tool)
        return;
    CTransform* pTransform = m_pCamera_Tool->Get_Transform();
    if (nullptr == pTransform)
        return;

    ImGui::Checkbox("Use Lerp", &m_bUseLerp);

    if (true == m_bUseLerp)
    {
        ImGui::Text("PREVLERP: ");
        ImGui::SameLine();
        ImGui::Text(m_strSelect_lerpPrevSceneName.c_str());

        ImGui::Text("NEXT LERP: ");
        ImGui::SameLine();
        ImGui::Text(m_strSelect_lerpNextSceneName.c_str());
    }

    Lerp_CutSceneData();

    // Add Cut Scene Viewer
    if (ImGui::Button("Add_CutSceneViewer"))
    {
        OUTMATRIX OutMatrix = CMethod::Out_Matrix(pTransform->Get_WorldMatrix());
        CUTSCENEDATA tData;
        tData.bCutSceneTexShow = m_bMakeTexture;
        tData.bCutSceneEffectGroupShow = m_bMakeEffect;
        tData.fCurTime = m_fDragCamCurTime;

        if (false == m_bUseLerp)
        {
            tData.vCamPosition = pTransform->Get_PosF3();
            tData.vCamRotation = OutMatrix.vRot;
            tData.vCamQuaternion = OutMatrix.vQuaternionRot;
        }
        else
        {
            if (nullptr != m_pSelect_lerpPrevSceneData && nullptr != m_pSelect_lerpNextSceneData)
            {
                _float3			vLerpCamPosRatio;
                _float3			vLerpCamRotRatio;

                if (true == m_bAllCamLerp)
                {
                    vLerpCamPosRatio = XMVectorLerp(m_pSelect_lerpPrevSceneData->vCamPosition.Get_Vector(), m_pSelect_lerpNextSceneData->vCamPosition.Get_Vector(),
                        m_fCamLerpAll);
                    vLerpCamRotRatio = XMVectorLerp(m_pSelect_lerpPrevSceneData->vCamRotation.Get_Vector(), m_pSelect_lerpNextSceneData->vCamRotation.Get_Vector(),
                        m_fCamLerpAll);

                    tData.fCamSpeed = (m_pSelect_lerpNextSceneData->fCamSpeed - m_pSelect_lerpPrevSceneData->fCamSpeed) * m_fCamLerpAll + m_pSelect_lerpPrevSceneData->fCamSpeed;
                    tData.fSplineCamSpeed = (m_pSelect_lerpNextSceneData->fSplineCamSpeed - m_pSelect_lerpPrevSceneData->fSplineCamSpeed) * m_fCamLerpAll + m_pSelect_lerpPrevSceneData->fSplineCamSpeed;
                    tData.fBezierSpeed = (m_pSelect_lerpNextSceneData->fBezierSpeed - m_pSelect_lerpPrevSceneData->fBezierSpeed) * m_fCamLerpAll + m_pSelect_lerpPrevSceneData->fBezierSpeed;
                }
                else
                {
                    vLerpCamPosRatio = XMVectorLerp(m_pSelect_lerpPrevSceneData->vCamPosition.Get_Vector(), m_pSelect_lerpNextSceneData->vCamPosition.Get_Vector(),
                        m_fLerpCamPosRatio);
                    vLerpCamRotRatio = XMVectorLerp(m_pSelect_lerpPrevSceneData->vCamRotation.Get_Vector(), m_pSelect_lerpNextSceneData->vCamRotation.Get_Vector(),
                        m_fLerpCamRotRatio);
                    tData.fCamSpeed = (m_pSelect_lerpNextSceneData->fCamSpeed - m_pSelect_lerpPrevSceneData->fCamSpeed) * m_fLerpCamSpeedRatio + m_pSelect_lerpPrevSceneData->fCamSpeed;
                    tData.fSplineCamSpeed = (m_pSelect_lerpNextSceneData->fSplineCamSpeed - m_pSelect_lerpPrevSceneData->fSplineCamSpeed) * m_fLerpSplineSpeedRatio + m_pSelect_lerpPrevSceneData->fSplineCamSpeed;
                    tData.fBezierSpeed = (m_pSelect_lerpNextSceneData->fBezierSpeed - m_pSelect_lerpPrevSceneData->fBezierSpeed) * m_fLerpBezierSpeedRatio + m_pSelect_lerpPrevSceneData->fBezierSpeed;
                }

                tData.vCamPosition = vLerpCamPosRatio;
                tData.vCamRotation = vLerpCamRotRatio;
                tData.vCamQuaternion = CMethod::Convert_RotationToQuaternion(tData.vCamRotation);
            }
        }

        if (m_CutSceneDataVector.size() > 0)
        {
            tData.fPrevTime = m_CutSceneDataVector.back().fCurTime;
        }
        tData.vBezierPos = tData.vCamPosition;
        m_CutSceneDataVector.push_back(tData);
        m_pSelectSceneData = nullptr;
    }
}
// Add Cut Char Data 
void CCutSceneViewer_Tool::Add_CutCharInfo()
{
    if (true == ImGui::Button("Make Texture"))
    {
        m_CutSceneTexVector.push_back(CCutSceneTex_Tool::Create(m_pDevice, m_pContext));
    }

    ImGui::Checkbox("Use Char Lerp", &m_bUseCharLerp);

    if (true == m_bUseCharLerp)
    {
        ImGui::Text("PREVLERP: ");
        ImGui::SameLine();
        ImGui::Text(m_strSelect_lerpPrevCharName.c_str());

        ImGui::Text("NEXT LERP: ");
        ImGui::SameLine();
        ImGui::Text(m_strSelect_lerpNextCharName.c_str());
    }

    Lerp_CutCharData();

    if (ImGui::Button("Add Char Scene Data"))
    {
        CUTCHARDATA tData;
        tData.fCurTime = m_fDragCharCurTime;
        m_CutCharDataVector.push_back(tData);

        if (true == m_bUseCharLerp)
        {
            if (nullptr != m_pSelect_lerpPrevCharData && nullptr != m_pSelect_lerpNextSceneData)
            {
                _float3			vLerpOwnerPosRatio;
                _float3			vLerpOwnerRotRatio;
                _float3			vLerpTargetPosRatio;
                _float3			vLerpTargetRotRatio;

                if (true == m_bAllCharLerp)
                {
                    vLerpOwnerPosRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vOwnerPosition.Get_Vector(), m_pSelect_lerpNextCharData->vOwnerPosition.Get_Vector(),
                        m_fCamLerpAll);
                    vLerpOwnerRotRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vOwnerRotation.Get_Vector(), m_pSelect_lerpNextCharData->vOwnerRotation.Get_Vector(),
                        m_fCamLerpAll);
                    vLerpTargetPosRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vTargetPosition.Get_Vector(), m_pSelect_lerpNextCharData->vTargetPosition.Get_Vector(),
                        m_fCamLerpAll);
                    vLerpTargetRotRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vTargetRotation.Get_Vector(), m_pSelect_lerpNextCharData->vTargetRotation.Get_Vector(),
                        m_fCamLerpAll);

                    tData.fOwnerSpeed = (m_pSelect_lerpNextCharData->fOwnerSpeed - m_pSelect_lerpPrevCharData->fOwnerSpeed) * m_fCamLerpAll + m_pSelect_lerpPrevCharData->fOwnerSpeed;
                    tData.fTargetSpeed = (m_pSelect_lerpNextCharData->fTargetSpeed - m_pSelect_lerpPrevCharData->fTargetSpeed) * m_fCamLerpAll + m_pSelect_lerpPrevCharData->fTargetSpeed;
                }
                else
                {
                    vLerpOwnerPosRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vOwnerPosition.Get_Vector(), m_pSelect_lerpNextCharData->vOwnerPosition.Get_Vector(),
                        m_fLerpOwnerPosRatio);
                    vLerpOwnerRotRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vOwnerRotation.Get_Vector(), m_pSelect_lerpNextCharData->vOwnerRotation.Get_Vector(),
                        m_fLerpOwnerRotRatio);
                    vLerpTargetPosRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vTargetPosition.Get_Vector(), m_pSelect_lerpNextCharData->vTargetPosition.Get_Vector(),
                        m_fLerpTargetPosRatio);
                    vLerpTargetRotRatio = XMVectorLerp(m_pSelect_lerpPrevCharData->vTargetRotation.Get_Vector(), m_pSelect_lerpNextCharData->vTargetRotation.Get_Vector(),
                        m_fLerpTargetRotRatio);

                    tData.fTargetSpeed = (m_pSelect_lerpNextCharData->fTargetSpeed - m_pSelect_lerpPrevCharData->fTargetSpeed) * m_fLerpTargetSpeedRatio + m_pSelect_lerpPrevCharData->fTargetSpeed;
                    tData.fOwnerSpeed = (m_pSelect_lerpNextCharData->fOwnerSpeed - m_pSelect_lerpPrevCharData->fOwnerSpeed) * m_fLerpOwnerSpeedRatio + m_pSelect_lerpPrevCharData->fOwnerSpeed;
                }

                if (nullptr != m_pCutScene_OwnerModel)
                {
                    tData.vOwnerPosition = vLerpOwnerPosRatio;
                    tData.vOwnerRotation = vLerpOwnerRotRatio;
                }
                if (nullptr != m_pCutScene_TargetModel)
                {
                    tData.vTargetPosition = vLerpTargetPosRatio;
                    tData.vTargetRotation = vLerpTargetRotRatio;
                }
            }
        }
        else
        {
            if (nullptr != m_pCutScene_OwnerModel)
            {
                OUTMATRIX tOutMatrix = CMethod::Out_Matrix(m_pCutScene_OwnerModel->Get_Transform()->Get_WorldFloat4x4());
                tData.vOwnerPosition = tOutMatrix.vPos;
                tData.vOwnerRotation = tOutMatrix.vRot;
            }
            if (nullptr != m_pCutScene_TargetModel)
            {
                OUTMATRIX tOutMatrix = CMethod::Out_Matrix(m_pCutScene_TargetModel->Get_Transform()->Get_WorldFloat4x4());
                tData.vTargetPosition = tOutMatrix.vPos;
                tData.vTargetRotation = tOutMatrix.vRot;
            }
        }
    }
}
// Lerp
void CCutSceneViewer_Tool::Lerp_CutSceneData()
{
    if (true == m_bUseLerp)
    {
        if (ImGui::TreeNode("CamLerpValue"))
        {
            ImGui::Text("Prev Cam Lerp");
            if (ImGui::BeginListBox("Prev", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                _uint i = 0;
                for (auto& iter : m_CutSceneDataVector)
                {
                    _float fTime = iter.fCurTime;
                    _string str = CMethod::Combine_Int(i, "");
                    str.append("_Time: ");
                    str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                    if (ImGui::Selectable(str.c_str()))
                    {
                        m_pSelect_lerpPrevSceneData = &iter;
                        m_strSelect_lerpPrevSceneName = str;

                        if (m_pSelect_lerpNextSceneData == m_pSelect_lerpPrevSceneData)
                        {
                            m_pSelect_lerpPrevSceneData = nullptr;
                            m_strSelect_lerpPrevSceneName = "SAME";
                        }
                    }
                    i++;
                }
                ImGui::EndListBox();
            }

            ImGui::Text("Next Cam Lerp");
            if (ImGui::BeginListBox("Next", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                _uint i = 0;
                for (auto& iter : m_CutSceneDataVector)
                {
                    _float fTime = iter.fCurTime;
                    _string str = CMethod::Combine_Int(i, "");
                    str.append("_Time: ");
                    str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                    if (ImGui::Selectable(str.c_str()))
                    {
                        m_pSelect_lerpNextSceneData = &iter;
                        m_strSelect_lerpNextSceneName = str;

                        if (m_pSelect_lerpNextSceneData == m_pSelect_lerpPrevSceneData)
                        {
                            m_pSelect_lerpNextSceneData = nullptr;
                            m_strSelect_lerpNextSceneName = "SAME";
                        }
                    }
                    i++;
                }
                ImGui::EndListBox();
            }

            ImGui::Checkbox("All Cam Lerp", &m_bAllCamLerp);
            if (true == m_bAllCamLerp)
            {
                ImGui::DragFloat("LerpCamAll", &m_fCamLerpAll);
            }
            else
            {
                ImGui::DragFloat("LerpCamPosRatio", &m_fLerpCamPosRatio, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("LerpCamRotRatio", &m_fLerpCamRotRatio, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("LerpCamSpeedRatio", &m_fLerpCamSpeedRatio, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("LerpSplineSpeedRatio", &m_fLerpSplineSpeedRatio, 0.01f, 0.f, 1.f);
                ImGui::DragFloat("LerpBezierSpeedRatio", &m_fLerpBezierSpeedRatio, 0.01f, 0.f, 1.f);
            }
            ImGui::TreePop();
        }
    }
}
// Lerp Cut Chardata
void CCutSceneViewer_Tool::Lerp_CutCharData()
{
    if (true == m_bUseCharLerp)
    {
        ImGui::Text("Prev Char Lerp");
        if (ImGui::BeginListBox("Prev", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            _uint i = 0;
            for (auto& iter : m_CutCharDataVector)
            {
                _float fTime = iter.fCurTime;
                _string str = CMethod::Combine_Int(i, "");
                str.append("_Time: ");
                str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                if (ImGui::Selectable(str.c_str()))
                {
                    m_pSelect_lerpPrevCharData = &iter;
                    m_strSelect_lerpPrevCharName = str;

                    if (m_pSelect_lerpPrevCharData == m_pSelect_lerpNextCharData)
                    {
                        m_pSelect_lerpPrevCharData = nullptr;
                        m_strSelect_lerpPrevCharName = "SAME";
                    }
                }
                i++;
            }
            ImGui::EndListBox();
        }
        ImGui::Text("Next Char Lerp");
        if (ImGui::BeginListBox("Next", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            _uint i = 0;
            for (auto& iter : m_CutCharDataVector)
            {
                _float fTime = iter.fCurTime;
                _string str = CMethod::Combine_Int(i, "");
                str.append("_Time: ");
                str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                if (ImGui::Selectable(str.c_str()))
                {
                    m_pSelect_lerpNextCharData = &iter;
                    m_strSelect_lerpNextCharName = str;

                    if (m_pSelect_lerpNextCharData == m_pSelect_lerpNextCharData)
                    {
                        m_pSelect_lerpNextCharData = nullptr;
                        m_strSelect_lerpNextCharName = "SAME";
                    }
                }
                i++;
            }
            ImGui::EndListBox();
        }

        ImGui::Checkbox("All Cam Lerp", &m_bAllCharLerp);
        if (true == m_bAllCharLerp)
        {
            ImGui::DragFloat("LerpCharAll", &m_fCharLerpAll);
        }
        else
        {
            ImGui::DragFloat("LerpOwnerPosRatio", &m_fLerpOwnerPosRatio, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("LerpOwnerRotRatio", &m_fLerpOwnerRotRatio, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("LerpTargetPosRatio", &m_fLerpTargetPosRatio, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("LerpTargetRotRatio", &m_fLerpTargetRotRatio, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("LerpOwnerSpeedRatio", &m_fLerpOwnerSpeedRatio, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("LerpTargetSpeedRatio", &m_fLerpTargetSpeedRatio, 0.01f, 0.f, 1.f);
        }
    }
}
// Show Cut Scene Viewer 
void CCutSceneViewer_Tool::Show_And_CutSceneViewer()
{
    if (ImGui::TreeNode("Show_And_CutSceneViewer"))
    {
        ImGui::Checkbox("Render_Texture_DebugMode", &m_bTexture_Render_DebugMode);
        ImGui::Checkbox("Render_Restrict_To_Select", &m_bRender_Restrict);

        // Cut Scene List
        if (ImGui::BeginListBox("CutSceneList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            _uint i = 0;
            for (auto& iter : m_CutSceneDataVector)
            {
                _float fTime = iter.fCurTime;
                _string str = CMethod::Combine_Int(i, "");
                str.append("_Time: ");
                str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                if (ImGui::Selectable(str.c_str()))
                {
                    m_pSelectSceneData = &iter;
                    m_strSelectSceneName = str;
                    m_iCutScene_SelectIndex = i;
                }
                i++;
            }
            ImGui::EndListBox();
        }
        if (nullptr != m_pSelectSceneData)
        {
            if (ImGui::Button(u8" 위"))
            {
                if ((m_iCutScene_SelectIndex - 1) >= 0)
                {
                    CUTSCENEDATA tData = m_CutSceneDataVector[m_iCutScene_SelectIndex];
                    m_CutSceneDataVector[m_iCutScene_SelectIndex] = m_CutSceneDataVector[m_iCutScene_SelectIndex - 1];
                    m_CutSceneDataVector[m_iCutScene_SelectIndex - 1] = tData;
                    m_pSelectSceneData = &m_CutSceneDataVector[m_iCutScene_SelectIndex - 1];
                    _uint i = 0;
                    for (auto& iter : m_CutSceneDataVector)
                    {
                        _float fTime = iter.fCurTime;
                        _string str = CMethod::Combine_Int(i, "");
                        str.append("_Time: ");
                        str.append(CMethod::Convert_Number_S((_double)fTime, 2));

                        if (m_pSelectSceneData == &iter)
                        {
                            m_pSelectSceneData = &iter;
                            m_strSelectSceneName = str;
                            m_iCutScene_SelectIndex = i;
                        }
                        i++;
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"아래"))
            {
                if ((m_iCutScene_SelectIndex + 1) < m_CutSceneDataVector.size())
                {
                    CUTSCENEDATA tData = m_CutSceneDataVector[m_iCutScene_SelectIndex];
                    m_CutSceneDataVector[m_iCutScene_SelectIndex] = m_CutSceneDataVector[m_iCutScene_SelectIndex + 1];
                    m_CutSceneDataVector[m_iCutScene_SelectIndex + 1] = tData;
                    m_pSelectSceneData = &m_CutSceneDataVector[m_iCutScene_SelectIndex + 1];
                    _uint i = 0;
                    for (auto& iter : m_CutSceneDataVector)
                    {
                        _float fTime = iter.fCurTime;
                        _string str = CMethod::Combine_Int(i, "");
                        str.append("_Time: ");
                        str.append(CMethod::Convert_Number_S((_double)fTime, 2));

                        if (m_pSelectSceneData == &iter)
                        {
                            m_pSelectSceneData = &iter;
                            m_strSelectSceneName = str;
                            m_iCutScene_SelectIndex = i;
                        }
                        i++;
                    }
                }
            }
            // Delete Cut Node
            if (ImGui::Button("Delete Cut Node"))
            {
                for (vector<CUTSCENEDATA>::iterator it = m_CutSceneDataVector.begin(); it != m_CutSceneDataVector.end();)
                {
                    if ((*it).fCurTime == m_pSelectSceneData->fCurTime)
                    {
                        m_CutSceneDataVector.erase(it);
                        m_pSelectSceneData = nullptr;
                        m_pSelect_lerpNextSceneData = nullptr;
                        m_pSelect_lerpPrevSceneData = nullptr;
                        m_strSelectSceneName = "REMOVE";
                        m_strSelect_lerpNextSceneName = "REMOVE";
                        m_strSelect_lerpPrevSceneName = "REMOVE";
                        ImGui::TreePop();
                        return;
                    }
                    else
                        ++it;
                }
            }
            if (ImGui::Button("Setting Camera"))
            {
                if (nullptr != m_pCamera_Tool)
                {
                    CTransform* pTransform = m_pCamera_Tool->Get_Transform();
                    if (nullptr != pTransform)
                    {
                        pTransform->Set_Pos(m_pSelectSceneData->vCamPosition.Get_Vector());
                        pTransform->Rotate_Turn(m_pSelectSceneData->vCamRotation);
                    }
                }
            }

            // Modify Scene Data 
            if (ImGui::TreeNodeEx(m_strSelectSceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Prev Time", &m_pSelectSceneData->fPrevTime, 0.f, (_float)m_pSelectSceneData->fCurTime);
                ImGui::SliderFloat("Cur Time", &m_pSelectSceneData->fCurTime, 0.f, (_float)m_dTotalTime);
                ImGui::DragFloat("Cur Time DG", &m_pSelectSceneData->fCurTime, 0.f, (_float)m_dTotalTime);
                if (ImGui::TreeNodeEx("Camera_Show", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("==<Cam>==");
                    _bool bDragPos = ImGui::DragFloat3("Cam Position", &m_pSelectSceneData->vCamPosition.x, 0.01f, -1000.f, 1000.f);
                    if (true == bDragPos)
                    {
                        CalCulate_CamLoadLength(m_iCutScene_SelectIndex);
                    }
                    if (ImGui::DragFloat3("Cam Rotation", &m_pSelectSceneData->vCamRotation.x, 0.01f, -1000.f, 1000.f))
                    {
                        m_pSelectSceneData->vCamQuaternion = CMethod::Convert_RotationToQuaternion(m_pSelectSceneData->vCamRotation);
                    }
                    ImGui::Checkbox("CamOwnerLook", &m_pSelectSceneData->bCamOwnerLook); ImGui::SameLine();
                    ImGui::Checkbox("CamTargetLook", &m_pSelectSceneData->bCamTargetLook);
                    ImGui::Checkbox("CamLerp", &m_pSelectSceneData->bCamLerp); ImGui::SameLine();
                    ImGui::Checkbox("CamMove", &m_pSelectSceneData->bCamMove);
                    if (true == m_pSelectSceneData->bCamLerp)
                    {
                        ImGui::Checkbox("DistanceToRotLerp", &m_pSelectSceneData->bDistanceToRotLerp);  ImGui::SameLine();
                        ImGui::Checkbox("CurNodeNearDisPlus", &m_pSelectSceneData->bCurNodeNearDisNotPlus);
                    }
                    else
                    {
                        ImGui::Checkbox("CurNodeNearDisPlus", &m_pSelectSceneData->bCurNodeNearDisNotPlus);
                    }
                    ImGui::DragFloat("CamSpeed", &m_pSelectSceneData->fCamSpeed, 0.01f, -1000.f, 1000.f);
                    ImGui::DragFloat("CamLerpSpeed", &m_pSelectSceneData->fCamLerpSpeed, 0.01f, 0.001f, 1000.f);
                    ImGui::DragFloat("LerpRestrict", &m_pSelectSceneData->fLerpRestrict, 0.01f, 1.f, 1000.f);
                    if (true == m_pSelectSceneData->bCamMove || true == m_pSelectSceneData->bSplineCam
                        || true == m_pSelectSceneData->bUseBezier)
                    {
                        ImGui::DragFloat("DeltaSpeed", &m_pSelectSceneData->fDeltaSpeed);
                    }
                    ImGui::Text("==<CamSpline>==");
                    // Spline 
                    if (true == ImGui::Checkbox("SplineCam", &m_pSelectSceneData->bSplineCam))
                        m_pSelectSceneData->bUseBezier = false;
                    // Same Line 
                    ImGui::SameLine();
                    // Bezier 
                    if (true == ImGui::Checkbox("UseBezier", &m_pSelectSceneData->bUseBezier))
                    {
                        m_pSelectSceneData->bSplineCam = false;
                        if (true == m_pSelectSceneData->bUseBezier)
                        {
                            m_pSelectSceneData->vBezierPos = m_pSelectSceneData->vCamPosition;
                            if (m_iCutScene_SelectIndex - 1 >= 0)
                            {
                                m_pSelectSceneData->vBezierPrevPos = m_CutSceneDataVector[m_iCutScene_SelectIndex - 1].vCamPosition;
                            }
                        }
                    }
                    // True 

                    if (true == m_pSelectSceneData->bSplineCam)
                    {
                        ImGui::DragFloat("SplineCamSpeed", &m_pSelectSceneData->fSplineCamSpeed, 0.1f, -1000.f, 1000.f);
                    }
                    if (true == m_pSelectSceneData->bUseBezier)
                    {
                        _bool bIsTrue = ImGui::DragFloat3("BezierPrevPos", &m_pSelectSceneData->vBezierPrevPos.x, 0.1f, -10000.f, 10000.f);
                        bIsTrue |= ImGui::DragFloat3("BezierPos", &m_pSelectSceneData->vBezierPos.x, 0.1f, -10000.f, 10000.f);

                        if (true == bIsTrue)
                            CalCulate_CamLoadLength(m_iCutScene_SelectIndex);

                        ImGui::DragFloat("BezierSpeed", &m_pSelectSceneData->fBezierSpeed, 0.1f, 0.001f, 1000.f);
                    }

                    // Cam Shake
                    ImGui::Text("==<CamShake>==");
                    ImGui::Checkbox("CamMoveShake", &m_pSelectSceneData->bCamMoveShake); ImGui::SameLine();
                    ImGui::Checkbox("CamRotShake", &m_pSelectSceneData->bCamRotShake);
                    if (true == m_pSelectSceneData->bCamMoveShake)
                    {
                        ImGui::DragFloat3("ShakePosValue", &m_pSelectSceneData->v3ShakePosValue.x, 1, 1, 1000);
                        ImGui::DragFloat("MoveShakePassActiveTime", &m_pSelectSceneData->fMoveShakePassActiveTime, 0.01f, -1000.f, 1000.f);
                    }
                    if (true == m_pSelectSceneData->bCamRotShake)
                    {
                        ImGui::DragInt3("ShakeRotValue", &m_pSelectSceneData->u3ShakeRotValue.x, 1, 1, 1000);
                        ImGui::DragFloat("RotShakePassActiveTime", &m_pSelectSceneData->fRotShakePassActiveTime, 0.01f, 0.001f, 1000.f);
                    }
                    if(true == m_pSelectSceneData->bCamMoveShake)
                        ImGui::Checkbox("CamStrongMove", &m_pSelectSceneData->bCamStrongShake);

                    ImGui::NewLine();
                    ImGui::Checkbox("CutSceneTexShow", &m_pSelectSceneData->bCutSceneTexShow);
                    if (true == m_pSelectSceneData->bCutSceneTexShow)
                    {
                        ImGui::Checkbox("AllTextureDraw", &m_pSelectSceneData->bAllTextureDraw);
                        _int iSize = (_int)m_TextureVector.size() - 1;
                        if (iSize < 0)
                            iSize = 0;
                        ImGui::DragInt("TextureIndex", &m_pSelectSceneData->iTextureIndex, 1, 0, iSize);
                    }
                    ImGui::Checkbox("CamOpposite", &m_pSelectSceneData->bCamOpposite);
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
            Modify_CutSceneTexture();
        }
        ImGui::TreePop();
    }
}

// Texture Vector 수정
void CCutSceneViewer_Tool::Modify_CutSceneTexture()
{
    if (ImGui::TreeNode("Modify_CutSceneTexture"))
    {
        ImGui::DragInt("CutSceneTexture Drag", &m_iCutSceneTextureIndex, 1, 0, (_int)m_CutSceneTexVector.size());
        ImGui::InputInt("CutSceneTexture Input", &m_iCutSceneTextureIndex, 0, 1);
        _string str = "";
        _string TEXT = CMethod::Combine_Int((_int)m_CutSceneTexVector.size(), str);
        TEXT.append(" Num");
        ImGui::Text(TEXT.c_str());

        if (m_CutSceneTexVector.size() > m_iCutSceneTextureIndex)
        {
            _string strTexture = "CutSceneTexture: ";
            // Cut Scene Texture
            strTexture = CMethod::Combine_Int(m_iCutSceneTextureIndex, strTexture);
            if (ImGui::TreeNodeEx(strTexture.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                m_CutSceneTexVector[m_iCutSceneTextureIndex]->Show_CutSceneTexture((_float)m_dTotalTime);
                ImGui::TreePop();
            }

            if (ImGui::Button("Delete Texture"))
            {
                for (vector<CCutSceneTex_Tool*>::iterator it = m_CutSceneTexVector.begin(); it != m_CutSceneTexVector.end(); ++it)
                {
                    if ((*it) == m_CutSceneTexVector[m_iCutSceneTextureIndex])
                    {
                        Safe_Release((*it));
                        m_CutSceneTexVector.erase(it);
                        break;
                    }
                }
            }
        }
        ImGui::TreePop();
    }
}

#pragma region CUTSCENEVIEWER
// Cut Scnee 
void CCutSceneViewer_Tool::Show_CutSceneAnimationViewer()
{
    if (nullptr != m_pCutScene_OwnerModel)
    {
        m_pCutScene_OwnerModel->Show_AnimationClip();
    }
    if (nullptr != m_pCutScene_TargetModel)
    {
        m_pCutScene_TargetModel->Show_AnimationClip();
    }

    ImGui::NewLine();
    if (ImGui::Button("Receive Owner Animation Time"))
    {
        if (nullptr != m_pCutScene_TargetModel && nullptr != m_pCutScene_OwnerModel)
        {
            m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Receive_ClipOrder(m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->ClipOrder);
        }
    }

    if (ImGui::TreeNode("Sword"))
    {
        if (ImGui::TreeNode("Owner_Sword"))
        {
            Attach_SwordModel(m_pOwnerSword, m_pCutScene_OwnerModel);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Target_Sword"))
        {
            Attach_SwordModel(m_pTargetSword, m_pCutScene_TargetModel);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void CCutSceneViewer_Tool::Attach_SwordModel(CItem_Tool* _pItemTool, CCutSceneModel_Tool* _pCutSceneModel)
{
    if (nullptr == _pItemTool || nullptr == _pCutSceneModel)
        return;

    ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), u8"모델 선택");
    for (auto& iter : m_ItemModelList)
    {
        if (nullptr != iter)
        {
            FILEGROUP* pFileGroup = iter->Get_FileGroup();
            FILEDATA* pFileData = iter->Get_FileData();

            if (nullptr != pFileGroup && nullptr != pFileData)
            {
                _wstring str = pFileGroup->strUppderName;
                str.append(L" ");
                str.append(pFileData->strFileName);
                if (ImGui::Selectable(CMethod::ConvertWToS(str).c_str()))
                {
                    if (nullptr != m_pOwnerSword)
                        m_pOwnerSword->Bine_Model(iter);
                }
            }
        }
    }
    ImGui::NewLine();
    if (nullptr != m_pOwnerSword)
    {
        // Anim Model
         _pItemTool->Find_Bones(_pCutSceneModel);
    }
}

void CCutSceneViewer_Tool::CalCulate_CamLoadLength(const _int& _iIndex)
{
    if (_iIndex - 1 > 0 && m_CutSceneDataVector.size() > _iIndex)
    {
        if (true == m_CutSceneDataVector[_iIndex].bUseBezier)
        {
            _vector v1 = m_CutSceneDataVector[_iIndex - 1].vCamPosition.Get_Vector();
            _vector v2 = m_CutSceneDataVector[_iIndex].vBezierPrevPos.Get_Vector();
            _vector v3 = m_CutSceneDataVector[_iIndex].vCamPosition.Get_Vector();
            _vector v4 = m_CutSceneDataVector[_iIndex].vBezierPos.Get_Vector();

            _vector vDrawPevPos = v3;
            _float fLength = 0.f;
            for (_int i = 0; i <= 100; ++i)
            {
                _vector Pos = CMethod::Compute_BezierCurve(v1, v2, v4, v3, (_float)((_float)i / 100));
                if (i != 0)
                {
                    _float3 v3Pos = Pos - vDrawPevPos;
                    fLength += v3Pos.Length();
                }
                vDrawPevPos = Pos;
            }
            m_CutSceneDataVector[_iIndex].fPrevDistance = fLength;
        }
        else if (true == m_CutSceneDataVector[_iIndex].bSplineCam)
        {
            if (!(m_CutSceneDataVector.size() > _iIndex + 1))
                return;
            if (!(m_CutSceneDataVector.size() > _iIndex + 2))
                return;

            _vector vPrevPos = m_CutSceneDataVector[_iIndex - 1].vCamPosition.Get_Vector();
            _vector vPos = m_CutSceneDataVector[_iIndex].vCamPosition.Get_Vector();
            _vector vNext1 = m_CutSceneDataVector[_iIndex + 1].vCamPosition.Get_Vector();
            _vector vNext2 = m_CutSceneDataVector[_iIndex + 2].vCamPosition.Get_Vector();
            _vector vDrawPevPos = vPrevPos;

            _float fLength = 0.f;
            for (_int i = 0; i <= 100; ++i)
            {
                _vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, (_float)((_float)i / 100));
                if (i != 0)
                {
                    _float3 v3Pos = Pos - vDrawPevPos;
                    fLength += v3Pos.Length();
                }
                vDrawPevPos = Pos;
            }
            m_CutSceneDataVector[_iIndex].fPrevDistance = fLength;
        }
        else if (true == m_CutSceneDataVector[_iIndex].bCamMove)
        {
            _float3 v3Length = m_CutSceneDataVector[_iIndex].vCamPosition - m_CutSceneDataVector[_iIndex - 1].vCamPosition;
            m_CutSceneDataVector[_iIndex].fPrevDistance = v3Length.Length();
        }
    }
}
#pragma endregion CUTSCENEVIEWER

void CCutSceneViewer_Tool::Update_CutScene(const _double& _dTimeDelta)
{
    if (true == m_bPlayCutChar)
    {
        // End
        if (m_fCurrentCharTime >= m_dTotalTime)
        {
            Reset_CharData();
            return;
        }
        // Ing
        else
        {
            if (m_iCutChar_Index < m_CutCharDataVector.size())
            {
                if (m_CutCharDataVector[m_iCutChar_Index].fCurTime <= m_fCurrentCharTime)
                {
                    ++m_iCutChar_Index;
                    // Cut Scene Data Lis 
                    if (m_CutCharDataVector.size() > m_iCutChar_Index)
                    {
                        Bind_NextCharData();
                    }
                }


                MoveTo_Char(_dTimeDelta * m_fMultipleValue);
                RotateTo_Char(_dTimeDelta * m_fMultipleValue);
                JumpTo_Target(_dTimeDelta * m_fMultipleValue);
                if (m_iCutChar_Index < m_CutCharDataVector.size())
                {
                    if (nullptr != m_pCutScene_OwnerModel)
                        m_pCutScene_OwnerModel->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bOwnerActive);
                    if (nullptr != m_pCutScene_TargetModel)
                        m_pCutScene_OwnerModel->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bOwnerActive);

                    m_fOwnerDeltaSpeed += (_float)(_dTimeDelta)*m_fMultipleValue * m_CutCharDataVector[m_iCutChar_Index].fOwnerDeltaSpeed;
                    m_fTargetDeltaSpeed += (_float)(_dTimeDelta)*m_fMultipleValue * m_CutCharDataVector[m_iCutChar_Index].fTargetDeltaSpeed;
                    m_vOwnerRotationDeltaSpeed = m_CutCharDataVector[m_iCutChar_Index].vOwnerRotationDeltaSpeed * 
                        (_float)(_dTimeDelta)*m_fMultipleValue ;
                    m_vTargetRotationDeltaSpeed = m_CutCharDataVector[m_iCutChar_Index].vTargetRotationDeltaSpeed *
                        (_float)(_dTimeDelta)*m_fMultipleValue;
                }
            }
            // Cut Scene DataList
            m_fCurrentCharTime += (_float)(_dTimeDelta)*m_fMultipleValue;
            // Cut Scene DataList
        }
    }
    else
    {
        if (m_dFallTimeAcc > 0.0)
        {
            Fall_Target(_dTimeDelta * m_fMultipleValue);
        }
    }
}
// Update Cut Scene
void CCutSceneViewer_Tool::LateUpdate_CutScene(const _double& _dTimeDelta)
{
    if (true == m_bPlayCutScene)
    {
        // End
        if (m_fCurrentCamTime >= m_dTotalTime)
        {
            Reset_CamData();
            return;
        }
        // Ing
        else
        {
            if (m_iCutScene_Index < m_CutSceneDataVector.size())
            {
                if (m_CutSceneDataVector[m_iCutScene_Index].fCurTime <= m_fCurrentCamTime)
                {
                    ++m_iCutScene_Index;
                    // Cut Scene Data Lis 
                    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
                    {
                        Bind_NextSceneData();
                    }
                }
                else
                {
                    if (false == m_CutSceneDataVector[m_iCutScene_Index].bCurNodeNearDisNotPlus)
                    {
                        if (m_CutSceneDataVector[m_iCutScene_Index].bCamMove || m_CutSceneDataVector[m_iCutScene_Index].bSplineCam
                            || m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
                        {
                            _float3 v3Length = m_CutSceneDataVector[m_iCutScene_Index].vCamPosition - m_pCamera_Tool->Get_Transform()->Get_PosF3();
                            if (v3Length.Length() <= 1.f)
                            {
                                ++m_iCutScene_Index;
                                // Cut Scene Data Lis 
                                if (m_CutSceneDataVector.size() > m_iCutScene_Index)
                                {
                                    Bind_NextSceneData();
                                }
                            }
                        }
                    }
                }
                // Look At Cam
                LookAt_ToCam(_dTimeDelta * m_fMultipleValue);
                // Move To Cam
                MoveTo_Cam(_dTimeDelta * m_fMultipleValue);
                // Lerp To cam
                LerpTo_Cam(_dTimeDelta * m_fMultipleValue);
                // Shake
                CamShake(_dTimeDelta * m_fMultipleValue);
                if (m_iCutScene_Index < m_CutSceneDataVector.size())
                {
                    m_fCamDeltaSpeed += (_float)_dTimeDelta * m_fMultipleValue * m_CutSceneDataVector[m_iCutScene_Index].fDeltaSpeed;
                }
            }
            // Cut Scene DataList
            m_fCurrentCamTime += (_float)(_dTimeDelta)*m_fMultipleValue;
            // Cut Scene DataList
        }
    }
}
// Render To All Position
void CCutSceneViewer_Tool::Render_To_All_Position()
{
    // 렌더 올 포지션
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        // 제한적이지 않을 때, 모두 그림
        if (false == m_bRender_Restrict)
        {
            _float3 v3Prev;
            _int iIndex = 0;
            for (auto& iter : m_CutCharDataVector)
            {
                Draw_Value(pGameInstance, DR_CUTVIEWER_MODEL_OWNER, _float3(0.5f, 0.5f, 0.5f), iter.vOwnerPosition);
                Draw_Value(pGameInstance, DR_CUTVIEWER_MODEL_TARGET, _float3(0.5f, 0.5f, 0.5f), iter.vTargetPosition);

                if (true == iter.bJump)
                {
                    Draw_CameraLine(pGameInstance, iter.vTargetPosition.Get_Vector(),
                        iter.vAttackDir.Normalize().Get_Vector(), _float4(0.1f, 0.5f, 0.1f, 1.f));
                }
            }
            for (auto& iter : m_CutSceneDataVector)
            {
                Draw_Value(pGameInstance, DR_CUTVIEWER_CAMERA, _float3(1.f, 1.f, 1.f), iter.vCamPosition, iter.vCamQuaternion);
                Draw_Value_Model(pGameInstance, _float3(0.1f, 0.1f, 0.1f), iter.vCamPosition, _float4(1.f, 0.25f, 0.88f, 1.f));
                if (false == iter.bSplineCam && false == iter.bUseBezier)
                {
                    if ((&m_CutSceneDataVector.front() != &iter))
                    {
                        Draw_CameraLine(pGameInstance, v3Prev, iter.vCamPosition, _float4(0.75f, 0.25f, 025.f, 1.f));
                    }
                }
                else
                {
                    if (true == m_bRender_Spline && true == iter.bSplineCam)
                    {
                        if (iIndex > 0)
                        {
                            if (!(m_CutSceneDataVector.size() > iIndex + 1))
                                continue;
                            if (!(m_CutSceneDataVector.size() > iIndex + 2))
                                continue;

                            _vector vPrevPos = m_CutSceneDataVector[iIndex - 1].vCamPosition.Get_Vector();
                            _vector vPos = m_CutSceneDataVector[iIndex].vCamPosition.Get_Vector();
                            _vector vNext1 = m_CutSceneDataVector[iIndex + 1].vCamPosition.Get_Vector();
                            _vector vNext2 = m_CutSceneDataVector[iIndex + 2].vCamPosition.Get_Vector();

                            _vector vDrawPevPos = vPrevPos;

                            for (_int i = 0; i <= 50; ++i)
                            {
                                _vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, (_float)((_float)i / 50));
                                Draw_CameraLine(pGameInstance, vDrawPevPos, Pos, _float4(1.f, 0.f, 0.f, 1.f));
                                vDrawPevPos = Pos;
                            }
                        }
                    }
                    else if (true == m_bRender_Bezier && true == iter.bUseBezier)
                    {
                        if (!(m_CutSceneDataVector.size() > 0))
                            continue;

                        if (iIndex - 1 >= 0)
                        {
                            _vector v1 = m_CutSceneDataVector[iIndex - 1].vCamPosition.Get_Vector();
                            _vector v2 = m_CutSceneDataVector[iIndex].vBezierPrevPos.Get_Vector();
                            _vector v3 = m_CutSceneDataVector[iIndex].vCamPosition.Get_Vector();
                            _vector v4 = m_CutSceneDataVector[iIndex].vBezierPos.Get_Vector();

                            _vector vDrawPevPos = v3;

                            for (_int i = 0; i <= 50; ++i)
                            {
                                _vector Pos = CMethod::Compute_BezierCurve(v1, v2, v4, v3, (_float)((_float)i / 50));
                                if (0 != i)
                                    Draw_CameraLine(pGameInstance, vDrawPevPos, Pos, _float4(0.5f, 1.f, 1.f, 1.f));
                                vDrawPevPos = Pos;
                            }
                            Draw_CameraLine(pGameInstance, v1, v2, _float4(0.25f, 1.f, 0.25f, 1.f));
                            Draw_CameraLine(pGameInstance, v3, v4, _float4(0.25f, 1.f, 0.25f, 1.f));

                            Draw_Value_Model(pGameInstance, _float3(0.25f, 0.25f, 0.25f), _float3(v2), _float4(0.2f, 0.25f, 1.f, 1.f));
                            Draw_Value_Model(pGameInstance, _float3(0.25f, 0.25f, 0.25f), _float3(v4), _float4(0.2f, 0.25f, 1.f, 1.f));
                        }
                    }
                }
                v3Prev = iter.vCamPosition;
                ++iIndex;
            }
            Draw_Value_Model(pGameInstance, _float3(1.f, 1.f, 1.f), m_v3OwnerPosition, _float4(0.5f, 1.f, 0.f, 1.f));
            Draw_Value_Model(pGameInstance, _float3(1.f, 1.f, 1.f), m_v3TargetPosition, _float4(1.f, 0.25f, 0.f, 1.f));

            CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();

            OUTMATRIX tMatrix = CMethod::Out_Matrix(pCamTransform->Get_WorldFloat4x4());

            if (true == m_bChangeCamera)
            {
                Draw_Value(pGameInstance, DR_CUTVIEWER_CAMERA, _float3(5.f, 5.f, 5.f), pCamTransform->Get_PosF3(),
                    tMatrix.vQuaternionRot);
            }

            for (auto& iter : m_CutSceneTexVector)
                iter->Draw_TexturePosition(pGameInstance);
        }
        else
        {
            if (nullptr != m_pSelectCutCharData)
            {
                Draw_Value(pGameInstance, DR_CUTVIEWER_MODEL_OWNER, _float3(0.5f, 0.5f, 0.5f), m_pSelectCutCharData->vOwnerPosition);
                Draw_Value(pGameInstance, DR_CUTVIEWER_MODEL_TARGET, _float3(0.5f, 0.5f, 0.5f), m_pSelectCutCharData->vTargetPosition);

                if (true == m_pSelectCutCharData->bJump)
                {
                    Draw_CameraLine(pGameInstance, m_pSelectCutCharData->vTargetPosition.Get_Vector(),
                        m_pSelectCutCharData->vAttackDir.Normalize().Get_Vector(), _float4(0.1f, 0.5f, 0.1f, 1.f));
                }
            }

            // 선택된 노드만 그림
            if (nullptr != m_pSelectSceneData)
            {
                Draw_Value(pGameInstance, DR_CUTVIEWER_CAMERA, _float3(1.f, 1.f, 1.f), m_pSelectSceneData->vCamPosition, m_pSelectSceneData->vCamQuaternion);
            }
            Draw_Value_Model(pGameInstance, _float3(1.f, 1.f, 1.f), m_v3OwnerPosition, _float4(0.5f, 1.f, 0.f, 1.f));
            Draw_Value_Model(pGameInstance, _float3(1.f, 1.f, 1.f), m_v3TargetPosition, _float4(1.f, 0.25f, 0.f, 1.f));
            CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();

            OUTMATRIX tMatrix = CMethod::Out_Matrix(pCamTransform->Get_WorldFloat4x4());

            Draw_Value(pGameInstance, DR_CUTVIEWER_CAMERA, _float3(5.f, 5.f, 5.f), pCamTransform->Get_PosF3(),
                tMatrix.vQuaternionRot);
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}
// Render Terrain
void CCutSceneViewer_Tool::Render_Terrain()
{
    if (nullptr == m_pTerrain_ShaderCom || nullptr == m_pVIBuffer_TerrainData)
        return;
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
        WorldMatrix = m_pTerrain_TransformComp->Get_WorldFloat4x4_TP();
        ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
        ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

        if (FAILED(m_pTerrain_ShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }
        if (FAILED(m_pTerrain_ShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }
        if (FAILED(m_pTerrain_ShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        m_pTerrain_ShaderCom->Begin(1);
        m_pVIBuffer_TerrainData->Render();
    }
    RELEASE_INSTANCE(CGameInstance)
}
// Render Texture
void CCutSceneViewer_Tool::Render_Texture()
{
    if (m_iCutScene_Index < m_CutSceneDataVector.size())
    {
        if (m_TextureVector.size() <= 0)
            return;
        // 컷씬이 시작되지 않았을 떄
        if (false == m_bPlayCutScene)
        {
            // 디버그 용도, Move 만 사용
            if (true == m_bTexture_Render_DebugMode)
            {
                // Move Texture + 1
                for (auto& iter : m_CutSceneTexVector)
                {
                    iter->Render_Texture_Move(m_pCutScene_Shader, m_TextureVector[m_iCutScene_Texture_Index],
                        m_VIBuffer_CutSceneTexture);
                }
            }
        }
        else
        {
            if (m_iCutScene_Index < m_CutSceneDataVector.size())
            {
                if (true == m_CutSceneDataVector[m_iCutScene_Index].bCutSceneTexShow)
                {
                    if (m_iCutSceneTextureIndex < m_CutSceneTexVector.size())
                    {
                        if (nullptr != m_CutSceneTexVector[m_iCutSceneTextureIndex])
                        {
                            if (m_CutSceneTexVector[m_iCutSceneTextureIndex]->Get_CurTime() >= m_fCurrentCamTime)
                            {
                                m_CutSceneTexVector[m_iCutSceneTextureIndex]->Render_Screen(m_Texture_WorldMatrix, m_Texture_ViewMatrix, m_Texture_ProjMatrix,
                                    m_pCutScene_Shader, m_TextureVector[m_iCutScene_Texture_Index], m_VIBuffer_CutSceneTexture);
                            }
                            else
                            {
                                ++m_iCutSceneTextureIndex;
                                if (m_iCutSceneTextureIndex < m_CutSceneTexVector.size())
                                {
                                    m_CutSceneTexVector[m_iCutSceneTextureIndex]->Render_Screen(m_Texture_WorldMatrix, m_Texture_ViewMatrix, m_Texture_ProjMatrix,
                                        m_pCutScene_Shader, m_TextureVector[m_iCutScene_Texture_Index], m_VIBuffer_CutSceneTexture);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
// 현재 피킹 할 수 있는 지형을 띄워놓았는데 그거 피킹하는거
void CCutSceneViewer_Tool::Picking_Terrain()
{
    if (false == m_bIsPicking)
        return;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        if (true == pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
        {
            if (0 == m_iSelectTarget)
            {
                pGameInstance->Picking_Mesh(nullptr, m_pTerrain_TransformComp, m_pVIBuffer_TerrainData, &m_v3OwnerPosition);
            }
            else if (1 == m_iSelectTarget)
            {
                pGameInstance->Picking_Mesh(nullptr, m_pTerrain_TransformComp, m_pVIBuffer_TerrainData, &m_v3TargetPosition);
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}

void CCutSceneViewer_Tool::Bind_NextSceneData()
{
    if (nullptr == m_pCamera_Tool)
        return;

    m_fLerpAddValue = 0.f;
    m_fMoveSplineValue = 0.f;
    m_fMoveBezierValue = 0.f;
    m_fCamDeltaSpeed = 0.f;

    if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamMove && false == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam
        && false == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
    {
        m_pCamera_Tool->Get_Transform()->Set_Pos(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector());
    }

    if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamOwnerLook && false == m_CutSceneDataVector[m_iCutScene_Index].bCamTargetLook
        && false == m_CutSceneDataVector[m_iCutScene_Index].bCamLerp)
    {
        m_pCamera_Tool->Get_Transform()->Rotate_Turn(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation);
    }

    if (m_CutSceneDataVector[m_iCutScene_Index].bCutSceneTexShow)
        ++m_iCutSceneTextureIndex;

    m_tMoveShakeTimer = CUSTIMER(m_CutSceneDataVector[m_iCutScene_Index].fMoveShakePassActiveTime);
    m_tRotShakeTimer = CUSTIMER(m_CutSceneDataVector[m_iCutScene_Index].fRotShakePassActiveTime);
    m_tMoveShakeTimer.bPass = true;
    m_tRotShakeTimer.bPass = true;
}

void CCutSceneViewer_Tool::LookAt_ToCam(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (nullptr == m_pCamera_Tool)
            return;

        CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr == pCamTransform)
            return;

        if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamOwnerLook)
        {
            _float3 vLook;
            if (nullptr == m_pCutScene_OwnerModel)
                return;

            CTransform* pOwnerTr = m_pCutScene_OwnerModel->Get_Transform();
            if (nullptr == pOwnerTr)
                return;

            vLook = pOwnerTr->Get_PosF3();
            vLook.y = 0.f;
            pCamTransform->LookAt(pCamTransform->Get_Pos() + vLook.Get_Vector());
        }
        else if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamTargetLook)
        {
            _float3 vLook;
            if (nullptr == m_pCutScene_TargetModel)
                return;

            CTransform* pOwnerTr = m_pCutScene_TargetModel->Get_Transform();
            if (nullptr == pOwnerTr)
                return;

            vLook = pOwnerTr->Get_PosF3();
            vLook.y = 0.f;
            pCamTransform->LookAt(pCamTransform->Get_Pos() + vLook.Get_Vector());
        }
    }
}

void CCutSceneViewer_Tool::MoveTo_Cam(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamMove)
        {
            MoveTo_CamJust(_dTimeDelta);
        }
        else if (true == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam)
        {
            MoveTo_CamSpline(_dTimeDelta);
        }
        else if (true == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
        {
            MoveTo_CamBezier(_dTimeDelta);
        }
    }
}

void CCutSceneViewer_Tool::MoveTo_CamJust(const _double& _dTimeDelta)
{
    if (0 == m_iCutScene_Index)
        return;

    CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
    if (nullptr == pCamTransform)
        return;

    pCamTransform->Translate_Pos(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector(), _dTimeDelta,
        m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
}

void CCutSceneViewer_Tool::MoveTo_CamSpline(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (false == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam)
            return;

        if (nullptr == m_pCamera_Tool)
            return;

        if (0 == m_iCutScene_Index)
            return;

        CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr == pCamTransform)
            return;

        if (m_CutSceneDataVector.size() > m_iCutScene_Index + 1)
        {
            if (m_CutSceneDataVector.size() > m_iCutScene_Index + 2)
            {
                _vector vPrevPos = m_CutSceneDataVector[m_iCutScene_Index - 1].vCamPosition.Get_Vector();
                _vector vPos = m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector();
                _vector vNext1 = m_CutSceneDataVector[m_iCutScene_Index + 1].vCamPosition.Get_Vector();
                _vector vNext2 = m_CutSceneDataVector[m_iCutScene_Index + 2].vCamPosition.Get_Vector();

                _float fMulti = m_CutSceneDataVector[m_iCutScene_Index].fSplineCamSpeed;
                if (0 == fMulti)
                    fMulti = 1.f;

                if (m_fMoveSplineValue > 1.f)
                {
                    m_fMoveSplineValue = 1.f;
                    _vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, m_fMoveSplineValue);
                    pCamTransform->Translate_Pos(Pos, _dTimeDelta * fMulti, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed
                         + m_fCamDeltaSpeed, 0.5f);
                }
                else
                {
                    _vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, m_fMoveSplineValue);
                    pCamTransform->Translate_Pos(Pos, _dTimeDelta * fMulti, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed
                        + m_fCamDeltaSpeed, 0.5f);
                    m_fMoveSplineValue += (_float)(_dTimeDelta)*fMulti;
                }
            }
            else
            {
                MoveTo_CamJust(_dTimeDelta);
            }
        }
        else
        {
            MoveTo_CamJust(_dTimeDelta);
        }
    }
}

void CCutSceneViewer_Tool::MoveTo_CamBezier(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (false == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
            return;

        if (nullptr == m_pCamera_Tool)
            return;

        if (0 == m_iCutScene_Index)
            return;

        CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr == pCamTransform)
            return;

        if (m_CutSceneDataVector.size() > 0)
        {
            _vector vP1 = m_CutSceneDataVector[m_iCutScene_Index - 1].vCamPosition.Get_Vector();
            _vector vB1 = m_CutSceneDataVector[m_iCutScene_Index].vBezierPrevPos.Get_Vector();
            _vector vP2 = m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector();
            _vector vB2 = m_CutSceneDataVector[m_iCutScene_Index].vBezierPos.Get_Vector();

            _float fMulti = m_CutSceneDataVector[m_iCutScene_Index].fBezierSpeed;
            if (0 == fMulti)
                fMulti = 1.f;

            if (m_fMoveBezierValue > 1.f)
            {
                m_fMoveBezierValue = 1.f;
                _vector Pos = CMethod::Compute_BezierCurve(vP1, vB1, vB2, vP2, m_fMoveBezierValue);
                pCamTransform->Translate_Pos(Pos, _dTimeDelta, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
            }
            else
            {
                _vector Pos = CMethod::Compute_BezierCurve(vP1, vB1, vB2, vP2, m_fMoveBezierValue);
                pCamTransform->Translate_Pos(Pos, _dTimeDelta, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
                m_fMoveBezierValue += (_float)(_dTimeDelta)*fMulti;
            }
        }
        else
        {
            MoveTo_CamJust(_dTimeDelta);
        }
    }
}

void CCutSceneViewer_Tool::LerpTo_Cam(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (nullptr == m_pCamera_Tool)
            return;

        CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr == pCamTransform)
            return;

        if (0 == m_iCutScene_Index)
            return;


        if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamLerp)
        {
            OUTMATRIX OutMatrix = CMethod::Out_Matrix(pCamTransform->Get_WorldFloat4x4());

            _float3 v3Length = OutMatrix.vPos - pCamTransform->Get_Pos();
            if (v3Length.Length() <= 0.0001f)
                return;

            if (false == m_CutSceneDataVector[m_iCutScene_Index].bDistanceToRotLerp)
            {
                _float fValue = m_CutSceneDataVector[m_iCutScene_Index].fCamLerpSpeed;
                if (fValue == 0.f)
                    fValue = 1.f;

                _float fSpeed = 0.f;
                if (0 != fValue)
                    fSpeed = (_float)(_dTimeDelta)*fValue * 0.25f;
                else
                    fSpeed = (_float)(_dTimeDelta) * 0.25f;

                if (m_fLerpAddValue <= m_CutSceneDataVector[m_iCutScene_Index].fLerpRestrict)
                {
                    _float3 vRotation = _float3();

                    if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamOpposite)
                    {
                        OutMatrix.vRot.y += 360.f;
                        vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), m_CutSceneDataVector[m_iCutScene_Index].vCamRotation.Get_Vector(), fSpeed);
                    }
                    else
                    {
                         vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), m_CutSceneDataVector[m_iCutScene_Index].vCamRotation.Get_Vector(), fSpeed);
                    }

                    pCamTransform->Rotate_Turn(vRotation);
                }
                m_fLerpAddValue += fSpeed;
            }
            else
            {
                _float fDistnace = m_CutSceneDataVector[m_iCutScene_Index].fPrevDistance;
                if (fDistnace == 0.f)
                    fDistnace = 1.f;

                _float fValue = m_CutSceneDataVector[m_iCutScene_Index].fCamLerpSpeed * (1 / fDistnace) *
                    m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed;
                if (fValue == 0.f)
                    fValue = 1.f;

                _float fSpeed = 0.f;
                if (0 != fValue)
                    fSpeed = (_float)(_dTimeDelta)*fValue * 0.25f;
                else
                    fSpeed = (_float)(_dTimeDelta) * 0.25f;

                if (m_fLerpAddValue <= m_CutSceneDataVector[m_iCutScene_Index].fLerpRestrict)
                {
                    _float3 vRotation = _float3();

                    if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamOpposite)
                    {
                        OutMatrix.vRot.y += 360.f;
                        vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), m_CutSceneDataVector[m_iCutScene_Index].vCamRotation.Get_Vector(), fSpeed);
                    }
                    else
                    {
                         vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), m_CutSceneDataVector[m_iCutScene_Index].vCamRotation.Get_Vector(), fSpeed);
                    }

                    pCamTransform->Rotate_Turn(vRotation);
                }
                m_fLerpAddValue += fSpeed;
            }
        }
    }
}

void CCutSceneViewer_Tool::CamShake(const _double& _dTimeDelta)
{
    if (m_CutSceneDataVector.size() > m_iCutScene_Index)
    {
        if (nullptr == m_pCamera_Tool)
            return;

        CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr == pCamTransform)
            return;

        if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamMoveShake)
        {
            if (true == m_tMoveShakeTimer.Is_Over(_dTimeDelta))
            {
                _float3 v3Position = pCamTransform->Get_Pos();
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.x)
                    v3Position.x += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.x * Multiple_Minus_Random());
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.y)
                    v3Position.y += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.y * Multiple_Minus_Random());
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.z)
                    v3Position.z += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.z * Multiple_Minus_Random());

                if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamStrongShake)
                {
                    pCamTransform->Translate_Pos(v3Position.Get_Vector(), _dTimeDelta, 1.f);
                }
                else
                {
                    pCamTransform->Set_Pos(v3Position.Get_Vector());
                }
                m_tMoveShakeTimer.Reset_Timer();
            }
            else
            {
                if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamMove || 0.f == m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed)
                {
                    pCamTransform->Set_Pos(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector());
                }
            }
        }
        if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamRotShake)
        {
            if (true == m_tRotShakeTimer.Is_Over(_dTimeDelta))
            {
                _float3 v3Rotation = m_CutSceneDataVector[m_iCutScene_Index].vCamRotation;
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.x)
                    v3Rotation.x += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.x * Multiple_Minus_Random());
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.y)
                    v3Rotation.y += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.y * Multiple_Minus_Random());
                if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.z)
                    v3Rotation.z += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.z * Multiple_Minus_Random());

                pCamTransform->Rotate_Turn(v3Rotation);
                m_tRotShakeTimer.Reset_Timer();
            }
        }
    }
}

_int CCutSceneViewer_Tool::Multiple_Minus_Random()
{
    _int iRand = 0;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        iRand = pGameInstance->Get_RandomCount(100) % 2;
    }
    RELEASE_INSTANCE(CGameInstance);

    if (iRand == 0)
        return -1;

    return 1;
}

void CCutSceneViewer_Tool::Bind_NextCharData()
{
    if (nullptr == m_pCutScene_OwnerModel || nullptr == m_pCutScene_TargetModel)
        return;

    if (m_CutCharDataVector.size() <= 0)
        return;
    if (nullptr != m_pCutScene_OwnerModel)
    {
        if (false == m_CutCharDataVector[m_iCutChar_Index].bOwnerMove && false == m_bRootBone_Owner)
        {
            m_pCutScene_OwnerModel->Get_Transform()->Set_Pos(m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition.Get_Vector());
        }
        m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->vSavePosition = m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition;
    }
    m_pCutScene_OwnerModel->Get_Transform()->Rotate_Turn(m_CutCharDataVector[m_iCutChar_Index].vOwnerRotation);
    m_pCutScene_OwnerModel->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bOwnerActive);
    if (nullptr != m_pCutScene_TargetModel)
    {
        if (false == m_CutCharDataVector[m_iCutChar_Index].bTargetMove && false == m_CutCharDataVector[m_iCutChar_Index].bJump
            && false == m_bRootBone_Target)
        {
            m_pCutScene_TargetModel->Get_Transform()->Set_Pos(m_CutCharDataVector[m_iCutChar_Index].vTargetPosition.Get_Vector());
            m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->vSavePosition = m_CutCharDataVector[0].vTargetPosition;
        }
        m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->vSavePosition = m_CutCharDataVector[m_iCutChar_Index].vTargetPosition;
    }
    m_pCutScene_TargetModel->Get_Transform()->Rotate_Turn(m_CutCharDataVector[m_iCutChar_Index].vTargetRotation);
    m_pCutScene_OwnerModel->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bTargetActive);

    if (m_pCutScene_TargetModel->Get_Transform()->Get_PosF3().y < m_CutCharDataVector[0].vTargetPosition.y)
    {
        _float3 v3Pos = m_pCutScene_TargetModel->Get_Transform()->Get_PosF3();
        v3Pos.y = m_CutCharDataVector[0].vTargetPosition.y;
        m_pCutScene_TargetModel->Get_Transform()->Set_Pos(v3Pos.Get_Vector());
    }

    m_fTargetDeltaSpeed = 0.f;
    m_fOwnerDeltaSpeed = 0.f;
    m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
    m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
}

void CCutSceneViewer_Tool::MoveTo_Char(const _double& _dTimeDelta)
{
    if (m_CutCharDataVector.size() > m_iCutChar_Index)
    {
        if (false == m_bRootBone_Owner)
        {
            if (true == m_CutCharDataVector[m_iCutChar_Index].bOwnerMove)
            {
                m_pCutScene_OwnerModel->Get_Transform()->Translate_Pos(m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition.Get_Vector(), _dTimeDelta,
                    m_CutCharDataVector[m_iCutChar_Index].fOwnerSpeed + m_fOwnerDeltaSpeed, 0.5f);
            }
        }
        else
        {
            m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->RootBoneTransform();
        }
        if (false == m_bRootBone_Target)
        {
            if (true == m_CutCharDataVector[m_iCutChar_Index].bTargetMove)
            {
                m_pCutScene_TargetModel->Get_Transform()->Translate_Pos(m_CutCharDataVector[m_iCutChar_Index].vTargetPosition.Get_Vector(), _dTimeDelta,
                    m_CutCharDataVector[m_iCutChar_Index].fTargetSpeed + m_fTargetDeltaSpeed, 0.5f);
            }
        }
        else
        {
            m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->RootBoneTransform();
        }
    }
}

void CCutSceneViewer_Tool::RotateTo_Char(const _double& _dTimeDelta)
{
    if (m_CutCharDataVector.size() > m_iCutChar_Index)
    {
        if (false == m_bRootBone_Owner)
        {
            if (true == m_CutCharDataVector[m_iCutChar_Index].bOwnerRotationUse)
            {
                OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pCutScene_OwnerModel->Get_Transform()->Get_WorldMatrix());
                _float3 vRotation = (m_CutCharDataVector[m_iCutChar_Index].vOwnerRotationSpeed + m_vOwnerRotationDeltaSpeed)
                    * (_float)_dTimeDelta;
                m_pCutScene_OwnerModel->Get_Transform()->Rotate_Turn(vRotation + OutMatrix.vRot);
            }
        }
        if (false == m_bRootBone_Target)
        {
            if (true == m_CutCharDataVector[m_iCutChar_Index].bTargetRotationUse)
            {
                OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pCutScene_TargetModel->Get_Transform()->Get_WorldMatrix());
                _float3 vRotation = (m_CutCharDataVector[m_iCutChar_Index].vTargetRotationSpeed  + m_vTargetRotationDeltaSpeed )
                    * (_float)_dTimeDelta;
                m_pCutScene_TargetModel->Get_Transform()->Rotate_Turn(vRotation + OutMatrix.vRot);
            }
        }
    }
}

void CCutSceneViewer_Tool::JumpTo_Target(const _double& _dTimeDelta)
{
    if (m_CutCharDataVector.size() > m_iCutChar_Index)
    {
        CTransform* pTransform = m_pCutScene_TargetModel->Get_Transform();
        if (nullptr == pTransform)
            return;

        if (false == m_CutCharDataVector[m_iCutChar_Index].bNotResetJumpTarget)
        {
            if (true == m_CutCharDataVector[m_iCutChar_Index].bJump)
            {
                if (m_iJump_CheckCharIndex != m_iCutChar_Index)
                {
                    m_dFallTimeAcc = 0.0;
                    m_bPass = true;
                    m_iJump_CheckCharIndex = m_iCutChar_Index;
                }
            }
        }

        if (pTransform->Get_PosF3().y < 0)
        {
            m_dFallTimeAcc = 0.0;
            m_bPass = false;
        }
        else
        {
            if (true == m_bPass)
            {
                _float3 vNormalize = m_CutCharDataVector[m_iCutChar_Index].vAttackDir.Normalize();
                _vector v3Pos = Compute_MovePos_ByDist(_dTimeDelta, vNormalize.Get_Vector(), m_CutCharDataVector[m_iCutChar_Index].fDist,
                    m_CutCharDataVector[m_iCutChar_Index].fMaxHeight, m_CutCharDataVector[m_iCutChar_Index].fMaxHeightRatio, m_CutCharDataVector[m_iCutChar_Index].fPlayTime);

                m_pCutScene_TargetModel->Get_Transform()->Set_Pos(v3Pos + pTransform->Get_Pos());
                m_dFallTimeAcc += (_double)(_dTimeDelta) * (_double)(m_CutCharDataVector[m_iCutChar_Index].fJumpDelayTime);
            }
        }
    }
}

_bool CCutSceneViewer_Tool::Fall_Target(const _double& _dTimeDelta)
{
    if (m_CutCharDataVector.size() <= 0)
        return false;

    if (nullptr == m_pCutScene_TargetModel)
        return false;

    CTransform* pTransform = m_pCutScene_TargetModel->Get_Transform();
    if (nullptr == pTransform)
        return false;

    _bool Air = Is_AirTarget(pTransform);

    //if (true == Air)
    //{
    //    m_dFallTimeAcc = 0.0;
    //    m_bPass = true;
    //    return true;
    //}
    //else
    //{
    //    if (false == m_bPass)
    //    {
    //        _float3 vNormalize = m_CutCharDataVector[m_iCutChar_Index].vAttackDir.Normalize();
    //        _vector v3Pos = Compute_MovePos_ByDist(_dTimeDelta, vNormalize.Get_Vector(), m_CutCharDataVector[m_iCutChar_Index].fDist,
    //            m_CutCharDataVector[m_iCutChar_Index].fMaxHeight, m_CutCharDataVector[m_iCutChar_Index].fMaxHeightRatio, m_CutCharDataVector[m_iCutChar_Index].fPlayTime);

    //        m_pCutScene_TargetModel->Get_Transform()->Set_Pos(v3Pos + pTransform->Get_Pos());
    //        m_dFallTimeAcc += (_double)(_dTimeDelta);
    //        return false;
    //    }
    //}
    return false;
}

_bool CCutSceneViewer_Tool::Is_AirTarget(CTransform* pTransform)
{
    if (pTransform->Get_PosF3().y < m_CutCharDataVector[0].vTargetPosition.y)
    {
        return false;
    }
    return true;
}

_vector CCutSceneViewer_Tool::Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir, _float fMoveSpeed, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
    // 이동속도에 따른 Y를 제외한 이동값을 구한다.
    _vector vMovePos = XMVector3Normalize(vMoveDir) * fMoveSpeed * _float(_dTimeDelta);
    vMovePos = XMVectorSetY(vMovePos, 0.f);

    _float fGravity = 0.f;
    _float fHeight = 0.f;

    // 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
    if (fMaxHeightRatio * (_float)(fPlayTime) == 0.f)
    {
        fGravity = 400.f;
        fHeight = 0.f;
    }
    else
    {
        fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
        fHeight = fGravity * (_float)(fPlayTime)*fMaxHeightRatio;
    }
    // FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
    _float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
    vMovePos = XMVectorSetY(vMovePos, fY);

    return vMovePos;
}
_vector CCutSceneViewer_Tool::Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
    // 최대 이동할 거리를 이용하여 총 플레이타임과의 계산을 통해 프레임당 이동거리를 계산한다.
    if (0.f == fPlayTime)
        return XMVectorZero();

    // 거리에 따른 프레임당 이동거리
    _vector vMovePos = XMVector3Normalize(vMoveDir) * (fDist / fPlayTime) * _float(_dTimeDelta);
    vMovePos = XMVectorSetY(vMovePos, 0.f);

    _float fGravity = 0.f;
    _float fHeight = 0.f;

    // 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
    if (fMaxHeightRatio * (_float)(fPlayTime)*fMaxHeight == 0.f)
    {
        fGravity = 400.f;
        fHeight = 0.f;
    }
    else
    {
        fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
        fHeight = fGravity * (_float)(fPlayTime)*fMaxHeightRatio;
    }
    // FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
    _float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
    vMovePos = XMVectorSetY(vMovePos, fY);

    m_dFallTimeAcc += _dTimeDelta;

    return vMovePos;
}

#pragma region CUTCHARDATA

void CCutSceneViewer_Tool::Show_CutCharData()
{
    ImGui::Checkbox("RootBone_Owner", &m_bRootBone_Owner);
    ImGui::Checkbox("RootBone_Target", &m_bRootBone_Target);
    if (ImGui::TreeNodeEx("CutCharCharacter"))
    {
        if (ImGui::TreeNodeEx("CharModify", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Cut Scene List
            if (ImGui::BeginListBox("CutCharList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                _uint i = 0;
                for (auto& iter : m_CutCharDataVector)
                {
                    _float fTime = iter.fCurTime;
                    _string str = CMethod::Combine_Int(i, "");
                    str.append("_Time: ");
                    str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                    if (ImGui::Selectable(str.c_str()))
                    {
                        m_pSelectCutCharData = &iter;
                        m_iCutChar_SelectIndex = i;
                        m_strSelectCutCharName = str;
                    }
                    i++;
                }
                ImGui::EndListBox();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Show Char Modify", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (nullptr != m_pSelectCutCharData)
            {
                if (ImGui::Button(u8" 위"))
                {
                    if ((m_iCutChar_SelectIndex - 1) >= 0)
                    {
                        CUTCHARDATA tData = m_CutCharDataVector[m_iCutChar_SelectIndex];
                        m_CutCharDataVector[m_iCutChar_SelectIndex] = m_CutCharDataVector[m_iCutChar_SelectIndex - 1];
                        m_CutCharDataVector[m_iCutChar_SelectIndex - 1] = tData;
                        m_pSelectCutCharData = &m_CutCharDataVector[m_iCutChar_SelectIndex - 1];
                        _uint i = 0;
                        for (auto& iter : m_CutCharDataVector)
                        {
                            _float fTime = iter.fCurTime;
                            _string str = CMethod::Combine_Int(i, "");
                            str.append("_Time: ");
                            str.append(CMethod::Convert_Number_S((_double)fTime, 2));

                            if (m_pSelectCutCharData->fCurTime == iter.fCurTime)
                            {
                                m_pSelectCutCharData = &iter;
                                m_strSelectCutCharName = str;
                                m_iCutChar_SelectIndex = i;
                            }
                            i++;
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(u8"아래"))
                {
                    if ((m_iCutChar_SelectIndex + 1) < m_CutCharDataVector.size())
                    {
                        CUTCHARDATA tData = m_CutCharDataVector[m_iCutChar_SelectIndex];
                        m_CutCharDataVector[m_iCutChar_SelectIndex] = m_CutCharDataVector[m_iCutChar_SelectIndex + 1];
                        m_CutCharDataVector[m_iCutChar_SelectIndex + 1] = tData;
                        m_pSelectSceneData = &m_CutSceneDataVector[m_iCutChar_SelectIndex + 1];
                        _uint i = 0;
                        for (auto& iter : m_CutCharDataVector)
                        {
                            _float fTime = iter.fCurTime;
                            _string str = CMethod::Combine_Int(i, "");
                            str.append("_Time: ");
                            str.append(CMethod::Convert_Number_S((_double)fTime, 2));

                            if (m_pSelectCutCharData->fCurTime == iter.fCurTime)
                            {
                                m_pSelectCutCharData = &iter;
                                m_strSelectCutCharName = str;
                                m_iCutChar_SelectIndex = i;
                            }
                            i++;
                        }
                    }
                }
                if (ImGui::Button("Delete CutCharData"))
                {
                    for (vector<CUTCHARDATA>::iterator it = m_CutCharDataVector.begin(); it != m_CutCharDataVector.end(); ++it)
                    {
                        if (m_pSelectCutCharData == &(*it))
                        {
                            m_CutCharDataVector.erase(it);
                            m_pSelectCutCharData = nullptr;
                            m_strSelectCutCharName = "REMOVE";
                            m_pSelect_lerpNextCharData = nullptr;
                            m_pSelect_lerpPrevCharData = nullptr;
                            m_strSelect_lerpNextCharName = "REMOVE";
                            m_strSelect_lerpPrevCharName = "REMOVE";
                            ImGui::TreePop();
                            ImGui::TreePop();
                            return;
                        }
                    }
                }
                if (ImGui::Button("Setting Model"))
                {
                    if (nullptr != m_pCutScene_OwnerModel)
                    {
                        CTransform* pTransform = m_pCutScene_OwnerModel->Get_Transform();
                        if (nullptr != pTransform)
                        {
                            pTransform->Set_Pos(m_pSelectCutCharData->vOwnerPosition.Get_Vector());
                            pTransform->Rotate_Turn(m_pSelectCutCharData->vOwnerRotation);
                        }
                    }

                    if (nullptr != m_pCutScene_TargetModel)
                    {
                        CTransform* pTransform = m_pCutScene_TargetModel->Get_Transform();
                        if (nullptr != pTransform)
                        {
                            pTransform->Set_Pos(m_pSelectCutCharData->vTargetPosition.Get_Vector());
                            pTransform->Rotate_Turn(m_pSelectCutCharData->vTargetRotation);
                        }
                    }
                }
                ImGui::Text(m_strSelectCutCharName.c_str());
                ImGui::DragFloat("Prev Char Time", &m_pSelectCutCharData->fPrevTime, 0.f, (_float)m_dTotalTime);
                ImGui::SliderFloat("Cur Char Time Slide", &m_pSelectCutCharData->fCurTime, 0.f, (_float)m_dTotalTime);
                ImGui::DragFloat("Cur Char Time Drag", &m_pSelectCutCharData->fCurTime, 0.f, (_float)m_dTotalTime);
                if (ImGui::TreeNodeEx("OwnerTarget_Show", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("==<Owner>==");
                    ImGui::DragFloat3("Owner Position", &m_pSelectCutCharData->vOwnerPosition.x, 0.1f, -1000.f, 1000.f);
                    ImGui::DragFloat3("Owner Rotation", &m_pSelectCutCharData->vOwnerRotation.x, 0.1f, -1000.f, 1000.f);
                    ImGui::Text("==<Target>==");
                    ImGui::DragFloat3("Target Position", &m_pSelectCutCharData->vTargetPosition.x, 0.1f, -1000.f, 1000.f);
                    ImGui::DragFloat3("Target Rotation", &m_pSelectCutCharData->vTargetRotation.x, 0.1f, -1000.f, 1000.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Speed_Show", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    // Move
                    ImGui::Text("==<Is Move>==");
                    ImGui::Checkbox("OW Move", &m_pSelectCutCharData->bOwnerMove); ImGui::SameLine();
                    ImGui::Checkbox("TG Move", &m_pSelectCutCharData->bTargetMove);
                    ImGui::Text("==<Move Speed>==");
                    ImGui::DragFloat("OW Speed", &m_pSelectCutCharData->fOwnerSpeed, 0.1f, -1000.f, 1000.f);
                    ImGui::DragFloat("TG Speed", &m_pSelectCutCharData->fTargetSpeed, 0.1f, -1000.f, 1000.f);
                    ImGui::Checkbox("OwnerActive", &m_pSelectCutCharData->bOwnerActive); ImGui::SameLine();
                    ImGui::Checkbox("TargetActive", &m_pSelectCutCharData->bTargetActive);
                    ImGui::DragFloat("OwnerDeltaSpeed", &m_pSelectCutCharData->fOwnerDeltaSpeed, 0.05f, -1000.f, 1000.f);
                    ImGui::DragFloat("TargetDeltaSpeed", &m_pSelectCutCharData->fTargetDeltaSpeed, 0.05f, -1000.f, 1000.f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Rotation_Show", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("OwnerRotationUse", &m_pSelectCutCharData->bOwnerRotationUse); ImGui::SameLine();
                    ImGui::Checkbox("TargetRotationUse", &m_pSelectCutCharData->bTargetRotationUse);
                    if (true == m_pSelectCutCharData->bOwnerRotationUse)
                    {
                        ImGui::Text("OwnerRotationInfo");
                        ImGui::DragFloat3("OwnerRotationSpeed", &m_pSelectCutCharData->vOwnerRotationSpeed.x, 0.05f, -100000.f, 100000.f);
                        ImGui::DragFloat3("OwnerRotationDeltaSpeed", &m_pSelectCutCharData->vOwnerRotationDeltaSpeed.x, 0.05f, -100000.f, 100000.f);
                    }
                    if (true == m_pSelectCutCharData->bTargetRotationUse)
                    {
                        ImGui::Text("TaretRotationInfo");
                        ImGui::DragFloat3("TargetRotationSpeed", &m_pSelectCutCharData->vTargetRotationSpeed.x, 0.05f, -100000.f, 100000.f);
                        ImGui::DragFloat3("TargetRotationDeltaSpeed", &m_pSelectCutCharData->vTargetRotationDeltaSpeed.x, 0.05f, -100000.f, 100000.f);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("OwnerTarget_Drawing", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("UseOwnerGlow", &m_pSelectCutCharData->bUseOwnerGlow);
                    ImGui::Checkbox("UseTarggetGlow", &m_pSelectCutCharData->bUseTargetGlow);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Jump_Show", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Char Jump", &m_pSelectCutCharData->bJump);
                    if (true == m_pSelectCutCharData->bJump)
                    {
                        ImGui::DragFloat3("AttackDir", &m_pSelectCutCharData->vAttackDir.x, 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat("Dist", &m_pSelectCutCharData->fDist, 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat("MaxHeight", &m_pSelectCutCharData->fMaxHeight, 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat("MaxHeightRatio", &m_pSelectCutCharData->fMaxHeightRatio, 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat("PlayTime", &m_pSelectCutCharData->fPlayTime, 0.1f, -1000.f, 1000.f);
                        ImGui::DragFloat("JumpDelayTime", &m_pSelectCutCharData->fJumpDelayTime, 0.001f, 0.001f, 1000.f);
                        ImGui::Checkbox("NotResetJumpTarget", &m_pSelectCutCharData->bNotResetJumpTarget);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("AnimSpeed", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("OwnerAnimSpeed", &m_pSelectCutCharData->fOwnerAnimSpeed, 0.001f, 0, 10.f);
                    ImGui::DragFloat("TargetAnimSpeed", &m_pSelectCutCharData->fTargetAnimSpeed, 0.001f, 0, 10.f);
                    ImGui::TreePop();
                }
                ImGui::DragFloat("ControlTotalUpdateSpeed", &m_pSelectCutCharData->fControlTotalUpdateSpeed, 0.1f, 0.f, 1000.f);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("DmgValue"))
    {
        if (ImGui::Button("Add_Dmg"))
        {
            m_CutDmgDataVector.push_back(CUTDMGDATA());
        }

        // Cut Scene List
        if (ImGui::BeginListBox("DmgList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            _uint i = 0;
            for (auto& iter : m_CutDmgDataVector)
            {
                _float fTime = iter.fCurTime;
                _string str = CMethod::Combine_Int(i, "");
                str.append("_Time: ");
                str.append(CMethod::Convert_Number_S((_double)fTime, 3));

                if (ImGui::Selectable(str.c_str()))
                {
                    m_pSelectDmgData = &iter;
                    m_strSelectDmgName = str;
                }
                i++;
            }
            ImGui::EndListBox();
        }

        if (nullptr != m_pSelectDmgData)
        {
            ImGui::SliderFloat("Cur Dmg Time Slide", &m_pSelectDmgData->fCurTime, 0.f, (_float)m_dTotalTime);
            ImGui::DragFloat("Cur Dmg Time Drag", &m_pSelectDmgData->fCurTime, 0.f, (_float)m_dTotalTime);
            ImGui::DragFloat("Dmg Value", &m_pSelectDmgData->fDmgData, 0.f, 100000.f);
        }

        if (ImGui::Button("Arrange"))
        {
            sort(m_CutDmgDataVector.begin(), m_CutDmgDataVector.end(), [&](auto& p1, auto& p2)->bool
            {
                if (p1.fCurTime > p2.fCurTime)
                {
                    return true;
                }
                return false;
            });
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete DmgNode"))
        {
            for (vector<CUTDMGDATA>::iterator it = m_CutDmgDataVector.begin(); it != m_CutDmgDataVector.end(); ++it)
            {
                if (m_pSelectDmgData == &(*it))
                {
                    m_CutDmgDataVector.erase(it);
                    m_pSelectDmgData = nullptr;
                    m_strSelectDmgName = "";
                    ImGui::TreePop();
                    return;
                }
            }
        }

        ImGui::TreePop();
    }
}

#pragma endregion CUTCHARDATA

#pragma region SAVELOAD
void CCutSceneViewer_Tool::SaveLoad_ControlMethod()
{
    // Update Time
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
        {
            ImGui::DragFloat("UpdateTime", &m_fUpdateTime, -0.001f, 1000.f);
            ImGui::DragFloat("LateUpdateTime", &m_fUpdateLateTime, -0.001f, 1000.f);
            ImGui::NewLine();
        }
        RELEASE_INSTANCE(CGameInstance);
    }

    ImGui::Text(u8"SaveAndLoad");
    {
        _char SAVE_TEXT[MAX_PATH] = "";
        strcpy_s(SAVE_TEXT, m_strSaveCutScene.c_str());
        ImGui::InputText("CutSceneSaveName", SAVE_TEXT, MAX_PATH * sizeof(_char));
        m_strSaveCutScene = SAVE_TEXT;
    }
    if (ImGui::Button("Save Cut Scene Data"))
    {
        Save_CutSceneData(L"..\\..\\Resources\\CutScene\\Data");
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Cut Scene AfterData"))
    {
        Save_CutSceneData(L"..\\..\\Resources\\CutScene\\Data\\Save");
    }
    {
        _char LOAD_TEXT[MAX_PATH] = "";
        strcpy_s(LOAD_TEXT, m_strLoadCutScene.c_str());
        ImGui::InputText("CutSceneLoadName", LOAD_TEXT, MAX_PATH * sizeof(_char));
        m_strLoadCutScene = LOAD_TEXT;
    }
    if (ImGui::Button("Load Cut Scene Data"))
    {
        Load_CutSceneData();
    }
    ImGui::Text("InputTrigger_Name");
    {
        _char INPUT_TEXT[MAX_PATH] = "";
        strcpy_s(INPUT_TEXT, m_strInputTrigger.c_str());
        ImGui::InputText("InputTrigger", INPUT_TEXT, MAX_PATH * sizeof(_char));
        m_strInputTrigger = INPUT_TEXT;
    }
    ImGui::NewLine();
    if (ImGui::Button("Reset Data")) { Reset_Tool(); } ImGui::SameLine();
    if (ImGui::Button("All Convert Save")) { All_Save(); }
    ImGui::NewLine();
}
void CCutSceneViewer_Tool::Save_CutSceneData(const _wstring& _Path)
{
    const _tchar* PATH = _Path.c_str();
    _wmkdir(PATH);

    _wstring str = PATH;
    str.append(L"\\");
    str.append(CMethod::ConvertSToW(m_strSaveCutScene));
    str.append(L".bin");

    HANDLE hFile = CMethod::Begin_Save(str.c_str());
    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte = 0;
    WriteFile(hFile, &m_dTotalTime, sizeof(_double), &dwByte, nullptr);
    WriteFile(hFile, &m_fUpdateTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_fUpdateLateTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_bRootBone_Owner, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_bRootBone_Target, sizeof(_bool), &dwByte, nullptr);
    if (nullptr != m_pCutScene_OwnerModel)
        CMethod::Save_File_WString(hFile, dwByte, m_pCutScene_OwnerModel->Get_FileData()->strFileName);

    CMethod::Save_File_String(hFile, dwByte, m_strInputTrigger);

    _uint iSize = (_uint)m_CutSceneDataVector.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (auto& iter : m_CutSceneDataVector)
    {
        // ========================================================
        WriteFile(hFile, &iter, sizeof(CUTSCENEDATA), &dwByte, nullptr);
    }
    iSize = (_uint)m_CutCharDataVector.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    // Dg Scene Data 
    for (auto& iter : m_CutCharDataVector)
    {
        WriteFile(hFile, &iter, sizeof(CUTCHARDATA), &dwByte, nullptr);
    }

    iSize = (_uint)m_CutDmgDataVector.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    // Dg Scene Data 
    for (auto& iter : m_CutDmgDataVector)
    {
        WriteFile(hFile, &iter, sizeof(CUTDMGDATA), &dwByte, nullptr);
    }

    iSize = (_uint)m_CutSceneTexVector.size();
    WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (auto& iter : m_CutSceneTexVector)
        iter->Save_CutSceneTex(hFile, &dwByte);

    if (m_CutSceneTexVector.size() > 0)
    {
        FILEGROUP* pFileGroup = m_TextureVector[m_iCutScene_Texture_Index]->Get_FileGroup();
        if(nullptr != pFileGroup)
            CMethod::Save_File_WString(hFile, dwByte, pFileGroup->strPath);
    }

    if (nullptr != m_pCutScene_OwnerModel)
        m_pCutScene_OwnerModel->Save(hFile, &dwByte);

    if (nullptr != m_pCutScene_TargetModel)
        m_pCutScene_TargetModel->Save(hFile, &dwByte);

    CloseHandle(hFile);
}

void CCutSceneViewer_Tool::Load_CutSceneData()
{
    const _tchar* PATH = L"..\\..\\Resources\\CutScene\\Data";

    _wstring str = PATH;
    str.append(L"\\");
    str.append(CMethod::ConvertSToW(m_strLoadCutScene));
    str.append(L".bin");

    HANDLE hFile = CMethod::Begin_Load(str.c_str());
    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte = 0;
    ReadFile(hFile, &m_dTotalTime, sizeof(_double), &dwByte, nullptr);
    ReadFile(hFile, &m_fUpdateTime, sizeof(_float), &dwByte, nullptr);
    ReadFile(hFile, &m_fUpdateLateTime, sizeof(_float), &dwByte, nullptr);
    ReadFile(hFile, &m_bRootBone_Owner, sizeof(_bool), &dwByte, nullptr);
    ReadFile(hFile, &m_bRootBone_Target, sizeof(_bool), &dwByte, nullptr);
    str = CMethod::Load_File_WString(hFile, dwByte);

    for (auto& iter : m_CutSceneModelList)
    {
        if (iter->Get_FileData()->strFileName == str)
            m_pCutScene_OwnerModel = iter;
    }

    m_strInputTrigger = CMethod::Load_File_String(hFile, dwByte);

    _uint iSize = 0;
    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iSize; ++i)
    {
        CUTSCENEDATA tData;
        // ========================================================
        ReadFile(hFile, &tData, sizeof(CUTSCENEDATA), &dwByte, nullptr);
        m_CutSceneDataVector.push_back(tData);
        if (0.f == tData.fPrevDistance)
            CalCulate_CamLoadLength(i);
    }

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iSize; ++i)
    {
        CUTCHARDATA tData;
        ReadFile(hFile, &tData, sizeof(CUTCHARDATA), &dwByte, nullptr);

        if (!(tData.fControlTotalUpdateSpeed > 0.f && tData.fControlTotalUpdateSpeed < 1.f))
        {
            tData.fControlTotalUpdateSpeed = 1.f;
        }

        m_CutCharDataVector.push_back(tData);
    }

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    // Dg Scene Data 
    for (_uint i = 0; i < iSize; ++i)
    {
        CUTDMGDATA tData;
        ReadFile(hFile, &tData, sizeof(CUTDMGDATA), &dwByte, nullptr);
        m_CutDmgDataVector.push_back(tData);
    }

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iSize; ++i)
    {
        CCutSceneTex_Tool* pInstance = CCutSceneTex_Tool::Create(m_pDevice, m_pContext);
        pInstance->Load_CutSceneTex(hFile, &dwByte);
        m_CutSceneTexVector.push_back(pInstance);
    }

    if (m_CutSceneTexVector.size() > 0)
    {
           CMethod::Load_File_WString(hFile, dwByte);
    }

    if (nullptr != m_pCutScene_OwnerModel)
        m_pCutScene_OwnerModel->Load(hFile, &dwByte);

    if (nullptr != m_pCutScene_TargetModel)
        m_pCutScene_TargetModel->Load(hFile, &dwByte);
    else
    {
        if (nullptr != m_pCutScene_OwnerModel)
        {
            if (m_pCutScene_OwnerModel->Get_FileData()->strFileName == L"Sabito")
            {
                for (auto& iter : m_CutSceneModelList)
                {
                    if (iter->Get_FileData()->strFileName == L"Zenitsu")
                        m_pCutScene_TargetModel = iter;
                }
            }
            else
            {
                for (auto& iter : m_CutSceneModelList)
                {
                    if (iter->Get_FileData()->strFileName == L"Sabito")
                        m_pCutScene_TargetModel = iter;
                }
            }
            if (nullptr != m_pCutScene_TargetModel)
                m_pCutScene_TargetModel->Load(hFile, &dwByte);
        }
    }
    m_bPlayCutScene = false;


    if (nullptr != m_pCutScene_OwnerModel)
    {
        m_pCutScene_OwnerModel->Set_Active(true);
        m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->bInitStart = false;
    }

    if (nullptr != m_pCutScene_TargetModel)
    {
        m_pCutScene_TargetModel->Set_Active(true);
        m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->bInitStart = false;
    }

    if (m_CutCharDataVector.size() > 0)
    {
        if (nullptr != m_pCutScene_OwnerModel)
        {
            CTransform* pOwnerTransform = m_pCutScene_OwnerModel->Get_Transform();
            if (nullptr != pOwnerTransform)
            {
                pOwnerTransform->Set_Pos(m_CutCharDataVector.front().vOwnerPosition.Get_Vector());
                pOwnerTransform->Rotate_Turn(m_CutCharDataVector.front().vOwnerRotation.Get_Vector());
            }
        }

        if (nullptr != m_pCutScene_TargetModel)
        {
            CTransform* pTargetTransform = m_pCutScene_TargetModel->Get_Transform();
            if (nullptr != pTargetTransform)
            {
                pTargetTransform->Set_Pos(m_CutCharDataVector.front().vTargetPosition.Get_Vector());
                pTargetTransform->Rotate_Turn(m_CutCharDataVector.front().vTargetRotation.Get_Vector());
            }
        }
    }

    if (m_CutSceneDataVector.size() > 0)
    {
        if (nullptr != m_pCamera_Tool)
        {
            CTransform* pCamTransform = m_pCamera_Tool->Get_Transform();
            if (nullptr != pCamTransform)
            {
                pCamTransform->Set_Pos(m_CutSceneDataVector.front().vCamPosition.Get_Vector());
                pCamTransform->Rotate_Turn(m_CutSceneDataVector.front().vCamRotation);
            }
        }
    }

    CloseHandle(hFile);
}

void CCutSceneViewer_Tool::Reset_Tool()
{
    // Cut Scene Model 
    if (nullptr != m_pCutScene_OwnerModel)
    {
        m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->Reset_All();
        m_pCutScene_OwnerModel->Set_Active(false);
    }
    if (nullptr != m_pCutScene_TargetModel)
    {
        m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Reset_All();
        m_pCutScene_TargetModel->Set_Active(false);
    }

    m_pCutScene_OwnerModel = nullptr;
    m_pCutScene_TargetModel = nullptr;

    for (auto& iter : m_CutSceneTexVector)
        Safe_Release(iter);

    for (auto& iter : m_CutSceneEffectVector)
        Safe_Release(iter);

    m_fUpdateTime = 1.f; m_fUpdateLateTime = 1.f;

    m_fCurrentCharTime = 0.f;
    m_bPlayCutChar = false;
    // Cut Scene Texture Index;
    m_iCutSceneTextureIndex = 0; m_iCutSceneEffectIndex = 0;
    // bool Make Texure
    m_bMakeTexture = false; m_bMakeEffect = false;
    // Change Camera
    m_bChangeCamera = false;
    // Camera Rotation
    m_vCameraRotation = _float3(0.f, 0.f, 0.f);
    // Scene Data
    m_CutSceneDataVector.clear();  m_CutCharDataVector.clear();
    m_dTotalTime = 0.f;  m_fDragCamCurTime = 0.f;
    m_iCutScene_Index = 0; m_iCutScene_SelectIndex = 0;
    m_iCutChar_Index = 0; m_iCutChar_SelectIndex = 0;
    m_strSelectSceneName = ""; m_strSelectCutCharName = "";
    m_bPlayCutChar = false;  m_bPlayCutScene = false;
    m_fCurrentCharTime = 0.f; m_fCurrentCamTime = 0.f;
    m_fMultipleValue = 1.f;
    m_bRender_Restrict = false;
    m_v3OwnerPosition = _float3(0.f, 0.f, 0.f);   m_v3TargetPosition = _float3(0.f, 0.f, 0.f);
    m_bIsPicking = false;
    m_iSelectTarget = 0;
    m_strInputTrigger;
    m_bTexture_Render_DebugMode = false;
    m_bRender_Spline = true;   m_bRender_Bezier = true;
    m_strSelectEffectName = L"";
    m_iBindCutScene_CameraPosAndRotation_Index = 0;   m_bControl_PlayAnimation = false;
    m_bModelActive = true;  m_bUseLerp = false;   m_bAllCamLerp = false;
    m_bUseCharLerp = false;  m_bAllCharLerp = false;
    m_pSelect_lerpPrevSceneData = nullptr;  m_pSelect_lerpNextSceneData = nullptr;
    m_pSelect_lerpPrevCharData = nullptr;  m_pSelect_lerpNextCharData = nullptr;
    m_fCamLerpAll = 0.5f;   m_fCharLerpAll = 0.f;
    m_fLerpCamPosRatio = 0.5f;   m_fLerpCamRotRatio = 0.5f;
    m_fLerpOwnerPosRatio = 0.5f;   m_fLerpOwnerRotRatio = 0.5f;   m_fLerpTargetPosRatio = 0.5f;   m_fLerpTargetRotRatio = 0.5f;
    m_fLerpCamSpeedRatio = 0.5f;  m_fLerpOwnerSpeedRatio = 0.5f;
    m_fLerpTargetSpeedRatio = 0.5f;   m_fLerpSplineSpeedRatio = 0.5f;  m_fLerpBezierSpeedRatio = 0.5f;
    m_strSelect_lerpPrevSceneName = "";   m_strSelect_lerpNextSceneName = "";
    m_strSelect_lerpPrevCharName = "";    m_strSelect_lerpNextCharName = "";
    m_fLerpAddValue = 0.f;   m_fMoveSplineValue = 0.f;
    m_fMoveBezierValue = 0.f; m_tMoveShakeTimer = CUSTIMER();
    m_tRotShakeTimer = CUSTIMER();    m_pSelectCutCharData = nullptr;
    m_bCutCharData = false;
    m_bPass = false;
    m_bJumpEnd = false;
    m_dFallTimeAcc = 0.0;
    m_fDragCharCurTime = 0.f;
    m_bRootBone_Owner = false;
    m_strSelectDmgName = "";
    m_pSelectDmgData = nullptr;
    m_CutDmgDataVector.clear();
}
void CCutSceneViewer_Tool::All_Save()
{
    CGameInstance* pFileManager = GET_INSTANCE(CGameInstance);
    {
        PATHS paths = PATHS{ L"CutScene", L"Data" };
        FILEGROUP* pFileGroup = pFileManager->Find_Folder(&paths);
        if (nullptr != pFileGroup)
        {
            for (auto& iter : pFileGroup->vecFileDataList)
            {
                m_strLoadCutScene = CMethod::ConvertWToS(iter.strFileName);
                m_strSaveCutScene = CMethod::ConvertWToS(iter.strFileName);
                Load_CutSceneData();
                Save_CutSceneData(L"..\\..\\Resources\\CutScene\\Data");
                Reset_Tool();
            }
        }

        m_strLoadCutScene = "";
        m_strSaveCutScene = "";
    }
    RELEASE_INSTANCE(CGameInstance);
}
#pragma endregion SAVELOAD

// 디버그용 그리는 함수
void CCutSceneViewer_Tool::Draw_Value(CGameInstance* _pGameInstance, const _uint& _iType, const _float3& _v3Size,
    const _float3& _v3Pos, const _float4& _v3Rot)
{
    if (_iType == DR_CUTVIEWER_CAMERA)
    {
        _float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
        BoundingOrientedBox bBox1 = BoundingOrientedBox(_v3Pos, _v3Size, _v3Rot);
        _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, vColor.Get_Vector());
    }
    else if (_iType == DR_EFFECT_POSITION)
    {
        BoundingBox bBox1 = BoundingBox(_v3Pos, _v3Size);
        _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D);
    }
    else if (_iType == DR_CUTVIEWER_MODEL_OWNER)
    {
        _float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
        BoundingSphere bBox1 = BoundingSphere();
        bBox1.Center = _v3Pos;
        bBox1.Radius = _v3Size.x;
        _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, vColor.Get_Vector());
    }
    else
    {
        _float4 vColor = _float4(0.f, 1.f, 1.f, 1.f);
        BoundingSphere bBox1 = BoundingSphere();
        bBox1.Center = _v3Pos;
        bBox1.Radius = _v3Size.x;
        _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, vColor.Get_Vector());
    }
}

// 디버그용 그리는 함수
void CCutSceneViewer_Tool::Draw_Value_Model(CGameInstance* _pGameInstance, const _float3& _v3Size, const _float3& _v3Pos, const _float4& _vColor)
{
    BoundingBox bBox1 = BoundingBox(_v3Pos, _v3Size);
    _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, XMLoadFloat4(&_vColor));
}

void CCutSceneViewer_Tool::Draw_CameraLine(CGameInstance* _pGameInstance, _float3 _vPrevIndex, _float3 _vNextIndex, const _float4& _vColor)
{
    _pGameInstance->DrawLine(_vPrevIndex.Get_Vector(), _vNextIndex.Get_Vector(), CDrawing::VIEW_3D, XMLoadFloat4(&_vColor));
}

void CCutSceneViewer_Tool::Draw_CameraLine(CGameInstance* _pGameInstance, _fvector _vPrevIndex, _fvector _vNextIndex, const _float4& _vColor)
{
    _pGameInstance->DrawLine(_vPrevIndex, _vNextIndex, CDrawing::VIEW_3D, XMLoadFloat4(&_vColor));
}

void CCutSceneViewer_Tool::Reset_CharData()
{
    if (m_CutCharDataVector.size() <= 0)
        return;
    m_iCutChar_Index = 0;
    m_fCurrentCharTime = 0.f;
    m_iJump_CheckCharIndex = -1;
    m_fOwnerDeltaSpeed = 0.f;
    m_fTargetDeltaSpeed = 0.f;
    m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
    m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
    if (nullptr != m_pCutScene_OwnerModel)
    {
        m_pCutScene_OwnerModel->Set_PlayCutScene(false);
        m_pCutScene_OwnerModel->Get_CutSceneAnimDesc()->Reset();

        m_pCutScene_OwnerModel->Get_Transform()->Set_Pos(m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition.Get_Vector());
        m_pCutScene_OwnerModel->Get_Transform()->Rotate_Turn(m_CutCharDataVector[m_iCutChar_Index].vOwnerRotation);
    }

    if (nullptr != m_pCutScene_TargetModel)
    {
        m_pCutScene_TargetModel->Set_PlayCutScene(false);
        m_pCutScene_TargetModel->Get_CutSceneAnimDesc()->Reset();

        m_pCutScene_TargetModel->Get_Transform()->Set_Pos(m_CutCharDataVector[m_iCutChar_Index].vTargetPosition.Get_Vector());
        m_pCutScene_TargetModel->Get_Transform()->Rotate_Turn(m_CutCharDataVector[m_iCutChar_Index].vTargetRotation);
    }
    _uint iIndex = 0;
    for (auto& iter : m_CutCharDataVector)
        CalCulate_CamLoadLength(++iIndex);

    m_bPlayCutChar = false;
    m_bJumpEnd = false;
    m_bPass = false;
}

// 초기화 함수
void CCutSceneViewer_Tool::Reset_CamData()
{
    if (m_CutSceneDataVector.size() <= 0)
        return;
    m_fLerpAddValue = 0.f;
    m_fMoveSplineValue = 0.f;
    m_fMoveBezierValue = 0.f;
    m_iCutChar_Index = 0;
    m_dFallTimeAcc = 0.0;
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        pGameInstance->Set_Current_FrameValue(60);
    }
    RELEASE_INSTANCE(CGameInstance);
    m_bPlayCutScene = false;
    m_pCamera_Tool->Set_CutSceneMode(false);
    m_iCutScene_Index = 0;
    m_fCurrentCamTime = 0.f;
    m_iCutScene_Texture_Index = 0;
    m_iCutSceneTextureIndex = 0;
    m_iCutSceneEffectIndex = 0;
    m_fCamDeltaSpeed = 0.f;

    if (nullptr != m_pCamera_Tool)
    {
        CTransform* pTransform = m_pCamera_Tool->Get_Transform();
        if (nullptr != pTransform)
        {
            pTransform->Set_Pos(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Get_Vector());
            pTransform->Rotate_Turn(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation);
        }
    }
}


CCutSceneViewer_Tool* CCutSceneViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCutSceneViewer_Tool* pInstance = new CCutSceneViewer_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCutSceneViewer::Create To Failed");
#endif
    }
    return pInstance;
}

void CCutSceneViewer_Tool::Free()
{
    // Release Terrain
    {
        Safe_Release(m_pTerrain_ShaderCom);
        Safe_Release(m_pTerrain_TransformComp);
        Safe_Release(m_pVIBuffer_TerrainData);
    }
    // Cut Scene Texture
    {
        for (auto& iter : m_TextureVector)
            Safe_Release(iter);

        for (auto& iter : m_CutSceneTexVector)
            Safe_Release(iter);

        // Cut Scene Shader, Texture
        Safe_Release(m_pCutScene_Shader);
        Safe_Release(m_VIBuffer_CutSceneTexture);
    }

    for (auto& iter : m_CutSceneEffectVector)
        Safe_Release(iter);

    // Cut Scene Model
    for (auto& iter : m_CutSceneModelList)
        Safe_Release(iter);

    // Camera Free
    Safe_Release(m_pCamera_Tool);
    Safe_Release(m_pCamera_Free_Tool);

    for (auto& iter : m_ItemModelList)
        Safe_Release(iter);

    Safe_Release(m_pOwnerSword);
    Safe_Release(m_pTargetSword);

    __super::Free();
}
