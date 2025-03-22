#include "Camera_Free.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager.h"
#include "Camera_Manager.h"
#include "Battle_Manager.h"
#include "Player.h"

CCamera_Free::CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& _rhs)
    : CCamera(_rhs)
{
}

CCamera::CAMDESC CCamera_Free::Get_CamDesc()
{
    CCamera::CAMDESC CameraDesc;
    XMStoreFloat3(&CameraDesc.v3Eye, m_pTransformCom->Get_Pos());
    CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
    XMStoreFloat3(&CameraDesc.v3At, m_pTransformCom->Get_Look());
    CameraDesc.fSpeed = 30.f; //10
    CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
    CameraDesc.fNear = 0.2f;
    CameraDesc.fFar = 1000.f;
    return CameraDesc;
}

void CCamera_Free::Start_CameraShake(_float fShakeTime, _float fMagnitude)
{
    if (!m_bActive)
        return;

    m_bIsShake = true;
    m_vOriginalPos = m_pTransformCom->Get_PosF3();
    m_fShakeMagnitude = fMagnitude;
    m_ShakeTimer.fStandardTime = fShakeTime + 0.1f;

    //CRenderer::DEBUGRENDER* pDesc = m_pRendererCom->Get_DebugRender();
    //if (nullptr != pDesc)
    //    pDesc->bShake = true;

    //m_ZoomTimer.fStandardTime = 0.3f;
} 

HRESULT CCamera_Free::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Free::NativeConstruct_Clone(void* _pArg)
{
	
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

    if (CCamera_Manager::Get_Instance()->Add_Camera(CM_CAMERA_FREE, this))
        return E_FAIL;

    if (FAILED(SetUp_Components()))
        return E_FAIL;

    m_bActive = true;
    m_ZoomTimer = CUSTIMER(0.f);

    return S_OK;
}

void CCamera_Free::Tick(const _double& _dTimeDelta)
{   

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    if (pGameInstance->Get_DIKeyDown(DIK_ESCAPE))
    {
        if (CInput_Device::CORSOR_NONE != pGameInstance->Get_CursorMode())
            pGameInstance->Set_CursorMode(CInput_Device::CORSOR_NONE);
        else
            pGameInstance->Set_CursorMode(CInput_Device::CURSOR_LOCKED);
    }
    if (pGameInstance->Get_DIKeyDown(DIK_TAB))
        m_bIsFree = !m_bIsFree;
    RELEASE_INSTANCE(CGameInstance);

    __super::Tick(_dTimeDelta);

}

void CCamera_Free::Late_Tick(const _double& _dTimeDelta)
{
    if (!m_bActive)
        return;

    //Check_Collision_MapObj();

    if (m_bIsFree)
        FreeCam_Move(_dTimeDelta);
    else
    {
        if (LEVEL_BATTLE == g_iCurrentLevel)
            BattleCam_Move(_dTimeDelta);
        if (LEVEL_STORY == g_iCurrentLevel)
            StoryCam_Control(_dTimeDelta);
        if (LEVEL_RATTAN == g_iCurrentLevel)
            RattanCam_Move(_dTimeDelta);
        if (LEVEL_GAMEPLAY == g_iCurrentLevel)
            TeOniCam_Move(_dTimeDelta);

        if (m_bIsShake)
            ShakeCam_Move(_dTimeDelta);
    }

    __super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

void CCamera_Free::Extern_ImGuiClass()
{
    _float fCamFar = CGameInstance::Get_Instance()->Get_CamFar();

    ImGui::DragFloat("CamFar", &fCamFar, 0.f);

}

void CCamera_Free::StoryCam_Control(const _double& _dTimeDelta)
{
    if (CBattle_Manager::Get_Instance()->IsStoryBattle())
        StoryBattleCam_Move(_dTimeDelta);
    else
        StoryCam_Move(_dTimeDelta);
}

void CCamera_Free::FreeCam_Move(const _double& _dDeltaSeconds)
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
                m_pTransformCom->Move_Forward(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_LEFT))
                m_pTransformCom->Move_Left(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_RIGHT))
                m_pTransformCom->Move_Right(_dDeltaSeconds, m_tCameraDesc.fSpeed);
            if (pGameInstance->Get_DIKeyPressing(DIK_DOWN))
                m_pTransformCom->Move_Back(_dDeltaSeconds, m_tCameraDesc.fSpeed);

            _float2 MousePos = pGameInstance->Get_MousePosition();
            if (MousePos.x > 0 && MousePos.x < g_iWinSizeX && MousePos.y > 0 && MousePos.y < g_iWinSizeY)
            {
                _long		MouseMove = 0;

                if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
                    m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);

                if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
                    m_pTransformCom->Rotate_Turn(m_pTransformCom->Get_Right(), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
    RELEASE_INSTANCE(CImGuiClass_Manager);
}

