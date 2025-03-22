#include "CameraLight.h"

#include "ImGuiClass_Manager.h"
#include "Camera_Manager.h"
#include "GameInstance.h"

CCameraLight::CCameraLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCamera(_pDevice, _pContext)
{
}

CCameraLight::CCameraLight(const CCameraLight& _rhs)
    :CCamera(_rhs)
{
}

HRESULT CCameraLight::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraLight::NativeConstruct_Clone(void* _pArg)
{
    if (nullptr != _pArg)
    {
        m_tCameraDesc = *static_cast<CAMDESC*>(_pArg);
    }

    if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    if (CCamera_Manager::Get_Instance()->Add_Camera(CM_CAMERA_LIGHT, this))
        return E_FAIL;

    Set_LightViewMatrix(XMLoadFloat3(&m_tCameraDesc.v3Eye), XMLoadFloat3(&m_tCameraDesc.v3At));

    _matrix		ProjMatrix;
    ProjMatrix = XMMatrixPerspectiveFovLH(m_tCameraDesc.fFovy, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
    CGameInstance::Get_Instance()->Set_LightTransform(CPipeLine::D3DTS_PROJ, ProjMatrix);
    CGameInstance::Get_Instance()->Set_LightFar(m_tCameraDesc.fFar);

    m_bActive = false;

    return S_OK;
}

void CCameraLight::Tick(const _double& _dTimeDelta)
{
    if (!m_bActive)
        return;

    if (FAILED(Bind_Matrices()))
        return;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        //pGameInstance->Set_LightTransform(CPipeLine::D3DTS_PREVVIEW, m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
        _matrix		ViewMatrix = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());
        pGameInstance->Set_LightTransform(CPipeLine::D3DTS_VIEW, ViewMatrix);
    }
    RELEASE_INSTANCE(CGameInstance);


    FreeCam_Move(_dTimeDelta);
}

void CCameraLight::Late_Tick(const _double& _dTimeDelta)
{

}

HRESULT CCameraLight::Render()
{
    return S_OK;
}

void CCameraLight::Set_LightViewMatrix(_vector vPos, _vector vLookAt)
{
    m_pTransformCom->Set_Pos(vPos);
    m_pTransformCom->LookAt(vLookAt);
    _matrix	ViewMatrix = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());
    CGameInstance::Get_Instance()->Set_LightTransform(CPipeLine::D3DTS_VIEW, ViewMatrix);
}

void CCameraLight::FreeCam_Move(const _double& _dTimeDelta)
{
    CImGuiClass_Manager* pInstance = GET_INSTANCE(CImGuiClass_Manager);
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        if (pInstance->Is_ImGuiActive() && !pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_R))
        {
            RELEASE_INSTANCE(CGameInstance);
            RELEASE_INSTANCE(CImGuiClass_Manager);
            return;
        }

        {
            if (pGameInstance->Get_DIKeyPressing(DIK_UP))
                m_pTransformCom->Move_Forward(_dTimeDelta, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_LEFT))
                m_pTransformCom->Move_Left(_dTimeDelta, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_RIGHT))
                m_pTransformCom->Move_Right(_dTimeDelta, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_DOWN))
                m_pTransformCom->Move_Back(_dTimeDelta, m_tCameraDesc.fSpeed);

            _float2 MousePos = pGameInstance->Get_MousePosition();
            if (MousePos.x > 0 && MousePos.x < g_iWinSizeX && MousePos.y > 0 && MousePos.y < g_iWinSizeY)
            {
                _long		MouseMove = 0;

                if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
                    m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _dTimeDelta * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);

                if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
                    m_pTransformCom->Rotate_Turn(m_pTransformCom->Get_Right(), _dTimeDelta * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
    RELEASE_INSTANCE(CImGuiClass_Manager);
}

CCameraLight* CCameraLight::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCameraLight* pInstance = new CCameraLight(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCameraLight::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CCameraLight::Clone(void* pArg)
{
    CCameraLight* pInstance = new CCameraLight(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCameraLight::Clone To Failed");
#endif
    }
    return pInstance;
}

void CCameraLight::Free()
{
    __super::Free();
}
