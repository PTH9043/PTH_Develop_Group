#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CCamera_Tool.h"
#include "CGameInstance.h"
#include "CSkyBox.h"

CCamera_Tool::CCamera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool& _rhs)
    : CCamera(_rhs)
{
}

CCamera::CAMDESC CCamera_Tool::Get_CamDesc()
{
    CCamera::CAMDESC CameraDesc;
    XMStoreFloat3(&CameraDesc.v3Eye, m_pTransformCom->Get_Pos());
    CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
    XMStoreFloat3(&CameraDesc.v3At, m_pTransformCom->Get_Look());
    CameraDesc.fSpeed = 10.f;
    CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
    CameraDesc.fNear = 0.2f;
    CameraDesc.fFar = 1000.f;
    return CameraDesc;
}

HRESULT CCamera_Tool::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Tool::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Tool::Tick(const _double& _dTimeDelta)
{
    Move(_dTimeDelta);
    if (nullptr != m_pSkyBox)
    {
        CTransform* pTransform = static_cast<CTransform*>(m_pSkyBox->Get_Component(COM_TRANSFORM));
        if (nullptr == pTransform)
            return;

        pTransform->Set_Pos(m_pTransformCom->Get_Pos());

    }
    else
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            m_pSkyBox = static_cast<CSkyBox*>(pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_SKYBOX, 0));
        }
        END_SINGLETON(pGameInstance);
    }

    __super::Tick(_dTimeDelta);
}

void CCamera_Tool::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_Tool::Render()
{
    return S_OK;
}

_bool CCamera_Tool::Show_CamControlData()
{
    if (ImGui::TreeNodeEx("Cam Tool"))
    {
        if (nullptr != m_pTransformCom)
        {
            ImGui::InputFloat("Speed", &m_tCameraDesc.fSpeed, 0.f, 0.f, "%.1f");
            ImGui::InputFloat("Rotate", &m_tCameraDesc.fRotSpeed, 0.f, 0.f, "%.1f");
        }
        ImGui::TreePop();
    }
    return false;
}

void CCamera_Tool::Move(const _double& _dDeltaSeconds)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (true == pGameInstance->Get_DIKeyPressing(DIK_LSHIFT))
        {
            if (pGameInstance->Get_DIKeyPressing(DIK_W))
                m_pTransformCom->Move_Forward(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_A))
                m_pTransformCom->Move_Left(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_D))
                m_pTransformCom->Move_Right(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_S))
                m_pTransformCom->Move_Back(_dDeltaSeconds, m_tCameraDesc.fSpeed);

            _float2 MousePos = pGameInstance->Get_MousePosition();
            if (MousePos.x > 0 && MousePos.x < g_iWinSizeX)
            {
                if (MousePos.y > 0 && MousePos.y < g_iWinSizeY)
                {
                    _long		MouseMove = 0;

                    if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
                        m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);

                    if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
                        m_pTransformCom->Rotate_Turn(m_pTransformCom->Get_Right(), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);
                }
            }
        }
    }
    END_SINGLETON(pGameInstance);
}

CCamera_Tool* CCamera_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Tool* pInstance = new CCamera_Tool(_pDevice, _pContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSG_BOX("CCamera_Tool::Create to Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCamera_Tool::Clone(void* pArg)
{
    CCamera_Tool* pInstance = new CCamera_Tool(*this);

    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        MSG_BOX("CCamera_Tool::Clone to Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCamera_Tool::Free()
{
    __super::Free();
}