void CCamera_Free::BattleCam_Move(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
    {

        CCharacter* pPlayer1 = pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
        CCharacter* pPlayer2 = pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
        if (nullptr == pPlayer1 || nullptr == pPlayer2)
        {
            RELEASE_INSTANCE(CBattle_Manager);
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        // Player와 Target Transform 가져오기
        CTransform* pPlayerTransform = pPlayer1->Get_Transform();
        CTransform* pTargetTransform = pPlayer2->Get_Transform();//pPlayer1->Get_TargetTransform();
        if ((nullptr == pPlayerTransform) || (nullptr == pTargetTransform))
        {
            RELEASE_INSTANCE(CBattle_Manager);
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        // 2대 이상 피격된 경우 카메라 시점 전환
        {
            if ((true == pPlayer1->Get_BattleData().bIsOverHit) || (true == pPlayer2->Get_BattleData().bIsOverHit))
                m_bIsAttack = true;
            else
                m_bIsAttack = false;
        }

        {
            _vector vPlayerPos = pPlayerTransform->Get_Pos();
            _vector vTargetPos = pTargetTransform->Get_Pos();
            _vector vCamPos = m_pTransformCom->Get_Pos();

            // 낮은 플레이어 기준으로 Y값 조정
            _float fPlayerY = XMVectorGetY(vPlayerPos);
            _float fTargetY = XMVectorGetY(vTargetPos);
            _vector vCenterPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.8f);
            // 전 프레임 centerPos와 Lerp 통해 현재 CenterPos를 구한다.
            if (fTargetY > fPlayerY)
            {
                _vector vTmpPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.2f);
                _float fY = XMVectorGetY(XMVectorLerp(m_vPreCenter.Get_Vector(), vTmpPos, (_float)_dTimeDelta));
                vCenterPos = XMVectorSetY(vCenterPos, fY);
            }
            m_vPreCenter = vCenterPos;

            // 스크린의 중점 월드 위치
            _vector vScreenCenterPos = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f - 15.f, 0.f, 0.f), vCenterPos);

            _vector vCenterToCam = XMVectorSetY(vCamPos - vScreenCenterPos, 0.f);
            _vector vCenterToPlayer = XMVectorSetY(vPlayerPos - vCenterPos, 0.f);
            _vector vCenterToTarget = XMVectorSetY(vTargetPos - vCenterPos, 0.f);
            _vector vCamLook = XMVectorSetY(m_pTransformCom->Get_Look(), 0.f);

            _bool   bIsOtherSide = false;
            _float  fDistance = max(0.f, XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCenterToCam), vCenterToPlayer)));
            // 플레이어가 타겟보다 멀리 있을 경우
            if (0.f == fDistance)
                bIsOtherSide = true;
            if (bIsOtherSide)
            {
                fDistance = fabsf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCenterToCam), vCenterToTarget)));
            }

            _float  fOffsetDistance = 40.f;
            // 줌인 줌아웃
            if (0.f != m_ZoomTimer.fStandardTime)
            {
                _float fRatio = m_ZoomTimer.fTimer / m_ZoomTimer.fStandardTime;
                fOffsetDistance = 30.f + 10.f * fRatio;
                if (m_ZoomTimer.Is_Over(_dTimeDelta))
                {
                    m_ZoomTimer.fStandardTime = 0.f;
                    m_ZoomTimer.Reset_Timer();
                }
            }
            _float  fHeight = min(18.f, (fDistance + fOffsetDistance) * 0.3f);
            _vector vPos = vCenterPos + XMVector3Normalize(vCenterToCam) * (fDistance + fOffsetDistance) + XMVectorSet(0.f, fHeight, 0.f, 0.f);

            // 카메라 시점 변환
            if (m_bIsAttack)
            {
                _vector vDir = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCenterToPlayer);
                vDir = XMVectorLerp(XMVector3Normalize(vCenterToCam), XMVector3Normalize(vDir), (_float)_dTimeDelta * 10.f);
                vPos = vCenterPos + XMVector3Normalize(vDir) * (fDistance + fOffsetDistance) + XMVectorSet(0.f, fHeight, 0.f, 0.f);
            }

            // 플레이어의 스크린 좌표가 일정 범위를 벗어나면 벗어난 만큼 카메라를 이동해준다. 
            _float3 vPlayerScreenPos = pGameInstance->Get_WorldToScreen(m_pContext, vPlayerPos);

            if (vPlayerScreenPos.x < 320.f || vPlayerScreenPos.x > 960.f)
            {
                _vector vLeftWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(320.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);
                _vector vRightWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(960.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);

                // 화면 왼쪽 벗어날때
                if (320.f > vPlayerScreenPos.x)
                {
                    _vector vTmpPos;
                    if (bIsOtherSide)
                        vTmpPos = vPos - XMVectorSetY(vPlayerPos - vLeftWorld, 0.f);
                    else
                        vTmpPos = vPos + XMVectorSetY(vPlayerPos - vLeftWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
                // 화면 오른쪽 벗어날때
                else
                {
                    _vector vTmpPos;
                    if (bIsOtherSide)
                        vTmpPos = vPos - XMVectorSetY(vPlayerPos - vRightWorld, 0.f);
                    else
                        vTmpPos = vPos + XMVectorSetY(vPlayerPos - vRightWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
            }

            //vPos = XMVectorLerp(vCamPos, vPos, (_float)_dTimeDelta * 20.f);
            vCenterPos = vCenterPos + XMVectorSet(0.f, 15.f, 0.f, 0.f);

            m_pTransformCom->Set_Pos(vPos);
            m_pTransformCom->LookAt(vCenterPos);
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
    RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::TeOniCam_Move(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
    {
        CPlayer* pPlayer = (CPlayer*)pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
        if (Is_Nullptr(pPlayer))
        {
            RELEASE_INSTANCE(CBattle_Manager);
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        // Player와 Target Transform 가져오기
        CTransform* pPlayerTransform = pPlayer->Get_Transform();
        CTransform* pTargetTransform = pPlayer->Get_TargetTransform();
        if ((nullptr == pPlayerTransform) || (nullptr == pTargetTransform))
        {
            RELEASE_INSTANCE(CBattle_Manager);
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        {
            _vector vPlayerPos = pPlayerTransform->Get_Pos();
            _vector vTargetPos = pTargetTransform->Get_Pos();
            _vector vCamPos = m_pTransformCom->Get_Pos();

            // 낮은 플레이어 기준으로 Y값 조정
            _float fPlayerY = XMVectorGetY(vPlayerPos);
            _float fTargetY = XMVectorGetY(vTargetPos);
            _vector vCenterPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.8f);
            // 전 프레임 centerPos와 Lerp 통해 현재 CenterPos를 구한다.
            if (fTargetY > fPlayerY)
            {
                _vector vTmpPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.2f);
                _float fY = XMVectorGetY(XMVectorLerp(m_vPreCenter.Get_Vector(), vTmpPos, (_float)_dTimeDelta));
                vCenterPos = XMVectorSetY(vCenterPos, fY);
            }
            m_vPreCenter = vCenterPos;

            // 스크린의 중점 월드 위치
            _vector vScreenCenterPos = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f - 15.f, 0.f, 0.f), vCenterPos);

            _vector vCenterToCam = XMVectorSetY(vCamPos - vScreenCenterPos, 0.f);
            _vector vCenterToPlayer = XMVectorSetY(vPlayerPos - vCenterPos, 0.f);
            _vector vCenterToTarget = XMVectorSetY(vTargetPos - vCenterPos, 0.f);
            _vector vCamLook = XMVectorSetY(m_pTransformCom->Get_Look(), 0.f);

            _float fDistance = max(0.f, XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCenterToCam), vCenterToPlayer)));
            _float fHeight = min(20.f, (fDistance + 50.f) * 0.3f);

            _vector vPos = vCenterPos + XMVector3Normalize(vCenterToCam) * (fDistance + 50.f) + XMVectorSet(0.f, fHeight, 0.f, 0.f);

            _float3 vPlayerScreenPos = pGameInstance->Get_WorldToScreen(m_pContext, vPlayerPos);
            if (vPlayerScreenPos.x < 500.f || vPlayerScreenPos.x > 780.f)
            {
                _vector vLeftWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(500.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);
                _vector vRightWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(780.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);

                // 화면 왼쪽 벗어날때
                if (500.f > vPlayerScreenPos.x)
                {
                    _vector vTmpPos = vPos + XMVectorSetY(vPlayerPos - vLeftWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
                // 화면 오른쪽 벗어날때
                else
                {
                    _vector vTmpPos = vPos + XMVectorSetY(vPlayerPos - vRightWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
            }

            vPos = XMVectorLerp(vCamPos, vPos, (_float)_dTimeDelta * 20.f);
            vCenterPos = vCenterPos + XMVectorSet(0.f, 15.f, 0.f, 0.f);

            m_pTransformCom->Set_Pos(vPos);
            m_pTransformCom->LookAt(vCenterPos);

        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
    RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::StoryCam_Move(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_Component(g_iCurrentLevel, LAYER_STORYPLAYER, COM_TRANSFORM, 0);
        if (nullptr == pPlayerTransform)
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        if (m_bStoryEnding)
        {
            m_pTransformCom->LookAt(pPlayerTransform->Get_Pos() + XMVectorSet(0.f, 10.f, 0.f, 0.f));
        }
        else
        {
            _float2 MousePos = pGameInstance->Get_MousePosition();
            _long MouseMoveX = 0;

            if (MousePos.x > 0 && MousePos.x < g_iWinSizeX && MousePos.y > 0 && MousePos.y < g_iWinSizeY)
            {
                MouseMoveX = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X);
            }

            _float3 vPlayerToCam = m_pTransformCom->Get_PosF3() - pPlayerTransform->Get_PosF3();

            _float fCamAngle = CMethod::Compute_Radian_XZ_Plane(_float3(0.f, 0.f, 1.f), vPlayerToCam);

            _vector vecDir = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMMatrixRotationY(fCamAngle + XMConvertToRadians((_float)MouseMoveX * 0.5f)));

            _vector vOffset = XMVector3Normalize(vecDir) * 40.f - XMVectorSet(0.f, 15.f, 0.f, 0.f);

            //_vector vCurPos = XMVectorLerp(m_pTransformCom->Get_Pos(), pPlayerTransform->Get_Pos() - vOffset, (_float)_dTimeDelta);

            m_pTransformCom->Set_Pos(pPlayerTransform->Get_Pos() - vOffset);
            m_pTransformCom->LookAt(pPlayerTransform->Get_Pos() + XMVectorSet(0.f, 10.f, 0.f, 0.f));
        }

    }
    RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::RattanCam_Move(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {

        _float2 MousePos = pGameInstance->Get_MousePosition();
        _long MouseMoveX = 0;

        if (MousePos.x > 0 && MousePos.x < g_iWinSizeX && MousePos.y > 0 && MousePos.y < g_iWinSizeY)
        {
            MouseMoveX = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X);
        }

        CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_Component(g_iCurrentLevel, LAYER_STORYPLAYER, COM_TRANSFORM, 0);
        if (nullptr == pPlayerTransform)
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        _float3 vPlayerToCam = m_pTransformCom->Get_PosF3() - pPlayerTransform->Get_PosF3();

        _float fCamAngle = CMethod::Compute_Radian_XZ_Plane(_float3(0.f, 0.f, 1.f), vPlayerToCam);

        _vector vecDir = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMMatrixRotationY(fCamAngle + XMConvertToRadians((_float)MouseMoveX)));

        _vector vOffset = XMVector3Normalize(vecDir) * 25.f - XMVectorSet(0.f, 20.f, 0.f, 0.f);

        m_pTransformCom->Set_Pos(pPlayerTransform->Get_Pos() - vOffset);
        m_pTransformCom->LookAt(pPlayerTransform->Get_Pos() + XMVectorSet(0.f, 10.f, 0.f, 0.f));

    }
    RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::StoryBattleCam_Move(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
    {

        CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0);
        if (Is_Nullptr(pPlayer))
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        // Player와 Target Transform 가져오기
        CTransform* pPlayerTransform = pPlayer->Get_Transform();
        CTransform* pTargetTransform = pPlayer->Get_TargetTransform();
        if ((nullptr == pPlayerTransform) || (nullptr == pTargetTransform))
        {
            RELEASE_INSTANCE(CBattle_Manager);
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        // 2대 이상 피격된 경우 카메라 시점 전환
        {
            /*if ((pPlayer1->Get_BattleData().iHitCount >= 2) || (pPlayer2->Get_BattleData().iHitCount >= 2))
                m_bIsAttack = true;
            else
                m_bIsAttack = false;*/
            if (pPlayer->Get_BattleData().iHitCount >= 2)
                m_bIsAttack = true;
            else
                m_bIsAttack = false;
        }

        {
            _vector vPlayerPos = pPlayerTransform->Get_Pos();
            _vector vTargetPos = pTargetTransform->Get_Pos();
            _vector vCamPos = m_pTransformCom->Get_Pos();

            // 낮은 플레이어 기준으로 Y값 조정
            _float fPlayerY = XMVectorGetY(vPlayerPos);
            _float fTargetY = XMVectorGetY(vTargetPos);
            _vector vCenterPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.5f);
            // 전 프레임 centerPos와 Lerp 통해 현재 CenterPos를 구한다.
            /*if (fTargetY > fPlayerY)
            {
                _vector vTmpPos = XMVectorLerp(vPlayerPos, vTargetPos, 0.5f);
                _float fY = XMVectorGetY(XMVectorLerp(m_vPreCenter.Get_Vector(), vTmpPos, (_float)_dTimeDelta));
                vCenterPos = XMVectorSetY(vCenterPos, fY);
            }
            m_vPreCenter = vCenterPos;*/

            // 스크린의 중점 월드 위치
            _vector vScreenCenterPos = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f - 15.f, 0.f, 0.f), vCenterPos);

            _vector vCenterToCam = XMVectorSetY(vCamPos - vScreenCenterPos, 0.f);
            _vector vCenterToPlayer = XMVectorSetY(vPlayerPos - vCenterPos, 0.f);
            _vector vCenterToTarget = XMVectorSetY(vTargetPos - vCenterPos, 0.f);
            _vector vCamLook = XMVectorSetY(m_pTransformCom->Get_Look(), 0.f);

            _bool   bIsOtherSide = false;
            _float  fDistanceOffset = 50.f;
            _float  fDistance = max(0.f, XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCenterToCam), vCenterToPlayer)));
            _float  fHeight = min(18.f, (fDistance + fDistanceOffset) * 0.3f);
            // 플레이어가 타겟보다 멀리 있을 경우
            if (0.f == fDistance)
                bIsOtherSide = true;
            if (bIsOtherSide)
            {
                fDistance = fabsf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCenterToCam), vCenterToTarget)));
                fHeight = min(18.f, (fDistance + fDistanceOffset) * 0.3f);
            }

            _vector vPos = vCenterPos + XMVector3Normalize(vCenterToCam) * (fDistance + fDistanceOffset) + XMVectorSet(0.f, fHeight, 0.f, 0.f);

            // 카메라 시점 변환
            if (m_bIsAttack)
            {
                _vector vDir = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCenterToPlayer);
                vDir = XMVectorLerp(XMVector3Normalize(vCenterToCam), XMVector3Normalize(vDir), (_float)_dTimeDelta * 10.f);
                vPos = vCenterPos + XMVector3Normalize(vDir) * (fDistance + fDistanceOffset) + XMVectorSet(0.f, fHeight, 0.f, 0.f);
            }

            // 플레이어의 스크린 좌표가 일정 범위를 벗어나면 벗어난 만큼 카메라를 이동해준다. 
            _float3 vPlayerScreenPos = pGameInstance->Get_WorldToScreen(m_pContext, vPlayerPos);
            if (vPlayerScreenPos.x < 500.f || vPlayerScreenPos.x > 780.f)
            {
                _vector vLeftWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(500.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);
                _vector vRightWorld = pGameInstance->Get_ScreenToWorld(m_pContext, XMVectorSet(780.f, vPlayerScreenPos.y, 0.f, 0.f), vPlayerPos);

                // 화면 왼쪽 벗어날때
                if (500.f > vPlayerScreenPos.x)
                {
                    _vector vTmpPos;
                    if (bIsOtherSide)
                        vTmpPos = vPos - XMVectorSetY(vPlayerPos - vLeftWorld, 0.f);
                    else
                        vTmpPos = vPos + XMVectorSetY(vPlayerPos - vLeftWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
                // 화면 오른쪽 벗어날때
                else
                {
                    _vector vTmpPos;
                    if (bIsOtherSide)
                        vTmpPos = vPos - XMVectorSetY(vPlayerPos - vRightWorld, 0.f);
                    else
                        vTmpPos = vPos + XMVectorSetY(vPlayerPos - vRightWorld, 0.f);
                    vPos = XMVectorLerp(vPos, vTmpPos, (_float)_dTimeDelta * 15.f);
                }
            }

            // vPos = XMVectorLerp(vCamPos, vPos, (_float)_dTimeDelta * 20.f);
            vCenterPos = vCenterPos + XMVectorSet(0.f, 12.f, 0.f, 0.f);

            m_pTransformCom->Set_Pos(vPos);
            m_pTransformCom->LookAt(vCenterPos);
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
    RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Free::ShakeCam_Move(const _double& _dTimeDelta)
{
    _bool bIsDelay = false;
    bIsDelay = m_ShakeTimer.Is_Over(_dTimeDelta);

    // 타이머가 끝나면 초기화 한 뒤 리턴해준다.
    if (true == bIsDelay)
    {
        m_ShakeTimer.Reset_Timer();
        m_bIsShake = false;

        CRenderer::DEBUGRENDER* pDesc = m_pRendererCom->Get_DebugRender();
        if (nullptr != pDesc)
            pDesc->bShake = false;
        return;
    }

    // 타이머가 돌 동안에 카메라를 흔든다.
    _float3 vRandomUnitSphere = _float3(_float(rand() % 100 - 50), _float(rand() % 100 - 50), _float(rand() % 100 - 50)).Normalize();

    m_pTransformCom->Set_PosF3(vRandomUnitSphere * m_fShakeMagnitude + m_pTransformCom->Get_PosF3());
}

void CCamera_Free::Check_Collision_MapObj()
{
    if (nullptr == m_pColliderCom)
        return;

    _matrix scaleMatrix = XMMatrixScaling(20.f, 20.f, 20.f);
    m_pColliderCom->Tick(scaleMatrix * m_pTransformCom->Get_WorldMatrix());

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    list<CGameObject*> mapObjList = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_MAPOBJ);
    for (auto& mapObj : mapObjList)
    {
        CCollider* pMapCollider = (CCollider*)mapObj->Get_Component(COM_COLLIDER_SPHERE);
        if (nullptr == pMapCollider)
            continue;
        if (m_pColliderCom->Is_Collision(pMapCollider))
            mapObj->Set_Active(false);
        else
            mapObj->Set_Active(true);
    }
    
    RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCamera_Free::SetUp_Components()
{
    CCollider::COLLIDERDESC tColliderDesc;
    ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
    tColliderDesc.vScale = _float3{ 0.5f, 0.5f, 0.5f };
    tColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_SPHERE, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &tColliderDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_Free::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_Free::Create To Failed");
#endif
    }
    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pRendererCom);
}
