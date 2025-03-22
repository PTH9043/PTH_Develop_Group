#include "ClientDefines.h"
#include "CFreeModeCamera.h"
#include"UGameInstance.h"
#include "UTransform.h"
#include "CMainCamera.h"

CFreeModeCamera::CFreeModeCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, 
	const CLONETYPE& _eCloneType) : 
	UCamera(_spDevice, _wstrLayer, _eCloneType)
{
}

CFreeModeCamera::CFreeModeCamera(const CFreeModeCamera& _rhs) :
	UCamera(_rhs)
{
}

void CFreeModeCamera::Free()
{
}

HRESULT CFreeModeCamera::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFreeModeCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
    if (FAILED(__super::NativeConstructClone(_vecDatas)))
        return E_FAIL;

    return S_OK;
}

void CFreeModeCamera::TickActive(const _double& _dTimeDelta)
{
}

void CFreeModeCamera::LateTickActive(const _double& _dTimeDelta)
{
    if (false == IsActive()) return;

    _double dMoveSpeed = _dTimeDelta;
    SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
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

    m_spMainCamera->GetTransform()->SetNewWorldMtx(GetTransform()->GetWorldMatrix());
}
