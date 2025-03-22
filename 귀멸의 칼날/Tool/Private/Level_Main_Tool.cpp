#include "MainView_Tool.h"
#include "Level_Main_Tool.h"
#include "GameInstance.h"
#include "Camera_Tool.h"
#include "ImGuiClass_Manager_Tool.h"
#include "Animator.h"

CLevel_Main_Tool::CLevel_Main_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex)
    : CLevel(_pDevice, _pContex)
{
}

HRESULT CLevel_Main_Tool::NativeConstruct()
{

    g_iCurrentLevel = LEVEL_TOOL;
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    if (FAILED(Ready_LightDesc()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
        return E_FAIL;

    ///  무조건 마지막 // ImGui Inner Value 할당
    CImGuiClass_Manager_Tool* pImGui_Manager = GET_INSTANCE(CImGuiClass_Manager_Tool);
    {
        if (FAILED(pImGui_Manager->NativeConstruct_InnerValue()))
            return E_FAIL;
    }
    RELEASE_INSTANCE(CImGuiClass_Manager_Tool);

    {
        CGameInstance* pGameInstance = CGameInstance::Get_Instance();
        Safe_AddRef(pGameInstance);

       /*if (FAILED(pGameInstance->Init_Effect(0)))
           return E_FAIL;*/

        Safe_Release(pGameInstance);
    }
    return S_OK;
}

void CLevel_Main_Tool::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CLevel_Main_Tool::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Main_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Main_Tool::Ready_LightDesc()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        LIGHTDESC			LightDesc;
        ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

        LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
        LightDesc.vDirection = _float3(1.f, -1.f, -1.f);
        LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
        LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
            return E_FAIL;
    }
    RELEASE_INSTANCE(CGameInstance);
    return S_OK;
}

HRESULT CLevel_Main_Tool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        // Caemra::Camera_Tool
        {
            CCamera::CAMDESC CameraDesc;
            ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

            CameraDesc.v3Eye = _float3(-50.f,50.f, 0.0f);
            CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
            CameraDesc.fSpeed = 5.f;
            CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
            CameraDesc.fFovy = XMConvertToRadians(60.0f);
            CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
            CameraDesc.fNear = 0.2f;
            CameraDesc.fFar = 5000;
            if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, PROTO_GOB_CAMERA_TOOL, &CameraDesc)))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }
        }
        // Camera::Camera_Free_Tool
        {
            CCamera::CAMDESC CameraDesc;
            ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

           CameraDesc.v3Eye = _float3(0.f, 5.f, -5.0f);
            CameraDesc.v3At = _float3(0.f, 5.f, 0.f);
            CameraDesc.fSpeed = 5.f;
            CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
            CameraDesc.fFovy = XMConvertToRadians(60.0f);
            CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
            CameraDesc.fNear = 0.2f;
            CameraDesc.fFar = 5000;

            if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, PROTO_GOB_CAMERA_FREE_TOOL, &CameraDesc)))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
    return S_OK;
}

HRESULT CLevel_Main_Tool::Ready_Layer_UI(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

   if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag,PROTO_GOB_UI_TOOL)))
        return E_FAIL;
	
    Safe_Release(pGameInstance);

    return S_OK;
}

CLevel_Main_Tool* CLevel_Main_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Main_Tool* pInstance = new CLevel_Main_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CLevel_Main_Tool::Create To Failed");
    }
    return pInstance;
}

void CLevel_Main_Tool::Free()
{
       __super::Free();
}
