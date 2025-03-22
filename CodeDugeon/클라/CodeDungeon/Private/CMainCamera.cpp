#include "ClientDefines.h"
#include "CMainCamera.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UNavigation.h"

CMainCamera::CMainCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCamera(_spDevice, _wstrLayer, _eCloneType),
	m_isMoveState{ true }, m_spCurNavi{nullptr}, m_vPrevPos{}
{
}

CMainCamera::CMainCamera(const CMainCamera& _rhs) :
	UCamera(_rhs),
	m_isMoveState{ true }, m_spCurNavi{ nullptr }, m_vPrevPos{}
{
}

void CMainCamera::Free()
{
}

HRESULT CMainCamera::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
    if (FAILED(__super::NativeConstructClone(_vecDatas)))
        return E_FAIL;
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    UNavigation::NAVDESC navDesc;
    navDesc.iCurIndex = 0;
    m_spCurNavi = std::static_pointer_cast<UNavigation>(spGameInstance->CloneComp(PROTO_NAVI_CAMERA, { &navDesc }));
    assert(nullptr != m_spCurNavi);

    return S_OK;
}

void CMainCamera::TickActive(const _double& _dTimeDelta)
{
    m_vPrevPos = GetTransform()->GetPos();
}

void CMainCamera::LateTickActive(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
    if (true == pGameInstance->IsFreeModeCameraEnable() &&
        false == pGameInstance->IsGamePause() && pGameInstance->IsMouseInWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT))
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

}
