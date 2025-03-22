#include "ShaderView_Tool.h"
#include "GameInstance.h"
#include "Debug_RenderObj_Tool.h"
#include <sstream>

CShaderView_Tool::CShaderView_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CShaderView_Tool::NativeConstruct()
{
    m_tWinDesc = WINDESC("ShaderView", ImGuiWindowFlags_MenuBar,
        ImVec2(1.f, 1.f), ImVec2(620.f, 620.f), true);

    m_pName = "Shader_View";

    m_pDebugRenderObj = CDebug_RenderObj_Tool::Create(m_pDevice, m_pContext);
    if (Is_Nullptr(m_pDebugRenderObj))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView_Tool::NativeConstruct_InnerValue()
{
    return S_OK;
}

HRESULT CShaderView_Tool::Tick(const _double& _dDeltaSeconds)
{
    if (nullptr != m_pDebugRenderObj)
        m_pDebugRenderObj->Tick(_dDeltaSeconds);

    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView_Tool::Late_Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Late_Tick(_dDeltaSeconds)))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CShaderView_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderView_Tool::Release_InnerValue()
{

    return S_OK;
}

void CShaderView_Tool::Show_Window()
{
    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tWinDesc.v2Pos.x, 5.f + imVec2.y + m_tWinDesc.v2Pos.y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);
    ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable);
    {
        const char* pArr[] = {  "Show_RenderTarget" , "LightControlMode", "Render_Debug" };

        ImGui::Combo(u8"모드 선택", &m_iModeSelect, pArr, 3);

        if (0 == m_iModeSelect)
        {
            CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
            {
                pInstance->Out_RenderTarget_Texture(&m_RenderTarget_TextureList);
            }
            RELEASE_INSTANCE(CGameInstance);

            Show_RenderTargetImg();
        }
        else if (1 == m_iModeSelect)
        {
            LightControlMode();
        }
        else if (2 == m_iModeSelect)
        {
            ImGui::Text("Debug_Obj");
            ImGui::NewLine();
            m_pDebugRenderObj->Extern_ImGuiClass();
        }
    }
    ImGui::End();

    m_RenderTarget_TextureList.clear();
}

void CShaderView_Tool::Show_RenderTargetImg()
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

void CShaderView_Tool::LightControlMode()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        if (false == m_bReceiveValue)
        {
            m_v3Position = pGameInstance->Get_LightPositionF3();
            m_bReceiveValue = true;
        }
        else
        {
            ImGui::Checkbox(u8"업데이트 라이트", &m_bUpdateLight);

            {
                _float Arr[3] = { m_v3Position.x, m_v3Position.y, m_v3Position.z };
                ImGui::DragFloat3("Light Pos", Arr, 1.f);
                m_v3Position = _float3(Arr[0], Arr[1], Arr[2]);
            }

            {
                _float Arr[3] = { m_v3LookAt.x, m_v3LookAt.y, m_v3LookAt.z };
                ImGui::DragFloat3("Light Look At", Arr, 1.f);
                m_v3LookAt = _float3(Arr[0], Arr[1], Arr[2]);
            }

            if (true == m_bUpdateLight)
            {
                pGameInstance->Set_LightViewMatrix(XMLoadFloat3(&m_v3Position), XMLoadFloat3(&m_v3LookAt));
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}

CShaderView_Tool* CShaderView_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CShaderView_Tool* pInstance = new CShaderView_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CShaderView::Create To Failed");
#endif
    }
    return pInstance;
}

void CShaderView_Tool::Free()
{
    __super::Free();

    Safe_Release(m_pDebugRenderObj);
}
