#include "ShaderView.h"

#include "Camera.h"
#include "Camera_Manager.h"
#include "GameInstance.h"
#include <sstream>
#include "Debug_RenderObj.h"

CShaderView::CShaderView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass(_pDevice, _pContext)
{
}

HRESULT CShaderView::NativeConstruct()
{
    m_tWinDesc = DOCKDESC("ShaderView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(1.f, 1.f), ImVec2(620.f, 620.f), true);
    m_tRTDesc = DOCKDESC("RenderTarget", ImGuiWindowFlags_NoFocusOnAppearing);
    m_tLightDesc = DOCKDESC("LightControl", ImGuiWindowFlags_NoFocusOnAppearing);
    m_tDebugDesc = DOCKDESC("DebugRenderer", ImGuiWindowFlags_NoFocusOnAppearing);

    m_pName = "Shader_View";

    m_pDebugRenderObj = CDebug_RenderObj::Create(m_pDevice, m_pContext);
    if (Is_Nullptr(m_pDebugRenderObj))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView::NativeConstruct_InnerValue()
{
    return S_OK;
}

HRESULT CShaderView::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView::Late_Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Late_Tick(_dDeltaSeconds)))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CShaderView::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView::Release_InnerValue()
{

    return S_OK;
}

void CShaderView::Show_Window()
{


    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tWinDesc.v2Pos.x, 5.f + imVec2.y + m_tWinDesc.v2Pos.y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);

    ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable, m_tWinDesc.imGuiWindowFlags);
    {
        {
            m_tWinDesc.iID = ImGui::GetID(m_tWinDesc.strName.c_str());
            if (false == m_bInitSetting)
            {
                ImGui::DockBuilderRemoveNode(m_tWinDesc.iID); // Clear out existing layout
                ImGui::DockBuilderAddNode(m_tWinDesc.iID, 0); // Add empty node

                ImGuiID dock_main_id = m_tWinDesc.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                ImGuiID dock_id_RT = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);
                ImGuiID dock_id_Light = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);
                ImGuiID dock_id_Debug = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);

                m_tRTDesc.iID = dock_id_RT;
                m_tLightDesc.iID = dock_id_Light;
                m_tDebugDesc.iID = dock_id_Debug;

                // Perspective , Heirecky
                ImGui::DockBuilderDockWindow(m_tRTDesc.strName.c_str(), dock_id_RT);
                ImGui::DockBuilderDockWindow(m_tLightDesc.strName.c_str(), dock_id_Light);
                ImGui::DockBuilderDockWindow(m_tDebugDesc.strName.c_str(), dock_id_Debug);
                ImGui::DockBuilderFinish(m_tWinDesc.iID);

                m_bInitSetting = true;
            }
            ImGui::DockSpace(m_tWinDesc.iID);
        }

        {
            ImGui::Begin(m_tRTDesc.strName.c_str(), &m_tRTDesc.bEnable, m_tRTDesc.imGuiWindowFlags);
            {
                CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
                {
                    pInstance->Out_RenderTarget_Texture(&m_RenderTarget_TextureList);
                }
                RELEASE_INSTANCE(CGameInstance);

                Show_RenderTargetImg();
            }
            ImGui::End();
        }
        {
            ImGui::Begin(m_tLightDesc.strName.c_str(), &m_tLightDesc.bEnable, m_tLightDesc.imGuiWindowFlags);
            {
                LightControlMode();
            }
            ImGui::End();
        }
        {
            ImGui::Begin(m_tDebugDesc.strName.c_str(), &m_tDebugDesc.bEnable, m_tDebugDesc.imGuiWindowFlags);
            {
                m_pDebugRenderObj->Extern_ImGuiClass();
            }
            ImGui::End();
        }
    }
    ImGui::End();

    m_RenderTarget_TextureList.clear();
}

