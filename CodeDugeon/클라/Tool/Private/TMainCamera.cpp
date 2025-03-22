#include "ToolDefines.h"
#include "TMainCamera.h"
#include "UGameInstance.h"
#include "UTransform.h"

TMainCamera::TMainCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
    : UCamera(_spDevice, _wstrLayer, _eCloneType),
    m_isMoveState{ true }
{
}

TMainCamera::TMainCamera(const TMainCamera& _rhs) :
    UCamera(_rhs),
    m_isMoveState{ true }
{
}

void TMainCamera::Free()
{
}

HRESULT TMainCamera::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    return S_OK;
}

HRESULT TMainCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
    if (FAILED(__super::NativeConstructClone(_vecDatas)))
        return E_FAIL;

    return S_OK;
}

void TMainCamera::TickActive(const _double& _dTimeDelta)
{
}

void TMainCamera::LateTickActive(const _double& _dTimeDelta)
{
   // if (true == m_isMoveState)
    {
        SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);

        if (true == pGameInstance->GetDIMBtnPressing(DIMOUSEBUTTON::DIMB_R))
        {
            _double dMoveSpeed = _dTimeDelta;
            if (true == pGameInstance->GetDIKeyPressing(DIK_LSHIFT))
                dMoveSpeed = _dTimeDelta * 3;

            if (pGameInstance->GetDIKeyPressing(DIK_W))
                GetTransform()->MoveForward(dMoveSpeed, GetCamMoveSpeed());
            if (pGameInstance->GetDIKeyPressing(DIK_A))
                GetTransform()->MoveLeft(dMoveSpeed, GetCamMoveSpeed());
            if (pGameInstance->GetDIKeyPressing(DIK_D))
                GetTransform()->MoveRight(dMoveSpeed, GetCamMoveSpeed());
            if (pGameInstance->GetDIKeyPressing(DIK_S))
                GetTransform()->MoveBack(dMoveSpeed, GetCamMoveSpeed());

            _long		MouseMove = 0;

            if (MouseMove = pGameInstance->GetDIMMoveState(DIMOUSEMOVE::DIMM_X))
                GetTransform()->RotateTurn(_float3(0.f, 1.f, 0.f), GetCamRotSpeed() * MouseMove * 5.f, _dTimeDelta);

            if (MouseMove = pGameInstance->GetDIMMoveState(DIMOUSEMOVE::DIMM_Y))
                GetTransform()->RotateTurn(GetTransform()->GetRight(), GetCamRotSpeed() * MouseMove * 5.f, _dTimeDelta);
        }
        if (pGameInstance->IsMouseInWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT))
            GetTransform()->MoveForward(pGameInstance->GetDIMMoveState(DIMOUSEMOVE::DIMM_WHEEL) * 0.003f, GetCamMoveSpeed());
    }
}
