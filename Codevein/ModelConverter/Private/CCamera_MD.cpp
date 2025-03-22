#include"imgui.h"
#include"imgui_internal.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "CImGui_Manager_MD.h"
#include "CCamera_MD.h"
#include "CGameInstance.h"

CCamera_MD::CCamera_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_MD::CCamera_MD(const CCamera_MD& _rhs)
    : CCamera(_rhs)
{
}

HRESULT CCamera_MD::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_MD::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_MD::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
    Move(_dTimeDelta);
}

void CCamera_MD::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_MD::Render()
{
    return S_OK;
}

_bool CCamera_MD::Show_CamControlData()
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

void CCamera_MD::Move(const _double& _dDeltaSeconds)
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

CCamera_MD* CCamera_MD::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_MD* pInstance = new CCamera_MD(_pDevice, _pContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSG_BOX("CCamera_Tool::Create to Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCamera_MD::Clone(void* pArg)
{
    CCamera_MD* pInstance = new CCamera_MD(*this);

    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        MSG_BOX("CCamera_Tool::Clone to Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCamera_MD::Free()
{
    __super::Free();
}