void CShaderView::Show_RenderTargetImg()
{
    ImGui::Checkbox("SelectMode", &m_bSelectMode);
    if (false == m_bSelectMode)
    {
        int i = 0;
        for (auto& iter : m_RenderTarget_TextureList)
        {
            _string str = CMethod::ConvertWToS(iter.first);
            ID3D11ShaderResourceView* pView = iter.second;

            ImGui::Text(str.c_str());
            ImGui::Image((void*)pView, ImVec2(g_iWinSizeX /2.f, g_iWinSizeY / 2.f));
            ++i;
        }
    }
    else
    {
        for (auto& iter : m_RenderTarget_TextureList)
        {
            _string str = CMethod::ConvertWToS(iter.first);
            if (ImGui::Selectable(str.c_str()))
            {
                m_strSelectImageName = iter.first;
            }
        }

        if (lstrcmp(L"", m_strSelectImageName.c_str()))
        {
            // 쉐이더 리소스 뷰를 가져옴 
            ID3D11ShaderResourceView* pView = m_RenderTarget_TextureList[m_strSelectImageName.c_str()];
            ImGui::NewLine();
            _string str = CMethod::ConvertWToS(m_strSelectImageName);
            ImGui::Text(str.c_str());
            ImGui::Begin(m_tWinDesc.strName.c_str(), &m_bSelectMode, ImGuiWindowFlags_AlwaysAutoResize);
            {
                ImVec2 ScreenSize = ImVec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);

                _float2 v2MousePosition;

                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
                {
                    v2MousePosition = pGameInstance->Get_MousePosition();
                }
                RELEASE_INSTANCE(CGameInstance);

                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImGui::Image((void*)pView, ScreenSize);
                ImGuiIO io = ImGui::GetIO();

                if (pos.x <= io.MousePos.x && io.MousePos.x <= pos.x + g_iWinSizeX)
                {
                    if (pos.y <= io.MousePos.y && io.MousePos.y <= pos.y + g_iWinSizeY)
                    {
                        v2MousePosition.x = io.MousePos.x - pos.x;
                        v2MousePosition.y = io.MousePos.y - pos.y;

                        if (v2MousePosition.x >= 0 && v2MousePosition.y >= 0)
                        {
                            ID3D11Resource* pResoure = nullptr;
                            pView->GetResource(&pResoure);

                            _wstring PATH = L"..\\Bin\\";
                            PATH.append(m_strSelectImageName);
                            PATH.append(L".dds");
                            // Save DDS Texture
                            SaveDDSTextureToFile(m_pContext, pResoure, PATH.c_str());

                            _ulong			dwByte = 0;
                            HANDLE			hFile = CreateFile(PATH.c_str(),
                                GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                            if (0 != hFile)
                            {
                                BITMAPFILEHEADER		fh;
                                BITMAPINFOHEADER		ih;

                                ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
                                ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

                                m_pPixelArr = new _ulong[g_iWinSizeX * g_iWinSizeY];
                                ReadFile(hFile, m_pPixelArr, sizeof(_ulong) * g_iWinSizeX * g_iWinSizeY, &dwByte, nullptr);
                                CloseHandle(hFile);
                                _uint iIndex = (_uint)((g_iWinSizeX * v2MousePosition.y + v2MousePosition.x));


                                if (iIndex < g_iWinSizeX * g_iWinSizeY)
                                    m_PixelList.push_back(m_pPixelArr[iIndex]);
                            }

                            Safe_Release_DV(pResoure);
                        }
                    }
                }
            }
            ImGui::End();

            if (m_PixelList.size() > 0)
            {
                _ulong RGBValue = m_PixelList.front();
                CUSRGB tRgb = CMethod::HexToRGB(RGBValue);

                _string RString = CMethod::Convert_Number_S((_uint)tRgb.r);
                _string GString = CMethod::Convert_Number_S((_uint)tRgb.g);
                _string BString = CMethod::Convert_Number_S((_uint)tRgb.b);
                _string AString = CMethod::Convert_Number_S((_uint)tRgb.a);

                _char FILE[MAX_PATH] = {};
                sprintf_s(FILE, "%#x", m_PixelList.front());

                ImGui::Text(FILE);

                //        _string AString = CMethod::Convert_Number_S((_uint)tRgb.a);
                _string Text = "R: ";
                Text.append(RString);
                Text.append(", G: ");
                Text.append(AString);
                Text.append(", B: ");
                Text.append(BString);
                Text.append(", A: ");
                Text.append(GString);
                ImGui::Text(Text.c_str());

                m_PixelList.clear();
            }
            Safe_Delete_Array(m_pPixelArr);
        }
    }
}

