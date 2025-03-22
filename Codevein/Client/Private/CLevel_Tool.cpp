#include "CImGui_Manager.h"
#include "CLevel_Tool.h"
#include "CGameInstance.h"

#include "CCamera_Tool.h"

#include "CData_Manager.h"

#include "CGiantDevil.h"
#include "CSkyBox.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex)
    : CLevel(_pDevice, _pContex)
{
}

HRESULT CLevel_Tool::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    if (FAILED(Ready_LightDesc()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(LAYER_CAM)))
        return E_FAIL;

    BEGIN_SINGLETON(CImGui_Manager, pInstance)
    {
        pInstance->Set_ToolMode(true);
   
        if (FAILED(pInstance->NativeConstruct_Tool()))
        {
            MSG_BOX("ImGuiManager::NativaeConstruct_Tool Failed");
            END_SINGLETON(pInstance);
            return E_FAIL;
        }
    }
    END_SINGLETON(pInstance);

    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        pDataManager->Get_PlayerMgr()->Spawn_Player(LEVEL_TOOL, _float3(0.f, 0.f, 100.f));
        
    }
    END_SINGLETON(pDataManager);

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        CSkyBox::SKYBOXDESC tSkyBoxDesc;
        tSkyBoxDesc.strSkyBoxName = L"SkyBox_Stage1";
        pGameInstance->Clone_GameObject(LEVEL_TOOL, LAYER_SKYBOX,PROTO_GOB_SKYBOX, &tSkyBoxDesc);
    }
    END_SINGLETON(pGameInstance);

    g_iCurrentLevel = LEVEL_TOOL;
    return S_OK;
}

void CLevel_Tool::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CLevel_Tool::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CLevel_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Tool::Ready_LightDesc()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        LIGHTDESC			LightDesc;
        ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

        LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
        LightDesc.v4Direction = _float4(0.f, -1.f, 0.f, 0.f);
        LightDesc.v4Diffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
        LightDesc.v4Ambient = _float4(0.2f, 0.2f, 0.2f, 1.f);
        LightDesc.v4Specular = _float4(1.f, 1.f, 1.f, 1.f);

        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        CCamera::CAMDESC CameraDesc;
        ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

        CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
        CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
        CameraDesc.fSpeed= 10.f;
        CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
        CameraDesc.fFovy = XMConvertToRadians(60.0f);
        CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
        CameraDesc.fNear = 0.2f;
        CameraDesc.fFar = 500;
        if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, pLayerTag, PROTO_GOB_CAMERA_TOOL, &CameraDesc)))
        {
            END_SINGLETON(pGameInstance);
            return E_FAIL;
        }
    }
    END_SINGLETON(pGameInstance);
     return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Tool* pInstance = new CLevel_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CLevel_Tool::Create To Failed");
    }
    return pInstance;
}

void CLevel_Tool::Free()
{
    BEGIN_SINGLETON(CImGui_Manager, pInstance)
    {
        pInstance->Set_ToolMode(false);
    }
    END_SINGLETON(pInstance)
    __super::Free();
}
