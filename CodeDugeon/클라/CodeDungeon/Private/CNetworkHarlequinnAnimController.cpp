#include "ClientDefines.h"
#include "CNetworkHarlequinnAnimController.h"
#include "CHarlequinn.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "CWarriorPlayer.h"
#include "CShurikenThrowing.h"

CNetworkHarlequinnAnimController::CNetworkHarlequinnAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_didleRandomValueChoosingTimerforPatrol{ 0 },
    m_bAttack1FirstTime{ false },
    m_iRandomValueforPatrol{ 0 },
    m_bTauntMode{ false },
    m_f3ThrowingPos{},
    m_arrThrowingDir{},
    m_bDodge{ false },
    m_bWillWalkUntilCloseRange{ false },
    m_bWillJumpToCloseRange{ false },
    m_iRandomValueforDodge{}
{
}

CNetworkHarlequinnAnimController::CNetworkHarlequinnAnimController(const CNetworkHarlequinnAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_didleRandomValueChoosingTimerforPatrol{ 0 },
    m_bAttack1FirstTime{ false },
    m_iRandomValueforPatrol{ 0 },
    m_bTauntMode{ false },
    m_f3ThrowingPos{},
    m_arrThrowingDir{},
    m_bDodge{ false },
    m_bWillWalkUntilCloseRange{ false },
    m_bWillJumpToCloseRange{ false },
    m_iRandomValueforDodge{}
{
}

void CNetworkHarlequinnAnimController::Free()
{
}

HRESULT CNetworkHarlequinnAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CNetworkHarlequinnAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpHarlequinnMob = std::dynamic_pointer_cast<CHarlequinn>(GetOwnerCharacter());

    return S_OK;
}

void CNetworkHarlequinnAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<CHarlequinn> spHarlequinn = m_wpHarlequinnMob.lock();
    SHPTR<UAnimModel> spAnimModel = spHarlequinn->GetAnimModel();
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    if (CurAnimName == L"Attack 1")
    {
        m_f3ThrowingPos = spHarlequinn->GetTransform()->GetPos();
        m_f3ThrowingPos.y += 7.f;

        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.2 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.22)
        {
            for (int i = 0; i < 3; i++)
            {
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 0)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = 10 * (DirectX::XM_PI / 180.0f);;
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    else if (i == 1)
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();
                    else if (i == 2)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = -10 * (DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }

                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
        else if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.6 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.62)
        {
            for (int i = 3; i < 6; i++)
            {
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 3)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = 10 * (DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    else if (i == 4)
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();
                    else if (i == 5)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = -10 * (DirectX::XM_PI / 180.0f);;
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
    }

    if (CurAnimName == L"Attack 4")
    {
        m_f3ThrowingPos = spHarlequinn->GetTransform()->GetPos();
        m_f3ThrowingPos.y += 7.f;
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.2 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.22)
        {
            for (int i = 0; i < 6; i++)
            {
                _float angle = 0;
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 0)
                    {
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();
                    }
                    else
                    {
                        angle = 60 * (i + 1);
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = angle * (DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }

                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
    }

    //½´¸®ÄË ´øÁö±â 
    for (int i = 0; i < 6; i++)
    {
        if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() > 100)
        {
            (*spHarlequinn->GetShurikens())[i]->SetThrow(false);
            (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(spHarlequinn->GetTransform()->GetPos());
            (*spHarlequinn->GetShurikens())[i]->SetTraveledDistance(0);
        }
        else
            spHarlequinn->ThrowShurikens(i, _dTimeDelta, m_arrThrowingDir[i]);
    }
}