void CShaderView::LightControlMode()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        if (ImGui::BeginTabBar("Light"))
        {
            if (ImGui::BeginTabItem("Shadow"))
            {
                CCamera_Manager* pCameraInstance = GET_INSTANCE(CCamera_Manager);
                {
                    if (ImGui::Checkbox(u8"그림자 카메라", &m_bChangeToShadowCam))
                    {
                        if (m_bChangeToShadowCam)
                            pCameraInstance->Set_ActiveCamera(CM_CAMERA_LIGHT);
                        else
                            pCameraInstance->Set_ActiveCamera(CM_CAMERA_FREE);
                    }

                    CCamera* pActiveCamera = pCameraInstance->Get_ActiveCamera();
                    if (nullptr != pActiveCamera)
                    {
                        CCamera::CAMDESC* pDesc = nullptr;
                        pActiveCamera->Out_CameraDesc(pDesc);

                        if (nullptr != pDesc)
                        {
                            ImGui::DragFloat("Cam Speed", &pDesc->fSpeed, 0.f, 1000.f);

                            _float fRot = XMConvertToDegrees(pDesc->fRotSpeed);
                            ImGui::DragFloat("Cam RotSpeed", &fRot, 0.f, 1000.f);
                            pDesc->fRotSpeed = XMConvertToRadians(fRot);

                            _float3 vCamPos = pActiveCamera->Get_Transform()->Get_PosF3();
                            ImGui::DragFloat3("Cam Position", &vCamPos.x, 0.f);

                            _float3 vCamLook = pActiveCamera->Get_Transform()->Get_LookF3();
                            ImGui::DragFloat3("Cam Look", &vCamLook.x, 0.f);

                            ImGui::DragFloat("Cam Far", &pDesc->fFar, 0.f);

                        }
                    }
                }
                RELEASE_INSTANCE(CCamera_Manager);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Light"))
            {
                _uint iLightNum = pGameInstance->Get_LightsNum();

                vector<_string> LightList;
                for (_uint i = 0; i < iLightNum; i++)
                {
                    _string str = "Light";
                    str += to_string(i);
                    LightList.push_back(str);
                }

                if (ImGui::BeginListBox("LightList"))
                {
                    _uint iIndex = 0;
                    for (auto& iter : LightList)
                    {
                        const bool is_selected = (m_iLightIndex == iIndex);
                        if (ImGui::Selectable(iter.c_str(), is_selected))
                        {
                            m_iLightIndex = iIndex;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        iIndex++;
                    }
                    ImGui::EndListBox();
                }

                // 선택한 라이트 정보 띄우고 변경해주기
                {
                    const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(m_iLightIndex);
                    if (nullptr != pLightDesc)
                    {
                        m_CurLightDesc = *pLightDesc;

                        ImGui::Checkbox(u8"라이트 업데이트", &m_bUpdateLight);

                        const char* LightType[] = { "TYPE_DIRECTIONAL" , "TYPE_POINT" , "TYPE_SPOT" };
                        _int iType = m_CurLightDesc.eLightType;
                        ImGui::Combo("RENDERGRUOP", &iType, LightType, IM_ARRAYSIZE(LightType));
                        m_CurLightDesc.eLightType = (LIGHTDESC::TYPE)iType;

                        ImGui::DragFloat3("Light Direction", &m_CurLightDesc.vDirection.x, 0.01f);
                        ImGui::DragFloat3("Light Position", &m_CurLightDesc.vPosition.x);
                        ImGui::DragFloat("Light Range", &m_CurLightDesc.fRange, 0.1f);
                        ImGui::DragFloat4("Light Diffuse", &m_CurLightDesc.vDiffuse.x, 0.01f, 0.f, 1.f);
                        ImGui::DragFloat4("Light Ambient", &m_CurLightDesc.vAmbient.x, 0.01f, 0.f, 1.f);
                        ImGui::DragFloat4("Light Specular", &m_CurLightDesc.vSpecular.x, 0.01f, 0.f, 1.f);

                        if (true == m_bUpdateLight)
                        {
                            // 포인트 라이트의 경우 현재 카메라 위치로 위치 변환 가능
                            if (LIGHTDESC::TYPE_POINT == m_CurLightDesc.eLightType)
                            {
                                if (ImGui::Button("Update Position"))
                                {
                                    _vector vCamPos = pGameInstance->Get_CamPosition();
                                    m_CurLightDesc.vPosition = vCamPos;
                                }
                            }
                            pGameInstance->Modify_Light(m_CurLightDesc, m_iLightIndex);
                        }
                    }

                    if (ImGui::Button("Create PointLight"))
                    {
                        _vector vCamPos = pGameInstance->Get_CamPosition();

                        LIGHTDESC tLightDesc;
                        ZeroMemory(&tLightDesc, sizeof(&tLightDesc));

                        tLightDesc.eLightType = LIGHTDESC::TYPE_POINT;
                        tLightDesc.vPosition = vCamPos;
                        tLightDesc.fRange = 1.f;
                        tLightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
                        tLightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
                        tLightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

                        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, tLightDesc, nullptr)))
                            MSG_BOX("Failed To Create PointLight");
                    } ImGui::SameLine();

                    if (ImGui::Button("Delete Light"))
                    {
                        if (FAILED(pGameInstance->Delete_Light(m_iLightIndex)))
                            MSG_BOX("Failed To Delete Light");
                    }

                    ImGui::PushItemWidth(150.f);
                    ImGui::InputText("SaveName", m_szLightSaveName, sizeof(char)* MAX_PATH);
                    ImGui::PopItemWidth();

                    if (ImGui::Button("Save"))
                    {
                        _wstring strName = CMethod::ConvertSToW(m_szLightSaveName);
                        if (FAILED(pGameInstance->Save_LightsData(strName)))
                            MSG_BOX("Failed To Save : Light");
                    }

                    ImGui::PushItemWidth(150.f);
                    ImGui::InputText("LoadName", m_szLightLoadName, sizeof(char) * MAX_PATH);
                    ImGui::PopItemWidth();

                    if (ImGui::Button("Load"))
                    {
                        _wstring strName = CMethod::ConvertSToW(m_szLightLoadName);
                        if (FAILED(pGameInstance->Load_LightsData(strName, m_pDevice, m_pContext)))
                            MSG_BOX("Failed To Load : Light");
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
       
    }
    RELEASE_INSTANCE(CGameInstance);
}

CShaderView* CShaderView::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CShaderView* pInstance = new CShaderView(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CShaderView::Create To Failed");
#endif
    }
    return pInstance;
}

void CShaderView::Free()
{
    Safe_Release(m_pDebugRenderObj);

    __super::Free();
}
